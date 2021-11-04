# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xx-encrypted [<options>] <encrypted-file> [<encrypted file> ...]

Compare and merge contents of encrypted files relatively safely.

This script wraps around xxdiff, first decrypting the input files to temporary
files (for a short time) and running xxdiff on these files.  There are two
typical uses of this program:

1) it is used to compare two encrypted files.  With the --output option, a
   decision is required and an encrypted version of the merged file is output to
   the specified file and the merged file deleted promptly.  Note that without
   the --output option, even if the merged file is saved, it is deleted once
   xxdiff exits.

2) it is used to split and resolve CVS conflicts in an armored encrypted file
   (see --unmerge option).  The merged file is encrypted and output over the
   conflictual input file (i.e. it replaces it with the encrypted version of the
   merged file).  This is very useful if you maintain armored encrypted files in
   CVS repositories because otherwise an encrypted file with a CVS conflict in
   it becomes useless.

Using gpg-agent
---------------

Usage of this program with password caching using gpg-agent makes it much easier
to call on multiple files.  The user's password given key is asked only once by
gpg-agent, kept in memory, and then decryption occurs without user intervention.

Merging Encrypted Files with Mercurial
--------------------------------------

Mercurial can be easily configured so that this tool gets invoked automatically
when encrypted files need to be merged. This way you can independently
edit encrypted files using Emacs (I use a custom mailcrypt setup to seamlessly
decode on open and encode on save) and when tracked in a Mercurial repository,
if the encrypted files have been edited in two separate revisions, xx-encrypted
is invoked automatically and brings up a 3-way xxdiff to merge them interactively.
Here's the configuration I use in my .hgrc::

  [merge-tools]
  xx-encrypted =
  xx-encrypted.priority = 100
  xx-encrypted.premerge = False
  xx-encrypted.args = $local $base $other -o $output

  [merge-patterns]
  **.asc = xx-encrypted
  **.gpg = xx-encrypted

Alternatively, you may just use xx-hg-merge which deals with all file types,
encrypted or not:

  [merge-patterns]
  ** = xx-hg-merge

"""



__moredoc__ = """
Safety Notes
------------

The encrypted files are decrypted to temporary files for a short amount of time,
and are deleted when xxdiff appears.  Note that their deletion is as safe as
Python's tempfile module allows it to be (in the author's opinion, safe
enough). I left comments in the code to allow a user to review where the files
are decrypted so they can judge by themselves if it is safe enough for their
use.

We could do much better in terms of safety if we could feed the input files to
xxdiff through different file descriptors (not impossible to implement) AND
calculate the diffs internally.

(Note that if someone can manipulate which program is used to actually perform
the diffs (e.g. modifying an unsuspecting user's resources in ~/.xxdiffrc), they
could feed the decrypted files to an arbitrary program.)
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4', 'GnuPG']

# stdlib imports.
import sys, os, re
from os.path import *
from tempfile import NamedTemporaryFile
from subprocess import Popen, PIPE

# xxdiff imports.
import xxdiff.scripts
import xxdiff.invoke
import xxdiff.scm.cvs
from xxdiff.scripts import tmpprefix


decodecmd = '%(gpg)s --decrypt --use-agent '
encodecmd_noarmor = '%(gpg)s --encrypt --use-agent '
encodecmd = encodecmd_noarmor + '--armor '


def get_recipient(text, gpg):
    """ Extract the recipient/key name from the given encrypted text, without
    saving any temporary file. """
    p = Popen(('gpg', '--list-only'), stdin=PIPE, stdout=PIPE, stderr=PIPE, text=True)
    out, err = p.communicate(text)
    mo = re.compile('.*public key is ([0-9A-F]+)', re.M).search(err)
    if mo:
        return mo.group(1)


def diff_encrypted(textlist, opts, outmerged=None):
    """
    Run a comparison of the encrypted texts specified in textlists and if an
    'outmerged' filename is specified, encrypt the merged file into it. Note
    that the texts are not filenames, but actual contents of files.
    """
    m = {'gpg': opts.gpg}

    # Create temporary files.
    tempfiles = []
    for t in range(len(textlist)):
        f = NamedTemporaryFile(prefix=tmpprefix)
        print('== TEMPFILE', f.name)
        tempfiles.append(f)

    # Figure out the key/recipient for the first file.
    if opts.recipient is None:
        opts.recipient = get_recipient(textlist[0], opts.gpg)

    # Always create a temporary file for the merged file, we will delete it for
    # sure, since it would contain decrypted content if saved.

    # Decode the files.
    for i in range(len(textlist)):
        t, f = textlist[i], tempfiles[i]

        # Decode one file to an existing temporary file.
        fin, fout = os.popen2(decodecmd % m, 'w')
        fin.write(t)
        fin.close()
        decoded_output = fout.read()
        fout.close()

        f.write(decoded_output)
        f.flush()

    # Spawn xxdiff on the temporary/decoded files.
    waiter = xxdiff.invoke.xxdiff_decision(
        opts, nowait=1, *[x.name for x in tempfiles])

    # Close and automatically delete the temporary/decoded files.
    for f in tempfiles:
        f.close()

    print('Waiting...')
    decision, mergedf, retcode = waiter()

    if decision != 'NODECISION' and outmerged:
        # Read the decoded merged output file from xxdiff.
        textm = mergedf.read()
        assert textm

        # Close and automatically delete the decoded merged output file.
        mergedf.close()

        # Encode the merged output text.
        if not opts.dont_armor:
            cmd = encodecmd % m
        else:
            cmd = encodecmd_noarmor % m

        if opts.recipient:
            cmd += ' --recipient "%s"' % opts.recipient

        fin, fout = os.popen2(cmd, 'w')
        fin.write(textm)
        fin.close()
        encoded_output = fout.read()
        fout.close()

        # Write out the encoded output file.
        try:
            f = open(outmerged, 'w')
            f.write(encoded_output)
            f.close()
        except IOError as e:
            print('Error: cannot write to encoded merged file.', file=sys.stderr)
            raise e

    return decision


def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__)

    xxdiff.invoke.options_graft(parser)

    parser.add_option('-g', '--gpg', default="gpg",
                      help="Specify path to gpg program to use.")

    parser.add_option('-A', '--dont-armor', action='store_true',
                      help="Create output file in binary format.")

    parser.add_option('-o', '--output', action='store',
                      help="Require and encrypt merged output.")

    parser.add_option('-u', '--unmerge', action='store_true',
                      help="Split CVS conflicts in single input file and "
                      "encrypt required output merged file over input.")

    parser.add_option('-r', '--recipient', action='store',
                      help="Encrypt for user id name.")

    global opts
    xxdiff.scripts.install_autocomplete(parser)

    opts, args = parser.parse_args()

    if not args:
        raise parser.error('No files to decrypt and compare.')

    xxdiff.invoke.options_validate(opts, parser, logs=sys.stdout)

    return opts, args

def encrypted_main():
    opts, args = parse_options()

    if isabs(opts.gpg) and not exists(opts.gpg):
        raise SystemExit('Error: gpg program does not exist in "%s"' %
                         opts.gpg)

    if not (opts.unmerge or opts.output):
        if opts.dont_armor:
            print("Warning: there will be no output file, " +
                  "--dont-armor will means nothing special.", file=sys.stderr)
        if opts.recipient:
            print("Warning: there will be no output file, " +
                  "--recipient will means nothing special.", file=sys.stderr)

    if opts.unmerge:
        for fn in args: # do all files specified on cmdline, why not.
            # Read input conflict file.
            text = open(fn, 'r').read()
            text1, text2 = xxdiff.scm.cvs.unmerge2(text)
            diff_encrypted([text1, text2], opts, fn)
    else:
        if len(args) <= 1:
            raise SystemExit("Error: you need to specify 2 or 3 arguments.")

        textlist = []
        for fn in args:
            text = open(fn, 'r').read()
            textlist.append(text)

        diff_encrypted(textlist, opts, opts.output)


main = encrypted_main

if __name__ == '__main__':
    main()

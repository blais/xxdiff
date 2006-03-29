#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xxdiff-encrypted [<options>] <encrypted-file> [<encrypted file> ...]

Compare and merge contents of encrypted files relatively safely.

This script wraps around xxdiff, first decrypting the input files to temporary
files (for a short time) and running xxdiff on these files.  There are two
typical uses of this program:

1) it is used to compare two encrypted files.  With the --merge option, a
   decision is required and an encrypted version of the merged file is output to
   the specified file and the merged file deleted promptly.  Note that without
   the --merge option, even if the merged file is saved, it is deleted once
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
__depends__ = ['xxdiff', 'Python-2.3', 'GnuPG']


# stdlib imports.
import sys, os
from os.path import *
import re
from tempfile import NamedTemporaryFile

# xxdiff imports.
import xxdiff.scripts


#-------------------------------------------------------------------------------
#
pfx = join("%s.tmp." % basename(sys.argv[0]))

diffcmd = '%(xxdiff)s --merged-filename "%(merged)s" ' + \
          '--indicate-input-processed ' # + filenames
# Note: we are not even using --decision for the case where output is requested
# (i.e. --output=filename), because we want to let the user decide at the time
# of comparison if he wants to back out on merging the files. Not letting him
# "not save" the merged output (i.e. forcing accept/merge/reject with
# --decision does give the option of the status quo.)

decodecmd = '%(gpg)s --decrypt --use-agent '
encodecmd_noarmor = '%(gpg)s --encrypt --use-agent '
encodecmd = encodecmd_noarmor + '--armor '

#-------------------------------------------------------------------------------
#
# Note: this is cut-n-paste from cvs.py, for simplicity of distribution.
def unmerge2( text ):
    """Unmerges conflicts between two files and returns the two split original
    files."""

    begre = re.compile('^<<<<<<< (.*)')
    sepre = re.compile('^=======\s*$')
    endre = re.compile('^>>>>>>> (.*)')
    OUT, IN1, IN2 = 0, 1, 2
    status = OUT
    lines1, lines2 = [], []
    for l in text.splitlines():
        if status == OUT:
            if begre.match(l):
                status = IN1
            else:
                lines1.append(l)
                lines2.append(l)
        elif status == IN1:
            if sepre.match(l):
                status = IN2
            else:
                lines1.append(l)
        elif status == IN2:
            if endre.match(l):
                status = OUT
            else:
                lines2.append(l)
    text1 = os.linesep.join(lines1)
    text2 = os.linesep.join(lines2)
    return text1, text2

cvs_unmerge2 = unmerge2


#-------------------------------------------------------------------------------
#
def do_xxdiff( textlist, outmerged=None ):

    """Run a comparison of the encrypted texts specified in textlists and if
    outmerged filename is specified, encrypt the merged file into it."""

    # Create temporary files.
    tempfiles = []
    for t in xrange(len(textlist)):
        f = NamedTemporaryFile(prefix=pfx)
        print '== TEMPFILE', f.name
        tempfiles.append(f)

    # Always create a temporary file for the merged file, we will delete it for
    # sure, since it would contain decrypted content if saved.
    fmerge = NamedTemporaryFile(prefix=pfx)
    print '== TEMPFILE', fmerge.name

    m = {'xxdiff': opts.xxdiff,
         'gpg': opts.gpg,
         'merged': fmerge.name}

    # Decode the files.
    for i in xrange(len(textlist)):
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
    cmd = diffcmd % m + ' '.join(map(lambda x: '"%s"' % x.name, tempfiles))
    fout = os.popen(cmd, 'r')

    # Leave time for xxdiff to read the files before hiding, it will let us know
    # when it is done with the input files.
    l = fout.readline()
    assert l.startswith('INPUT-PROCESSED')

    # Close and automatically delete the temporary/decoded files.
    for f in tempfiles:
        f.close()

    if opts.unmerge or opts.output:
        print 'Waiting... save as merged file to merge back to encrypted file.'
    else:
        print 'Waiting...'
    fout.read()

    # Read the decoded merged output file from xxdiff.
    textm = fmerge.read()

    # Close and automatically delete the decoded merged output file.
    fmerge.close()

    if outmerged:
        # Processed the merged text.
        if not textm:
            print >> sys.stderr, \
                  'Warning: merged file was not saved. Not replacing.'
        else:
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
            except IOError, e:
                print >> sys.stderr, \
                      'Error: cannot write to encoded merged file.'
                raise e










#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__)
    parser.add_option('-x', '--xxdiff', default="xxdiff",
                      help="specify path to xxdiff program to use")
    parser.add_option('-g', '--gpg', default="gpg",
                      help="specify path to gpg program to use")
    parser.add_option('-o', '--output', action='store',
                      help="require and encrypt merged output.")
    parser.add_option('-u', '--unmerge', action='store_true',
                      help="split CVS conflicts in single input file and "
                      "encrypt required output merged file over input")
    parser.add_option('-A', '--dont-armor', action='store_true',
                      help="Create output file in binary format.")
    parser.add_option('-r', '--recipient', action='store',
                      help="Encrypt for user id name.")
    global opts
    xxdiff.scripts.install_autocomplete(parser)

    opts, args = parser.parse_args()

    if not args:
        raise parser.error('no files to decrypt and compare.')

    return opts, args

#-------------------------------------------------------------------------------
#
def encrypted_main():
    """
    Main program for cond-replace script.
    """
    opts, args = parse_options()

    if isabs(opts.xxdiff) and not exists(opts.xxdiff):
        raise SystemExit('Error: xxdiff program does not exist in "%s"' %
                         opts.xxdiff)
    if isabs(opts.gpg) and not exists(opts.gpg):
        raise SystemExit('Error: gpg program does not exist in "%s"' %
                         opts.gpg)

    if not (opts.unmerge or opts.output):
        if opts.dont_armor:
            print >> sys.stderr, "Warning: there will be no output file, " + \
                  "--dont-armor will means nothing special."
        if opts.recipient:
            print >> sys.stderr, "Warning: there will be no output file, " + \
                  "--recipient will means nothing special."

    if opts.unmerge:
        for fn in args: # do all files specified on cmdline, why not.
            # Read input conflict file.
            text = open(fn, 'r').read()
            text1, text2 = cvs_unmerge2(text)
            do_xxdiff([text1, text2], fn)
    else:
        if len(args) <= 1:
            raise SystemExit("Error: you need to specify 2 or 3 arguments.")

        textlist = []
        for fn in args:
            text = open(fn, 'r').read()
            textlist.append(text)
        do_xxdiff(textlist, opts.output)


#-------------------------------------------------------------------------------
#
main = encrypted_main

if __name__ == '__main__':
    main()


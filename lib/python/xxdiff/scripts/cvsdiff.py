#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xxdiff-cvs-diff [<options>] [<file> <file> ...]

This simple script invokes 'cvs diff' with the given file arguments, then splits
the output patch for individual files, applies the reverse patches to temporary
files and for each file it then spawns an xxdiff to preview each modified file
separately.  This allows you to preview the current changes that are made in a
cvs checkout.

Optionally, you can decide to accept changed and they are committed file by file
by this script.  In that case, the spawned xxdiff asks for a decision by the
user, then the script takes the following actions upon the answer:

- ACCEPT: keep the new file as it is and commit
- MERGED: copy the merged file on the new file and commit
- REJECT: don't do anything, keep the new file as it but do not commit.

For more generic behaviour about merging patches graphically, see also
xxdiff-patch.  The current script is really about committing "some" cvs changes.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4', 'cvs', 'diffutils']


# stdlib imports.
import os, shutil
from subprocess import Popen, PIPE, STDOUT
from tempfile import NamedTemporaryFile

# xxdiff imports.
import xxdiff.scripts
import xxdiff.patches
from xxdiff.scripts import tmpprefix


#-------------------------------------------------------------------------------
#
def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    parser.add_option('-c', '--commit', action='store_true',
                      help="Ask for confirmation and commit accepted changes.")

    xxdiff.scripts.install_autocomplete(parser)

    opts, args = parser.parse_args()
    return opts, args


#-------------------------------------------------------------------------------
#
def cvsdiff_main():
    """
    Main program for cvs-diff script.
    """
    opts, args = parse_options()

    # run cvs diff and read its output.

    cmd = 'cvs diff -u ' + ' '.join(map(lambda x: '"%s"' % x, args))
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=STDOUT)
    stdout, stderr = p.communicate()
    if p.returncode != 0 and not stdout:
        raise SystemExit("Error: running cvs command (%s): %s" % (s, cmd))
    chunks = xxdiff.patches.splitpatch(stdout)

    #
    # For each subpatch, apply it individually
    #
    for filename, patch in chunks:
        ## print '*' * 80
        ## print fn
        ## print text

        # print patch contents for this file.
        print '*' * 40
        print patch
        print '*' * 40

        # Feed diffs to patch, patch will do its deed and save the output to a
        # temporary file.
        tmpf = NamedTemporaryFile(prefix=tmpprefix)
        cin, cout = os.popen2(
            'patch --reverse --output "%s"' % tmpf.name, 'rw')
        cin.write('Index: %s\n' % filename)
        cin.write(patch)
        # avoid "patch unexpectedly ends in middle of line" warning.
        if patch[-1] != '\n':
            cin.write('\n')
        cin.close()

        # read output from patch.
        print cout.read()

        if not opts.commit:
            # simply invoke xxdiff on the files.
            xxdiff.invoke.xxdiff_display(opts, tmpf.name, filename)
        else:
            decision, mergedf, retcode = xxdiff.invoke.xxdiff_decision(
                opts, '--title2', 'NEW FILE', tmpf.name, filename)

            # print output of xxdiff command.
            print decision

            # if the user merged, copy the merged file over the original.
            if decision == 'MERGED':
                # save a backup, in case.
                shutil.copyfile(filename, "%s.bak" % filename)
                shutil.copyfile(tmpf2.name, filename)

            if decision == 'ACCEPT' or decision == 'MERGED':
                # the user accepted, commit the file to CVS.
                xxdiff.checkout.commit(filename)

            elif decision == 'REJECT' or decision == 'NODECISION':
                pass # do nothing
            else:
                raise SystemExit(
                        "Error: unexpected answer from xxdiff: %s" % decision)


#-------------------------------------------------------------------------------
#
main = cvsdiff_main

if __name__ == '__main__':
    main()


# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xx-patch [<options>] [<patchfile>]

This simple script splits the input patch for the individual files it
represents, applies the individual patches to temporary files and for each,
spawns an xxdiff for it.  This effectively allows you merge the patch
interactively.  You can accept or reject a patch for each file.  You can also
merge the patch interactively with xxdiff, and select for the merged results to
be applied on the output file.

If no <patchfile> is specified, xx-patch attempts to read the patch from
stdin.

Important: the new (patched) file will always be shown on the right.  The
patched file will display (patched) in its filename title.  When a decision is
asked to the user, ACCEPT always means keep/save the file that appears on the
right.  The script takes the following actions upon the answer:

- ACCEPT: keep/copy the file on the right side to the output file;
- REJECT: keep/copy the file on the left side to the output file;
- MERGED: copy the merged file on the output file;

If a patch for a file is accepted or merged, before being overwritten, a copy of
the original files are saved with the extension .orig, like patch itself.

Note: notice that the --reverse and --invert options below are implementing
orthogonal behaviour.  It can be confusing to understand the differences
between these two.  If the a reverse patch is indeed reverting changes (and
not constructed in the incorrect order), applying the patch will revert the
changes as well (the display order is not changed).  This behaviour is
exactly like that of patch.  Using invert, however, changes the logic of
what this script does upon accepting or rejecting a patch.  In any case, you
will always find the 'new file if accepted' on the right side and the
patched file is indicated with a string in the filename title.

Examples
--------

  To apply a patch, simply cd into the root of the directory where the patch
  should be applied, and invoke like this:

     cat patchfile | xx-patch

  To preview the changes that have been made in a cvs checkout area, use the
  following command::

     cvs diff | xx-patch --reverse --invert --preview

  (For better cvs-specific behaviour about committing changes, see script
  xx-cvs-diff.)

.. important::

   This script is unfinished, we still need to implement file addition/removal
   support.

"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4', 'diffutils']

# stdlib imports.
import sys, os
import shutil
from tempfile import NamedTemporaryFile

# xxdiff imports
from xxdiff.scripts import tmpprefix
import xxdiff.patches
import xxdiff.invoke
import xxdiff.backup


def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    xxmodules = xxdiff.backup, xxdiff.invoke
    for mod in xxmodules:
        mod.options_graft(parser)

    parser.add_option('-p', '--patch-options', action='store', metavar="OPTS",
                      default='',
                      help="specifies additional options to pass on to patch.")

    parser.add_option('-s', '--strip', action='store', type='int',
                      help="strip option to patch, provided for convenience.")

    parser.add_option('-R', '--reverse', action='store_true',
                      help="""apply the patch as a reverse patch (see
                      patch(1) option --reverse).  The patched file will be
                      shown as the new file regardless.""")

    parser.add_option('-I', '--invert', action='store_true',
                      help="""change the order of the displayed files, so that
                      the patched file shows as the old file.  Accepting a
                      change will thus do nothing and rejecting a change
                      will copy the patched file over.""")

    parser.add_option('-n', '--dry-run', '--preview', action='store_true',
                      help="""don't apply the patch, just show the
                      differences (you don't have to make any decisions).""")

    xxdiff.scripts.install_autocomplete(parser)
    opts, args = parser.parse_args()

    for mod in xxmodules:
        mod.options_validate(opts, parser, logs=sys.stdout)

    return opts, args


def patch_main():
    """
    Main program for patch script.
    """
    opts, args = parse_options()
    
    #
    # read the input file.
    #
    if not args:
        args = ['-']

    chunks = []
    for fn in args:
        try:
            if fn == '-':
                iif = sys.stdin
            else:
                iif = open(fn, 'r')
            text = iif.read()
            iif.close()
        except IOError, e:
            raise SystemExit(
                "Error: reading patchfile %s: %s" % (fn, str(e)))

        chunks += xxdiff.patches.splitpatch(text)

    #
    # For each subpatch, apply it individually
    #
    for filename, patch in chunks:
        ## print '*' * 80
        ## print fn
        ## print text

        # print patch contents for this file.
        print
        print
        print "FILE:", filename
        print '*' * 40
        print patch
        print '*' * 40

        # feed diffs to patch, patch will do its deed and save the output to
        # a temporary file.
        tmpfp = NamedTemporaryFile(prefix=tmpprefix)

        popts = opts.patch_options
        popts += ['--reverse', ''][ opts.reverse == None ]
        if opts.strip:
            popts += '--strip=%d' % opts.strip
        cmd = 'patch %s --output "%s"' % (popts, tmpfp.name)
        cin, cout = os.popen2(cmd, 'rw')
        cin.write('Index: %s\n' % filename)
        cin.write(patch)
        cin.close()

        # read output from patch.
        print cout.read()
        if cout.close() != None:
            print >> sys.stderr, "Error: running patch."

        #
        # spawn xxdiff
        #

        # compute stripped filename
        sfilename = filename
        if opts.strip:
            sfilename = os.sep.join(sfilename.split(os.sep)[opts.strip:])

        pno = 2
        leftfn, rightfn = sfilename, tmpfp.name

###FIXME not finished have to deal with deleted and added files

        # swap files if requested
        if opts.invert:
            leftfn, rightfn = rightfn, leftfn
            pno = 1

        # Invoke xxdiff.
        decision, mergedf, retcode = xxdiff.invoke.xxdiff_decision(
            opts, '--title%d' % pno, '%s (patched)' % sfilename,
            leftfn, rightfn)

        # print output of xxdiff command.
        if decision:
            print decision

        # perform the requested action
        if decision and not opts.dry_run:

            if decision == 'ACCEPT':
                if not opts.invert:
                    xxdiff.backup.backup_file(sfilename, opts)
                    shutil.copyfile(rightfn, sfilename)
                else:
                    assert rightfn == sfilename

            elif decision == 'REJECT':
                if not opts.invert:
                    xxdiff.backup.backup_file(sfilename, opts)
                    shutil.copyfile(leftfn, sfilename)
                else:
                    assert leftfn == sfilename

            elif decision == 'MERGED':
                xxdiff.backup.backup_file(sfilename, opts)
                shutil.copyfile(tmpfp.name, sfilename)

            elif decision == 'NODECISION':
                pass # do nothing

            else:
                raise SystemExit(
                        "Error: unexpected answer from xxdiff: %s" % decision)


def main():
    xxdiff.scripts.interruptible_main(patch_main)

if __name__ == '__main__':
    main()


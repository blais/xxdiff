# This file is part of the xxdiff package.  See xxdiff for license and details.

"""xx-diff-proxy [<options>] <file> <file> [<file>]

Simple wrapper script for xxdiff that presents options that are the same as the
diff or diff3 commands.  This can be used with programs that expect to call
diff/diff3 to present differences to users, and that can be configured with a
different diff program.

This was originally created for integration with Subversion.  This is meant to
be used in a user's configuration file to set diff-cmd and diff3-cmd options, so
that when you run ``svn diff`` it spawns xxdiffs instead of textual diffs (see
also xx-svn-diff).

Add the following lines ``~/.subversion/config``::

  [helpers]
  diff-cmd = xx-diff-proxy
  diff3-cmd = xx-diff-proxy

This script returns the same value as the underlying xxdiff with option
--exit-with-merge-status, so that you can do something special if there are
remaining conflicts.  I think Subversion uses that on 'svn update' if you
configure it in your helper hooks.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4']


# stdlib imports.
import sys, os

# xxdiff imports.
import xxdiff.scripts
import xxdiff.invoke


def parse_options():
    """
    Parse the options.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    xxdiff.invoke.options_graft(parser)

    parser.add_option('--decision', action='store_true',
                      help="Use decision mode for the diffed files.  This "
                      "allows you to save merged files over the originals.")


    # For diff-cmd invocation.
    parser.add_option('-u', action='store_true',
                      help="See diff(1). Ignored for xxdiff.")

    parser.add_option('-L', '--label', action='append', default=[],
                      help="See diff(1). Label gets set as file title string "
                      "for xxdiff.")

    # For diff3-cmd invocation.
    parser.add_option('-E', '--show-overlap', action='store_true',
                      help="See diff3(1). Ignored for xxdiff.")

    parser.add_option('-m', '--merge', action='store_true',
                      help="See diff3(1). Passed to xxdiff for automatic "
                      "selection of non-conflictual regions..")

    ## parser.add_option('--reverse', action='store_true',
    ##                   help="Reverse the display order of the files.")

    xxdiff.scripts.install_autocomplete(parser)
    opts, args = parser.parse_args()

    if len(args) > 3:
        raise parser.error("Cannot invoke wrapper with more than 3 files.")

    xxdiff.invoke.options_validate(opts, parser)

    return opts, args


def diffproxy_main():
    """
    Main program for cond-replace script.
    """
    opts, args = parse_options()

    dargs = ['--exit-with-merge-status']
    if opts.merge:
        dargs.append('--merge')

    for o1, o2, fn in zip(['--title1', '--title2', '--title3'],
                          ['"%s"' % x for x in opts.label],
                          args):
        if os.path.basename(fn) == 'empty-file':
            o2 = '"(NON-EXISTANT FILE)"'

        dargs.extend([o1, o2])

    dargs.extend(args)

    # Decision xxdiff: if we force it or if we have 3 arguments.
    if opts.decision or len(args) == 3:
        decision, mergedf, retcode = xxdiff.invoke.xxdiff_decision(opts, *dargs)

        # If the user merged, copy the merged file over the original.
        if decision in ('MERGED', 'ACCEPT'):
            mergedf.flush()
            mergedf.seek(0)
            text = mergedf.read()
            mergedf.close()
            sys.stdout.write(text)
    
        elif decision in ('NODECISION', 'REJECT'):
            pass # do nothing
        else:
            raise SystemExit(
                    "Error: unexpected answer from xxdiff: %s" % decision)

    # Just spawn xxdiff and discard the results.
    else:
        retcode = xxdiff.invoke.xxdiff_display(opts, *dargs)
        
    # Process return code
    if retcode not in [0, 3]:
        raise SystemExit('Warning: Unexpected xxdiff exit code: %d' % retcode)

    return retcode


main = diffproxy_main

if __name__ == '__main__':
    main()



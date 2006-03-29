#!/usr/bin/env python
#******************************************************************************\
#* Copyright (C) 2003-2006 Martin Blais <blais@furius.ca>
#*
#* This program is free software; you can redistribute it and/or modify
#* it under the terms of the GNU General Public License as published by
#* the Free Software Foundation; either version 2 of the License, or
#* (at your option) any later version.
#*
#* This program is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU General Public License for more details.
#*
#* You should have received a copy of the GNU General Public License
#* along with this program; if not, write to the Free Software
#* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#*
#*****************************************************************************/

"""xxdiff-patch [<options>] [<patchfile>]

This simple script splits the input patch for the individual files it
represents, applies the individual patches to temporary files and for each,
spawns an xxdiff for it.  This effectively allows you merge the patch
interactively.  You can accept or reject a patch for each file.  You can also
merge the patch interactively with xxdiff, and select for the merged results to
be applied on the output file.

If no <patchfile> is specified, xxdiff-patch attempts to read the patch from
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

     cat patchfile | xxdiff-patch

  To preview the changes that have been made in a cvs checkout area, use the
  following command::

     cvs diff | xxdiff-patch --reverse --invert --preview

  (For better cvs-specific behaviour about committing changes, see script
  xxdiff-cvs-diff.)

.. important::

   This script is unfinished, we still need to implement file addition/removal
   support.

"""

__title__ = 'xxdiff-patch'
__version__ = "$Revision: 766 $"
__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.3', 'diffutils (patch)']
__copyright__ = """Copyright (C) 2003-2004 Martin Blais <blais@furius.ca>.
This code is distributed under the terms of the GNU General Public License."""

import sys
print >> sys.stderr, \
    "Error: FIXME not finished have yet to deal with deleted and added files"


# stdlib imports.
import sys, os, os.path
import re
import commands, shutil
from tempfile import NamedTemporaryFile


tmppfx = '%s.' % os.path.basename(sys.argv[0])

#-------------------------------------------------------------------------------
#
def splitpatch( text ):

    """Split output in chunks starting with ^Index.  Returns a list of pairs
    (tuples), each with (filename, patch) contents."""

    ## splitre = re.compile('^Index: (.*)$', re.M)
    splitre = re.compile('^diff (.*)\s+(\S+)\s+(\S+)\s*$', re.M)
    chunks = []
    curbeg, curfn = None, None
    for mo in splitre.finditer(text):
        if curbeg != None:
            assert curfn
            chunks.append( (curfn, text[curbeg:mo.start()]) )
        curbeg = mo.start()
        curfn = mo.groups()[-1]
    if curbeg != None:
        chunks.append( (curfn, text[curbeg:]) )

    return chunks

#-------------------------------------------------------------------------------
#
def complete( parser ):
    "Programmable completion support. Script should work without it."
    try:
        import optcomplete
        optcomplete.autocomplete(parser)
    except ImportError:
        pass


#-------------------------------------------------------------------------------
#
def main():
    import optparse
    parser = optparse.OptionParser(__doc__.strip(), version=__version__)

    parser.add_option('-x', '--xxdiff-options', action='store', metavar="OPTS",
                      default='',
                      help="specifies additional options to pass on to xxdiff.")

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

    parser.add_option('-B', '--no-backup', action='store_true',
                      help="disable backup filesa")

    complete(parser)
    opts, args = parser.parse_args()

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

        chunks += splitpatch(text)

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
        tmpfp = NamedTemporaryFile(prefix=tmppfx)

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

        # calculate xxdiff options
        dopts = ''
        if not opts.dry_run:
            # create temporary file to hold merged results.
            tmpfm = NamedTemporaryFile('w', prefix=tmppfx)
            dopts += '--decision --merged-filename "%s" ' % tmpfm.name

        dopts += '--title%d="%s (patched)"' % (pno, sfilename)
        dopts += opts.xxdiff_options
        cmd = 'xxdiff %s "%s" "%s"' % (dopts, leftfn, rightfn)

        try:
            s, o = commands.getstatusoutput(cmd)
        except KeyboardInterrupt:
            raise SystemExit("Interrupted.")

        # print output of xxdiff command.
        if o: print o

        # perform the requested action
        if o and not opts.dry_run:

            if o == 'ACCEPT':
                if not opts.invert:
                    if not opts.no_backup:
                        shutil.copyfile(sfilename, "%s.bak" % sfilename)
                    shutil.copyfile(rightfn, sfilename)
                else:
                    assert rightfn == sfilename

            elif o == 'REJECT':
                if not opts.invert:
                    if not opts.no_backup:
                        shutil.copyfile(sfilename, "%s.bak" % sfilename)
                    shutil.copyfile(leftfn, sfilename)
                else:
                    assert leftfn == sfilename

            elif o == 'MERGED':
                if not opts.no_backup:
                    shutil.copyfile(sfilename, "%s.bak" % sfilename)
                shutil.copyfile(tmpfm.name, sfilename)

            elif o == 'NODECISION':
                pass # do nothing

            else:
                raise SystemExit(
                        "Error: unexpected answer from xxdiff: %s" % o)

if __name__ == '__main__':
    main()


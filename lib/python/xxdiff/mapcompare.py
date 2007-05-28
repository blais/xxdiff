# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to help creating files to compare key/value pairs/triples using
xxdiff.  The code in this module generates files in a way that will diff well,
lining up corresponding items.  It does this by inserting unique but matching
garbage patterns between the key/values.

This is used, for example, to compare objects in SQL schemas from databases.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import os, optparse, tempfile, random, string
from os.path import *

# xxdiff imports.
from xxdiff.scripts import tmpprefix


def options_graft(parser):
    """
    Graft options on given parser for diff maps.
    """
    group = optparse.OptionGroup(parser, "Diff Maps Options",
                                 "These options affect diff maps generation.")

    group.add_option('-g', '--garbage-lines', action='store', type='int',
                     metavar='NBLINES', default=3,
                     help="Specifies the number of garbage lines to use to "
                     "separate the map items.")

    group.add_option('--show-common-only', '--common', action='store_true',
                     help="Only render the keys that are in common in all "
                     "the maps.")

    parser.add_option_group(group)

    return group


def options_validate(opts, parser, logs=None):
    """
    Validate backup options.
    """


def render_diffable_maps(opts, *maps):
    """
    Render the given maps in respective files in a way that they will diff
    nicely to be able to compare corresponding keys side-by-side.  We return the
    temporary files that were created in the process (we return the same number
    of files as the number of given maps).

    Note: random markers may be inserted to improve the quality of the diff.
    """
    # Produce two files that will diff nicely.
    tmpfiles = [tempfile.NamedTemporaryFile('w', prefix=tmpprefix)
                for x in maps]
    
    mapkeys = set()
    for x in maps: mapkeys.update(x.keys())
    mapkeys = sorted(mapkeys)
    for key in mapkeys:
        # Check if some of the keys are missing.  If so, skip this entry.
        if opts.show_common_only:
            if False in [x.has_key(key) for x in maps]:
                continue

        # Generate random garbage that will match up the entries when diffed.
        ranlines = (['=== %s\n' % str(key)] + 
                    ['===[%s] \n' % ''.join(random.choice(string.letters)
                                            for x in xrange(70))
                     for x in xrange(opts.garbage_lines)])

        # Write the output to the files.
        for dmap, tmpf in zip(maps, tmpfiles):
            tmpf.write('\n')
            for line in ranlines:
                tmpf.write(line)
            tmpf.write('\n')
            try:
                tmpf.write(dmap[key])
                tmpf.write('\n\n')
            except KeyError:
                pass

    # Make sure all the files are flushed
    [f.flush() for f in tmpfiles]

    return tmpfiles


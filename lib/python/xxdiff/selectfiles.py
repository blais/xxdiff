#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to select files within directories.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import os, re, optparse
from os.path import join


#-------------------------------------------------------------------------------
#
def options_graft( parser ):
    """
    Adds a new option group for file selection options to the given parser and
    returns this new group.
    """
    group = optparse.OptionGroup(parser, "File selection options",
                                 "These options affect which files are "
                                 "selected for grepping in the first place.")

    def regexp_cb( option, opt_str, value, parser ):
        "Compile and set the regular expression"
        try:
            regexp = re.compile(value)
            getattr(parser.values, option.dest).append(regexp)
        except re.error, e:
            raise optparse.OptionValueError(
                "compiling regular expression '%s': %s" % (value, e))

    for short, long, help in (
        ('-s', '--select',
         "Adds a regular expression for filenames to process."),
        ('-I', '--ignore',
         "Adds a regular expression for filenames to ignore."),
        ):
        group.add_option(short, long, 
                         action='callback', type='str', callback=regexp_cb,
                         metavar="REGEXP", default=[],
                         help=help)

    # Note: we could improve this as soon as Python will include a more recent
    # version of Optik, which implements the action 'append_const'.
    def append_const_cb( const ):
        "Create callback to implement an append_const action via a callback."
        def cb( option, opt_str, value, parser ):
            getattr(parser.values, option.dest).append(const)
        return cb

    for fname, ftype, fregexp in (('C++', 'cpp', '.*\.(h(pp)?|c(pp|\+\+|c)?)$'),
                                  ('Python', 'py', '.*\.py$')):

        group.add_option('--%s' % ftype, '--select-%s' % ftype, dest='select',
            action='callback', callback=append_const_cb(re.compile(fregexp)),
            help=("Adds a regular expression for selecting %s files to "
                  "match against.") % fname)

    group.add_option('-f', '--select-from-file', action='store',
                     metavar='FILE',
                     help="Do not run find but instead use the list of files "
                     "in the specified filename.")

    parser.add_option_group(group)

    return group


def options_validate( opts, rootdirs ):
    """
    Validate and prepare the parsed options for the file selection option group.
    This method returns an appropriate generator for selecting files.

    'rootdirs': a list of root directories.
    """

    # Process file selection options

    if opts.select_from_file:
        if opts.select or opts.ignore:
            parser.error("You cannot use select-from-file and other "
                         "select options together.")
        if rootdirs:
            parser.error("You cannot use select-from-file and specify "
                         "a list of root directories to search for.")
        # Note: eventually we might want to support chaining the generators
        # instead.
    else:
        # Process all files if no filter specified
        if not opts.select:
            opts.select.append(re.compile('.*'))

    # Create an appropriate generator for the "select" method
    if opts.select_from_file:
        selector = select_from_file(opts.select_from_file)
    else:
        selector = select_patterns(rootdirs, opts.select, opts.ignore)

    return selector


#-------------------------------------------------------------------------------
#
def select_patterns( rootdirs, select, ignore ):
    """
    Generator that selects files to process by regexps.

    Arguments:
    - rootdirs: root directories -> list of strings
    - select: patterns to select -> list of re regexp objects
    - ignore: patterns to ignore -> list of re regexp objects

    Yields: selected filenames -> string
    """
    # Walk the tree of files and select files as requested
    for root in rootdirs:
        for dn, dirs, files in os.walk(root):
            # Filter directories.
            for d in list(dirs):
                add = True
                # Only ignore applies to directories.
                for ire in ignore:
                    if ire.match(d):
                        add = False
                        break
                if not add:
                    dirs.remove(d)

            # Filter files.
            for fn in files:
                add = False
                for sre in select:
                    if sre.match(fn):
                        add = True
                        break
                for ire in ignore:
                    if ire.match(fn):
                        add = False
                        break
                if add:
                    yield join(dn, fn)

#-------------------------------------------------------------------------------
#
def select_from_file( fn ):
    """
    Generator that yields selected filenames read from a file.

    Arguments:
    - fn: filename of the file that contains the list of filenames -> string

    Yields: selected filenames -> string
    """
    try:
        for fn in open(fn, 'r').xreadlines():
            yield fn.strip()
    except IOError, e:
        raise SystemExit("Error: cannot open/read file (%s)" % e)


#-------------------------------------------------------------------------------
#
def test():
    """
    Test stub for selection options.
    """
    parser = optparse.OptionParser(__doc__.strip())
    options_graft(parser)
    opts, args = parser.parse_args()

    selector = options_validate(opts, args)

    print 'from-file:', opts.select_from_file
    print 'select:', map(lambda x: x.pattern, opts.select)
    print 'ignore:', map(lambda x: x.pattern, opts.ignore)
    print 'roots:', args
    print

    for fn in selector:
        print fn

        
if __name__ == '__main__':
    test()


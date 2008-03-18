# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to select files within directories.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import sys, os, re, optparse
from os.path import join, isfile, exists


def options_graft(parser):
    """
    Adds a new option group for file selection options to the given parser and
    returns this new group.
    """
    group = optparse.OptionGroup(parser, "File selection options",
                                 "These options affect which files are "
                                 "selected for grepping in the first place.")

    def regexp_cb(option, opt_str, value, parser):
        "Compile and set the regular expression"
        try:
            regexp = re.compile(value)
            getattr(parser.values, option.dest).append(regexp)
        except re.error, e:
            raise optparse.OptionValueError(
                "compiling regular expression '%s': %s" % (value, e))

    for oshort, olong, ohelp in (
        ('-s', '--select',
         "Adds a regular expression for filenames to process."),
        ('-I', '--ignore',
         "Adds a regular expression for filenames to ignore."),
        ):
        group.add_option(oshort, olong, 
                         action='callback', type='str', callback=regexp_cb,
                         metavar="REGEXP", default=[],
                         help=ohelp)

    # Note: we could improve this as soon as Python will include a more recent
    # version of Optik, which implements the action 'append_const'.
    def append_const_cb(const):
        "Create callback to implement an append_const action via a callback."
        def cb(option, opt_str, value, parser):
            getattr(parser.values, option.dest).append(const)
        return cb

    for fname, ftype, fregexp in (('C++', 'cpp', '.*\.(h(pp)?|c(pp|\+\+|c)?)$'),
                                  ('Python', 'py', '.*\.py$')):

        group.add_option('--%s' % ftype, '--select-%s' % ftype, dest='select',
            action='callback', callback=append_const_cb(re.compile(fregexp)),
            default=[],
            help=("Adds a regular expression for selecting %s files to "
                  "match against.") % fname)

    group.add_option('--select-grep', action='store',
                     metavar='REGEXP',
                     help="Further restrict the files to those which match "
                     "the given regular expression.")

    group.add_option('--ignore-grep', action='store',
                     metavar='REGEXP',
                     help="Do not select files to those which match "
                     "the given regular expression.")

    group.add_option('-f', '--select-from-file', action='store',
                     metavar='FILE',
                     help="Do not recurse through directories to find files "
                     "but instead read the list of filenames from the "
                     "given file.")

    group.add_option('-r', '--root', action='append', dest='roots',
                     default=[], metavar='ROOT',
                     help="Specify a root to perform the search from "
                     "(default is CWD).  You can use this option many times"
                     "for multiple roots.")

    # This is a convenient option rather than making a test script, because we
    # don't pollute the bin directory with a script that does not do much, and
    # the option is available in all the scripts that use the select files
    # proces.
    group.add_option('-T', '-@', '--select-debug', action='store_true',
                     help="Only list the files and exit.  This is used to "
                     "debug and test out which files will match your "
                     "replacement process when you run it.")

    group.add_option('-N', '--select-no-defaults', action='store_true',
                     help="Do not setup default ignores (i.e. by default, "
                     "appropriate ignore patterns are set for Subversion "
                     "and CVS directories, etc.)")
    
    parser.add_option_group(group)

    return group


ignore_defaults = ('CVS', '.svn', '.hg')

def options_validate(opts, parser, logs=None):
    """
    Validate and prepare the parsed options for the file selection option group.
    This method returns an appropriate generator for selecting files.
    """
    # Process file selection options
    if opts.select_from_file:
        if opts.select or opts.ignore:
            parser.error("You cannot use select-from-file and other "
                         "select options together.")
        if opts.roots:
            parser.error("You cannot use select-from-file and specify "
                         "a list of root directories to search for.")
        # Note: eventually we might want to support chaining the generators
        # instead.
    else:
        # If no root directory has been specified, use the CWD.
        if not opts.roots:
            opts.roots.append(os.getcwd())
        else:
            missing = filter(lambda x: not exists(x), opts.roots)
            if missing:
                parser.error("Some root directories do not exist: %s" %
                             ' '.join(missing))

        # By default ignore some common directories.
        if not opts.select_no_defaults:
            opts.ignore = map(re.compile, ignore_defaults) + opts.ignore

        # Process all files if no filter specified
        if not opts.select:
            opts.select.append(re.compile('.*'))

    # Compile the regular expressions if given.
    for regname in 'select_grep', 'ignore_grep':
        regstr = getattr(opts, regname)
        if regstr:
            try:
                setattr(opts, regname, re.compile(regstr))
            except re.error, e:
                parser.error("Error compiling regexp '%s':\n%s" % (regstr, e))

    # Create an appropriate generator for the "select" method
    if opts.select_from_file:
        selector = select_from_file(opts.select_from_file)
    else:
        selector = select_patterns(opts.roots, opts)

    # If we're asked to print the list of files, exhaust the generator and print
    # the filenames and exit.
    if opts.select_debug:
        for fn in selector:
            print fn
        sys.exit(0)

    return selector


def select_patterns(rootdirs, opts):
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
                for ire in opts.ignore:
                    if ire.match(d):
                        add = False
                        break
                if not add:
                    dirs.remove(d)

            # Filter files.
            for fn in files:
                # Skip symlinks and special files.
                if not isfile(join(dn, fn)):
                    continue

                add = False

                # Select.
                for sre in opts.select:
                    if sre.match(fn):
                        add = True
                        break
                
                # Further restrict by grepping the file for a pattern.
                if add and opts.select_grep:
                    try:
                        text = open(join(dn, fn), 'r').read()
                    except IOError, e:
                        raise SystemExit(
                            "Error: could not read file '%s' for grep." % fn)
                    if not opts.select_grep.search(text):
                        add = False

                # Ignore files.
                for ire in opts.ignore:
                    if ire.match(fn):
                        add = False
                        break

                # Further ignore by grepping the file for a pattern.
                if add and opts.ignore_grep:
                    try:
                        text = open(join(dn, fn), 'r').read()
                    except IOError, e:
                        raise SystemExit(
                            "Error: could not read file for ignore '%s'." % fn)
                    if opts.ignore_grep.search(text):
                        add = False

                if add:
                    yield join(dn, fn)

def select_from_file(fn):
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


def test():
    """
    Test stub for selection options.
    """
    parser = optparse.OptionParser(__doc__.strip())
    options_graft(parser)
    opts, args = parser.parse_args()

    selector = options_validate(opts, parser)

    print 'from-file:', opts.select_from_file
    print 'select:', map(lambda x: x.pattern, opts.select)
    print 'ignore:', map(lambda x: x.pattern, opts.ignore)
    print 'roots:', args
    print

    for fn in selector:
        print fn

        
if __name__ == '__main__':
    test()


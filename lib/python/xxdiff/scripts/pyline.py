# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
xx-pyline [<options>] <python-expr> [<python-expr> ...]

Run the given Python code on each line of the input.  This is a bit like sed,
except that it's just Python code.

  Inspired by Graham Fawcett's Python Cookbook recipe:
  http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/437932

    'This utility was born from the fact that I keep forgetting how to use
    sed, and I suck at Perl. It brings ad-hoc command-line piping
    sensibilities to the Python interpeter.'

This program walks a directory hierarchy, selects some files to be processed by
filenames (or by default, process all files found), then the given action is
performed on the selected file, compared, if some changes were carried out, a
graphical diff is presented to ask the user whether he wants to apply the
changes or not.
"""

__author__ = "Martin Blais <blais@furius.ca>"
__depends__ = ['xxdiff', 'Python-2.4']


# stdlib imports.
import sys, os, traceback
from os.path import *

# xxdiff imports.
import xxdiff.scripts
import xxdiff.xformloop


class PylineTransformer(xxdiff.xformloop.Transformer):
    """
    Transformer that greps the file for a regular expression, and if it matches,
    runs it through a sed command.
    """
    def __init__(self, opts, expr):
        xxdiff.xformloop.Transformer.__init__(self, opts)

        # Try to compile the given expression.
        try:
            self.codeobj = compile(expr, 'command', 'eval')
        except Exception:
            raise SystemExit("Error: could not compile expression:\n%s" %
                             traceback.format_exc(limit=0))

    def transform(self, fn, outf):
        # Open and read input file in memory.
        try:
            inf = file(fn, 'r')
        except IOError, e:
            raise SystemExit("Error: Could not read file '%s':\n  %s" % (fn, e))

        # Run the code on each line.
        modified = False
        for numz, line in enumerate(inf):
            line = line[:-1]
            num = numz + 1
            words = [w for w in line.strip().split(' ') if len(w)]
            try:
                result = eval(self.codeobj, globals(), locals())
            except Exception:
                raise SystemExit("Error: running expression on line %d:\n%s\n%s"
                                 % (numz, line, traceback.format_exc(limit=0)))
                
            if result is None or result is False:
                modified = True
                continue
            elif isinstance(result, list) or isinstance(result, tuple):
                result = ' '.join(map(str, result))
            else:
                result = str(result)

            if not modified and line != result:
                modified = True

            outf.write(result)
            if not result.endswith('\n'):
                outf.write('\n')

        return modified


def parse_options():
    import optparse
    parser = optparse.OptionParser(__doc__.strip())

    parser.add_option('-m', '--import-lib', action='append',
                      default=[],
                      help="Import the given module before running the script.")

    opts, args, selector = xxdiff.xformloop.parse_args(parser)

    # Check that we got two arguments
    expr = ' '.join(args)
    if not expr.strip():
        expr = 'line'  # no-op

    # Force to always perform a diff on output.
    opts.verbose = 2

    # Import the necessary libraries.
    for libname in opts.import_lib:
        try:
            globals()[libname] = __import__(libname.strip())
        except ImportError, e:
            parser.error("Importing module '%s': %s" % (libname, e))
        
    return expr, opts, selector


def pyline_main():
    """
    Main program for pyline script.
    """
    # Parse the options.
    expr, opts, selector = parse_options()

    # Create an appropriate transformer.
    xformer = PylineTransformer(opts, expr)

    try:
        decision_map = xxdiff.xformloop.transform_replace_loop(
            opts, selector, xformer, sys.stdout)
    finally:
        xxdiff.xformloop.postloop_footer(opts)

def main():
    xxdiff.scripts.interruptible_main(pyline_main)

if __name__ == '__main__':
    main()


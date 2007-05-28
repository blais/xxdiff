# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to checkout files from various SCM systems before writing to them.
ClearCase requires that, and there is a configuration option in CVS that
requires 'cvs edit' too.  This is where the director code goes.  Code for
specific SCM systems is located under in the xxdiff.scm package.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import optparse


checkout_choices = ('clearcase',)

def options_graft(parser):
    """
    Graft options on given parser for checkout options.
    """
    group = optparse.OptionGroup(parser, "SCM options",
                                 "These options are related to source code "
                                 "management systems you may be using.")

    group.add_option('-C', '--checkout', action='store',
                     type='choice', choices=checkout_choices,
                     metavar="NAME",
                     help="Checkout files from an SCM before overwriting. "
                     "Choices include: %s" % ', '.join(checkout_choices))

    parser.add_option_group(group)

    return group

def options_validate(opts, parser, logs=None):
    """
    Validate checkout options.
    """
    pass # Nothing to do.


def get_module(modname):
    """
    Returns the appropriate module for the given SCM.
    """
    try:
        mod = __import__("xxdiff.scm.%s" % modname)
        mod = getattr(mod.scm, modname)
    except ImportError, e:
        raise SystemExit("Internal Error: SCM module not found.")
    return mod

def insure_checkout(fn, opts, logs=None):
    """
    Check the file out from the SCM, if necessary.
    Returns 'True' if the file was checked out.
    """
    scm_module = get_module(opts.checkout)
    
    # Verify if the file is already checked out.
    if scm_module.is_checked_out(fn):
        return False

    # Checkout the file.
    if logs:
        logs.write("Checking out file '%s'" % fn)

    scm_module.checkout(fn)
    return True


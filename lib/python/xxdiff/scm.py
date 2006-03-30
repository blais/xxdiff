#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to checkout files from various SCM systems before writing to them.
ClearCase requires that, and there is a configuration option in CVS that
requires 'cvs edit' too.  This is where all that code goes.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import subprocess, optparse


#-------------------------------------------------------------------------------
#
checkout_choices = ('clearcase',)

def options_graft( parser ):
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

def options_validate( opts ):
    """
    Validate checkout options.
    """
    pass # Nothing to do.


#-------------------------------------------------------------------------------
#
def insure_checkout( fn, opts, logs=None ):
    """
    Check the file out from the SCM, if necessary.
    Returns 'True' if the file was checked out.
    """
    if opts.checkout is None:
        return
    
    # Verify if the file needs to be checked out.
    if opts.checkout == 'clearcase':
        # FIXME: TODO -- implement this.
        # ret = subprocess.call(['cleartool', 'desc', fn])
        pass

    # Checkout the file.
    if logs:
        logs.write("Checking out file '%s'" % fn)

    if opts.checkout == 'clearcase':
        ret = subprocess.call(['cleartool', 'co', '-nc', fn])

        # FIXME: TODO -- check the return value.
        # if ret != 0:
        #     raise RuntimeError

        # Note: I do not have access to ClearCase at this moment (2006-03-29),
        # so I cannot complete the clearcase checkout code above.  This will
        # work, though, in cases where all the files are checked-in, since it
        # will unconditionally checkout the files.

        return True

    return False


#-------------------------------------------------------------------------------
#
# Note: this is cut-n-paste from cvs.py, for simplicity of distribution.
def cvs_unmerge2( text ):
    """
    Unmerges conflicts between two files and returns the two split original
    files.  This is not meant to work with three files.
    """
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


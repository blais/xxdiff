#!/usr/bin/env python
# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions to invoke xxdiff in various ways.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import os, optparse, tempfile
from os.path import *
from subprocess import Popen, PIPE

# xxdiff imports.
from xxdiff.scripts import tmpprefix


#-------------------------------------------------------------------------------
#
def options_graft( parser ):
    """
    Graft options on given parser for invocations of xxdiff.
    """
    group = optparse.OptionGroup(parser, "Options for xxdiff subprocesses",
                                 "These options affect how xxdiff is called.")

    group.add_option('-Z', '--xxdiff-exec', action='store', metavar='XXDIFF',
                     default='xxdiff',
                     help="Specifies which xxdiff executable to use (default "
                     "is looked up in PATH.")

    group.add_option('-O', '--xxdiff-options', '--xxdiff-opts',
                     action='store', metavar="OPTS",
                     default='',
                     help="Additional options to pass on to xxdiff.")

    group.add_option('-Y', '--xxdiff-verbose', action='store_true',
                     help="Output xxdiff commands on stdout, for debugging.")

    parser.add_option_group(group)

    return group


def options_validate( opts, parser, logs=None ):
    """
    Validate xxdiff options.
    """
    pass


#-------------------------------------------------------------------------------
#
decisions = ('ACCEPT', 'REJECT', 'MERGED', 'NODECISION')

def xxdiff_decision( opts, *arguments, **kwds ):
    """
    Runs xxdiff with the given arguments, passed directly to subprocess.call(),
    and return the decision code, the temporary file containing merged output,
    and xxdiff's return code.  We create this temporary file to be used as the
    merged output to be collected and modify the options accordingly.  It is a
    named file.  If you close the merged temporary file, it is deleted
    automatically.  This file is opened in 'rw' mode, so you can rewind it and
    read it directly if desired, or you can use its filename with high-level
    operations, like file copy, as long as you keep the reference to the file.

    You should unpack its arguments like this::

       decision, mergedf, retcode = xxdiff_decision( ... )

       # use mergedf.name to copy the file or seek(0) and read() it again.

    If the decision code is 'NODECISION', the merged file is set to None (its
    contents are undefined).

    'stdin' (in kwds) can be used to pass input to the subordinate xxdiff
    process.

    If you want to perform actions asynchronously while xxdiff is waiting for
    the user's decision, you can set 'nowait' (in kwds) to True, the function
    then returns immediately after starting xxdiff, and a proxy callable object
    is returned that can be invoked to wait for the final results and returns
    the same data as the function specifies.  You use it like this::

      xxwait = xxdiff_decision( ..., nowait=1)

      # Do something...

      # Wait for the xxdiff results
      decision, merged, retcode = xxwait()

    """
    # Create a temporary file to contain the output or merged results.
    mergedf = tempfile.NamedTemporaryFile('rw', prefix=tmpprefix)

    # Get the appropriate xxdiff executable and options.
    xexec = getattr(opts, 'xxdiff_exec', None) or 'xxdiff'
    options = getattr(opts, 'xxdiff_options', None) or []

    # Make sure that xxdiff is invoked with the decision switch.
    if '--decision' not in options:
        options.insert(0, '--decision')

    assert '--merged-filename' not in (list(arguments) + options)
    alloptions = options + ['--merged-filename', mergedf.name] + list(arguments)

    # If we're not waiting, we only want to return after all the input has been
    # processed.  This is always the case.
    nowait = kwds.pop('nowait', None)
    if nowait and '--indicate-input-processed' not in alloptions:
        alloptions.insert(0, '--indicate-input-processed')

    # Run xxdiff.
    cmd = [xexec] + alloptions
    if getattr(opts, 'xxdiff_verbose', None):
        print '===', ' '.join(cmd)

    stdin_text = kwds.pop('stdin', None)
    p = Popen(cmd,
              stdout=PIPE,
              stderr=PIPE,
              stdin=(stdin_text and PIPE or None))

    # Write to stdin if necessary.
    if stdin_text:
        p.stdin.write(stdin_text)
        p.stdin.close()
    
    # Define waiter object.
    def waiter():
        # Select-wait for stdout and stderr
        stdout, stderr = p.communicate()

        # If xxdiff failed, we bail out of the script.
        if p.returncode == 2:
            raise SystemExit(
                "Error: Running xxdiff as '%s'. Aborting.\n" % ' '.join(cmd) +
                stderr)

        # Get the decision code from xxdiff.
        lines = stdout.splitlines()
        decision = lines[0].strip()
        assert decision in decisions

        if decision == 'NODECISION':
            mergedf.close()
            out_mergedf = None
        else:
            out_mergedf = mergedf

        return decision, out_mergedf, p.returncode

    # Eat up the processed tag.
    #
    # Important note: there is a subtle race condition that can cause an error
    # here, but it is rather unlikely to happen (in practice it will never
    # happen because xxdiff is faster than the user's fingers).
    if nowait and p.returncode is None:
        # Wait for the processed tag.
        line = p.stdout.readline().strip()
        if line != 'INPUT-PROCESSED':
            stdout, stderr = p.communicate()
            raise SystemExit(
                "Error: Running xxdiff as '%s'. Aborting.\n" % ' '.join(cmd) +
                stderr)
            
        return waiter

    # Wait for the results and return them.
    return waiter()


#-------------------------------------------------------------------------------
#
def xxdiff_display( opts, *arguments, **kwds ):
    """
    Runs xxdiff with the given arguments, passed directly to subprocess.call().
    We do not run it in decision mode.  We return xxdiff's own return code (the
    --exit-with-merge-status will have an effect on this).  There is a similar
    'nowait' option as xxdiff_decision() with a waiter object returned to wait
    on the child process.  Otherwise, this function will wait for the child to
    terminate.

    We do not create a merged file, but you are free to pass in these options if
    so desired.
    """
    if '--decision' in arguments:
        raise RuntimeError("Internal error: use xxdiff_decision() "
                           "instead of xxdiff_display() if you want a user "
                           "return code.")

    # Get the appropriate xxdiff executable and options.
    xexec = getattr(opts, 'xxdiff_exec', None) or 'xxdiff'
    options = getattr(opts, 'xxdiff_options', None) or []

    alloptions = options + list(arguments)

    # If we're not waiting, we only want to return after all the input has been
    # processed.  This is always the case.
    nowait = kwds.pop('nowait', None)
    if nowait and '--indicate-input-processed' not in alloptions:
        alloptions.insert(0, '--indicate-input-processed')

    # Run xxdiff.
    cmd = [xexec] + alloptions
    if getattr(opts, 'xxdiff_verbose', None):
        print '===', ' '.join(cmd)

    stdin_text = kwds.pop('stdin', None)
    p = Popen(cmd,
              stdout=PIPE,
              stderr=PIPE,
              stdin=(stdin_text and PIPE or None))

    # Write to stdin if necessary.
    if stdin_text:
        p.stdin.write(stdin_text)
        p.stdin.close()
    

    # Define waiter object.
    def waiter():
        # Select-wait for stdout and stderr
        stdout, stderr = p.communicate()

        # If xxdiff failed, we bail out of the script.
        if p.returncode == 2:
            raise SystemExit(
                "Error: Running xxdiff as '%s'. Aborting.\n" % ' '.join(cmd) +
                stderr)

        return p.returncode

    # Eat up the processed tag.
    #
    # Important note: there is a subtle race condition that can cause an error
    # here, but it is rather unlikely to happen (in practice it will never
    # happen because xxdiff is faster than the user's fingers).
    if nowait and p.returncode is None:
        # Wait for the processed tag.
        line = p.stdout.readline().strip()
        if line != 'INPUT-PROCESSED':
            stdout, stderr = p.communicate()
            raise SystemExit(
                "Error: Running xxdiff as '%s'. Aborting.\n" % ' '.join(cmd) +
                stderr)
            
        return waiter

    # Wait for the results and return them.
    return waiter()


#-------------------------------------------------------------------------------
#
def test():
    """
    Test launcher.
    """
    import optparse
    parser = optparse.OptionParser()
    opts, args = parser.parse_args()

    class Opts:
        xxdiff = 'xxdiff'
        
    f1, f2, f3 = map(lambda x: join('/home/blais/p/xxdiff/test', x),
                     ('mine', 'older', 'yours'))

    for t in args:
        if t == 'simple':
            print xxdiff_decision(Opts, f1, f2)

        elif t == 'async':
            w = xxdiff_decision(Opts, f1, f2, nowait=1)
            print 'Deleting temp input files...'
            print w()

        elif t == 'error':
            print xxdiff_decision(Opts, '--bullshit', f1, f2)

        elif t == 'error-nowait':
            print xxdiff_decision(Opts, '--bullshit', f1, f2, nowait=1)

        if t == 'display':
            print xxdiff_display(Opts, f1, f2)

        if t == 'display-nowait':
            w = xxdiff_display(Opts, f1, f2, nowait=1)
            print 'Deleting temp input files...'
            print w()

if __name__ == '__main__':
    test()


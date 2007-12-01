# This file is part of the xxdiff package.  See xxdiff for license and details.

"""
Functions for Subversion.

Note: we do not use the official Python bindings because we want to evolve
independently and not break when Subversion changes.  This only makes sense
inasmuch as we do not do much with Subversion.  Should we get do more involved
stuff with it, we should switch this code to use the Subversion Python bindings.
"""

__author__ = 'Martin Blais <blais@furius.ca>'


# stdlib imports.
import os, tempfile, re
from subprocess import Popen, call, PIPE
from os.path import abspath, isabs

# xxdiff imports.
from xxdiff.scripts import script_name, tmpprefix


def is_checked_out(filename):
    """
    There is no real global mutual editing policy in Subversion.  The restricted
    locking mechanism that is available is not meant to be automated (on the
    contrary).
    """
    return True


def checkout(filename):
    """
    Checkout the given filename from Subversion.
    """
    return


def commit(filenames, comments=None):
    """
    Commit the given filename into CVS.
    """
    if comments is None:
        comments = 'Committed by %s' % script_name

    if isinstance(filenames, (str, unicode)):
        filenames = [filenames]
    else:
        assert isinstance(filenames, (tuple, list))
        
    # Note: if the comments are ever so huge, we consider using a file with the
    # -f option to 'svn commit'.  Wondering if there may be any important issues
    # with passing large objects through the environment.  This is certainly not
    # the usual thing to do.
    if len(comments) < 512:
        call(['svn', 'commit', '--message', comments] + filenames)
    else:
        tmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)
        tmpf.write(comments)
        tmpf.flush()
        call(['svn', 'commit', '--file', tmpf.name] + filenames)

        
def resolve(filename):
    """
    Commit the given filename into CVS.
    """
    p = Popen(['svn', 'resolved', filename], stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()
    if p.returncode != 0:
        raise SystemExit("Error: Resolving file '%s':\n%s\n" %
                         (filename, stderr))

class SvnStatus(object):
    """
    Status result for a file in a subversion repository.
    """

def status(rootdirs):
    """
    Obtains the status from all the given root directories.
    A list of status objects is returned.
    """
    if isinstance(rootdirs, (str, unicode)):
        rootdirs = [rootdirs]
    else:
        assert isinstance(rootdirs, (list, tuple))
    if len(rootdirs) == 0:
        rootdirs.append(os.getcwd())

    # Note: since we specify abspaths we should get abspaths on the output
    p = Popen(['svn', 'status'] + map(abspath, rootdirs), stdout=PIPE)
    stdout, stderr = p.communicate()

    statii = []
    for line in stdout.splitlines():
        if not line or re.match('^Performing', line):
            continue

        status = SvnStatus()
        status.parsed_line = line

        # The first six columns in the output are each one character wide:
        lc = iter(line)

        #   First column: Says if item was added, deleted, or otherwise
        #   changed
        #     ' ' no modifications
        #     'A' Added
        #     'C' Conflicted
        #     'D' Deleted
        #     'G' Merged
        #     'I' Ignored
        #     'M' Modified
        #     'R' Replaced
        #     'X' item is unversioned, but is used by an externals
        #         definition
        #     '?' item is not under version control
        #     '!' item is missing (removed by non-svn command) or incomplete
        #     '~' versioned item obstructed by some item of a different kind
        status.status = lc.next()

        #   Second column: Modifications of a file's or directory's
        #   properties
        #     ' ' no modifications
        #     'C' Conflicted
        #     'M' Modified
        status.modprops = lc.next()

        #   Third column: Whether the working copy directory is locked
        #     ' ' not locked
        #     'L' locked
        status.dirlocked = lc.next()

        #   Fourth column: Scheduled commit will contain
        #   addition-with-history
        #     ' ' no history scheduled with commit
        #     '+' history scheduled with commit
        status.withhist = lc.next()

        #   Fifth column: Whether the item is switched relative to its
        #   parent
        #     ' ' normal
        #     'S' switched
        status.switched = lc.next()

        #   Sixth column: Repository lock token
        #     (without -u)
        #     ' ' no lock token
        #     'K' lock token present
        #     (with -u)
        #     ' ' not locked in repository, no lock token
        #     'K' locked in repository, lock toKen present
        #     'O' locked in repository, lock token in some Other working
        #          copy
        #     'T' locked in repository, lock token present but sTolen
        #     'B' not locked in repository, lock token present but Broken
        status.locktoken = lc.next()

        # There is a space and the rest is a filename:
        status.filename = line[7:].strip()
        assert isabs(status.filename)

        statii.append(status)


    # Note: we could parse this information, but this requires server access:
    # -----------------------------------------------------------------------
    #   The out-of-date information appears in the eighth column (with -u):
    #       '*' a newer revision exists on the server
    #       ' ' the working copy is up to date
    #
    #   Remaining fields are variable width and delimited by spaces:
    #     The working revision (with -u or -v)
    #     The last committed revision and last committed author (with -v)
    #     The working copy path is always the final field, so it can
    #       include spaces.

    return statii



        

def getinfo(filename):
    """
    Return the fields of 'svn info', in a dictionary.
    """
    p = Popen(['svn', 'info', filename], stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()
    if p.returncode != 0:
        raise SystemExit("Error: Cannot get history for: %s" % stderr)

    colon_split_re = re.compile(': ')
    d = dict([colon_split_re.split(x, 1) for x in stdout.strip().splitlines()])

    return d


def cat_revision_temp(filename, revision):
    """
    Fetches a specific revision of a file and place it in a temporary file,
    which is returned.
    """
    # Fetch the local ancestor to a temporary file.
    tmpf = tempfile.NamedTemporaryFile('w', prefix=tmpprefix)

    p = Popen(['svn', 'cat', '--revision', revision, filename],
              stdout=tmpf)
    stdout, stderr = p.communicate()
    if p.returncode != 0:
        raise SystemExit("Error: Cannot fetch BASE revision: %s" % stderr)

    return tmpf




    
def test():
    """
    Simple interactive test.
    """
    import optparse
    parser = optparse.OptionParser(__doc__.strip())
    opts, args = parser.parse_args()

    for s in status(args[0]):
        print s.filename, ':', s.status


if __name__ == '__main__':
    test()


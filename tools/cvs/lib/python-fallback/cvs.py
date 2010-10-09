#!/usr/bin/env python
#******************************************************************************\
#* $Source$
#* $Id$
#*
#* Copyright (C) 2001-2002, Martin Blais <blais@furius.ca>
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

"""Interface to CVS."""

__version__ = "$Revision$"
__author__ = "Martin Blais <blais@furius.ca>"

#===============================================================================
# EXTERNAL DECLARATIONS
#===============================================================================

import sys, os
import string, re
import commands
import fcntl, select

from pprint import pprint, pformat

#===============================================================================
# PUBLIC VARIABLES
#===============================================================================

program = 'cvs -f -z6'
revre = '[\d\.]+'
revre_re = re.compile('^%s$' % revre)
brrevre = '[\d\.]+\.0\.\d+'
brrevre_re = re.compile('([\d\.]+)\.0\.(\d+)')
attic_pfx = 'Attic' + os.sep

trace_level = 0 # 1 is cmd, 2 is output

statii_sep = 67 * '=' + os.linesep
logs_sep = 77 * '=' + os.linesep 


#===============================================================================
# PUBLIC FUNCTIONS
#===============================================================================

#===============================================================================
# CLASS Error
#===============================================================================

class Error:
    """Exception class for this module."""

    def __init__(self, msg):
        self.msg = msg
    def __repr__(self):
        return repr(self.msg)
    def __str__(self):
        return str(self.msg)

#-------------------------------------------------------------------------------
#
def getRoot(dir=None):
    """Returns the CVS root for a command run from dir."""
    if not dir:
        dir = os.getcwd()
    subdir = os.path.join(dir, 'CVS')
    if os.path.exists(subdir):
        f = open(os.path.join(subdir, 'Root'), 'r')
        root = f.readline().strip()
        f.close()
    else:
        root = os.environ.get('CVSROOT', '')
    return root

#-------------------------------------------------------------------------------
#
def rootPath(root):
    """Returns the repository path on the server for a command run from dir."""
    comps = string.split(root, ':')
    return comps[-1]

#-------------------------------------------------------------------------------
#
def run(command, chin_text=None, indir=None):
    """Run a CVS command and return (status, output, error output) of executing
    cmd in a shell. Command output is NOT bound to the terminal."""

    if indir:
        command = '{ cd "' + indir + '" && ' + command + '; }'

    if trace_level >= 1:
        print "== (cvs):", command
    
    (chin, chout, cherr) = os.popen3(command, 'r')

    if chin_text:
        chin.write(chin_text)
        chin.close()
        
    output = chout.read()
    error_output = cherr.read()
    cherr.close()
    sts = chout.close()
    if sts is None: sts = 0
    
    if trace_level >= 2:
        print "cvs (stdout) ------------------------------"
        for l in output.split('\n'):
            print "   o|", l
        print "cvs (stderr) ------------------------------"
        for l in error_output.split('\n'):
            print "   e|", l
        print "cvs ---------------------------------------"

    return sts, output, error_output


#-------------------------------------------------------------------------------
#
def delegate(command, indir=None):
    """Run a CVS command with output is bound to the terminal."""

    if indir:
        command = '{ cd "' + indir + '" && ' + command + '; }'

    if trace_level >= 1:
        print "== (cvs):", command

    # This is the simplest way for now.
    return os.system(command)


#-------------------------------------------------------------------------------
#
def fetch(filename, revision=None, indir=None, module=None):
    """Fetches a particular filename and revision from the repository and
    returns a pair of the buffer and the error output. None is returned for
    output if there was an error.

    Note that if you specify the module name, the filename must be that from the
    root of the module.
    """
    
    revstr = ''
    if revision:
        revstr = '-r "%s"' % revision

    if not module:
        command = '%s update -p %s %s' % (program, revstr, filename)
        if indir:
            command = '{ cd "' + indir + '" && ' + command + '; }'
    else:
        command = '%s checkout -p %s %s' % \
                  (program, revstr, os.path.join(module, filename))
        
    (sts, out, err) = run(command)

    # remove out as an error msg
    correctre = re.compile('^Checking out ')
    wsre = re.compile('^\s*$')
    elines = err.split('\n')
    ok = 0
    for l in elines:
        if correctre.match(l) or wsre.match(l):
            ok = 1
            break
    if not ok:
        out = None

    return (out, err)

#-------------------------------------------------------------------------------
#
def isRevisionNb(rev):
    """Returns true if the string is in the revision number format."""
    return revre_re.match(rev)
    
#-------------------------------------------------------------------------------
#
def isBranchRev(rev):
    """Returns true if the revision is a branch revision."""

    nb = string.count(rev, '.')
    if nb % 2 == 0:
        return 1
    else:
        # check to make sure there is not a zero before the end
        if brrevre_re.match(rev):
            return 1
    return 0

#-------------------------------------------------------------------------------
#
def normRev(rev):
    """Normalizes the branch revision to a common format (without the 0 before
    the end."""

    mo = brrevre_re.match(rev)
    if mo:
        rev = mo.group(1) + '.' + mo.group(2)
    return rev

#-------------------------------------------------------------------------------
#
def cmpRevisions(rev1, rev2):
    """Comparison functor for revision strings.
    This method can be used to sort revision strings correctly numerically."""

    rs1 = rev1.split('.')
    rs2 = rev2.split('.')
    while rs1 or rs2:
        if not rs1:
            return -1
        elif not rs2:
            return 1
        else:
            c = cmp(int(rs1[0]), int(rs2[0]))
            if c != 0:
                return c
            rs1 = rs1[1:]
            rs2 = rs2[1:]

    return 0

#-------------------------------------------------------------------------------
#
def sortRevisionList(list):
    """Sorts a revision list, a little faster than with comparing strings.

    The advantage of using this method is that one does not need to convert
    to/from ints for every comparison (O(n log(n))).
    """
    
    nl = map(lambda x: map(int, x.split('.')), list)
    nl.sort()
    return map(lambda x: string.join(map(repr, x), '.'), nl)
    
#-------------------------------------------------------------------------------
#
def getModule(dir):
    """Returns the name of the module within the given directory."""

    # go cat the CVS/Repository file because only it is guaranteed to
    # contain the correct module name.
    try:
        f = open(os.path.join(dir, 'CVS', 'Repository'), 'r')
        module = f.readline()
        f.close()
    except:
        raise Error('Reading repository file for module.')
    return module.strip()

#-------------------------------------------------------------------------------
#
def getTag(dir):
    """Returns the name of the tag within the given directory.
    If no tag exists, return None."""

    fn = os.path.join(dir, 'CVS', 'Tag')
    if not os.path.exists(fn):
        return None
    
    # go cat the CVS/Tag file.
    try:
        f = open(fn, 'r')
        module = f.readline()
        f.close()
    except:
        raise Error('Reading tag file for module.')
    return module.strip()

#-------------------------------------------------------------------------------
#
def previousRev(rev):
    """Returns the previous revision number string or None if there aren't."""
    if rev:
        rn = rev.split('.')
        if len(rn) % 2 == 1:
            rn = rn[:-1]
        else:
            lastnum = int(rn[-1])
            if lastnum == 1:
                return None
            rn[-1] = '%d' % (lastnum - 1)
        return string.join(rn, '.')
    return None

#-------------------------------------------------------------------------------
#
def parentBranchRev(rev):
    """Returns the parent revision of a branched revision or None if there isn't
    any."""
    if rev:
        rn = rev.split('.')
        rn = rn[ : -(1+rev.count('.')%2) ]
        return string.join(rn, '.')
    return None

#-------------------------------------------------------------------------------
#
def previousOrParent(rev):
    """Returns the previous revision or parent branch if there is one."""
    rev_prev = previousRev(rev)
    if rev_prev:
        return rev_prev
    else:
        return parentBranchRev(rev)
    
#-------------------------------------------------------------------------------
#
def isChildRev(rchld, rpar):
    """Returns 1 if rchld is a child branch or revision of rpar."""

    rnchld = rchld.split('.')
    rnpar = rpar.split('.')
    if len(rnchld) <= len(rnpar):
        return 0
    if cmp(rnpar, rnchld[0:len(rnpar)]) == 0:
        return 1
    return 0

#-------------------------------------------------------------------------------
#
def isInAttic(fn):
    """Removes the attic string in filenames (returns the filename without it)."""
    if fn.find(attic_pfx):
        return 1
    return 0

#-------------------------------------------------------------------------------
#
def removeAttic(fn):
    """Removes the attic string in filenames (returns the filename without it)."""
    return string.replace(fn, attic_pfx, '', 1)

#-------------------------------------------------------------------------------
#
unkfile_re = re.compile('^\? (.*)$')

def getUnknownFiles(dir):
    """Returns a list of the files that are not known or ignored to CVS."""

    cmd = '%s -n -q update %s' % (program, dir)
    (sts, output, error_output) = run(cmd)

    unkfn = []
    for line in output.splitlines():
        mo = unkfile_re.match(line)
        if mo:
            unkfn.append(mo.group(1))
    return unkfn


#===============================================================================
# CLASS MultipleOutputs
#===============================================================================

# Notes:
#
# status multiple outputs goes like this: unk?, sep, out, sep, out, sep, out
# log multiple outputs goes like this: out, sep, out, sep, out, sep

class MultipleOutputs:
    """Get the output of a command for multiple files and split them into a map.
    When working remotely, this is MUCH faster than issuing multiple cvs
    commands.

    Creating an instance launches an asynchronous comamnd. Get the results one
    by one with the method below.
    """

    unkre = re.compile('(^\? (.*)|^$\s*)%')

    def __init__(self, command, separator, indir):
        
        if indir:
            command = '{ cd "' + indir + '" && ' + command + '; }'

        if trace_level >= 1:
            print "== (cvs):", command

        (chin, self.chout, self.cherr) = os.popen3(command, 'r')
        chin.close()
        self.choutfd = self.chout.fileno()
        self.cherrfd = self.cherr.fileno()
        
        # initialize
        self.outeof = 0
        self.erreof = 0

        self.error_output = ''

        #self.makeNonBlocking(self.choutfd)
        #self.makeNonBlocking(self.cherrfd)

        self.separator = separator

    def skipStart(self):
        """Skips the stuff that might be at the start of the status output."""

        while 1:
            line = self.chout.readline()
            if not line:
                break # there is no output, there is an error...
            if line == self.separator:
                break
            #elif unk

        # Note: hoping there won't be too much that it'll block on stderr...
        # we could easily change this to read both pipes like below.

    def next(self):
        """Returns the next status. Return None when done."""

        if self.outeof:
            return None

        output = ''
        nblines = 0
        while not self.outeof or not self.erreof:
            ready = select.select([self.choutfd, self.cherrfd], [], [])

            if self.choutfd in ready[0]:
                line = self.chout.readline()
                if not line:
                    self.outeof = 1

                elif line == self.separator:
                    # Note: removed, we now skip the beginning explicitly.
                    # if nblines > 0: # for skipping first line
                    #     break
                    break
                else:
                    nblines += 1
                    output += line

            # empty the error buffer if needed
            if self.cherrfd in ready[0]:
                line = self.cherr.readline()
                if not line:
                    self.erreof = 1

                self.error_output += line

        if self.outeof:
            rest = self.cherr.read()
            if rest:
                self.error_output += rest

            # close business
            self.sts = self.chout.close()
            if self.sts is None: sts = 0
            self.cherr.close()
            
        if nblines > 1:
            # check for nb of lines because log ends with separator
            return self.processOne(self.separator + output)
     
    # FIXME this could be removed
    def makeNonBlocking(self, fd):
        fl = fcntl.fcntl(fd, fcntl.F_GETFL)
        try:
            fcntl.fcntl(fd, fcntl.F_SETFL, fl | fcntl.O_NDELAY)
        except AttributeError:
            fcntl.fcntl(fd, fcntl.F_SETFL, fl)

#===============================================================================
# CLASS MultipleStatii
#===============================================================================

class MultipleStatii(MultipleOutputs):
    """Get multiple statii."""

    def __init__(self, files, with_tags=0, recursive=0, indir=None):
    
        self.files = files
        self.with_tags = with_tags
        sw = ''
        if with_tags:
            sw += ' -v'
        if recursive:
            sw += ' -R'
        else:
            sw += ' -l'
    
        command = '%s status %s %s' % (program, sw, string.join(files))
        MultipleOutputs.__init__(self, command, statii_sep, indir)

        self.skipStart() # for status only

        # initialize
        self.outputs = {}
        self.statii = {}

    def processOne(self, output):
        """Processes a status. Return None when done."""
        try:
            status = Status(None, self.with_tags, output)
            self.outputs[status.filename] = output
            self.statii[status.filename] = status
        except Error, e:
            e.msg += "\nErrors from command:\n"
            e.msg += self.error_output
            raise e
        return status


#===============================================================================
# CLASS Status
#===============================================================================

class Status:
    """Status of a checkedout file."""

    UP_TO_DATE = 0
    LOCALLY_MODIFIED = 1
    LOCALLY_ADDED = 2
    LOCALLY_REMOVED = 3
    NEEDS_CHECKOUT = 4
    NEEDS_PATCH = 5
    NEEDS_MERGE = 6
    CONFLICTS_ON_MERGE = 7
    UNKNOWN = 8
    ENTRY_INVALID = 9

    STATSET_MODIFIED = [LOCALLY_MODIFIED,
                        CONFLICTS_ON_MERGE]

    STATSET_LOCAL = [LOCALLY_MODIFIED,
                     LOCALLY_ADDED,
                     LOCALLY_REMOVED,
                     CONFLICTS_ON_MERGE]

    STATSET_MERGE = [NEEDS_CHECKOUT,
                     NEEDS_PATCH,
                     NEEDS_MERGE]

    status_strings = [ 'Up-to-date',
                       'Locally Modified',
                       'Locally Added',
                       'Locally Removed',
                       'Needs Checkout',
                       'Needs Patch',
                       'Needs Merge',
                       'File had conflicts on merge',
                       'Unknown',
                       'Entry Invalid' ]

    longuest_status_str = 29
    
    sre = re.compile('^File:\s(.+)\sStatus:\s(.+)$')
    wrre = re.compile('^\s*Working revision:\s+[-]?('+revre+')\s*(.*)')
    wrre2 = re.compile('^\s*Working revision:\s+New file!\s*$')
    wrre3 = re.compile('^\s*Working revision:\s+No entry for (.*)\s*$')
    rrre = re.compile('^\s*Repository revision:\s+('+revre+')\s*(.*)$')
    rrre2 = re.compile('^\s*Repository revision:\s+No revision control file\s*$')
    stre = re.compile('^\s*Sticky Tag:\s+([^\s]+)(\s+.*\s*)?$')
    stre2 = re.compile('^\s*Sticky Tag:\s+([^\s]+)\s+\((revision:.*|branch:.*)\)\s*$')
    existags = re.compile('^\s*Existing Tags:')
    tagre = re.compile('^\s+([^\s]+)\s+\((revision:(.*)|branch:(.*))\)\s*$')
                      
    repofile_fmt = '^%%s%s[^%s]+%s(.*),v$' % tuple([os.sep]*3)
    
    # ===================================================================
    # File: exp2              Status: Up-to-date
    # 
    #    Working revision:    1.1.1.1 Tue May 14 06:15:29 2002
    #    Repository revision: 1.1.1.1 /mnt/fatboy/home/blais/cvsroot/expscripts/exp2,v
    #    Sticky Tag:          (none)
    #    Sticky Date:         (none)
    #    Sticky Options:      (none)


    def __init__(self, filename=None, with_tags=0, status_output=None, indir=None):
        """Initializes a status object with full pathname."""
        if with_tags:
            gtsw = '-v'
        else:
            gtsw = ''

        self.filename = None
        self.attic = None
        if not status_output and filename:
            self.filename = filename
            (sts, out, err) = \
                  run('%s status %s %s' % (program, gtsw, self.filename), indir=indir)
            if sts:
                raise Error('Error getting status.')
            error = Error('Error parsing status on file %s.' % self.filename)
        else:
            assert(status_output)
            out = status_output
            err = ''
            error = Error('Error parsing status on output: \n---\n%s\n---' % out)

        lines = out.split('\n')

        mo = Status.sre.match(lines[1])
        if not mo: raise error
        self.status_filename = mo.group(1).strip()
        try:
            self.status_id = Status.status_strings.index(mo.group(2).strip())
        except ValueError, e:
            raise error

        self.work_rev = ''
        self.repo_rev = ''
        self.repo_file = ''
        self.sticky_tag = None
        self.sticky_rev = None
        self.branch_tags = {}
        self.symbolic_tags = {}

        if self.status_id in [Status.UNKNOWN, Status.ENTRY_INVALID]:
            if not self.filename:
                self.attic = isInAttic(self.status_filename)
                self.filename = removeAttic(self.status_filename)
            return

        mo = Status.wrre.match(lines[3])
        if mo:
            self.work_rev = mo.group(1).strip()
        else:
            mo1 = Status.wrre2.match(lines[3])
            mo2 = Status.wrre3.match(lines[3])
            if not (mo1 or mo2): raise error
            
        mo = Status.rrre.match(lines[4])
        if mo:
            self.repo_rev = mo.group(1).strip()
            repofile_re = re.compile(Status.repofile_fmt % rootPath(cvsroot))
            mof = repofile_re.match(mo.group(2).strip())
            if mof:
                self.attic = isInAttic(mof.group(1))
                self.repo_file = removeAttic(mof.group(1))
        else:
            mo = Status.rrre2.match(lines[4])
            if not mo: raise error

        if not self.filename:
            self.filename = self.repo_file
            if not self.filename:
                self.attic = isInAttic(self.status_filename)
                self.filename = removeAttic(self.status_filename)
            
        mo = Status.stre.match(lines[5])
        if mo: # there could be no sticky spec
            self.sticky_tag = mo.group(1).strip()
            if self.sticky_tag == '(none)':
                self.sticky_tag = None

        mo = Status.stre2.match(lines[5])
        if mo:
            self.sticky_rev = mo.group(2).split(':')[1].strip()

        if with_tags:
            idx = 0
            for i in range(0,len(lines)):
                if Status.existags.match(lines[i]):
                    idx = i+1
                    break
            self.symbolic_tags = {}
            self.branch_tags = {}
            if idx:
                while idx < len(lines):
                    mo = Status.tagre.match(lines[idx])
                    if not mo:
                        break
                    tag = mo.group(1).strip()
                    revstr = mo.group(2).strip()
                    if revstr.startswith('branch'):
                        self.branch_tags[tag] = mo.group(4).strip()
                    else:
                        self.symbolic_tags[tag] = mo.group(3).strip()
                    idx += 1
            
    def status(self):
        """Returns the status of the file."""
        return self.status_id

    def statusStr(self):
        return Status.status_strings[self.status_id]

    def workingRev(self):
        """Returns the working revision of the file."""
        return self.work_rev

    def repositoryRev(self):
        """Returns the repository revision of the file."""
        return self.repo_rev

    def repositoryFile(self):
        """Returns the repository file path."""
        return self.repo_rev

    def stickyTag(self):
        """Returns the sticky tag of the file."""
        return self.sticky_tag

    def stickyRev(self):
        """Returns the revision prefix for the sticky tag, whether it's a branch
        or a simple tag."""
        return self.sticky_rev

    def stickyDate(self):
        """Returns the sticky date of the file."""
        pass # FIXME todo
        assert(false)

    def stickyOptions(self):
        """Returns the sticky options of the file."""
        pass # FIXME todo
        assert(false)

    def symbolicTags(self):
        """Returns a map of the ordinary tags to their revisions."""
        return self.symbolic_tags

    def branchTags(self):
        """Returns a map of the branch tags to their revisions."""
        return self.branch_tags

    def tags(self):
        """Returns a map of all tags to their revisions."""
        nmap = {}
        nmap.update(self.symbolic_tags)
        nmap.update(self.branch_tags)
        return nmap

    def inAttic(self):
        """Returns 1 if the file is in the Attic on the repository, 0 if not.
        Returns None if unknown."""
        return self.attic

    def __repr__(self):
        """Prints out textural representation."""
        s = 'status = %s\n' % Status.status_strings[self.status_id]
        s += 'working revision = %s\n' % self.work_rev
        s += 'repository revision = %s\n' % self.repo_rev
        s += 'repository file = %s\n' % self.repo_file
        s += 'sticky tag = %s\n' % self.sticky_tag

        s += 'symbolic tags = \n'
        keys = self.symbolic_tags.keys()
        keys.sort()
        for k in keys:
            s += '   %s: %s\n' % (k, self.symbolic_tags[k])

        s += 'branch tags = \n'
        keys = self.branch_tags.keys()
        keys.sort()
        for k in keys:
            s += '   %s: %s\n' % (k, self.branch_tags[k])

        return s

    def previousRev(self, rev_or_tag):
        """Returns the previous revision number for revision or tag or None if
        there aren't."""
        if revre_re.match(rev_or_tag):
            return previousRev(rev_or_tag)
        elif rev_or_tag in self.symbolic_tags:
            return previousRev(self.symbolic_tags[rev_or_tag])
        elif rev_or_tag in self.branch_tags:
            print rev_or_tag, self.branch_tags[rev_or_tag]
            return previousRev(self.branch_tags[rev_or_tag])
        return None
    
    def parentBranchRev(self, rev_or_tag):
        """Returns the parent revision of a branched revision or None if there isn't
        any. This method accepts a tag."""
        if revre_re.match(rev_or_tag):
            return parentBranchRev(rev_or_tag)
        elif rev_or_tag in self.symbolic_tags:
            return parentBranchRev(self.symbolic_tags[rev_or_tag])
        elif rev_or_tag in self.branch_tags:
            return parentBranchRev(self.branch_tags[rev_or_tag])
        return None
   
    def previousOrParent(self, rev_or_tag):
        """Returns the previous revision or parent branch if there is one.
        This method accepts a tag."""
        rev_prev = self.previousRev(rev_or_tag)
        if rev_prev:
            return rev_prev
        else:
            return self.parentBranchRev(rev_or_tag)

    def isBranched(self):
        """Returns true if the current sticky tag of this status is a branch
        tag."""
        return isBranchRev(self.sticky_tag)

    
#===============================================================================
# CLASS MultipleLogs
#===============================================================================

class MultipleLogs(MultipleOutputs):
    """Get multiple logs."""

    def __init__(self, files, indir=None, remote=0, with_revisions=0):
    
        self.files = files

        if remote:
            cmd = 'rlog'
        else:
            cmd = 'log'
    
        command = '%s %s %s' % (program, cmd, string.join(files))
        MultipleOutputs.__init__(self, command, logs_sep, indir)

        # initialize
        self.outputs = {}
        self.logs = {}
        self.with_revisions = with_revisions
        self.remote = remote
        
    def processOne(self, output):
        """Processes a status. Return None when done."""
        log = Log(None, self.remote, output,
                   with_revisions=self.with_revisions)
        self.outputs[log.filename] = output
        self.logs[log.filename] = log
        return log

    def notr():

        try:
            pass
        except Error, e:
            e.msg += "\nErrors from command:\n"
            e.msg += self.error_output
            raise e
        return log


#===============================================================================
# CLASS RevInfo
#===============================================================================

class RevInfo:
    """Class that holds revision information for each revision in a tree."""

    descre = re.compile('^description:')
    sepre = re.compile('----------------------------')
    revire = re.compile('revision ('+revre+')')
    brre = re.compile('branches: ('+revre+');')

    def __init__(self):
        self.rev = None
        self.date = ''
        self.author = ''
        self.state = ''
        self.lines = ''
        self.branches = []

    def __repr__(self):
        """Prints out textural representation."""
        s = '(revision = %s, fields = %s)' % (self.rev, self.fields)
        return s
        

#===============================================================================
# CLASS Log
#===============================================================================

class Log:
    """Log information of a repository file."""

    # Example:
    #
    # RCS file: /mnt/fatboy/home/blais/cvsroot/expscripts/exp2,v
    # Working file: exp2
    # head: 1.1
    # branch: 1.1.1
    # locks: strict
    # access list:
    # symbolic names:
    #         start: 1.1.1.1
    #         blais: 1.1.1
    # keyword substitution: kv
    # total revisions: 2
    # =============================================================================

    rcsfre = re.compile('^RCS file:\s+(.*)$')
    wfre = re.compile('^Working file:\s(.*)')
    headre = re.compile('^head:\s('+revre+')\s*$')
    symre = re.compile('^symbolic names:')
    tagre = re.compile('^\s+([^:]*):\s('+revre+')$')

    def __init__(self, filename, remote=0, log_output=None, with_revisions=0):
        """Initializes a log object with full pathname."""
        
        self.filename = None
        self.attic = None
        if not log_output and filename:
            self.filename = filename

            if remote:
                cmd = 'rlog'
            else:
                cmd = 'log'

            if with_revisions:
                wrsw = ''
            else:
                wrsw = '-h'

            (sts, out, err) = \
                  run('%s %s %s %s' % (program, cmd, wrsw, filename))
            if sts or len(out) == 0:
                raise Error('Error getting log.')
            error = Error('Error parsing log on file %s.' % self.filename)
        else:
            assert(log_output)
            out = log_output
            err = ''
            error = Error('Error parsing log on output: \n---\n%s\n---' % out)

        lines = out.split('\n')

        # find beginning
        ii = -1
        for i in range(0,len(lines)):
            if Log.rcsfre.match(lines[i]):
                ii = i
                break
        assert(ii != -1)

        mo = Log.rcsfre.match(lines[ii])
        if not mo: raise error
        self.rcsfile = mo.group(1).strip()

        # extract filename from rcsfile
        repofile_re = re.compile(Status.repofile_fmt % rootPath(cvsroot))
        mof = repofile_re.match(self.rcsfile)
        if mof:
            self.attic = isInAttic(mof.group(1))
            self.filename = removeAttic(mof.group(1))

        if not remote:
            ii += 1
            mo = Log.wfre.match(lines[ii])
            if not mo: raise error
            self.work_file = mo.group(1).strip()
        else:
            self.work_file = None

        ii += 1
        mo = Log.headre.match(lines[ii])
        if not mo: raise error
        self.head_rev = mo.group(1).strip()

        for i in range(0,len(lines)):
            if Log.symre.match(lines[i]):
                idx = i+1
                break
        self.symbolic_tags = {}
        self.branch_tags = {}
        while idx < len(lines):
            mo = Log.tagre.match(lines[idx])
            if not mo:
                break
            tag = mo.group(1).strip()
            rev = mo.group(2).strip()
            if isBranchRev(rev):
                self.branch_tags[tag] = normRev(rev)
            else:
                self.symbolic_tags[tag] = rev
            idx += 1

        if with_revisions:
            self.revisions = self.parseRevisions(lines)
        else:
            self.revisions = {}

    def parseRevisions(self, lines):
        """Parses the revision info."""

        error = Error('Error parsing log revisions')

        # find beginning
        idx = -1
        for i in range(0,len(lines)):
            if RevInfo.descre.match(lines[i]):
                idx = i+1
                break
        if idx == -1: return {}

        revisions = {}

        # first separator is skipped in (a)
        #if RevInfo.sepre.match(lines[idx]): idx += 1 

        strip_logs_sep = string.strip(logs_sep)
        done = 0
        while not done:
            rlines = []

            while 1:
                assert(idx < len(lines))
                idx += 1 # (a)
                if idx == len(lines):
                    done = 1
                    break
                elif RevInfo.sepre.match(lines[idx]):
                    break
                elif strip_logs_sep == lines[idx]:
                    done = 1
                    break
                else:
                    rlines.append(lines[idx])

            if rlines:
                # parse revision
                revinfo = RevInfo()
                mo = RevInfo.revire.match(rlines[0])
                if not mo: raise error
                revinfo.rev = mo.group(1)
                revinfo.fields = map(string.strip, rlines[1].split(';'))
                
                revisions[ revinfo.rev ] = revinfo

        return revisions


    def rcsFile(self):
        """Returns the path of the RCS file."""
        return self.rcsfile
    
    def headRev(self):
        """Returns the head revision of the file."""
        return self.head_rev

    def headBranch(self):
        """Returns the branch for the head of the file."""
        return self.head_branch

    def symbolicTags(self):
        """Returns a map of the ordinary tags to their revisions."""
        return self.symbolic_tags

    def branchTags(self):
        """Returns a map of the branch tags to their revisions."""
        return self.branch_tags

    def tags(self):
        """Returns a map of all tags to their revisions."""
        nmap = {}
        nmap.update(self.symbolic_tags)
        nmap.update(self.branch_tags)
        return nmap

    def inAttic(self):
        """Returns 1 if the file is in the Attic on the repository, 0 if not.
        Returns None if unknown."""
        return self.attic

    def __repr__(self):
        """Prints out textural representation."""
        s = 'RCS file = %s\n' % self.rcsfile
        s += 'working file = %s\n' % self.work_file

        s += 'head revision = %s\n' % self.head_rev

        s += 'symbolic tags = \n'
        keys = self.symbolic_tags.keys()
        keys.sort()
        for k in keys:
            s += '   %s: %s\n' % (k, self.symbolic_tags[k])

        s += 'branch tags = \n'
        keys = self.branch_tags.keys()
        keys.sort()
        for k in keys:
            s += '   %s: %s\n' % (k, self.branch_tags[k])

        if self.revisions:
            s += 'revisions = \n'
            s += pformat(self.revisions)

        return s


    def allRevisions(self):
        """Returns a list of all the revisions.
        This only returns valids results if it was parsed."""
        return self.revisions
    
    def getMainRevisions(self):
        """Returns a list of the main revisions.
        This only returns valids results if it was parsed."""
        assert(false)


#===============================================================================
# Load initialization
#===============================================================================

# Note: clients must set this if the commands are meant to run in a different
# dir. For now this is used only to parse the repository filename.
cvsroot = getRoot(os.getcwd())


#===============================================================================
# TESTS
#===============================================================================

def selftest():
    """Self-test."""
    print "Performing tests."
    assert(not isBranchRev('1.2'))
    assert(isBranchRev('1.2.3'))
    assert(isBranchRev('1.2.0.2'))
    assert(normRev('1.2.3') == '1.2.3')
    assert(normRev('1.2.0.2') == '1.2.2')
    assert(previousRev('1.2.1.2') == '1.2.1.1')
    assert(previousRev('1.2.1.1') == None)
    assert(not parentBranchRev('1'))
    assert(not parentBranchRev('1.2'))
    assert(parentBranchRev('1.2.3') == '1.2')
    assert(parentBranchRev('1.2.3.4') == '1.2')
    assert(parentBranchRev('1.2.3.4.5') == '1.2.3.4')
    assert(parentBranchRev('1.2.3.4.5.6') == '1.2.3.4')
    
    assert(cmpRevisions('1', '2') == -1)
    assert(cmpRevisions('1', '1') == 0)
    assert(cmpRevisions('2', '1') == 1)
    assert(cmpRevisions('1', '1.1') == -1)
    assert(cmpRevisions('1.1', '1.1') == 0)
    assert(cmpRevisions('1.1', '1') == 1)
    assert(cmpRevisions('1.2', '1.1') == 1)
    assert(cmpRevisions('1.2.1', '1.1') == 1)
    assert(cmpRevisions('1.2.1', '1.10') == -1)

    #print sortRevisionList(['1.3', '1.1.0', '1.10'])

    assert(isChildRev('1.4', '1.6') == 0)
    assert(isChildRev('1.4.2', '1.5') == 0)
    assert(isChildRev('1.4.2', '1.4') == 1)
    assert(isChildRev('1.4.2.6', '1.4') == 1)

    #print getUnknownFiles('.')


# Run main if loaded as a script
if __name__ == "__main__":
    selftest()


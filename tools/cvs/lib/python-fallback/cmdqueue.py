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

"""Implements a command queue.

Implement a queue for commands, where only a given number of commands is allowed
to run at the same time and we wait for the run commands to finish before
starting a new one.

Implementation note: we do not use Python threading capabilities because of
annoying behaviour related to interruptibility of subprocesses. You will need to
poll occasionnally to let this queue execute some of its command before being
destroyed. The queue empties itself (i.e. runs commands) automatically when
destroyed.
"""

__version__ = "$Revision$"
__author__ = "Martin Blais <blais@furius.ca>"

#===============================================================================
# EXTERNAL DECLARATIONS
#===============================================================================

import sys, os
import time

from pprint import pprint, pformat

#===============================================================================
# PUBLIC FUNCTIONS
#===============================================================================

#===============================================================================
# CLASS Command
#===============================================================================

class Command:
    """Just a class to hold information about a command."""

    def __init__(self, command):
        self.command = command # this is in fact a list of arguments
        self.pid = None
        self.interruptible = 1

#===============================================================================
# CLASS CmdQueue
#===============================================================================

class CmdQueue:
    """Command queue object."""

    def __init__(self, nbcmds=1):
        self.nbcmds = nbcmds
        self.waiting = []
        self.running = []
        self.done = []
        self.waitingForAll = 0

    def __del__(self):
        try:
            self.waitAll()
        except KeyboardInterrupt:
            pass
        
    def queue(self, cmdl, cmdobj=None):
        """Queues a command to be run.
        If cmdobj is specified, methods preRun() and postRun() are called before and after
        running the command, respectively."""
        cmd = Command(cmdl)
        cmd.cmdobj = cmdobj
        self.waiting.append(cmd)

    def poll(self):
        """Run this command periodically."""
        for cmd in self.running:
            if self.wait(cmd):
                self.running.remove(cmd)
                self.done.append(cmd)
                cmd.cmdobj.postRun(self)
        if len(self.running) >= self.nbcmds:
            return

        while len(self.running) < self.nbcmds and len(self.waiting) > 0:
            cmd = self.waiting.pop(0)
            self.running.append(cmd)
            cmd.cmdobj.preRun(self)
            self.run(cmd)

    def waitAll(self):
        self.waitingForAll = 1
        self.poll()
        while len(self.running) > 0 or len(self.waiting) > 0:
            os.wait()
            self.poll()
        self.waitingForAll = 0
            
    def run(self, cmd):
        cmd.pid = os.fork()
        if cmd.pid == 0:
            return os.execvp(cmd.command[0], cmd.command)

    def wait(self, cmd):
        try:
            (id, status) = os.waitpid(cmd.pid, os.WNOHANG)
            if id:
                return os.WIFSTOPPED(status) or os.WIFSIGNALED(status) or \
                       os.WIFEXITED(status)
        except OSError, e:
            import errno
            if e.errno == errno.ECHILD:
                return 1
        return 0


#===============================================================================
# CLASS SyncFakeQueue
#===============================================================================

class SyncFakeQueue:
    """Fake command queue which is in fact synchronous."""

    def __init__(self, nbcmds=1):
        self.nbcmds = nbcmds

    def queue(self, cmdl, cmdobj):
        """Queues a command to be run."""
        pid = os.fork()
        cmdobj.preRun(self)
        if pid == 0:
            return os.execvp(cmdl[0], cmdl)
        else:
            os.waitpid(pid, 0)
        cmdobj.postRun(self)

    def poll(self):
        pass


#===============================================================================
# TESTS
#===============================================================================

def selftest():
    """Self-test."""
    print "Performing tests."

    queue = CmdQueue(2)
    queue.queue('xxdiff /users/blais/xxdiff/src/mine /users/blais/xxdiff/src/older')
    queue.queue('xxdiff /users/blais/xxdiff/src/mine /users/blais/xxdiff/src/yours')
    queue.queue('xxdiff /users/blais/xxdiff/src/mine /users/blais/xxdiff/src/older')
    queue.queue('xxdiff /users/blais/xxdiff/src/mine /users/blais/xxdiff/src/yours')
    


# Run main if loaded as a script
if __name__ == "__main__":
    selftest()


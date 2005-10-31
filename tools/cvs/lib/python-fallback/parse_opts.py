#!/usr/bin/env python2
#******************************************************************************\
#* $Source$
#* $Id$
#*
#* Copyright (C) 2001, Martin Blais <blais@iro.umontreal.ca>
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

"""Functions to help parsing options.
"""

__version__ = "$Revision$"
__author__ = "Martin Blais <blais@iro.umontreal.ca>"

#===============================================================================
# EXTERNAL DECLARATIONS
#===============================================================================

import sys, os
import string
import distutils.fancy_getopt as fgetopt  # getopt()
from distutils.errors import DistutilsArgError


#===============================================================================
# PUBLIC DECLARATIONS
#===============================================================================

#-------------------------------------------------------------------------------
#
def detectCollisions( optmap ):
    """Detects option name and letter collisions."""

    names = letters = []
    for o in optmap:
        (name, letter) = o[0:2]
        if name and name in names:
            print >> sys.stderr, "Warning: options name collision:", o
        if letter and letter in letters:
            print >> sys.stderr, "Warning: options letter collision:", o
        names.append(name)
        letters.append(letter)


    
# INLINE-BEGIN parse_options
#-------------------------------------------------------------------------------
#
# Standard options parsing routine.
# Takes care of handling --help and --version, returns options object and
# remaining arguments pair.
#

#import distutils.fancy_getopt as fgetopt  # getopt()
#from distutils.errors import DistutilsArgError

def parse_options( optmap, moduledoc=None, version_msg=None ):
    optmap = [
        ( 'help', 'h', "Prints help message." ),
        ( 'version', 'V', "Prints version." ),
        ] + optmap

    def opt_translate(opt):
        o = opt[0]
        if o[-1] == '=': # option takes an argument?
            o = o[:-1]
        return fgetopt.translate_longopt( o )
    
    opts = fgetopt.OptionDummy( map( opt_translate, optmap ) )
    detectCollisions( optmap )
    parser = fgetopt.FancyGetopt( optmap )
    
    try:
        args = parser.getopt( args=sys.argv[1:], object=opts )
    #except distutils.errors.DistutilsGetoptError, e:
    except DistutilsArgError, e:
        print >> sys.stderr, "Argument error:", e
        raise SystemExit(1)

    if opts.version:
        if version_msg:
            print version_msg
        raise SystemExit(1)

    if opts.help:
        for i in parser.generate_help( moduledoc ):
            print i
        print
        raise SystemExit(1)

    return (opts, args)
# INLINE-END


# INLINE-BEGIN parse_options_subcmds
#-------------------------------------------------------------------------------
#
# Standard options parsing routine with subcommands support.
# Takes care of handling --help and --version, as well as --help for subcmd.
#
# Subcmd objects must have 'name', 'optmap' (optional) and a doc string
# (optional) which is in the GvR format..
#
# Returns (opts, subcmd, args)

#import distutils.fancy_getopt as fgetopt  # getopt()
#from distutils.errors import DistutilsArgError

def parse_options_subcmds( optmap, subcmds=[], moduledoc=None, version_msg=None ):

    optmap = [
        ( 'help', 'h', "Prints help message." ),
        ( 'version', 'V', "Prints version." ),
        ] + optmap

    def opt_translate(opt):
        o = opt[0]
        if o[-1] == '=': # option takes an argument?
            o = o[:-1]
        return fgetopt.translate_longopt( o )

    # parse global options
    opts = fgetopt.OptionDummy( map( opt_translate, optmap ) )
    detectCollisions( optmap )
    parser = fgetopt.FancyGetopt( optmap )
    try:
        args = parser.getopt( args=sys.argv[1:], object=opts )
    except DistutilsArgError, e:
        print >> sys.stderr, "Argument error:", e
        raise SystemExit(1)

    # print version information
    if opts.version:
        if version_msg:
            print version_msg
        raise SystemExit(1)

    # print global help
    if opts.help:
        for i in parser.generate_help( moduledoc ):
            print i
        print
        print "Available subcommands"
        print "------------------------------"
        subcopy = list(subcmds)
        for c in subcmds:
            if type(c.name) == type([]):
                c.main_name = c.name[0]
            else:
                c.main_name = c.name
        subcopy.sort( lambda x, y: cmp(x.main_name, y.main_name) )
        for c in subcopy:
            if c.__doc__:
                first_line = c.__doc__.split('\n')[0]
                print "  '%s':" % c.main_name, first_line
            else:
                print "  '%s'" % c.main_name
        print
        raise SystemExit(1)

    # build map of subcommand names to command objects
    scnmap = {}
    for s in subcmds:
        if type(s.name) == type(""):
            namelist = [s.name]
        elif type(s.name) == type([]):
            namelist = s.name
        for n in namelist:
            assert( n not in scnmap )
            scnmap[n] = s

    # find current subcommand
    if len(args) == 0:
        print >> sys.stderr, "Error: no subcommand"
        raise SystemExit(1)

    sc = None
    if args[0] in scnmap:
        # we have an exact match! prefer that.
        sc = scnmap[ args[0] ]
    else:
        # check for partial matches.
        def fbeg( x ): return x.startswith( args[0] )
        scl = filter( fbeg, scnmap.keys() )
        if len(scl) == 0:
            print >> sys.stderr, "Error: invalid subcommand '%s'" % args[0]
            raise SystemExit(1)
        elif len(scl) > 1:
            sc = scnmap[ scl[0] ]
            ambig = 0
            for s in scl[1:]:
                if sc != scnmap[ s ]:
                    ambig = 1
                    break
            if ambig:
                print >> sys.stderr, "Error: ambiguous subcommand '%s'" % args[0]
                print >> sys.stderr, "Possible partial completions: ", scl
                raise SystemExit(1)
        else:
            sc = scnmap[ scl[0] ]

    # parse command options
    suboptmap = [( 'help', 'h', "Prints this subcommand's help." )]
    if getattr(sc, 'optmap', None):
        suboptmap = sc.optmap + suboptmap
    opts.__init__( map( opt_translate, suboptmap ) )

    detectCollisions( suboptmap )
    subparser = fgetopt.FancyGetopt( suboptmap )
    try:
        subargs = subparser.getopt( args[1:], object=opts )
    except DistutilsArgError, e:
        print >> sys.stderr, "Argument error:", e
        raise SystemExit(1)

    # print command help
    if opts.help:
        print string.join( moduledoc.split('\n')[0:2], '\n' ) # first 2 lines.
        if type(sc.name) == type([]):
            name = sc.name[0]
        else:
            name = sc.name
        print "Help for '" + name + "' subcommand:"
        print "------------------------------"
        for i in subparser.generate_help( sc.__doc__ ):
            print i
        print
        if type(sc.name) == type([]) and len(sc.name) > 1:
            print "(Aliases: %s)" % string.join(sc.name[1:], ', ')
            print
        raise SystemExit(1)

    return (opts, sc, subargs)
# INLINE-END

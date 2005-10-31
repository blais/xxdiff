/******************************************************************************\
 * $Id: util.h 56 2000-12-25 20:15:47Z  $
 * $Date: 2000-12-25 15:15:47 -0500 (Mon, 25 Dec 2000) $
 *
 * Copyright (C) 1999, 2000  Martin Blais <blais@iro.umontreal.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *****************************************************************************/

#ifndef INCL_XXDIFF_UTIL
#define INCL_XXDIFF_UTIL

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_STD_STDIO
#include <stdio.h>
#define INCL_STD_STDIO
#endif

#ifndef INCL_STD_SIGNAL
#include <signal.h>
#define INCL_STD_SIGNAL
#endif


XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxUtil
 *============================================================================*/

// <summary> utilities </summary>

class XxUtil {

public:

   /*----- static member functions -----*/

   // Return true if the file is a directory.
   static bool isDirectory( const char* filename );

   // Copy a stream to a file.
   static void copyToFile( FILE* fin, const char* name );

   // Conduct some tests to determine if the input file is suitable for
   // processing by our program.  Return true on success.
   static bool testFile( const char* filename, bool& isDirectory );

   // Quick heuristic to test whether a file's contents are ascii text
   static bool isAsciiText( const char *filename );

   // Run a program with command line arguments and two pathname arguments via
   // fork/exec and forget about it. Returns true on success.
   static bool spawnCommand( 
      const char* command,
      const char* args[],
      void (*sigChldHandler)(int) = 0
   );

   // Run a program with command line arguments and two pathname arguments via
   // fork/exec and return a pipe file descriptor into which standard output and
   // standard error have been redirected.
   static FILE* spawnCommandWithOutput( 
      const char* command,
      const char* args[]
   );

   // This is code for an interruptible system() call as shown as suggested in
   // GNU libc/Linux system(3) man page.
   static int interruptibleSystem( const char* command );

};


XX_NAMESPACE_END

#endif

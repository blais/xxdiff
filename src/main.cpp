/* -*- c-file-style: "xxdiff" -*- */
/******************************************************************************\
 * $RCSfile$
 *
 * Copyright (C) 1999-2003  Martin Blais <blais@furius.ca>
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
 ******************************************************************************/

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <main.h>
#include <cmdline.h>
#include <app.h>
#include <exceptions.h>

#include <exception>
#include <iostream>
#include <memory>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

const char* exceptionPreface = "xxdiff: exception caught: ";

}

/*==============================================================================
 * MAIN
 *============================================================================*/

XX_NAMESPACE_USING

extern char** environ;
static char s_env_string_LC_ALL[] = "LC_ALL=C";

//------------------------------------------------------------------------------
//
int main( int argc, char** argv, char** envp )
{
   environ = envp;

   // Override user locale or xxdiff could not parse localized diff output...
   putenv(s_env_string_LC_ALL);

   int retval = 2; // errors.
   try {
      // Parse command line.
      XxCmdline cmdline;
      bool run = cmdline.parseCommandLine( argc, argv );

      if ( run ) {
         // Initialization framework.
         XxResParser::initialize();

         int qtargc;
         char** qtargv;
         cmdline.getQtOptions( qtargc, qtargv );

	 // Create application and connect.
         XxApp app( qtargc, qtargv, cmdline );

         // Run event loop.
         app.exec();

	 retval = app.getReturnValue();
      }
      else {
         retval = 0;
      }
   }
   catch ( const XxError& ex ) {
      std::cerr << static_cast<const char*>( ex.getMsg() ) << std::endl;
      // Note: we're casting for Windows MSVC streams which are broken.
   }
   catch ( const std::exception& ex ) {
      std::cerr << ex.what() << std::endl;
   }
   catch ( const char* ex ) {
      std::cerr << ex << std::endl;
   }
   catch ( ... ) {
      std::cerr << exceptionPreface << "unspecified internal error."
                << std::endl;
   }
   return retval;
}

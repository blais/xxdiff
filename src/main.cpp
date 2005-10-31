/******************************************************************************\
 * $Id: main.cpp 311 2001-10-24 22:47:06Z blais $
 * $Date: 2001-10-24 18:47:06 -0400 (Wed, 24 Oct 2001) $
 *
 * Copyright (C) 1999-2001  Martin Blais <blais@iro.umontreal.ca>
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

//------------------------------------------------------------------------------
//
int main( int argc, char** argv, char** envp ) 
{
   environ = envp;

   int retval = 2; // errors.
   try {
      // Parse command line.
      XxCmdline cmdline;
      bool run = cmdline.parseCommandLine( argc, argv );

      if ( run ) {
         // Initialization framework.
         XxResParser::initialize();
         
         // Create application and connect.
         XxApp app( argc, argv, cmdline );

         // Run event loop.
         app.exec();
         retval = app.getReturnValue();
      }
      else {
         retval = 0;
      }
   }
   catch ( const XxError& ex ) {
      std::cerr << ex.getMsg() << std::endl;
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

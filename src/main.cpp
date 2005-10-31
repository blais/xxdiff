/******************************************************************************\
 * $Id: main.cpp 48 2000-10-03 04:43:36Z  $
 * $Date: 2000-10-03 00:43:36 -0400 (Tue, 03 Oct 2000) $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <main.h>
#include <app.h>
#include <exceptions.h>

#include <exception>
#include <iostream>


/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

//------------------------------------------------------------------------------
//
void silentMsgHandler( QtMsgType type, const char *msg )
{
   switch ( type ) {
      case QtFatalMsg: {
         std::cerr << "Fatal: " << msg << std::endl;
      } break;

      case QtDebugMsg:
      case QtWarningMsg: {
      }
   }
}

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
#ifndef XX_DEBUG
   // Shut up!
   qInstallMsgHandler( silentMsgHandler );
#endif

   int retval = 2; // errors.
   try {
      XxApp app( argc, argv );
      app.exec();
      retval = app.getReturnValue();
   }
   catch ( XxUsageError* ex ) {
      std::cerr << ex->what() << std::endl;
      delete ex;
   }
   catch ( std::exception* ex ) {
      std::cerr << exceptionPreface << ex->what() << std::endl;
      delete ex;
   }
   catch ( const char* ex ) {
      std::cerr << exceptionPreface << ex << std::endl;
      delete[] ex;
   }
   catch ( ... ) {
      std::cerr << exceptionPreface << "unspecified internal error."
                << std::endl;
   }
   return retval;
}

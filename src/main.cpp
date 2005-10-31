/******************************************************************************\
 * $Id: main.cpp 64 2001-03-11 01:06:13Z  $
 * $Date: 2001-03-10 20:06:13 -0500 (Sat, 10 Mar 2001) $
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
      // Search original arguments to see if the style has been explicitly
      // forced.
      int aa;
      for ( aa = 0; aa < argc; ++aa ) {
         if ( ::strncmp( argv[aa], "-style", 6 ) == 0 ) {
            break;
         }
      }

      XxApp app( argc, argv, (aa == argc) );
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

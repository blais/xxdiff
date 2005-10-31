/******************************************************************************\
 * $Id: xrmParser.cpp 2 2000-09-15 02:19:22Z blais $
 * $Date: 2000-09-14 22:19:22 -0400 (Thu, 14 Sep 2000) $
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

#include <defs.h>

#ifdef XX_USE_XRM

#include <xrmParser.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>

//#define LOCAL_TRACE
#ifdef LOCAL_TRACE
#define XX_LOCAL_TRACE(x) XX_TRACE(x)
#else
#define XX_LOCAL_TRACE(x)
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxXrmParser
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxXrmParser::XxXrmParser( 
   void*       display,
   const char* appClassName, 
   int&        argc, 
   char**      argv
) :
   _className( appClassName ),
   _database( 0 )
{
   using namespace std;

   Display* mdisplay = (Display*)display;
   XX_ASSERT( mdisplay != 0 );
   XX_ASSERT( appClassName != 0 );

   // Initialize resource manager.
   XrmInitialize();
   XrmDatabase database = 0;

   // Parse file in standard location.
   XX_LOCAL_TRACE( "Parsing resources: standard location" );
   string stdloc = "/usr/lib/X11/app-defaults";
   XrmDatabase stdLocDB = XrmGetFileDatabase( stdloc.c_str() );
   if ( stdLocDB != 0 ) {
      XrmCombineDatabase( stdLocDB, &database, True );
      // XrmDestroyDatabase( appResDB ); for some reason this crashes
   }

   // Parse file in XENVIRONMENT
   XX_LOCAL_TRACE( "Parsing resources: XENVIRONMENT" );
   const char* XENVIRONMENT = getenv( "XENVIRONMENT" );   
   string xenv;
   if ( XENVIRONMENT != 0 ) {
      xenv = XENVIRONMENT;
   }
   else {
      const char* home = getenv( "HOME" );
      if ( home != 0 ) {
         xenv.append( home );
      }
      xenv.append( "/.Xdefaults-" );
      const char* hn = getenv( "HOSTNAME" );
      if ( hn != 0 ) {
         xenv.append( hn );
      }
   }
   XrmDatabase xenvDB = XrmGetFileDatabase( xenv.c_str() );
   if ( xenvDB != 0 ) {
      XrmCombineDatabase( xenvDB, &database, True );
      // XrmDestroyDatabase( xenvDB ); for some reason this crashes
   }

   // Parse file in application resources directory.
   XX_LOCAL_TRACE( "Parsing resources: app res directory" );
   const char* XAPPLRESDIR = getenv( "XAPPLRESDIR" );
   if ( XAPPLRESDIR != 0 ) {
      string filename = XAPPLRESDIR;
      filename.append( "/" );
      filename.append( appClassName );
      XrmDatabase appResDB = XrmGetFileDatabase( filename.c_str() );
      if ( appResDB != 0 ) {
         XrmCombineDatabase( appResDB, &database, True );
         // XrmDestroyDatabase( appResDB ); for some reason this crashes
      }
   }

   // Parse file in application user file search path
   XX_LOCAL_TRACE( "Parsing resources: user file search path" );
   const char* XUSERFILESEARCHPATH = getenv( "XUSERFILESEARCHPATH" );
   if ( XUSERFILESEARCHPATH != 0 ) {
      XrmDatabase userFileDB = 0;

      string ufsp = XUSERFILESEARCHPATH;
      string::size_type pos = 0;
      for ( ;; ) {
         pos = ufsp.find( "%N", pos, 2 );
         if ( pos == string::size_type(-1) ) {
            break;
         }
         ufsp.replace( pos, 2, appClassName );
         pos += 2;
      }

      string::size_type ppos = 0;
      for ( ;; ) {
         pos = ufsp.find( ":", ppos, 1 );
         string path;
         if ( pos == string::size_type(-1) ) {
            path = ufsp.substr( ppos, ufsp.length() - pos );
         }
         else {
            path = ufsp.substr( ppos, pos - ppos );
         }
         ppos = pos + 1;

         XX_LOCAL_TRACE( "File: " << path );
         XrmDatabase db = XrmGetFileDatabase( path.c_str() );
         if ( db != 0 ) {
            XrmCombineDatabase( db, &userFileDB, True );
            // XrmDestroyDatabase( db ); for some reason this crashes.
         }

         if ( pos == string::size_type(-1) ) {
            break;
         }
      }

      XX_LOCAL_TRACE( "Done with user files." );
      if ( userFileDB != 0 ) {
         XrmCombineDatabase( userFileDB, &database, True );
         // XrmDestroyDatabase( userFileDB ); for some reason this crashes
      }
   }

   XX_LOCAL_TRACE( "Parsing resources: done." );
   _database = database;
}

//------------------------------------------------------------------------------
//
XxXrmParser::~XxXrmParser()
{
   XrmDestroyDatabase( (XrmDatabase)_database );
}

//------------------------------------------------------------------------------
//
bool XxXrmParser::query( 
   XxResources::Resource resource,
   const char*           name,
   std::string&          value
)
{
   std::string resName = _className + "." + name;

   char* retType;
   XrmValue xvalue;
   bool found = XrmGetResource( 
      (XrmDatabase)_database,
      resName.c_str(), 
      _className.c_str(),
      &retType,
      &xvalue
   );
   if ( found == False ) {
      return false;
   }

   value = xvalue.addr;

   return true;
}


XX_NAMESPACE_END

#endif

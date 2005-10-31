/******************************************************************************\
 * $Id: xrmParser.h 138 2001-05-20 18:08:45Z blais $
 * $Date: 2001-05-20 14:08:45 -0400 (Sun, 20 May 2001) $
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

#ifndef INCL_XXDIFF_XRMPARSER
#define INCL_XXDIFF_XRMPARSER

#ifdef XX_USE_XRM


/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_RESOURCES
#include <resources.h>
#endif

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_STD_STRING
#include <string>
#define INCL_STD_STRING
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * CLASS XxXrmParser
 *============================================================================*/

// <summary> abstract interface for resource parsers </summary>

class XxXrmParser : public XxResourcesParser {

public:

   /*----- member functions -----*/

   // Constructor.  This will create a database from various sources, and parse
   // and remove the relevant arguments from the given command line.
   XxXrmParser( 
      void*       display,
      const char* appClassName, 
      int&        argc, 
      char**      argv
   );

   // Destructor.
   virtual ~XxXrmParser();

   // Query the database for the named resource.  Returns false is not present.
   // Otherwise return true and fills in the value string.
   virtual bool query( 
      XxResources::Resource resource,
      const char*           name,
      std::string&          value
   );

private:

   /*----- data members -----*/

   std::string		_className;
   void*		_database;

};


XX_NAMESPACE_END

#endif

#endif

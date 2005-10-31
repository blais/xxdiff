/******************************************************************************\
 * $Id: stringResParser.h 2 2000-09-15 02:19:22Z blais $
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

#ifndef INCL_XXDIFF_STRINGRESPARSER
#define INCL_XXDIFF_STRINGRESPARSER

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

#ifndef INCL_STD_MAP
#include <map>
#define INCL_STD_MAP
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * CLASS XxStringResParser
 *============================================================================*/

// <summary> resource parser for strings </summary>

// This parser can accumulate resources in the same form as a single line given
// in the rcfile format.

class XxStringResParser : public XxResourcesParser {

public:

   /*----- member functions -----*/

   // Constructor.
   XxStringResParser();

   // Destructor.
   virtual ~XxStringResParser();

   // Query the database for the named resource.  Returns false is not present.
   // Otherwise return true and fills in the value string.
   virtual bool query( 
      XxResources::Resource resource,
      const char*           name,
      std::string&          value
   );

   // Add a new string to the resources parser.
   void addString(
      const std::string& str
   );

private:

   /*----- types and enumerations -----*/

   typedef std::map< std::string, ResValue, Rltstr > ResourceMap;

   /*----- data members -----*/

   ResourceMap 	_resources;
   std::string	_valueText;
};


XX_NAMESPACE_END

#endif

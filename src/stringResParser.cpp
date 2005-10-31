/******************************************************************************\
 * $Id: stringResParser.cpp 250 2001-10-04 19:56:59Z blais $
 * $Date: 2001-10-04 15:56:59 -0400 (Thu, 04 Oct 2001) $
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

#include <defs.h>

#include <stringResParser.h>
#include <exceptions.h>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

/*----- classes -----*/

/*==============================================================================
 * CLASS XxStringResError
 *============================================================================*/

class XxStringResError : public std::exception {

public:

   /*----- member functions -----*/

   // Constructor from a resource string.
   XxStringResError( const QString& res );

   // Destructor.
   virtual ~XxStringResError() XX_THROW_NOTHING;

   // See base class.
   virtual const char* what() const XX_THROW_NOTHING;

private:

   /*----- data members -----*/

   QString _msg;

};

//------------------------------------------------------------------------------
//
XxStringResError::XxStringResError( const QString& res )
{
   QTextOStream oss( &_msg );
   oss << "Error parsing string \"" << res << "\"" << endl;
}

//------------------------------------------------------------------------------
//
XxStringResError::~XxStringResError() XX_THROW_NOTHING
{}

//------------------------------------------------------------------------------
//
const char* XxStringResError::what() const XX_THROW_NOTHING
{
   return _msg.latin1();
}

}



XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxStringResParser
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxStringResParser::XxStringResParser() :
   _resources()
{
}

//------------------------------------------------------------------------------
//
XxStringResParser::~XxStringResParser()
{
   // Warn about unused resources.
   for ( ResourceMap::const_iterator iter = _resources.begin();
         iter != _resources.end();
         ++iter ) {
      if ( (*iter).second._used == false ) {
         std::cerr << "xxdiff warning: ignored resource: "
                   << (*iter).first
                   << " : " 
                   << (*iter).second._value << std::endl;
      }
   }
}

//------------------------------------------------------------------------------
//
void XxStringResParser::addString(
   const QString& str
)
{
   typedef int PosType;
   const PosType notfound = -1;

   PosType pos = str.find( ':' );
   if ( pos == notfound ) {
      throw XxStringResError( str );
   }
   QString resname = str.mid( 0, pos ).stripWhiteSpace();
   QString resvalue = str.mid( pos + 1 ).stripWhiteSpace();

   ResValue& rv = _resources[ resname ];
   rv._value = resvalue;
   rv._used = false;
}

//------------------------------------------------------------------------------
//
bool XxStringResParser::query( 
   XxResources::Resource /*resource*/,
   const QString&        name,
   QString&              value
)
{
   ResourceMap::iterator iter = _resources.find( name );
   if ( iter == _resources.end() ) {
      return false;
   }
   value = (*iter).second._value;
   (*iter).second._used = true;
   return true;
}


XX_NAMESPACE_END

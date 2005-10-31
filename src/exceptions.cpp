/******************************************************************************\
 * $Id: exceptions.cpp 138 2001-05-20 18:08:45Z blais $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <exceptions.h>
#include <help.h>

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


XX_NAMESPACE_BEGIN
using namespace std;


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxUsageError
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxUsageError::XxUsageError( bool benine, bool version ) :
   _benine( benine )
{
   ostringstream oss;
   if ( version == false ) {
      XxHelp::dumpUsage( oss );
   }
   else {
      XxHelp::dumpVersion( oss );      
   }
   oss << ends;
   _msg = oss.str();
}

//------------------------------------------------------------------------------
//
XxUsageError::~XxUsageError() XX_THROW_NOTHING
{}

//------------------------------------------------------------------------------
//
const char* XxUsageError::what() const XX_THROW_NOTHING
{
   return _msg.c_str();
}

//------------------------------------------------------------------------------
//
bool XxUsageError::isBenine() const
{
   return _benine;
}


/*==============================================================================
 * CLASS XxIoError
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxIoError::XxIoError()
{
   std::ostringstream oss;
   oss << "System error: " << strerror( errno ) << endl << ends;
   _msg = oss.str();
}

//------------------------------------------------------------------------------
//
XxIoError::XxIoError( const char* msg )
{
   _msg = msg;
}

//------------------------------------------------------------------------------
//
XxIoError::~XxIoError() XX_THROW_NOTHING
{}

//------------------------------------------------------------------------------
//
const char* XxIoError::what() const XX_THROW_NOTHING
{
   return _msg.c_str();
}


/*==============================================================================
 * CLASS XxInternalError
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxInternalError::XxInternalError( const char* file, int line )
{
   ostringstream oss;
   oss << "Internal error at: "
       << "  file=" << file 
       << "  line=" << line << std::endl;
   oss << std::endl;
   oss << "There has been an internal error within xxdiff." << std::endl
       << "To report bugs, please use the sourceforge bug tracker" << std::endl
       << "at http://sourceforge.net/tracker/?group_id=2198" << std::endl
       << "and log the above information above and if possible," << std::endl
       << "the files that caused the error, and as much detail as" << std::endl
       << "you can to reproduce the error." << std::endl;
   _msg = oss.str();
}

//------------------------------------------------------------------------------
//
XxInternalError::~XxInternalError() XX_THROW_NOTHING
{}

//------------------------------------------------------------------------------
//
const char* XxInternalError::what() const XX_THROW_NOTHING
{

   return _msg.c_str();
}

XX_NAMESPACE_END


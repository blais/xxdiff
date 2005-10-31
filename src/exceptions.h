/******************************************************************************\
 * $Id: exceptions.h 2 2000-09-15 02:19:22Z blais $
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

#ifndef INCL_XXDIFF_EXCEPTIONS
#define INCL_XXDIFF_EXCEPTIONS

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_STD_EXCEPTION
#include <exception>
#define INCL_STD_EXCEPTION
#endif

#ifndef INCL_STD_STRING
#include <string>
#define INCL_STD_STRING
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * CLASS XxUsageError
 *============================================================================*/

// <summary> This class is used to indicate a usage error, and also to print out
// the version message </summary>

class XxUsageError : public std::exception {

public:

   /*----- member functions -----*/

   // Constructor.  Benine indicates if this exception should return a nice
   // error code.  Version indicates if this exception should spew out the
   // version messages instead of the usage.
   XxUsageError( bool benine = false, bool version = false );
   virtual ~XxUsageError() XX_THROW_NOTHING;


   // See base class.
   virtual const char* what() const XX_THROW_NOTHING;

   // Returns true if this is a benine exception.
   bool isBenine() const;

private:

   /*----- data members -----*/

   bool		_benine;
   std::string	_msg;

};


/*==============================================================================
 * CLASS XxIoError
 *============================================================================*/

class XxIoError : public std::exception {

public:

   /*----- member functions -----*/

   // Constructor from current errno.
   XxIoError();

   // Constructor with a string.
   XxIoError( const char* msg );

   // Destructor.
   virtual ~XxIoError() XX_THROW_NOTHING;

   // See base class.
   virtual const char* what() const XX_THROW_NOTHING;


private:

   /*----- data members -----*/

   std::string _msg;

};


/*==============================================================================
 * CLASS XxInternalError
 *============================================================================*/

// <summary> an internal error that has been detected </summary>

#define XX_INTERROR_PARAMS	__FILE__, __LINE__

class XxInternalError : public std::exception {

public:

   /*----- member functions -----*/

   // Constructor.  Use above macro to fill in the parameters.
   XxInternalError( const char*, int );

   // Destructor.
   virtual ~XxInternalError() XX_THROW_NOTHING;

   // See base class.
   virtual const char* what() const XX_THROW_NOTHING;

private:

   /*----- data members -----*/

   std::string _msg;

};


XX_NAMESPACE_END

#endif

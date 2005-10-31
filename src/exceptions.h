/******************************************************************************\
 * $Id: exceptions.h 257 2001-10-08 04:28:33Z blais $
 * $Date: 2001-10-08 00:28:33 -0400 (Mon, 08 Oct 2001) $
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

#ifndef INCL_XXDIFF_EXCEPTIONS
#define INCL_XXDIFF_EXCEPTIONS

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_QT_QSTRING
#include <qstring.h>
#define INCL_QT_QSTRING
#endif

#ifndef INCL_QT_QTEXTSTREAM
#include <qtextstream.h>
#define INCL_QT_QTEXTSTREAM
#endif

#ifndef INCL_STD_EXCEPTION
#include <exception>
#define INCL_STD_EXCEPTION
#endif

#ifndef INCL_STD_STDEXCEPT
#include <stdexcept>
#define INCL_STD_STDEXCEPT
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC DECLARATIONS
 *============================================================================*/

#define XX_EXC_PARAMS	__FILE__, __LINE__
#define XX_EXC_PARAMS_DECL(file,line)  const QString& file, const int line

/*==============================================================================
 * CLASS XxError
 *============================================================================*/

// <summary> Base class for all xxdiff exceptions/errors </summary>

class XxError {

public:

   /*----- member functions -----*/

   XxError( 
      XX_EXC_PARAMS_DECL(file,line),
      const QString& msg = QString::null 
   );
   virtual ~XxError() XX_THROW_NOTHING;

   // See base class.
   virtual const QString& getMsg() const XX_THROW_NOTHING;

protected:

   /*----- data members -----*/

   QString _msg;

};

/*==============================================================================
 * CLASS XxUsageError
 *============================================================================*/

// <summary> This class is used to indicate a usage error, and also to print out
// the version message </summary>

class XxUsageError : public XxError, 
                     public std::domain_error {

public:

   /*----- member functions -----*/

   // Constructor.  Benine indicates if this exception should return a nice
   // error code.  Version indicates if this exception should spew out the
   // version messages instead of the usage.
   XxUsageError( 
      XX_EXC_PARAMS_DECL(file,line),
      const QString& msg = QString::null,
      bool           benine = false,
      bool           version = false 
   );

   // Returns true if this is a benine exception.
   bool isBenine() const;

private:

   /*----- data members -----*/

   bool    _benine;

};


/*==============================================================================
 * CLASS XxIoError
 *============================================================================*/

class XxIoError : public XxError,
                  public std::runtime_error {

public:

   /*----- member functions -----*/

   // Constructor with a string.
   XxIoError( 
      XX_EXC_PARAMS_DECL(file,line),
      const QString& msg = QString::null // use errno string only.
   );

};

/*==============================================================================
 * CLASS XxInternalError
 *============================================================================*/

// <summary> an internal error that has been detected </summary>

class XxInternalError : public XxError,
                        public std::runtime_error {

public:

   /*----- member functions -----*/

   // Constructor.  Use above macro to fill in the parameters.
   XxInternalError( 
      XX_EXC_PARAMS_DECL(file,line)
   );

};


XX_NAMESPACE_END

#endif


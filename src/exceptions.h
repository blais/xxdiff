
/* -*- c-file-style: "xxdiff" -*- */
/******************************************************************************\
 * $RCSfile$
 *
 * Copyright (C) 1999-2003  Martin Blais <blais@furius.ca>
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
 ******************************************************************************/

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
 * PUBLIC FUNCTIONS
 *============================================================================*/

// Issues a text warning (no popup windows).
void issueWarning( const QString& );

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
   // error code.
   XxUsageError( 
      XX_EXC_PARAMS_DECL(file,line),
      const QString& msg = QString::null,
      int            helpMask = 0
   );
   virtual ~XxUsageError() XX_THROW_NOTHING {}

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
   virtual ~XxIoError() XX_THROW_NOTHING {}

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
   virtual ~XxInternalError() XX_THROW_NOTHING {}

};

XX_NAMESPACE_END

#endif


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

#ifndef INCL_XXDIFF_RESPARSER
#define INCL_XXDIFF_RESPARSER

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_RESOURCES
#include <resources.h>
#endif

#ifndef INCL_QT_QSTRING
#include <qstring.h>
#define INCL_QT_QSTRING
#endif

#ifndef INCL_QT_QTEXTSTREAM
#include <qtextstream.h>
#define INCL_QT_QTEXTSTREAM
#endif

#ifndef INCL_STD_MAP
#include <map>
#define INCL_STD_MAP
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxResParser
 *============================================================================*/

// <summary> resource parser for resource </summary>

class XxResParser {

public:

   /*----- static member functions -----*/

   // Initialize tables.
   static void initialize();

   // Returns the xxdiffrc filename, or a null string if none can be found.
   static QString getRcFilename();

   // Returns the name or description of a color resource.
   // <group>
   static QString getColorName( XxColor color );
   static QString getColorDescription( XxColor color );
   // </group>

   // Compare given resources objects and output differences to the given stream
   // in a format that can be parsed by this parser.  The values are taken from
   // res1.
   static void genInitFile(
      const XxResources& res1,
      const XxResources& res2,
      QTextStream&       os
   );

   // Print out the resource list, default values and documentation.
   static void listResources( QTextStream& );

   // Returns resource reference help, formatted with HTML.
   static QString getResourceRef();

   // Special tags for cmdline resources.
   // <group>
   static QString getKwdName( int kwd );
   static QString getShowOptName( XxShowOpt bo );
   static QString getBoolOptName( XxBoolOpt bo );
   // </group>

   /*----- member functions -----*/

   // Constructor.
   XxResParser();

   // Run with filename into given resources.
   void parse( const QString& filename, XxResources& resources );

   // Run with stream into given resources.
   void parse( QTextStream& is, XxResources& resources );

   // Destructor.
   virtual ~XxResParser();

   /*----- static data members -----*/

   static const int BOOLKWD_BASE = 1000;

};

XX_NAMESPACE_END

#endif


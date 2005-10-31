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

#ifndef INCL_XXDIFF_DIFFUTILS
#define INCL_XXDIFF_DIFFUTILS

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

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxDiffutils
 *============================================================================*/

// <summary> interface class to diffutils compiled against xxdiff </summary>

class XxDiffutils {

public:

   /*----- member functions -----*/

   XxDiffutils();
   virtual ~XxDiffutils();

   // Run diff.
   void diff( int argc, char** out_args );

   // Run diff3.
   void diff3( int argc, char** out_args );

   // Reads a line.  A null string will be returned on EOF.
   QString readLine();

   
private:

   /*----- member functions -----*/

   void diff_fun( 
      int argc,
      char** out_args, 
      int(*main)(int, char**) 
   );

   /*----- data members -----*/

   QString      _output;
   QByteArray   _outputB;
   QTextStream* _istream;

};


XX_NAMESPACE_END

#endif


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

#ifndef INCL_XXDIFF_LINENUMBERS
#define INCL_XXDIFF_LINENUMBERS

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_TYPES
#include <types.h>
#endif

#ifndef INCL_QT_QFRAME
#include <qframe.h>
#define INCL_QT_QFRAME
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;
class XxCentralFrame;

/*==============================================================================
 * CLASS XxLineNumbers
 *============================================================================*/

// <summary> a widget that can display line numbers </summary>

class XxLineNumbers : public QFrame {

public:

   /*----- member functions -----*/

   // Constructor.  no is which file this widget is supposed to display (-1 for
   // none).
   XxLineNumbers( 
      XxApp*          app, 
      XxCentralFrame* central, 
      const XxFno     no = -1,
      QWidget*        parent = 0,
      const char*     name = 0 
   );

   // Destructor.
   virtual ~XxLineNumbers();

   // See base class.
   virtual void drawContents( QPainter* );


private:

   /*----- data members -----*/

   XxApp*          _app;
   XxCentralFrame* _central;
   const int       _no;

};

XX_NAMESPACE_END

#endif

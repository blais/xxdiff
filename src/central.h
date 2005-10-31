/******************************************************************************\
 * $Id: central.h 423 2001-11-29 05:32:12Z blais $
 * $Date: 2001-11-29 00:32:12 -0500 (Thu, 29 Nov 2001) $
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

#ifndef INCL_XXDIFF_CENTRAL
#define INCL_XXDIFF_CENTRAL

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_TYPES
#include <types.h>
#endif

#ifndef INCL_QT_QMAINWINDOW
#include <qmainwindow.h>
#define INCL_QT_QMAINWINDOW
#endif

#ifndef INCL_QT_QFRAME
#include <qframe.h>
#define INCL_QT_QFRAME
#endif

#ifndef INCL_QT_QWIDGET
#include <qwidget.h>
#define INCL_QT_QWIDGET
#endif

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QScrollBar;

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;
class XxCentralText;
class XxCentralFrame;
class XxCentralWindow;

/*==============================================================================
 * CLASS XxCentralFrame
 *============================================================================*/

// <summary> central text with scrollbars </summary>

class XxCentralFrame : public QWidget {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef QWidget BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxCentralFrame( 
      XxApp*      app, 
      QWidget*    parent = 0,
      const char* name = 0 
   );

   // Access to the scrollbars.
   QScrollBar* getHorizontalScrollbar();
   QScrollBar* getVerticalScrollbar();

   // See base class.
   virtual void update();

public slots:

   void appCursorChanged( int );
   void appScrolled( int );

private:

   /*----- data members -----*/

   XxCentralText* _central;
   QScrollBar*    _vscroll;
   QScrollBar*    _hscroll;

};

XX_NAMESPACE_END

#endif

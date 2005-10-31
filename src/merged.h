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

#ifndef INCL_XXDIFF_MERGED
#define INCL_XXDIFF_MERGED

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_TYPES
#include <types.h>
#endif

#ifndef INCL_XXDIFF_SCROLLVIEW
#include <scrollView.h>
#endif

#ifndef INCL_XXDIFF_KDESUPPORT
#include <kdeSupport.h>
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
class XxText;

/*==============================================================================
 * CLASS XxMergedFrame
 *============================================================================*/

// <summary> merged text with scrollbars </summary>

class XxMergedFrame : public XxScrollView {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef XxScrollView BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxMergedFrame( 
      XxApp*          app, 
      QWidget*        parent = 0,
      const char*     name = 0 
   );

   // See base class.
   virtual void update();

   // See base class XxScrollView.
   // <group>
   virtual QSize computeDisplaySize() const;
   virtual uint computeTextLength() const;
   virtual XxDln getNbDisplayLines() const;
   // </group>
   
public slots:

   // Reacts to a cursor change.
   void onCursorChanged( int cursorLine );

private:

   /*----- data members -----*/

   XxText* _text;

};

/*==============================================================================
 * CLASS XxMergedWindow
 *============================================================================*/

// <summary> a main window with merged frame inside, and menus </summary>

class XxMergedWindow : public QkMainWindow {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef QkMainWindow BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxMergedWindow( 
      XxApp*          app, 
      QWidget*        parent = 0,
      const char*     name = 0 
   );

   // See base class.
   // <group>
   virtual void update();
   virtual void show();
   virtual void hide();
   // </group>


private:

   /*----- data members -----*/

   XxMergedFrame* _frame;
   XxApp*         _app;

};

XX_NAMESPACE_END

#endif

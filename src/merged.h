/******************************************************************************\
 * $Id: merged.h 48 2000-10-03 04:43:36Z  $
 * $Date: 2000-10-03 00:43:36 -0400 (Tue, 03 Oct 2000) $
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

#ifndef INCL_XXDIFF_MERGED
#define INCL_XXDIFF_MERGED

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
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
class XxMergedText;
class XxMergedFrame;
class XxMergedWindow;

/*==============================================================================
 * CLASS XxMergedText
 *============================================================================*/

// <summary> a text widget that displays the merged diffs </summary>

class XxMergedText : public QFrame {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef QFrame BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxMergedText( 
      XxMergedFrame*  main,
      XxApp*          app,
      QWidget*        parent = 0,
      const char*     name = 0
   );

   // Destructor.
   virtual ~XxMergedText();

   // See base class.
   // Override to expand.
   virtual QSizePolicy sizePolicy() const;

   // See base class.
   virtual void drawContents( QPainter* );

   // Get/set the top visible line.
   // <group>
   void setTopLine( uint lineNo ) const;
   uint getTopLine() const;
   // </group>

   // Get/set the center line.
   // <group>
   void setCenterLine( uint lineNo );
   // </group>

public slots:

   void verticalScroll( int );
   void horizontalScroll( int );
   void adjustVertically();


protected:

   /*----- member functions -----*/

   // See base class.
   // <group>
   virtual void mousePressEvent( QMouseEvent* );
   virtual void mouseMoveEvent( QMouseEvent* );
   virtual void mouseReleaseEvent( QMouseEvent* );
   // </group>
   virtual void resizeEvent( QResizeEvent* );
   // </group>

private:

   /*----- types and enumerations -----*/

   enum { HEIGHT_DELETED_REGION = 2 };

   /*----- data members -----*/

   XxMergedFrame* _main;
   XxApp*         _app;
   uint           _nbDisplayLines;
   int            _topLine;
   bool           _grab;
   int            _grabTopLine;
   int            _grabDeltaLineNo;

};

/*==============================================================================
 * CLASS XxMergedFrame
 *============================================================================*/

// <summary> merged text with scrollbars </summary>

class XxMergedFrame : public QWidget {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef QWidget BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxMergedFrame( 
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

   XxMergedText* _merged;
   QScrollBar*   _vscroll;
   QScrollBar*   _hscroll;

};

/*==============================================================================
 * CLASS XxMergedWindow
 *============================================================================*/

// <summary> a main window with merged frame inside, and menus </summary>

class XxMergedWindow : public QMainWindow {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef QMainWindow BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxMergedWindow( 
      XxApp*          app, 
      QWidget*        parent = 0,
      const char*     name = 0 
   );

   // See base class.
   virtual void update();

private:

   /*----- data members -----*/

   XxMergedFrame* _frame;

};


XX_NAMESPACE_END

#endif

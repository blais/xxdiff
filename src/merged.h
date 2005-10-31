/******************************************************************************\
 * $Id: merged.h 2 2000-09-15 02:19:22Z blais $
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


/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QScrollBar;


XX_NAMESPACE_BEGIN


/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;
class XxMerged;


/*==============================================================================
 * CLASS XxMergedWindow
 *============================================================================*/

// <summary> the main window of the merged view </summary>

class XxMergedWindow : public QMainWindow {

   Q_OBJECT

public:

   /*----- member functions -----*/

   // Constructor.
   XxMergedWindow( 
      XxApp*          app, 
      QWidget*        parent = 0,
      const char*     name = 0 
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

   XxMerged*   _merged;
   QScrollBar* _vscroll;
   QScrollBar* _hscroll;

};


/*==============================================================================
 * CLASS XxMerged
 *============================================================================*/

// <summary> a text widget that displays the merged diffs </summary>

class XxMerged : public QFrame {

   Q_OBJECT

public:

   /*----- member functions -----*/

   // Constructor.
   XxMerged( 
      XxMergedWindow* main, 
      XxApp*          app, 
      QWidget*        parent = 0,
      const char*     name = 0 
   );

   // Destructor.
   virtual ~XxMerged();

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

   XxMergedWindow* _main;
   XxApp*          _app;
   uint            _nbDisplayLines;
   int             _topLine;
   bool            _grab;
   int             _grabTopLine;
   int             _grabDeltaLineNo;

};


XX_NAMESPACE_END

#endif

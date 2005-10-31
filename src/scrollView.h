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

#ifndef INCL_XXDIFF_SCROLLVIEW
#define INCL_XXDIFF_SCROLLVIEW

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_TYPES
#include <types.h>
#endif

#ifndef INCL_QT_QWIDGET
#include <qwidget.h>
#define INCL_QT_QWIDGET
#endif

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QScrollBar;
class QSize;

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;

/*==============================================================================
 * CLASS XxScrollView
 *============================================================================*/

// <summary> interface class for scrollable views of the main text </summary>

// This class defines an interface for getting/setting any scrollable view of
// the main text.  For example, the overview area will be monitoring one of the
// XxScrollView instances (probably the center area widget), or the text
// renderers will use one instance of this to perform their rendering.

class XxScrollView : public QWidget {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef QWidget BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxScrollView( 
      XxApp*      app, 
      QWidget*    parent = 0,
      const char* name = 0 
   );

   // Destructor.
   virtual ~XxScrollView();

   // Computes the text display width in pixels.
   virtual QSize computeDisplaySize() const = 0;

   // Computes the number of lines in the text to display.
   virtual uint computeTextLength() const = 0;

   // Returns the number of fully visible display lines that this display can
   // currently see.
   virtual XxDln getNbDisplayLines() const = 0;

   // Returns the top/center/bottom/current display line.
   // <group>
   virtual XxDln getTopLine() const;
   virtual XxDln getCenterLine() const;
   virtual XxDln getBottomLine() const;
   // </group>

   // Requests for a certain top line to be displayed.  The actual top line that
   // will be displayed might not be the one that is requested, due to the fact
   // that we want to avoid letting the user scroll past the end of the file, by
   // resizing or otherwise).  The actual top line that was computed is
   // returned.  Out-of-bounds values are permitted.
   //
   // The center and bottom methods work similarly.
   // <group>
   virtual XxDln setTopLine( const XxDln lineNo );
   virtual XxDln setCenterLine( const XxDln lineNo );
   virtual XxDln setBottomLine( const XxDln lineNo );
   // </group>

   // Centers the display so that the cursor is displayed as close to the center
   // line as possible.
   virtual void recenter();

   // Returns the horizontal position of the horizontal scrollbar in pixels.
   virtual uint getHorizontalPos() const;

public slots:

   // Adjusts the scrollbars for a change in either widget size or text size.
   void adjustScrollbars();

protected slots:

   // Horizontal scrollbar callback.
   void horizontalScroll( int );

   // Vertical scrollbar callback.
   // <group>
   void verticalScroll( int );
   void verticalScroll2( int );
   // </group>

protected:

   /*----- member functions -----*/

   // Virtual functions to adjust the scrollbars (see adjustScrollbars()).
   // <group>
   virtual void adjustHorizontalScrollbars( const QSize& displaySize );
   virtual void adjustVerticalScrollbars( const QSize& displaySize );
   // </group>

   // See base class.
   virtual void wheelEvent( QWheelEvent * );

   /*----- data members -----*/

   XxApp*      _app;
   QScrollBar* _vscroll[2];
   QScrollBar* _hscroll;

private:

   /*----- data members -----*/

   // Cached values for display width/height and text width/height.
   // <group>
   uint        _displayWidth;
   uint        _displayHeight;
   uint        _textWidth; // in chars
   uint        _textHeight; // in lines
   // </group>

};

XX_NAMESPACE_END

#endif

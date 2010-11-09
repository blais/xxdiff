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

#ifndef INCL_XXDIFF_TEXT
#define INCL_XXDIFF_TEXT

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
#include <QtGui/QFrame>
#define INCL_QT_QFRAME
#endif

#ifndef INCL_STD_VECTOR
#include <vector>
#define INCL_STD_VECTOR
#endif

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QResizeEvent;
class QWheelEvent;
class QMouseEvent;

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;
class XxScrollView;

/*==============================================================================
 * CLASS XxText
 *============================================================================*/

// <summary> a text widget that can display diffs </summary>

class XxText : public QFrame {
   
   Q_OBJECT

public:

   /*----- member functions -----*/

   // Constructor.  no is which file this text is supposed to display.  If you
   // use -1, this text will display the merged results.
   XxText( 
      XxApp*        app,
      XxScrollView* sv,
      const XxFno   no = -1,
      QWidget*      parent = 0
   );

   // Destructor.
   virtual ~XxText();

   // See base class.
   // Override to expand.
   virtual QSizePolicy sizePolicy() const;

   // See base class.
   virtual void paintEvent( QPaintEvent* );

   // Returns the number of lines that can be displayed.
   uint computeDisplayLines() const;

   // Returns the width of pixels that can display text.
   uint getDisplayWidth() const;

   // Computes approx. number of lines that would be needed to be fit inside a
   // merged view, and keep track of line positions.
   uint computeMergedLines();
   
   // Returns approx. number of lines that would be needed to be fit inside a
   // merged view.
   uint getMergedLines() const;

   // Returns the line number in the merged view for a given line number.
   XxDln getMergedLineFromLine( XxDln line ) const;

   // Returns the line number for a given line number in the merged view.
   XxDln getLineFromMergedLine( XxDln mergedLine ) const;

public slots:

   virtual void clearRegionSelect();
   
protected:

   /*----- member functions -----*/

   // See base class.
   // <group>
   virtual void mousePressEvent( QMouseEvent* );
   virtual void mouseMoveEvent( QMouseEvent* );
   virtual void mouseReleaseEvent( QMouseEvent* );
   virtual void mouseDoubleClickEvent( QMouseEvent* );
   virtual void wheelEvent( QWheelEvent* );
   virtual void resizeEvent( QResizeEvent* );
   // </group>

private:

   /*----- types and enumerations -----*/

   enum GrabMode { NONE, MOUSE_DRAG, REGION_SELECT };

   /*----- static member functions -----*/

   // Returns true if this text is displaying merged results.
   bool isMerged() const;

   static QString formatClipboard(
      const QString& clipboardFormat,
      const XxFno    fileno,
      const XxFln    lineno,
      const QString& filename,
      const QString& lineContents
   );

   // Returns the region text, formatted.
   QString getRegionText( XxDln start, XxDln end ) const;

   /*----- data members -----*/

   XxApp*        _app;
   XxScrollView* _sv;
   const XxFno   _no;
   GrabMode      _grabMode;
   XxDln         _grabTopLine;
   int           _grabDeltaLineNo;
   int           _regionSelect[2]; // Selected text region for review.
   bool          _dontClearOnce; // one-time flag for our own clipboard calls.
   uint          _mergedLines; // approx. number of lines that would be needed to be fit inside a merged view
   uint          _idxMergedLinesSize; // allocated size of _idxMergedLines
   XxDln*        _idxMergedLines; // Keep track of merged line positions.

};


XX_NAMESPACE_END

#include <text.inline.h>

#endif

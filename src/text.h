/******************************************************************************\
 * $Id: text.h 376 2001-11-13 05:13:06Z blais $
 * $Date: 2001-11-13 00:13:06 -0500 (Tue, 13 Nov 2001) $
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
#include <qframe.h>
#define INCL_QT_QFRAME
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;


/*==============================================================================
 * CLASS XxText
 *============================================================================*/

// <summary> a text widget that can display diffs </summary>

class XxText : public QFrame {

public:

   /*----- member functions -----*/

   // Constructor.  no is which file this text is supposed to display (-1 for
   // none).
   XxText( 
      XxApp*      app, 
      const XxFno no = -1,
      QWidget*    parent = 0,
      const char* name = 0 
   );

   // Destructor.
   virtual ~XxText();

   // See base class.
   // Override to expand.
   virtual QSizePolicy sizePolicy() const;

   // See base class.
   virtual void drawContents( QPainter* );

   // Returns the number of lines that can be displayed.
   XxDln computeDisplayLines() const;

   // Returns the width of pixels that can display text.
   uint getDisplayWidth() const;

protected:

   /*----- member functions -----*/

   // See base class.
   // <group>
   virtual void mousePressEvent( QMouseEvent* );
   virtual void mouseMoveEvent( QMouseEvent* );
   virtual void mouseReleaseEvent( QMouseEvent* );
   virtual void mouseDoubleClickEvent( QMouseEvent* );
   virtual void resizeEvent( QResizeEvent* );
   // </group>

private:

   /*----- static member functions -----*/

   static QString formatClipboardLine(
      const QString& clipboardFormat,
      const XxFno    fileno,
      const XxFln    lineno,
      const QString& filename,
      const QString& lineContents
   );

   /*----- data members -----*/

   XxApp*      _app;
   const XxFno _no;
   bool        _grab;
   XxDln       _grabTopLine;
   int         _grabDeltaLineNo;

};


XX_NAMESPACE_END

#endif


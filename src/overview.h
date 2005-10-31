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

#ifndef INCL_XXDIFF_OVERVIEW
#define INCL_XXDIFF_OVERVIEW

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
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
 * CLASS XxOverview
 *============================================================================*/

// <summary> overview area </summary>

class XxOverview : public QFrame {

public:

   /*----- types and enumerations -----*/

   typedef QFrame BaseClass;

   /*----- member functions -----*/

   // Constructor.
   XxOverview(
      XxApp*          app,
      XxCentralFrame* central,
      QWidget *       parent = 0,
      const char*     name = 0
   );

   // Destructor.
   virtual ~XxOverview();

   // See base class.
   virtual void drawContents( QPainter* );

   // Override to expand.
   virtual QSizePolicy sizePolicy() const;

   // Recomputes the vertical extents, either because the widget size has
   // changed or the file lengths may have changed.
   void updateVerticalExtents();

   // See base class.
   virtual void update();

protected:

   /*----- member functions -----*/

   // See base class.
   // <group>
   virtual void mousePressEvent( QMouseEvent* );
   virtual void mouseMoveEvent( QMouseEvent* );
   virtual void mouseReleaseEvent( QMouseEvent* );
   virtual void resizeEvent( QResizeEvent* );
   virtual void wheelEvent( QWheelEvent * );
   // </group>

private:

   /*----- data members -----*/

   XxApp*          _app;
   XxCentralFrame* _central;

   // Data computed upon resizing.
   int             _fileL[3]; // Left borders of files (in pixels).
   int             _fileR[3]; // Right borders of file (in pixels).
   int             _fileT[3]; // Top of files (in pixels).
   int             _fileB[3]; // Bottom of files (in pixels).
   int             _fileDy[3]; // Length of files (in pixels).

   int             _regL[3]; // Left borders of visible region (in pixels).
   int             _regR[3]; // Right borders of visible region (in pixels).

   int             _manipNo;
   int             _manipTopLine;
   int             _manipFlines;
   int             _manipAnchor;
   
};

XX_NAMESPACE_END

#endif


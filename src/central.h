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

#ifndef INCL_XXDIFF_SCROLLVIEW
#include <scrollView.h>
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

class QLabel;
class QScrollBar;

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;
class XxLineNumbers;
class XxText;

/*==============================================================================
 * CLASS XxCentralFrame
 *============================================================================*/

// <summary> central text with scrollbars </summary>

class XxCentralFrame : public XxScrollView {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef XxScrollView BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxCentralFrame( 
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
   
   // Hides or shows the filenames labels.
   void showFilenames( const bool show );

   // Sets the filename displayed in one of the filenames labels.
   void setFilename( XxFln no, const QString& fn );

   // Returns the width and height that one of the text widgets would have if it
   // was adorned with a scrollbar on each side.  This is used specifically by
   // the merged view.
   // <group>
   int getOneWidth() const;
   int getOneHeight() const;
   // </group>

public slots:

   // Updates the line number labels with current cursor line.
   void updateLineNumberLabels( int );

   // Updates the line numbers widgets sizes and shows or hides them depending
   // on the current state.
   void adjustLineNumbers( bool show, const QFont& fontText );

   // Reacts to a cursor change.
   void onCursorChanged( int cursorLine );

   // Keyboard accelerators scrolling callbacks.
   // This is only defined on the central widget.
   // <group>
   void scrollRight();
   void scrollLeft();
   // </group>

protected:

   /*----- member functions -----*/

   // See base class.
   virtual void verticalScroll( int /*value*/ );

private:

   /*----- member functions -----*/

   // Set what's-this strings.
   void createOnContextHelp();

   /*----- data members -----*/

   QLabel*        _filenameLabel[3];
   QLabel*        _lineNumberLabel[3];
   XxLineNumbers* _lineNumbers[3];
   XxText*        _text[3];

};

XX_NAMESPACE_END

#endif

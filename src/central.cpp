/******************************************************************************\
 * $Id: central.cpp 423 2001-11-29 05:32:12Z blais $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <central.h>
#include <text.h>
#include <app.h>
#include <resources.h>
#include <diffs.h>
#include <buffer.h>

#include <qscrollview.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qlayout.h>

#include <qapplication.h>
#include <qclipboard.h>

#include <math.h>
#include <stdio.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxCentralFrame
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxCentralFrame::XxCentralFrame( 
   XxApp*      app, 
   QWidget*    parent, 
   const char* name 
) :
   BaseClass( parent, name )
{
   QVBoxLayout* vlayout = new QVBoxLayout( this );
   QHBoxLayout* hlayout = new QHBoxLayout( vlayout );

   _central = new XxCentralText( this, app, this );
   hlayout->addWidget( _central );

   _vscroll = new QScrollBar( this );
   _vscroll->setFixedWidth( 20 );
   hlayout->addWidget( _vscroll );

   _hscroll = new QScrollBar( Qt::Horizontal, this );
   _hscroll->setFixedHeight( 20 );

   vlayout->addWidget( _hscroll );

   connect( _vscroll, SIGNAL(valueChanged(int)),
            _central, SLOT(verticalScroll(int)) );
   connect( _hscroll, SIGNAL(valueChanged(int)),
            _central, SLOT(horizontalScroll(int)) );

   // Track application's scrolling window.
   connect( app, SIGNAL(cursorChanged(int)),
            this, SLOT(appCursorChanged(int)) );
   connect( app, SIGNAL(scrolled(int)), this, SLOT(appScrolled(int)) );
}

//------------------------------------------------------------------------------
//
QScrollBar* XxCentralFrame::getHorizontalScrollbar()
{
   return _hscroll;
}

//------------------------------------------------------------------------------
//
QScrollBar* XxCentralFrame::getVerticalScrollbar()
{
   return _vscroll;
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::update()
{
   BaseClass::update();
   _central->update();  // We need this (verified Mon Nov 26 21:12:10 EST 2001),
                       // otherwise the central view does not update when the
                       // cursor does not change but the selection changes.
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::appCursorChanged( int cursorLine )
{
   // When cursor changes, try to track it at the center line of the central
   // view.  Important note: this will try to set as the center line what SHOULD
   // be the center line if the diffs were all expanded.  Thus the region where
   // the cursor would show up in the central view will most of the time be
   // higher than the center, but never above the top of the window.  This is a
   // good heuristic, since it allows you to see more of the region when
   // collapsing/expanding (IOW if the central view was really centered that
   // wouldn't be very good because the top half wouldn't be very useful (you do
   // need some though, for context).  If you don't understand all this shtuff,
   // just think that this is why the central view isn't really centered on the
   // cursor.
   _central->setCenterLine( cursorLine );
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::appScrolled( int /*topLine*/ )
{
   // Use this to track the top line of the application.
   // _central->setTopLine( topLine );
}

XX_NAMESPACE_END

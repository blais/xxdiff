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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <merged.h>
#include <text.h>
#include <app.h>
#include <resources.h>
#include <diffs.h>
#include <buffer.h>

#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QMenu>
#include <QMenuBar>
#include <QLayout>
#include <QCloseEvent>

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <math.h>
#include <stdio.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxMergedFrame::XxMergedFrame( 
   XxApp*      app, 
   QWidget*    parent
) :
   BaseClass( app, parent )
{
   QVBoxLayout* vlayout = new QVBoxLayout( this );
   vlayout->setSpacing( 0 );
   vlayout->setContentsMargins( 0,0,0,0 );

   QHBoxLayout* hlayout = new QHBoxLayout;
   vlayout->addLayout( hlayout );
   hlayout->setSpacing( 0 );
   hlayout->setContentsMargins( 0,0,0,0 );
   
   _text = new XxText( _app, this, -1 );

   hlayout->addWidget( _text );
   
   _vscroll[0] = new QScrollBar;
  // _vscroll[0]->setFixedWidth( 20 ); to be removed, at least on OSX and Solaris
   hlayout->addWidget( _vscroll[0] );

   _hscroll = new QScrollBar( Qt::Horizontal );
   //_hscroll->setFixedHeight( 20 ); to be removed, at least on OSX and Solaris

   vlayout->addWidget( _hscroll );

   connect( _vscroll[0], SIGNAL(valueChanged(int)),
            this, SLOT(verticalScroll(int)) );
   connect( _hscroll, SIGNAL(valueChanged(int)),
            this, SLOT(horizontalScroll(int)) );

   // Watch cursor changes.
   connect( app, SIGNAL(cursorChanged(int)), this, SLOT(onCursorChanged(int)) );
}

//------------------------------------------------------------------------------
//
void XxMergedFrame::update()
{
   BaseClass::update();
   _text->update();
}

//------------------------------------------------------------------------------
//
void XxMergedFrame::show()
{
   BaseClass::show();
   updateMergedLines();
   // Too bad that at this time, the pane height is still 0, so the cursor
   // gets located at the topline
   recenter();
}

//------------------------------------------------------------------------------
//
void XxMergedFrame::updateMergedLines()
{
   _text->computeMergedLines();
   adjustVerticalScrollbars( computeDisplaySize() );
}

//------------------------------------------------------------------------------
//
QSize XxMergedFrame::computeDisplaySize() const
{
   uint displayWidth = _text->contentsRect().width();
   uint displayHeight = _text->contentsRect().height();
   return QSize( displayWidth, displayHeight );
}

//------------------------------------------------------------------------------
//
uint XxMergedFrame::getTextLength() const
{
   return _text->getMergedLines();
}

//------------------------------------------------------------------------------
//
XxDln XxMergedFrame::getTopLine() const
{
   return _text->getMergedLineFromLine( BaseClass::getTopLine() );
}

//------------------------------------------------------------------------------
//
XxDln XxMergedFrame::getBottomLine() const
{
   return _text->getMergedLineFromLine( BaseClass::getBottomLine() );
}

//------------------------------------------------------------------------------
//
XxDln XxMergedFrame::setTopLine( const XxDln lineNo )
{
    return BaseClass::setTopLine(  _text->getLineFromMergedLine( lineNo ) );
}

//------------------------------------------------------------------------------
//
XxDln XxMergedFrame::setBottomLine( const XxDln lineNo )
{
    return BaseClass::setBottomLine(  _text->getLineFromMergedLine( lineNo ) );
}

//------------------------------------------------------------------------------
//
XxDln XxMergedFrame::setCenterLine( const XxDln lineNo )
{
    return BaseClass::setCenterLine(  _text->getLineFromMergedLine( lineNo ) );
}

//------------------------------------------------------------------------------
//
XxDln XxMergedFrame::getNbDisplayLines() const
{
   return _text->computeDisplayLines();
}

//------------------------------------------------------------------------------
//
void XxMergedFrame::onCursorChanged( int cursorLine )
{
   // Scroll to make sure that the display always includes the cursor line.
   if ( cursorLine < getTopLine() ) {
      setTopLine( cursorLine );
   }
   else if ( cursorLine > getBottomLine() ) {
      setBottomLine( cursorLine );
   }
}

/*==============================================================================
 * CLASS XxMergedWindow
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxMergedWindow::XxMergedWindow( 
   XxApp*      app, 
   QWidget*    parent
) :
   BaseClass( parent ),
   _app( app )
{
   const XxResources& resources = app->getResources();

   QkMenu* menu = menuBar()->addMenu( "W&indow" );
   menu->addAction( 
      "Close", this, SLOT(hide()),
      resources.getAccelerator( ACCEL_MERGED_CLOSE )
   );

   _frame = new XxMergedFrame( app, this );

   setCentralWidget( _frame );
}

//------------------------------------------------------------------------------
//
void XxMergedWindow::show()
{
   BaseClass::show();
   _frame->show();
}

//------------------------------------------------------------------------------
//
void XxMergedWindow::updateMergedLines()
{
   _frame->updateMergedLines();
}

//------------------------------------------------------------------------------
//
void XxMergedWindow::hide()
{
   BaseClass::hide();
   _app->synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxMergedWindow::closeEvent( QCloseEvent *e)
{
   e->ignore();
   hide();
}

XX_NAMESPACE_END

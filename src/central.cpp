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

#include <central.h>
#include <text.h>
#include <app.h>
#include <resources.h>
#include <diffs.h>
#include <buffer.h>
#include <copyLabel.h>
#include <lineNumbers.h>
#include <help.h>

#include <qscrollview.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qaccel.h>

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
   BaseClass( app, parent, name )
{
   XX_CHECK( app );
   uint nbTextWidgets = _app->getNbFiles();

   // Texts widget (texts).
   //

   QVBoxLayout* textAndSbLayout =
      new QVBoxLayout( this, 0, -1, "textAndSbLayout" );
   QHBoxLayout* textLayout =
      new QHBoxLayout( textAndSbLayout, -1, "textLayout" );

   QFont smaller = font();
   smaller.setPointSize( smaller.pointSize() - 2 );

   for ( uint ii = 0; ii < nbTextWidgets; ++ii ) {
      if ( ii == 1 ) {
         _vscroll[0] = new QScrollBar( this, "vscroll[0]" );
         textLayout->addWidget( _vscroll[0] );
         connect( _vscroll[0], SIGNAL(valueChanged(int)),
                  this, SLOT(verticalScroll(int)) );
      }
      if ( ii == 2 ) {
         _vscroll[1] = new QScrollBar( this, "vscroll[1]" );
         textLayout->addWidget( _vscroll[1] );
         connect( _vscroll[1], SIGNAL(valueChanged(int)),
                  this, SLOT(verticalScroll2(int)) );
      }

      QVBoxLayout* layout = new QVBoxLayout( textLayout, -1, "layout" );
      //textLayout->setStretchFactor( layout, 1 ); useless to make equal

      // Create filename and line number labels.
      QHBoxLayout* fnLayout = new QHBoxLayout( layout, -1, "fnLayout" );
      _filenameLabel[ii] = new XxCopyLabel( this );
      _filenameLabel[ii]->setFont( smaller );
      _filenameLabel[ii]->setFrameStyle( QFrame::Panel | QFrame::Raised );
      _filenameLabel[ii]->setMinimumWidth( 1 );
      _filenameLabel[ii]->setLineWidth( 2 );

      _lineNumberLabel[ii] =
         new QLabel( "9999", this, "lineNumberLabel" );
      _lineNumberLabel[ii]->setAlignment( Qt::AlignCenter );
      _lineNumberLabel[ii]->setFrameStyle( QFrame::Panel | QFrame::Raised );
      _lineNumberLabel[ii]->setLineWidth( 2 );
      _lineNumberLabel[ii]->setMinimumSize( _lineNumberLabel[ii]->sizeHint() );
      _lineNumberLabel[ii]->setText( "" );

      fnLayout->addWidget( _filenameLabel[ii], 10 );
      fnLayout->addWidget( _lineNumberLabel[ii], 1 );

      // Create linenumbers widget and text widget.
      QHBoxLayout* fnLayout2 = new QHBoxLayout( layout, -1, "fnLayout2" );
      _text[ii] = new XxText( _app, this, ii, this, "text" );
      _lineNumbers[ii] =
         new XxLineNumbers( _app, this, ii, this, "lineNumbers" );

      fnLayout2->addWidget( _lineNumbers[ii] );
      fnLayout2->addWidget( _text[ii] );
   }

   _hscroll = new QScrollBar( Qt::Horizontal, this, "hscroll" );
   connect( _hscroll, SIGNAL(valueChanged(int)),
            this, SLOT(horizontalScroll(int)) );

   textAndSbLayout->addWidget( _hscroll );

   createOnContextHelp();

   // Add some extra accelerators.
   QAccel* a = new QAccel( this );

   a->connectItem( a->insertItem(Key_Right), this, SLOT(scrollRight()) );
   a->connectItem( a->insertItem(Key_Left), this, SLOT(scrollLeft()) );

   // Watch cursor changes.
   connect( app, SIGNAL(cursorChanged(int)), this, SLOT(onCursorChanged(int)) );
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::createOnContextHelp()
{
   QWhatsThis::add( _vscroll[0], XxHelp::getWhatsThisText( XxHelp::VSCROLL ) );
   if ( _vscroll[1] != 0 ) {
      QWhatsThis::add( _vscroll[1],
                       XxHelp::getWhatsThisText( XxHelp::VSCROLL ) );
   }
   QWhatsThis::add( _hscroll, XxHelp::getWhatsThisText( XxHelp::HSCROLL ) );

   for ( uint ii = 0; ii < _app->getNbFiles(); ++ii ) {
      QWhatsThis::add(
         _filenameLabel[ii], XxHelp::getWhatsThisText( XxHelp::FILENAME )
      );
      QWhatsThis::add(
         _lineNumberLabel[ii], XxHelp::getWhatsThisText( XxHelp::LINENO )
      );
      QWhatsThis::add(
         _text[ii], XxHelp::getWhatsThisText( XxHelp::TEXT_VIEW )
      );
   }
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::update()
{
   BaseClass::update();
   int nbFiles = _app->getNbFiles();
   for ( XxFno ii = 0; ii < nbFiles; ++ii ) {
      _lineNumbers[ii]->update();
      _text[ii]->update();
   }
   updateLineNumberLabels( _app->getCursorLine() );
}

//------------------------------------------------------------------------------
//
QSize XxCentralFrame::computeDisplaySize() const
{
   uint displayWidth = 0;
   for ( uint ii = 0; ii < _app->getNbFiles(); ++ii ) {
      displayWidth =
         std::max( displayWidth, uint(_text[ii]->contentsRect().width()) );
   }

   uint displayHeight = _text[0]->contentsRect().height();

   return QSize( displayWidth, displayHeight );
}

//------------------------------------------------------------------------------
//
uint XxCentralFrame::computeTextLength() const
{
   const XxDiffs* diffs = _app->getDiffs();
   if ( diffs != 0 ) {
      return diffs->getNbLines();
   }
   return 0;
}

//------------------------------------------------------------------------------
//
XxDln XxCentralFrame::getNbDisplayLines() const
{
   return _text[0]->computeDisplayLines();
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::showFilenames( const bool show )
{
   for ( uint ii = 0; ii < _app->getNbFiles(); ++ii ) { 
      if ( show ) {
         _filenameLabel[ii]->show();
         _lineNumberLabel[ii]->show();
      }
      else {
         _filenameLabel[ii]->hide();
         _lineNumberLabel[ii]->hide();
      }
   }
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::setFilename( XxFln no, const QString& fn )
{
   XX_CHECK( uint(no) < _app->getNbFiles() );
   _filenameLabel[ int(no) ]->setText( fn );
}

//------------------------------------------------------------------------------
//
int XxCentralFrame::getOneWidth() const
{
   return _text[0]->width() + _vscroll[0]->width();
}

//------------------------------------------------------------------------------
//
int XxCentralFrame::getOneHeight() const
{
   return _text[0]->height() + _hscroll->height();
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::updateLineNumberLabels( int cursorLine )
{
   XxDiffs* diffs = _app->getDiffs();
   if ( diffs != 0 ) {
      int nbFiles = _app->getNbFiles();
      for ( XxFno ii = 0; ii < nbFiles; ++ii ) {
         bool aempty;

         XxFln fline = diffs->getBufferLine( ii, cursorLine, aempty );
         XxBuffer* file = _app->getBuffer( ii );
         XX_ASSERT( file );
         XxFln dfline = file->getDisplayLineNo( fline );
         
         _lineNumberLabel[ii]->setNum( dfline );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::adjustLineNumbers( bool show, const QFont& fontText )
{
   int nbFiles = _app->getNbFiles();
   if ( show ) {
      // Compute the maximum line numbers width.  This has to be the same for
      // all the texts, in order to have a consistent horizontal scrollbar.
      uint lnw = 0;
      XxFno ii;
      for ( ii = 0; ii < nbFiles; ++ii ) {
         XxBuffer* file = _app->getBuffer( ii );
         XX_ASSERT( file );
         lnw = std::max(
            lnw,
            file->computeLineNumbersWidth( fontText ) +
            ( _lineNumbers[ii]->width() -
              _lineNumbers[ii]->contentsRect().width() + 2 ) );
         
      }
      for ( ii = 0; ii < nbFiles; ++ii ) {
         _lineNumbers[ii]->setFixedWidth( lnw );
         _lineNumbers[ii]->show();
      }
   }
   else {
      for ( XxFno ii = 0; ii < nbFiles; ++ii ) {
         _lineNumbers[ii]->hide();
      }
   }
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::onCursorChanged( int cursorLine )
{
   // Scroll to make sure that the display always includes the cursor line.
   if ( cursorLine < getTopLine() ) {
      setTopLine( cursorLine );
   }
   else if ( cursorLine > getBottomLine() ) {
      setBottomLine( cursorLine );
   }
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::verticalScroll( int value )
{
   // Make sure that the app's cursor is always within our boundaries.
   _app->adjustCursor( getTopLine(), getBottomLine() );

   BaseClass::verticalScroll( value );
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::scrollRight()
{
   _hscroll->setValue( _hscroll->value() + _hscroll->lineStep() * 10 );
}

//------------------------------------------------------------------------------
//
void XxCentralFrame::scrollLeft()
{
   _hscroll->setValue( _hscroll->value() - _hscroll->lineStep() * 10 );
}

XX_NAMESPACE_END

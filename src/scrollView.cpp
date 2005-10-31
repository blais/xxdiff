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

#include <scrollView.h>
#include <app.h>

#include <qscrollbar.h>
#include <qsize.h>
#include <qfont.h>
#include <qfontmetrics.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxScrollView
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxScrollView::XxScrollView( 
   XxApp*      app, 
   QWidget*    parent,
   const char* name
) :
   BaseClass( parent, name ),
   _app( app ),
   _displayWidth( 0 ),
   _displayHeight( 0 ),
   _textWidth( 0 ),
   _textHeight( 0 )
{
   // Initialize to null.  The derived classes create them.
   _hscroll = 0;
   _vscroll[0] = _vscroll[1] = 0;

   // Watch changes in the text size and adjust accordingly.
   XX_CHECK( _app != 0 );
   connect(
      _app, SIGNAL(textSizeChanged()),
      this, SLOT(adjustScrollbars())
   );
}

//------------------------------------------------------------------------------
//
XxScrollView::~XxScrollView()
{
}

//------------------------------------------------------------------------------
//
XxDln XxScrollView::getTopLine() const
{
   XxDiffs* diffs = _app->getDiffs();
   if ( diffs == 0 ) {
      return 0;
   }

   return _vscroll[0]->value() + 1;
}

//------------------------------------------------------------------------------
//
XxDln XxScrollView::getCenterLine() const
{
   XxDiffs* diffs = _app->getDiffs();
   if ( diffs == 0 ) {
      return 0;
   }

   return std::min(
      _vscroll[0]->value() + 1 + getNbDisplayLines() / 2, 
      diffs->getNbLines()
   );
}

//------------------------------------------------------------------------------
//
XxDln XxScrollView::getBottomLine() const
{
   XxDiffs* diffs = _app->getDiffs();
   if ( diffs == 0 ) {
      return 0;
   }

   int botline = std::min( 
      getTopLine() /* top line */
      + (getNbDisplayLines() - 1) /* to bottom line */
      - 1, /* don't allow cursor on half-displayed line */
      diffs->getNbLines()
   );
   return std::max( botline, 1 ); // treat special case where display is one
                                  // line high.
}

//------------------------------------------------------------------------------
//
XxDln XxScrollView::setTopLine( const XxDln lineNo )
{
   XxDiffs* diffs = _app->getDiffs();
   if ( diffs == 0 ) {
      return 1;
   }

   XxDln oldLine = getTopLine();
   XxDln validLine = 
      std::max( XxDln(1), std::min( XxDln(diffs->getNbLines()), lineNo ) );
   XxDln displayableLine = 
      std::min(
         validLine, 
         std::max( XxDln(1), 
                   XxDln(diffs->getNbLines() - (getNbDisplayLines() - 2)) )
      );
   _vscroll[0]->setValue( displayableLine - 1 );
   if ( _vscroll[1] != 0 ) {
      _vscroll[1]->setValue( displayableLine - 1 );
   }
   // Note: this will trigger the scroll signal indirectly.
   
   return oldLine;
}

//------------------------------------------------------------------------------
//
XxDln XxScrollView::setCenterLine( const XxDln lineNo )
{
   XxDiffs* diffs = _app->getDiffs();
   if ( diffs == 0 ) {
      return 1;
   }

   XxDln oldLine = getCenterLine();
   setTopLine( lineNo - getNbDisplayLines() / 2 );
   return oldLine;
}

//------------------------------------------------------------------------------
//
XxDln XxScrollView::setBottomLine( const XxDln lineNo )
{
   XxDiffs* diffs = _app->getDiffs();
   if ( diffs == 0 ) {
      return 1;
   }

   XxDln oldLine = getBottomLine();
   setTopLine( lineNo - (getNbDisplayLines() - 2) );
   return oldLine;
}

//------------------------------------------------------------------------------
//
void XxScrollView::recenter()
{
   XxDln cursorLine = _app->getCursorLine();
   setCenterLine( cursorLine );
}

//------------------------------------------------------------------------------
//
uint XxScrollView::getHorizontalPos() const
{
   if ( _hscroll ) {
      return _hscroll->value();
   }
   return 0;
}

//------------------------------------------------------------------------------
//
void XxScrollView::adjustScrollbars()
{
   // Compute the maximum of the text widths here, because the text widths may
   // vary depending on the varying widths of the lineNumbers widget.
   QSize displaySize = computeDisplaySize();

   adjustHorizontalScrollbars( displaySize );
   adjustVerticalScrollbars( displaySize );
}

//------------------------------------------------------------------------------
//
void XxScrollView::adjustHorizontalScrollbars( const QSize& displaySize )
{
   //
   // Horizontal scrollbar.
   //

   uint displayWidth = displaySize.width();
   uint textWidth = _app->getTextWidth();

   if ( _displayWidth != displayWidth || _textWidth != textWidth ) {

      _displayWidth = displayWidth;
      _textWidth = textWidth;

      // Leave a pixel margin to the right of text.
      textWidth += 16; // right margin in pixels.
      if ( textWidth <= _displayWidth ) {
         _hscroll->setRange( 0, 0 );
         _hscroll->hide();
         // This will generate a resize event for the text widgets.
      }
      else {
         _hscroll->setSteps( 1, _displayWidth );
         _hscroll->setRange( 0, textWidth - _displayWidth );
         _hscroll->show();
      }

      if ( int( textWidth - _displayWidth ) < _hscroll->value() ) {
         _hscroll->setValue( textWidth - _displayWidth );
         // Note: this will indirectly trigger a redraw.
      }
   }
}

//------------------------------------------------------------------------------
//
void XxScrollView::adjustVerticalScrollbars( const QSize& displaySize )
{
   //
   // Vertical scrollbar.
   //

   uint displayHeight = displaySize.height();
   const XxDiffs* diffs = _app->getDiffs();
   uint textHeight = computeTextLength();

   if ( _displayHeight != displayHeight || _textHeight != textHeight ) {

      _displayHeight = displayHeight;
      _textHeight = textHeight;

      uint displayLines = getNbDisplayLines();
      _vscroll[0]->setSteps( 1, displayLines );
      if ( _vscroll[1] != 0 ) {
         _vscroll[1]->setSteps( 1, displayLines );
      }
      
      if ( diffs == 0 ) {
         _vscroll[0]->setRange( 0,0 );
         if ( _vscroll[1] != 0 ) {
            _vscroll[1]->setRange( 0,0 );
         }
      }
      else {
         XxDln maxLine = std::max( 0, XxDln( textHeight - displayLines + 1 ) );
         _vscroll[0]->setRange( 0, maxLine );
         if ( _vscroll[1] != 0 ) {
            _vscroll[1]->setRange( 0, maxLine );
         }
      }
   }
}

//------------------------------------------------------------------------------
//
void XxScrollView::horizontalScroll( int /*value*/ )
{
   _app->updateWidgets();
}

//------------------------------------------------------------------------------
//
void XxScrollView::verticalScroll( int value )
{
   _app->updateWidgets();
   if ( _vscroll[1] != 0 ) {
      // Will only change if different.
      _vscroll[1]->setValue( value );
   }
}

//------------------------------------------------------------------------------
//
void XxScrollView::verticalScroll2( int value )
{
   _vscroll[0]->setValue( value );
   verticalScroll( value );
}

//------------------------------------------------------------------------------
//
void XxScrollView::wheelEvent( QWheelEvent* e )
{
   if ( e->state() & Qt::ControlButton ) {
      // Interactive font resize feature with mouse wheel.
      if ( e->delta() > 0 ) {
         _app->fontSizeDecrease();
      }
      else {
         _app->fontSizeIncrease();
      }
   }
   else {
      QApplication::sendEvent( _vscroll[0], e );
   }
}

XX_NAMESPACE_END

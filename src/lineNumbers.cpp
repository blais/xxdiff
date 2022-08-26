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

#include <lineNumbers.h>
#include <app.h>
#include <central.h>
#include <resources.h>
#include <diffs.h>
#include <buffer.h>
#include <text.h>

#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QColor>

#include <QApplication>
#include <QClipboard>
#include <QFrame>

#include <math.h>
#include <stdio.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxLineNumbers
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxLineNumbers::XxLineNumbers(
   XxApp*          app,
   XxCentralFrame* central,
   const XxFno     no,
   QWidget*        parent
) :
   QFrame( parent ),
   _app( app ),
   _central( central ),
   _no( no )
{
   // Frame borders must be set equal to the one in XxText for proper vertical
   // alignment of text.

   setSizePolicy(
      QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding )
   );
}

//------------------------------------------------------------------------------
//
XxLineNumbers::~XxLineNumbers()
{
}

//------------------------------------------------------------------------------
//
void XxLineNumbers::paintEvent( QPaintEvent *e )
{
   //XX_TRACE( "painting!" );

   QFrame::paintEvent(e);

   QPainter p( this );
   QRect rect = contentsRect();

   // We want 1:1 pixel/coord ratio.
   p.setViewport( rect );
   rect.translate( -rect.x(), -rect.y() );
   p.setWindow( rect );
//#define BACK_COLORING
#ifdef BACK_COLORING
   const int w = rect.width();
#endif
   const XxResources& resources = _app->getResources();

   XxBuffer* file = _app->getBuffer( _no );
   const XxDiffs* diffs = _app->getDiffs();
   if ( diffs == 0 ) {
      return;
   }

   //
   // Draw appropriate content.
   //
   uint topLine = _central->getTopLine();

   // Font.
   p.setFont( resources.getFontText() );
   QFontMetrics fm = p.fontMetrics();

   QPen pen;

   uint displayLines = _central->getNbDisplayLines();
   int nbLines = std::min( displayLines, diffs->getNbLines() - (topLine - 1) );

   QString lnFormat;
   lnFormat.asprintf( "%%%dd", _app->getMaxDigits() );

   p.setPen( palette().color( backgroundRole() ).darker( 200 ) );

   const int x = 0;
   int y = 0;
   for ( int ii = 0; ii < nbLines; ++ii, y += XxText::lineHeight(fm) ) {

      // Get line to display.
      const XxLine& line = diffs->getLine( topLine + ii );

      // This is ugly.  I prefer line numbers without back coloring, it makes it
      // less confusing for actual file text. Keep for reference only.
#ifdef BACK_COLORING
      // Set background and foreground colors.
      QColor bcolor;
      QColor fcolor;
      XxColor dtype, dtypeSup;
      line.getLineColorType(
         resources.getIgnoreFile(),
         _no,
         dtype, dtypeSup, false
      );
      resources.getRegionColor( dtype, bcolor, fcolor );
      QBrush brush( bcolor );
      p.fillRect( x, y, w, XxText::lineHeight(fm), brush );
#endif

      // Render text.
      XxFln fline = line.getLineNo( _no );
      if ( fline != -1 ) {
         XxFln dfline = file->getDisplayLineNo( fline );

         const QString& renderedNums =
            file->renderLineNumber( dfline, lnFormat );

         p.drawText( x+2, y + fm.ascent(), renderedNums );
      }
   }

   p.setPen( palette().color( backgroundRole() ).darker( 135 ) );
   p.drawLine( rect.topRight(), rect.bottomRight() );

   // Draw line cursor.
   if ( !resources.getBoolOpt( BOOL_DISABLE_CURSOR_DISPLAY ) ) {
      XxDln cursorLine = _app->getCursorLine() - topLine;
      if ( cursorLine >= 0 && cursorLine < nbLines ) {
         int cursorY1 = cursorLine * XxText::lineHeight(fm);
         int cursorY2 = cursorY1 + XxText::lineHeight(fm);
         QColor cursorColor = resources.getColor( COLOR_CURSOR );
         int w = rect.width();
         p.setPen( cursorColor );
         p.drawLine( 0, cursorY1 , w, cursorY1 );
         p.drawLine( 0, cursorY2 , w, cursorY2 );
         cursorColor.setAlpha(64);
         p.fillRect( 0, cursorY1 + 1 , w, cursorY2 - cursorY1 - 1, cursorColor );
      }
  }

}

XX_NAMESPACE_END

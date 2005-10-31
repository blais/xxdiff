/******************************************************************************\
 * $Id: lineNumbers.cpp 138 2001-05-20 18:08:45Z blais $
 * $Date: 2001-05-20 14:08:45 -0400 (Sun, 20 May 2001) $
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

#include <lineNumbers.h>
#include <app.h>
#include <resources.h>
#include <diffs.h>
#include <buffer.h>

#include <qpainter.h>
#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>

#include <qapplication.h>
#include <qclipboard.h>

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
   XxApp*      app, 
   const XxFno no, 
   QWidget*    parent, 
   const char* name 
) :
   QFrame( parent, name, WResizeNoErase ),
   _app( app ),
   _no( no )
{
   // This must be set equal to the one in XxText for proper vertical alignment
   // of text.
   setFrameStyle( QFrame::Panel | QFrame::Sunken );
   setLineWidth( 2 );
}

//------------------------------------------------------------------------------
//
XxLineNumbers::~XxLineNumbers()
{
}

//------------------------------------------------------------------------------
//
QSizePolicy XxLineNumbers::sizePolicy() const
{
   return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
}

//------------------------------------------------------------------------------
//
void XxLineNumbers::drawContents( QPainter* pp )
{
   //XX_TRACE( "painting!" );

   // QPainter p;
   // p.begin( this );
   QPainter& p = *pp;
   QRect rect = contentsRect();

   // We want 1:1 pixel/coord ratio.
   p.setViewport( rect );
   rect.moveBy( -rect.x(), -rect.y() );
   p.setWindow( rect );

   XxBuffer* file = _app->getFile( _no );
   const XxDiffs* diffs = _app->getDiffs();
   const XxResources* resources = XxResources::getInstance();

   //
   // Draw appropriate content.
   //
   uint topLine = _app->getTopLine();

   // Font.
   p.setFont( _app->getFont() );
   QFontMetrics fm = p.fontMetrics();

   QPen pen;

   uint displayLines = _app->getNbDisplayLines();
   uint nbLines = 
      std::min( displayLines, diffs->getNbLines() - (topLine - 1) );

   char lnFormat[16];
   sprintf( lnFormat, "%%%dd", _app->getMaxDigits() );

   const int x = 0;
   int y = 0;
   for ( uint ii = 0; ii < nbLines; ++ii, y += fm.lineSpacing() ) {

      // Get line to display.
      const XxLine& line = diffs->getLine( topLine + ii );

      // Set background and foreground colors.
      QColor bcolor;
      QColor fcolor;
      XxResources::Resource dtype =
         resources->getLineColorType( line, _no, false );
      resources->getRegionColor( dtype, bcolor, fcolor );
      QBrush brush( bcolor );

      // Render text.
      int fline = line.getLineNo( _no );
      if ( fline != -1 ) {

         const char* renderedNums = file->renderLineNumber( fline, lnFormat );
         QString str( renderedNums );
         
         p.drawText( x, y + fm.ascent(), str );
      }
   }

   // p.end();
}

XX_NAMESPACE_END

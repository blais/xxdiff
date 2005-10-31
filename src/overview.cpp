/******************************************************************************\
 * $Id: overview.cpp 56 2000-12-25 20:15:47Z  $
 * $Date: 2000-12-25 15:15:47 -0500 (Mon, 25 Dec 2000) $
 *
 * Copyright (C) 1999, 2000  Martin Blais <blais@iro.umontreal.ca>
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

#include <overview.h>
#include <app.h>
#include <resources.h>
#include <diffs.h>
#include <buffer.h>

#include <qpainter.h>
#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpopupmenu.h>

#include <math.h>


XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxOverview
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxOverview::XxOverview( XxApp* app, QWidget * parent, const char* name ) :
   QFrame( parent, name, WResizeNoErase ),
   _app( app )
{
   setFrameStyle( QFrame::Panel | QFrame::Sunken );
   setLineWidth( 2 );
   setBackgroundMode( NoBackground );

   const XxResources* resources = XxResources::getInstance();
   uint nbFiles = _app->getNbFiles();
   if ( nbFiles == 0 ) {
      nbFiles = 2;
   }

   uint width =
      2 * lineWidth() + 
      nbFiles * resources->getOverviewFileWidth() + 
      (nbFiles-1) * resources->getOverviewSepWidth();
   setFixedWidth( width );
}

//------------------------------------------------------------------------------
//
XxOverview::~XxOverview()
{
}

//------------------------------------------------------------------------------
//
QSizePolicy XxOverview::sizePolicy() const
{
   return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
}

//------------------------------------------------------------------------------
//
void XxOverview::drawContents( QPainter* pp )
{
   // QPainter p;
   // p.begin( this );
   QPainter& p = *pp;
   QRect rect = contentsRect();

   // We want 1:1 pixel/coord ratio.
   p.setViewport( rect );
   rect.moveBy( -rect.x(), -rect.y() );
   p.setWindow( rect );
   // int w = rect.width();
   int h = rect.height();

   uint nbFiles = _app->getNbFiles();
   const XxDiffs* diffs = _app->getDiffs();
   const XxResources* resources = XxResources::getInstance();

   // If it is empty, erase the whole widget with blank color.
   QColor backgroundColor = 
      resources->getColor( XxResources::COLOR_BACKGROUND );
   if ( nbFiles == 0 || diffs == 0 ) {
      QBrush brush( backgroundColor );
      p.fillRect( rect, brush );
      //p.end();
      return;
   }

   // Get the files and compute lengths.
   uint nbLines = diffs->getNbLines();
   XxBuffer* files[3];
   uint flines[3];
   uint maxlines = 0;
   uint ii;
   for ( ii = 0; ii < nbFiles; ++ii ) {
      files[ii] = _app->getFile( ii );
      XX_ASSERT( files[ii] != 0 );

      flines[ii] = files[ii]->getNbLines();
      
      maxlines = std::max( maxlines, flines[ii] );
   }
   if ( maxlines == 0 ) {
      QBrush brush( backgroundColor );
      p.fillRect( rect, brush );
      //p.end();
      return;
   }

   //
   // Draw appropriate content.
   //

   int fileWidth = resources->getOverviewFileWidth();
   int sepWidth = resources->getOverviewSepWidth();

   // Draw the file boxes.
   QPen pen;
   pen.setColor( Qt::black );
   p.setPen( pen );
   QBrush brush( QBrush::SolidPattern );

   int y1[3];
   int y2[3];
   int dy[3];
   int x[3];
   for ( ii = 0; ii < nbFiles; ++ii ) {
      y1[ii] = int( h * (1-flines[ii]/float(maxlines))/2.0f ) + 1;
      y2[ii] = h-y1[ii];
      dy[ii] = y2[ii]-y1[ii];
   }
   int cx = 0;
   for ( ii = 0; ii < nbFiles; ++ii ) {
      // Draw both ends.
      brush.setColor( backgroundColor );
      p.fillRect( cx, 0, fileWidth, y1[ii], brush );
      p.fillRect( cx, y2[ii], fileWidth, h-y2[ii], brush );

      if ( ii != (nbFiles-1) ) {
         // Draw separator background.
         p.fillRect( cx+fileWidth, 0, sepWidth, h, brush );
      }
      
      x[ii] = cx;
      cx += fileWidth + sepWidth;
   }

   // Draw regions.
   uint start;
   uint end = 0;

   // Start drawing at beginning of blocks.
   int prevy[3];
   prevy[0] = y1[0];
   prevy[1] = y1[1];
   prevy[2] = y1[2];
   int fline[3] = { 0,0,0 };
   QColor back, fore;

   // Draw very first line connecting regions.
   for ( ii = 0; ii < nbFiles; ++ii ) {
      if ( ii > 0 ) {
         p.drawLine( x[ii] - sepWidth, prevy[ii-1], x[ii], prevy[ii] );
      }
   }
   
   do {
      diffs->findRegionWithSel( end + 1, start, end );

      for ( ii = 0; ii < nbFiles; ++ii ) {

         int fsize = diffs->getNbFileLines( ii, start, end );
         if ( fsize == 0 ) {
            p.drawLine( x[ii], prevy[ii], x[ii] + fileWidth, prevy[ii] );
         }
         else {
            fline[ii] += fsize;

            int yend = y1[ii] + (dy[ii]*fline[ii])/flines[ii];
            int ddy = yend - prevy[ii] + 1;

            const XxLine& line = diffs->getLine( start );
            XxResources::Resource dtype =
               resources->getLineColorType( line, ii, false );

            resources->getRegionColor( dtype, back, fore );
            brush.setColor( back );
            p.fillRect( x[ii]+1, prevy[ii], fileWidth-2, ddy, brush );

            p.drawRect( x[ii], prevy[ii], fileWidth, ddy );

            prevy[ii] = yend;
         }

         // Draw lines between regions.
         if ( ii > 0 ) {
            p.drawLine( x[ii] - sepWidth, prevy[ii-1], x[ii], prevy[ii] );
         }
      }

   } while ( end < nbLines );

   // Draw current line pointer.
   const int dx = 8;
   const int dyo2 = 3;
   p.setBrush( backgroundColor );
   int curppos[3];
   int curline = _app->getCursorLine();
   int topline = _app->getTopLine();
   int bottomline = _app->getBottomLine();
   const int visRegionBorder = 6;
   QColor cursorColor = resources->getColor( XxResources::COLOR_CURSOR );
   for ( ii = 0; ii < nbFiles; ++ii ) {
      bool aempty;
      float curfline;

      // Compute and draw visible region.
      curfline = diffs->getFileLine( ii, topline, aempty );
      if ( aempty == true ) {
         curfline += 1.0;
      }
      int toppos = y1[ii] + int( (dy[ii]*(curfline-1.0f))/flines[ii] );

      curfline = diffs->getFileLine( ii, bottomline, aempty ) + 1.0;
      int bottompos = y1[ii] + int( (dy[ii]*(curfline-1.0f))/flines[ii] );

      p.setPen( cursorColor );
      p.drawRect( x[ii] + visRegionBorder, toppos, 
                  fileWidth - 2*visRegionBorder, bottompos - toppos );
      p.setPen( Qt::black );

      // Compute arrow position.
      curfline = diffs->getFileLine( ii, curline, aempty ) + 0.5;
      if ( aempty == true ) {
         curfline += 0.5;
      }
      curppos[ii] = y1[ii] + int( (dy[ii]*(curfline-1.0f))/flines[ii] );

      if ( ii > 0 ) {

         // Draw left arrow.
         int pts1[6] = { x[ii] - sepWidth - dx, curppos[ii-1] - dyo2,
                         x[ii] - sepWidth - dx, curppos[ii-1] + dyo2,
                         x[ii] - sepWidth, curppos[ii-1] };                
         QPointArray pa1( 3, pts1 );
         p.drawPolygon( pa1 );

         // Draw right arrow.
         int pts2[6] = { x[ii] + dx, curppos[ii] - dyo2,
                         x[ii] + dx, curppos[ii] + dyo2,
                         x[ii], curppos[ii] };                
         QPointArray pa2( 3, pts2 );
         p.drawPolygon( pa2 );
      }
   }

   // Draw search results.
   pen.setColor( Qt::white );
   p.setPen( pen );
   p.setBrush( Qt::white );
   const int sdx = 3;
   const int fw2 = fileWidth/2;
   const std::vector<XxDiffs::SeaResult>& sresults = diffs->getSearchResults();
   for ( uint is = 0; is < sresults.size(); ++is ) {
      for ( ii = 0; ii < nbFiles; ++ii ) {

         float fline = sresults[is]._fline[ii];
         if ( fline != -1 ) {
            fline += 0.5f;
            int ypos = y1[ii] + int( (dy[ii]*(fline-1.0f))/flines[ii] );
            int pts1[8] = { x[ii] + fw2 - sdx, ypos,
                            x[ii] + fw2, ypos + sdx,
                            x[ii] + fw2 + sdx, ypos,
                            x[ii] + fw2, ypos - sdx };
            QPointArray pa1( 4, pts1 );
            p.drawPolygon( pa1 );
         }
      }
   }

   p.resetXForm();
   // p.end();
}

XX_NAMESPACE_END

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

#include <overview.h>
#include <app.h>
#include <resources.h>
#include <diffs.h>
#include <buffer.h>

#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QFrame>
#include <QMouseEvent>

#include <math.h>

#include <iostream>


XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxOverview
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxOverview::XxOverview(
   XxApp*          app,
   XxCentralFrame* central,
   QWidget *       parent
) :
   XxBorderLabel( XxBorderLabel::BorderLeft, parent ),
   _app( app ),
   _central( central ),
   _manipNo( -1 )
{
   const XxResources& resources = _app->getResources();
   uint nbFiles = _app->getNbFiles();
   if ( nbFiles == 0 ) {
      nbFiles = 2;
   }

   uint width =
      2 * lineWidth() + 
      nbFiles * resources.getOverviewFileWidth() + 
      (nbFiles-1) * resources.getOverviewSepWidth() - 1;
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
void XxOverview::paintEvent( QPaintEvent* e )
{
   BaseClass::paintEvent(e);

   QPainter p( this );
   QRect rect = contentsRect();
   rect.adjust( 1, 0, 0, 0 );

   // We want 1:1 pixel/coord ratio.
   p.setViewport( rect );
   rect.translate( -rect.x(), -rect.y() );
   p.setWindow( rect );
   // int w = rect.width();
   int h = rect.height();

   uint nbFiles = _app->getNbFiles();
   const XxDiffs* diffs = _app->getDiffs();
   const XxResources& resources = _app->getResources();

   // If it is empty, erase the whole widget with blank color.
   QColor backgroundColor = 
      resources.getColor( COLOR_BACKGROUND );
   if ( nbFiles == 0 || diffs == 0 ) {
      QBrush brush( backgroundColor );
      p.fillRect( rect, brush );
      return;
   }

   // Get the files and compute lengths.
   XxDln nbLines = diffs->getNbLines();
   XxBuffer* files[3];
   XxFln flines[3];
   XxFln maxlines = 0;
   uint ii;
   for ( ii = 0; ii < nbFiles; ++ii ) {
      files[ii] = _app->getBuffer( ii );
      XX_ASSERT( files[ii] != 0 );
      flines[ii] = files[ii]->getNbLines();
      /*flines[ii] = diffs->getNbLinesWithText( ii );*/
      maxlines = std::max( maxlines, flines[ii] );
   }
   if ( maxlines == 0 ) {
      QBrush brush( backgroundColor );
      p.fillRect( rect, brush );
      return;
   }

   //
   // Draw appropriate content.
   //

   int fileWidth = resources.getOverviewFileWidth();
   int sepWidth = resources.getOverviewSepWidth();

   const bool perHunkWsEnabled = resources.getBoolOpt( BOOL_IGNORE_PERHUNK_WS );

   // Draw the file boxes.
   QPen pen;
   pen.setColor( Qt::black );
   p.setPen( pen );
   QBrush brush( Qt::SolidPattern );

      brush.setColor( backgroundColor );
   for ( ii = 0; ii < nbFiles; ++ii ) {
      p.fillRect( _fileL[ii], 0, fileWidth, _fileT[ii], brush );
      p.fillRect( _fileL[ii], _fileB[ii], fileWidth, h - _fileB[ii], brush );

      if ( ii != (nbFiles-1) ) {
         // Draw separator background.
         p.fillRect( _fileR[ii], 0, sepWidth, h, brush );
      }
   }

   // Draw regions.
   XxDln start;
   XxDln end = 0;

   // Start drawing at beginning of blocks.
   int prevy[3];
   prevy[0] = _fileT[0]-1;
   prevy[1] = _fileT[1]-1;
   prevy[2] = _fileT[2]-1;
   XxFln fline[3] = { 0,0,0 };
   QColor back, fore;

   // Draw very first line connecting regions.
   for ( ii = 0; ii < nbFiles; ++ii ) {
      if ( ii > 0 ) {
         p.drawLine( _fileR[ii-1], prevy[ii-1], 
                     _fileL[ii], prevy[ii] );
      }
   }
   
   do {
      diffs->findRegionWithSel( end + 1, start, end );

      for ( ii = 0; ii < nbFiles; ++ii ) {

         int fsize = diffs->getNbFileLines( ii, start, end );
         if ( fsize == 0 ) {
            p.drawLine( _fileL[ii], prevy[ii], 
                        _fileR[ii], prevy[ii] );
         }
         else {
            fline[ii] += fsize;

            int yend = _fileT[ii] + (_fileDy[ii]*fline[ii])/flines[ii];
            int ddy = yend - prevy[ii] + 1;

            const XxLine& line = diffs->getLine( start );
            XxColor dtype, dtypeSup;
            line.getLineColorType(
               resources.getIgnoreFile(),
               ii,
               dtype, dtypeSup,
               perHunkWsEnabled
            );

            resources.getRegionColor( dtype, back, fore );
            brush.setColor( back );
            p.fillRect( _fileL[ii]+1, prevy[ii], fileWidth-2, ddy, brush );

            p.drawRect( _fileL[ii], prevy[ii], fileWidth-1, ddy-1 );

            prevy[ii] = yend;
         }

         // Draw lines between regions.
         if ( ii > 0 ) {
            p.drawLine( _fileR[ii-1], prevy[ii-1], _fileL[ii], prevy[ii] );
         }
      }

   } while ( end < nbLines );

   // Draw current line pointer.
   const int dx = 8;
   const int dyo2 = 3;
   p.setBrush( backgroundColor );
   int curppos[3];
   XxDln curline = _app->getCursorLine();
   XxDln topline = _central->getTopLine();
   XxDln bottomline = _central->getBottomLine();
   QColor cursorColor = resources.getColor( COLOR_CURSOR );
   for ( ii = 0; ii < nbFiles; ++ii ) {
      bool aempty;
      XxFln cfline;

      // Compute and draw visible region.
      cfline = diffs->getBufferLine( ii, topline, aempty );
      if ( aempty == true ) {
         cfline += 1;
      }
      int toppos =
         _fileT[ii] + 
         int( (_fileDy[ii] * (cfline-1)) / float( flines[ii] ) );

      cfline = diffs->getBufferLine( ii, bottomline, aempty ) + 1;
      int bottompos =
         _fileT[ii] + 
         int( (_fileDy[ii] * (cfline-1)) / float( flines[ii] ) ) - 1;

      p.setPen( cursorColor );
      p.drawRect( _regL[ii], toppos,
                  _regR[ii] - _regL[ii], bottompos - toppos );
      p.setPen( Qt::black );

      // Compute arrow position.
      if ( curline == 0 ) {
         continue;
      }

      float fcfline = diffs->getBufferLine( ii, curline, aempty ) + 0.5;
      if ( aempty == true ) {
         fcfline += 0.5;
      }
      curppos[ii] =
         _fileT[ii] + 
         int( (_fileDy[ii] * (fcfline-1)) / float( flines[ii] ) );

      if ( ii > 0 ) {

         // Draw left arrow.
         QPolygon pa1;
         pa1.putPoints(0, 3, 
                         _fileL[ii] - sepWidth - dx, curppos[ii-1] - dyo2,
                         _fileL[ii] - sepWidth - dx, curppos[ii-1] + dyo2,
                         _fileL[ii] - sepWidth, curppos[ii-1] );                
         p.drawPolygon( pa1 );

         // Draw right arrow.
         QPolygon pa2;
         pa2.putPoints(0, 3, 
                         _fileL[ii] + dx, curppos[ii] - dyo2,
                         _fileL[ii] + dx, curppos[ii] + dyo2,
                         _fileL[ii], curppos[ii] );                
         p.drawPolygon( pa2 );
      }
   }

   // Draw search results.
   pen.setColor( Qt::white );
   p.setPen( pen );
   p.setBrush( Qt::white );
   const int sdx = 3;
   const int fw2 = fileWidth/2;
   const std::vector<XxDiffs::SearchResult>& sresults =
      diffs->getSearchResults();
   for ( uint is = 0; is < sresults.size(); ++is ) {
      for ( ii = 0; ii < nbFiles; ++ii ) {

         XxFln fline = sresults[is]._fline[ii];
         if ( fline != -1 ) {
            float ffline = fline + 0.5f;
            int ypos =
               _fileT[ii] +
               int( (_fileDy[ii] * (ffline-1)) / float( flines[ii] ) );
            QPolygon pa1;
            pa1.putPoints(0, 4, 
                            _fileL[ii] + fw2 - sdx, ypos,
                            _fileL[ii] + fw2, ypos + sdx,
                            _fileL[ii] + fw2 + sdx, ypos,
                            _fileL[ii] + fw2, ypos - sdx );
            p.drawPolygon( pa1 );
         }
      }
   }

   p.resetTransform();
}

//------------------------------------------------------------------------------
//
void XxOverview::mousePressEvent( QMouseEvent* e )
{
   // Map in contents rect.
   QRect rect = contentsRect();
   int x = e->x() - rect.x();
   int y = e->y() - rect.y();

   // Find out in which file it was clicked.
   uint nbFiles = _app->getNbFiles();
   uint no;
   for ( no = 0; no < nbFiles; ++no ) {
      if ( _fileL[no] < x && x < _fileR[no] ) {
         break;
      }
   }
   if ( no == nbFiles ) {
      return;
   }

   // Find out if it was clicked in the visible region.
   if ( !( _fileT[no] <= y && y <= _fileB[no] ) ) {
      return;
   }

   const XxDiffs* diffs = _app->getDiffs();
   /*XxDln curline = _app->getCursorLine();*/
   XxDln topline = _central->getTopLine();
   XxDln bottomline = _central->getBottomLine();

   XxBuffer* files[3];
   XxFln flines[3];
   for ( uint ii = 0; ii < nbFiles; ++ii ) {
      files[ii] = _app->getBuffer( ii );
      XX_ASSERT( files[ii] != 0 );
      flines[ii] = files[ii]->getNbLines();
   }

   bool aempty;
   float cfline = diffs->getBufferLine( no, topline, aempty );
   if ( aempty == true ) {
      cfline += 1.0;
   }
   int toppos =
      _fileT[no] +
      int( (_fileDy[no] * (cfline-1)) / float( flines[no] ) );

   cfline = diffs->getBufferLine( no, bottomline, aempty ) + 1;
   int bottompos =
      _fileT[no] +
      int( (_fileDy[no] * (cfline-1)) / float( flines[no] ) );

   if ( !( _regL[no] <= x && x <= _regR[no] ) ||
        !( toppos <= y && y <= bottompos ) ) {
      // Clicked outside the handle - jump to the line
      int displayheight = bottomline - topline + 1;
      XxFln lineclicked = ( y - _fileT[no] ) * flines[no] / _fileDy[no];
      XxDln displayline = diffs->getDisplayLine( lineclicked, *files[no], no );
      _central->setTopLine( displayline - ( displayheight / 2 ) );
      return;
   }

   // At this point, visible region `no' has been chosen.
   _manipNo = no;
   _manipAnchor = y;
   _manipTopLine = topline;
   _manipFlines = flines[no];
}

//------------------------------------------------------------------------------
//
void XxOverview::mouseMoveEvent( QMouseEvent* e )
{
   if ( _manipNo != -1 ) {
      // Map in contents rect.
      QRect rect = contentsRect();
      int y = e->y() - rect.y();
      int dy = y - _manipAnchor;

      XxFln dline =
         int( ( dy ) * _manipFlines / float( _fileDy[_manipNo] ) + 1 );

      XxFln newTopLine = _manipTopLine + dline;

      _central->setTopLine( newTopLine );
      // Will this be enough to make the rest happen too? e.g. merged view?
   }
}

//------------------------------------------------------------------------------
//
void XxOverview::mouseReleaseEvent( QMouseEvent* /*e*/ )
{
   if ( _manipNo != -1 ) {
      _manipNo = -1;
   }
}

//------------------------------------------------------------------------------
//
void XxOverview::resizeEvent( QResizeEvent* e )
{
   BaseClass::resizeEvent( e );

   // Recompute drawing data, and keep it around for picking.
   uint nbFiles = _app->getNbFiles();

   //
   // Compute left/right extents.
   //
   const XxResources& resources = _app->getResources();
   int fileWidth = resources.getOverviewFileWidth();
   int sepWidth = resources.getOverviewSepWidth();
   
   const int visRegionBorder = int( 0.25 * fileWidth );
   int cx = 0;
   uint ii;
   for ( ii = 0; ii < nbFiles; ++ii ) {
      _fileL[ii] = cx;
      cx += fileWidth;
      _fileR[ii] = cx;
      cx += sepWidth;

      _regL[ii] = _fileL[ii] + visRegionBorder;
      _regR[ii] = _fileR[ii] - visRegionBorder;
   }

   updateVerticalExtents();
}

//------------------------------------------------------------------------------
//
void XxOverview::wheelEvent( QWheelEvent *e )
{
   QApplication::sendEvent( _central, e );
}

//------------------------------------------------------------------------------
//
void XxOverview::update()
{
   updateVerticalExtents();
   BaseClass::update();
}

//------------------------------------------------------------------------------
//
void XxOverview::updateVerticalExtents()
{
   //
   // Compute top/bottom extents.
   //

   // Get the files and compute lengths.
   XxBuffer* files[3];
   XxFln flines[3];
   XxFln maxlines = 0;
   uint nbFiles = _app->getNbFiles();
   uint ii;
   for ( ii = 0; ii < nbFiles; ++ii ) {
      files[ii] = _app->getBuffer( ii );
      XX_ASSERT( files[ii] != 0 );

      flines[ii] = files[ii]->getNbLines();
      maxlines = std::max( maxlines, flines[ii] );
   }

   QRect rect = contentsRect();
   int h = rect.height();

   for ( ii = 0; ii < nbFiles; ++ii ) {
      _fileT[ii] = int( h * (1 - flines[ii] / float(maxlines) ) * 0.5f ) + 1;
      _fileB[ii] = h - _fileT[ii];
      _fileDy[ii] = _fileB[ii] - _fileT[ii];
   }
}

XX_NAMESPACE_END


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

#include <qpainter.h>
#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpopupmenu.h>

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
   QWidget *       parent,
   const char*     name
) :
   QFrame( parent, name, WResizeNoErase ),
   _app( app ),
   _central( central ),
   _manipNo( -1 )
{
   setFrameStyle( QFrame::Panel | QFrame::Sunken );
   setLineWidth( 2 );
   setBackgroundMode( NoBackground );

   const XxResources& resources = _app->getResources();
   uint nbFiles = _app->getNbFiles();
   if ( nbFiles == 0 ) {
      nbFiles = 2;
   }

   uint width =
      2 * lineWidth() + 
      nbFiles * resources.getOverviewFileWidth() + 
      (nbFiles-1) * resources.getOverviewSepWidth();
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
   const XxResources& resources = _app->getResources();

   // If it is empty, erase the whole widget with blank color.
   QColor backgroundColor = 
      resources.getColor( COLOR_BACKGROUND );
   if ( nbFiles == 0 || diffs == 0 ) {
      QBrush brush( backgroundColor );
      p.fillRect( rect, brush );
      //p.end();
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
      //p.end();
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
   QBrush brush( QBrush::SolidPattern );

   for ( ii = 0; ii < nbFiles; ++ii ) {
      // Draw both ends.
      brush.setColor( backgroundColor );
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
   prevy[0] = _fileT[0];
   prevy[1] = _fileT[1];
   prevy[2] = _fileT[2];
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

            p.drawRect( _fileL[ii], prevy[ii], fileWidth, ddy );

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
         int( (_fileDy[ii] * (cfline-1)) / float( flines[ii] ) );

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
         int pts1[6] = { _fileL[ii] - sepWidth - dx, curppos[ii-1] - dyo2,
                         _fileL[ii] - sepWidth - dx, curppos[ii-1] + dyo2,
                         _fileL[ii] - sepWidth, curppos[ii-1] };                
         QPointArray pa1( 3, pts1 );
         p.drawPolygon( pa1 );

         // Draw right arrow.
         int pts2[6] = { _fileL[ii] + dx, curppos[ii] - dyo2,
                         _fileL[ii] + dx, curppos[ii] + dyo2,
                         _fileL[ii], curppos[ii] };                
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
            int pts1[8] = { _fileL[ii] + fw2 - sdx, ypos,
                            _fileL[ii] + fw2, ypos + sdx,
                            _fileL[ii] + fw2 + sdx, ypos,
                            _fileL[ii] + fw2, ypos - sdx };
            QPointArray pa1( 4, pts1 );
            p.drawPolygon( pa1 );
         }
      }
   }

   p.resetXForm();
   // p.end();
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
   if ( !( _regL[no] <= x && x <= _regR[no] ) ) {
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

   if ( !( toppos <= y && y <= bottompos ) ) {
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


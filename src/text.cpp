/******************************************************************************\
 * $Id: text.cpp 405 2001-11-22 17:51:48Z blais $
 * $Date: 2001-11-22 12:51:48 -0500 (Thu, 22 Nov 2001) $
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

#include <text.h>
#include <app.h>
#include <resources.h>
#include <diffs.h>
#include <buffer.h>

#include <qpainter.h>
#include <qnamespace.h>
#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpopupmenu.h>

#include <qapplication.h>
#include <qclipboard.h>

#include <math.h>
#include <stdio.h>

#include <iostream>

#ifdef WINDOWS
#define snprintf _snprintf
#endif


//#define XX_DEBUG_TEXT  1


#ifndef XX_DEBUG_TEXT
#define XX_RED_RECT(x,y,w,h) x, y, w, h
#define XX_RED_WIDTH(w) w
#else
#define XX_RED_RECT(x,y,w,h) x+1, y+1, w-2, h-2
#define XX_RED_WIDTH(w) w+2
#endif


XX_NAMESPACE_BEGIN

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

/*----- constants -----*/

const int xch_search_delta = 100;
const int xch_draw_delta = 100;

//------------------------------------------------------------------------------
//
inline void rentxt(
   QPainter&           p,
   const char*         renderedText,
   int&                xch,
   const int           xend,
   int&                xpx,
   const int           wwidth,
   const int           y,
   const QFontMetrics& fm
)
{
   QRect brect;
   while ( xch < xend ) {
      int rlen = xch_draw_delta;
      if ( (xch + rlen) > xend ) {
         rlen = xend - xch;
      }
      
      QString str;
      XX_CHECK( rlen > 0 ); // always true, because xch < xend
      str.setLatin1( renderedText + xch, rlen );
      int nw = fm.width( str, rlen );

      p.drawText(
         XX_RED_RECT( xpx, y, wwidth - xpx, fm.lineSpacing() ),
         Qt::AlignLeft | Qt::AlignTop, 
         str, rlen,
         &brect
      );
      
      xpx += nw; // XX_RED_WIDTH( brect.width() );
      xch += rlen;
      
      if ( xpx > wwidth ) {
         break;
      }
   }
}

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxText
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxText::XxText( 
   XxApp*      app, 
   const XxFno no, 
   QWidget*    parent, 
   const char* name 
) :
   QFrame( parent, name, WResizeNoErase ),
   _app( app ),
   _no( no ),
   _grab( false )
{
   setFrameStyle( QFrame::Panel | QFrame::Sunken );
   setLineWidth( 2 );
#ifndef XX_DEBUG_TEXT
   setBackgroundMode( NoBackground );
#else
   setBackgroundColor( Qt::black );
#endif
}

//------------------------------------------------------------------------------
//
XxText::~XxText()
{
}

//------------------------------------------------------------------------------
//
QSizePolicy XxText::sizePolicy() const
{
   return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

//------------------------------------------------------------------------------
//
void XxText::drawContents( QPainter* pp )
{
   // Note: if only QPainter had a way to draw text which is not a QString, or
   // if there was a way to allocate a QString with shallow copy of non-unicode
   // text, this would be much faster.

   //XX_TRACE( "painting!" );

   // QPainter p;
   // p.begin( this );
   QPainter& p = *pp;
   QRect rect = contentsRect();

   // We want 1:1 pixel/coord ratio.
   QPoint offset = rect.topLeft();
   p.setViewport( rect );
   rect.moveBy( -offset.x(), -offset.y() );
   p.setWindow( rect );
   const int w = rect.width();
   const int h = rect.height();

   XxBuffer* file = _app->getBuffer( _no );
   const XxDiffs* diffs = _app->getDiffs();
   const XxResources& resources = _app->getResources();

   // If it is empty, erase the whole widget with blank color.
   if ( file == 0 || diffs == 0 ) {
      QColor backgroundColor = resources.getColor( COLOR_BACKGROUND );
      QBrush brush( backgroundColor );
      p.fillRect( rect, brush );

      p.end();
      return;
   }

   //
   // Draw appropriate content.
   //
   XxDln topLine = _app->getTopLine();
   uint horizontalPos = _app->getHorizontalPos();
   uint tabWidth = resources.getTabWidth();

   // The painter's clip region is already set (verified).

   // Font.
   p.setFont( resources.getFontText() );
   QFontMetrics fm = p.fontMetrics();

   // Don't draw background of chars since we'll draw first.
   p.setBackgroundMode( Qt::TransparentMode );
   QPen pen;

   XxDln displayLines = _app->getNbDisplayLines();
   uint nbLines = 
      std::min( displayLines, XxDln(diffs->getNbLines() - (topLine - 1)) );

   bool hori =
      ( resources.getHordiffType() != HD_NONE ) && !diffs->isDirectoryDiff();

   const int x = 0 - horizontalPos;

   int y = 0;
   p.setBackgroundMode( OpaqueMode );

   int* hbuffer0;
   int* hbuffer1;
#ifdef XX_DEBUG
   int hbufferSize = 
#endif
      resources.getHordiffBuffers( hbuffer0, hbuffer1 );
   XX_ASSERT( hbuffer0 && hbuffer1 );

   for ( uint ii = 0; ii < nbLines; ++ii, y += fm.lineSpacing() ) {

      // Get line to display.
      const XxLine& line = diffs->getLine( topLine + ii );

      // Set background and foreground colors.
      XxColor idtype, idtypeSup;
      line.getLineColorType( 
         resources.getIgnoreFile(),
         _no,
         idtype, idtypeSup 
      );
      
      QColor bcolor;
      QColor fcolor;
      resources.getRegionColor( idtype, bcolor, fcolor );

      // Render text.
      XxFln fline = line.getLineNo( _no );
      if ( fline != -1 ) {
         uint length;
         const char* lineText = file->getTextLine( fline, length );
         
         const int bhd = 0;
         int ehd;
         const char* renderedText;

         // Copy array of hordiffs.
         const int* hordiffs = line.getHorDiffs( _no );

         if ( hordiffs ) {
            int c = 0;
            const int* s;
            for ( s = hordiffs; *s != -1; ++s, ++c ) {
               XX_CHECK( c < hbufferSize );
               hbuffer0[c] = *s;
            }
            XX_CHECK( c < hbufferSize );
            hbuffer0[c] = -1;

            renderedText = file->renderTextWithTabs( 
               lineText, length, tabWidth, ehd, hbuffer0
            );

            // Append ehd and end array with -1.  We make sure there was enough
            // space in the preceding realloc above.
            hbuffer0[c++] = ehd;
            hbuffer0[c] = -1;
            XX_CHECK( c < hbufferSize );
         }
         else {
            renderedText = file->renderTextWithTabs( 
               lineText, length, tabWidth, ehd, 0
            );
         }
         
         // Loop to find an character reasonably close before the beginning of
         // the visible window in x.
         QString chunk;
         int xch = bhd;
         int xpx = x;
         int xend = ehd;
         while ( xch < xend ) {
            int rlen = xch_search_delta;
            if ( (xch + rlen) > xend ) {
               rlen = xend - xch;
            }

            XX_CHECK( rlen > 0 ); // always true, because xch < xend
            chunk.setLatin1( renderedText + xch, rlen );
            QRect brect = fm.boundingRect( 
               -128, -128, 8192, 2048, 
               Qt::AlignLeft | Qt::AlignTop | Qt::SingleLine,
               chunk, rlen
            );

            if ( (xpx + brect.width()) > 0 ) {
               break;
            }
            xpx += brect.width();
            xch += rlen;
         }

         QBrush fillerBrush( bcolor );

         if ( hori &&
              line.hasHorizontalDiffs( _no ) &&
              ( line.getType() == XxLine::DIFF_1 ||
                line.getType() == XxLine::DIFF_2 ||
                line.getType() == XxLine::DIFF_3 ||
                line.getType() == XxLine::DIFF_ALL ||
                line.getType() == XxLine::DIFFDEL_1 ||
                line.getType() == XxLine::DIFFDEL_2 ||
                line.getType() == XxLine::DIFFDEL_3 ) ) {

            //
            // Render with horizontal diffs.
            //
            QColor bcolorSup;
            QColor fcolorSup;
            resources.getRegionColor( idtypeSup, bcolorSup, fcolorSup );

            // We start out with the sup color, then just switch.  We made sure
            // that the end of the line was included in the list of hordiffs.

            int c = 0;
            while ( hbuffer0[c] != -1 ) {

               if ( (c % 2) == 0 ) {
                  p.setPen( fcolorSup );
                  p.setBackgroundColor( bcolorSup );
               }
               else {
                  p.setPen( fcolor );
                  p.setBackgroundColor( bcolor );
               }

               rentxt(
                  p, renderedText,
                  xch, hbuffer0[c],
                  xpx,
                  w, y, fm
               );
               if ( xpx > w ) {
                  break;
               }

               ++c;
            }

            if ( xpx < w ) {
               fillerBrush.setColor( bcolorSup );
            }
         }
         else {

            //
            // Render without horizontal diffs.
            //            

            p.setPen( fcolor );
            p.setBackgroundColor( bcolor );
            rentxt(
               p, renderedText,
               xch, ehd,
               xpx,
               w, y, fm
            );
            if ( xpx > w ) {
               continue;
            }
         }
         
         // Filler part.
         int fillerwidth = w - xpx;
         if ( fillerwidth > 0 ) {
            p.fillRect(
               XX_RED_RECT( xpx, y, fillerwidth, fm.lineSpacing() ),
               fillerBrush
            );
         }
      }
      else {
         // The line is empty, just fill in the background.
         QBrush backBrush( bcolor );
         p.fillRect(
            XX_RED_RECT( 0, y, w, fm.lineSpacing() ),
            backBrush
         );
      }
   }
   p.setBackgroundMode( TransparentMode );

   // Fill in at the bottom if necessary (at end of text).
   if ( y < h ) {
      QColor backgroundColor = resources.getColor( COLOR_BACKGROUND );
      QBrush brush( backgroundColor );
      p.fillRect( 0, y, w, h - y, brush );
   }

   // Draw line cursor.
   if ( !resources.getBoolOpt( BOOL_DISABLE_CURSOR_DISPLAY ) ) {

      XxDln cursorLine = _app->getCursorLine();
      int relLine = cursorLine - topLine;
      
      QColor cursorColor = resources.getColor( COLOR_CURSOR );
      p.setPen( cursorColor );
      y = relLine * fm.lineSpacing() - 1;
      p.drawRect( 0, y, w, fm.lineSpacing() + 2 );
   }

   // Draw vertical line.
   if ( resources.getShowOpt( SHOW_VERTICAL_LINE ) ) {

      uint cpos = resources.getVerticalLinePos();
      int posx = cpos * fm.maxWidth() - horizontalPos;

      QColor vlineColor = resources.getColor( COLOR_VERTICAL_LINE );
      p.setPen( vlineColor );
      p.drawLine( posx, 0, posx, h );
   }

   // p.end();
}

//------------------------------------------------------------------------------
//
XxDln XxText::computeDisplayLines() const
{
   const QFont& font = _app->getResources().getFontText();
   QFontMetrics fm( font );
   return contentsRect().height() / fm.lineSpacing() + 1;
}

//------------------------------------------------------------------------------
//
void XxText::mousePressEvent( QMouseEvent* event )
{
   // Find the line.
   XxDiffs* diffs = _app->getDiffs();
   XxBuffer* buffer = _app->getBuffer( _no );
   const XxResources& resources = _app->getResources();
   if ( diffs == 0 || buffer == 0 ) {
      return;
   }

   QString clipboardFormat = resources.getClipboardFormat();
   QString filename = buffer->getDisplayName();

   const QFont& font = resources.getFontText();
   QFontMetrics fm( font );
   XxDln dlineno = event->y() / fm.lineSpacing();
   XxDln lineno = _app->getTopLine() + dlineno;
   // Check for click out of valid region.
   if ( lineno > XxDln(diffs->getNbLines()) ) {
      return;
   }

   // Activate popup in third button.
   if ( event->button() == RightButton ) {
      if ( event->state() & ControlButton ) {
         _grab = true;
         _grabTopLine = _app->getTopLine();
         _grabDeltaLineNo = dlineno;
      }
      else {
         const XxLine& line = diffs->getLine( lineno );
         QPopupMenu* popup = _app->getViewPopup( line );
         popup->popup( event->globalPos() );
         return;
      }
   }

   // Perform the selection and create cut text.
   QString textCopy;
   if ( event->button() == MidButton ) {
      // Line event.
      if ( event->state() & ShiftButton ) {
         // Unselect line.
         diffs->selectLine( lineno, XxLine::UNSELECTED );
      }
      else if ( event->state() & ControlButton ) {
         // Delete line.
         diffs->selectLine( lineno, XxLine::NEITHER );
      }
      else { 
         // Select appropriate side.
         diffs->selectLine( lineno, XxLine::Selection( _no ) );
      }

      // Compute line text.
      const XxLine& line = diffs->getLine( lineno );
      if ( line.getType() == XxLine::SAME || 
           line.getType() == XxLine::DIRECTORIES || 
           line.getSelection() == XxLine::Selection( _no ) ) {
         XxFln fline = line.getLineNo( _no );
         if ( fline != -1 ) {
            uint len;
            const char* text = buffer->getTextLine( fline, len );
            if ( text != 0 ) {
               QString adt;
               if ( len > 0 ) {
                  adt.setLatin1( text, len );
               }
               if ( resources.getBoolOpt( BOOL_FORMAT_CLIPBOARD_TEXT )
                    == true ) {
                  QString forline = formatClipboardLine( 
                     clipboardFormat, _no, fline, filename, adt
                  );
                  textCopy += forline;
               }
               else {
                  textCopy += adt;
               }
               textCopy += QString("\n");
            }
         }
      }
   }
   else if ( event->button() == LeftButton ) {
      // Region event.
      if ( event->state() & ShiftButton ) {
         // Unselect region.
         diffs->selectRegion( lineno, XxLine::UNSELECTED );
      }
      else if ( event->state() & ControlButton ) {
         // Delete region.
         diffs->selectRegion( lineno, XxLine::NEITHER );
      }
      else { 
         // Select appropriate side.
         diffs->selectRegion( lineno, XxLine::Selection( _no ) );
      }

      // Compute region text.
      XxDln start, end;
      diffs->findRegion( lineno, start, end );
      for ( XxDln l = start; l <= end; ++l ) {
         const XxLine& line = diffs->getLine( l );
         XxFln fline = line.getLineNo( _no );
         if ( fline != -1 ) {
            uint len;
            const char* text = buffer->getTextLine( fline, len );
            if ( text != 0 ) {
               QString adt;
               if ( len > 0 ) {
                  adt.setLatin1( text, len );
               }
               if ( resources.getBoolOpt( BOOL_FORMAT_CLIPBOARD_TEXT )
                    == true ) {
                  QString forline = formatClipboardLine( 
                     clipboardFormat, _no, fline, filename, adt
                  );
                  textCopy += forline;
               }
               else {
                  textCopy += adt;
               }
               textCopy += QString("\n");
            }
         }
      }
   }

   QClipboard *cb = QApplication::clipboard();
   cb->setText( textCopy );

   if ( event->button() == LeftButton || 
        event->button() == MidButton ) {
      _app->setCursorLine( lineno );
   }
}

//------------------------------------------------------------------------------
//
void XxText::mouseMoveEvent( QMouseEvent* event )
{
   if ( _grab ) {
      const QFont& font = _app->getResources().getFontText();
      QFontMetrics fm( font );
      XxDln dlineno = event->y() / fm.lineSpacing();
      _app->setTopLine( _grabTopLine + (_grabDeltaLineNo - dlineno) );
   }
}

//------------------------------------------------------------------------------
//
void XxText::mouseReleaseEvent( QMouseEvent* /*event*/ )
{
   // Release grab in all case. It won't hurt.
   _grab = false;
}

//------------------------------------------------------------------------------
//
void XxText::mouseDoubleClickEvent( QMouseEvent* event )
{
   // Find the line.
   XxDiffs* diffs = _app->getDiffs();
   const XxResources& resources = _app->getResources();
   if ( diffs == 0 ) {
      return;
   }

   const QFont& font = resources.getFontText();
   QFontMetrics fm( font );
   XxDln dlineno = event->y() / fm.lineSpacing();
   XxDln lineno = _app->getTopLine() + dlineno;
   // Check for click out of valid region.
   if ( lineno > XxDln(diffs->getNbLines()) ) {
      return;
   }

   if ( event->button() == LeftButton ) {
      if ( diffs->isDirectoryDiff() ) {
         // The cursor is moved by the previous normal clicks.
         _app->diffFilesAtCursor();
      }
   }
}

//------------------------------------------------------------------------------
//
void XxText::resizeEvent( QResizeEvent* )
{
   // We postpone the resizing of the parent until when one of the texts has
   // been resized in order to be able to compute the number of lines properly.
   if ( _no == 0 ) {
      _app->adjustComponents();
   }
}

//------------------------------------------------------------------------------
//
uint XxText::getDisplayWidth() const
{
   return contentsRect().width();
}

//------------------------------------------------------------------------------
//
QString XxText::formatClipboardLine(
   const QString& clipboardFormat,
   const XxFno    fileno,
   const XxFln    lineno,
   const QString& filename,
   const QString& lineContents
)
{
   QString forline = clipboardFormat;
   
   typedef int PosType;
   PosType notfound = -1;

   PosType pos = 0;

   // Fileno.
   while ( 1 ) {
      PosType spos = forline.find( "%N", pos );
      if ( spos == notfound ) {
         break;
      }
      QString buf;
      buf.sprintf( "%d", fileno );
      forline.replace( spos, 2, buf );
      pos = spos + buf.length();
   }

   // Lineno.
   pos = 0;
   while ( 1 ) {
      PosType spos = forline.find( "%L", pos );
      if ( spos == notfound ) {
         break;
      }
      QString buf;
      buf.sprintf( "%d", lineno );
      forline.replace( spos, 2, buf );
      pos = spos + buf.length();
   }

   // Filename.
   pos = 0;
   while ( 1 ) {
      PosType spos = forline.find( "%F", pos );
      if ( spos == notfound ) {
         break;
      }
      forline.replace( spos, 2, filename );
      pos = spos + filename.length();
   }

   // Line contents.
   pos = 0;
   while ( 1 ) {
      PosType spos = forline.find( "%s", pos );
      if ( spos == notfound ) {
         break;
      }
      forline.replace( spos, 2, lineContents );
      pos = spos + lineContents.length();
   }

   return forline;
}

XX_NAMESPACE_END


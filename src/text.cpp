/******************************************************************************\
 * $Id: text.cpp 298 2001-10-23 03:18:14Z blais $
 * $Date: 2001-10-22 23:18:14 -0400 (Mon, 22 Oct 2001) $
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

XX_NAMESPACE_BEGIN

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
   setBackgroundMode( NoBackground );
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
   //XX_TRACE( "painting!" );

   // QPainter p;
   // p.begin( this );
   QPainter& p = *pp;
   QRect rect = contentsRect();

   // We want 1:1 pixel/coord ratio.
   p.setViewport( rect );
   rect.moveBy( -rect.x(), -rect.y() );
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


   // Should we set the clip region?

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
      resources.getBoolOpt( BOOL_HORIZONTAL_DIFFS ) &&  
      ! diffs->isDirectoryDiff();

   const int x = 0 - horizontalPos;
   int y = 0;
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
      QBrush brush( bcolor );
      p.setPen( fcolor );

      // Render text.
      XxFln fline = line.getLineNo( _no );
      if ( fline != -1 ) {
         uint length;
         const char* lineText = file->getTextLine( fline, length );
         
         int lhd = line.getLeftHdiffPos( _no );
         int rhd = line.getRightHdiffPos( _no );

         int rlength;
         const char* renderedText = file->renderTextWithTabs( 
            lineText, length, tabWidth, rlength, lhd, rhd
         );

         if ( hori && 
              ( line.getType() == XxLine::DIFF_1 ||
                line.getType() == XxLine::DIFF_2 ||
                line.getType() == XxLine::DIFF_3 ||
                line.getType() == XxLine::DIFF_ALL ||
                line.getType() == XxLine::DIFFDEL_1 ||
                line.getType() == XxLine::DIFFDEL_2 ||
                line.getType() == XxLine::DIFFDEL_3
              ) && 
              line.hasHorizontalDiffs( _no ) ) {

            //
            // Render with horizontal diffs.
            //
            QColor bcolorSup;
            QColor fcolorSup;
            resources.getRegionColor( idtypeSup, bcolorSup, fcolorSup );
            QBrush brushSup( bcolorSup );

            // Pre-part.
            p.setPen( fcolorSup );
            int lx = x;
            int prelen = lhd;
            XX_CHECK( prelen >= 0 );
            if ( prelen > 0 ) {
               QString prestr;
               if ( prelen > 0 ) {
                  prestr.setLatin1( renderedText, prelen );
               }
               int prewidth = fm.width( prestr, prelen );
               p.fillRect( lx, y, prewidth, fm.lineSpacing(), brushSup );
               p.drawText( lx, y + fm.ascent(), prestr, lhd );
               lx += prewidth;
            }

            // Mid-part.
            p.setPen( fcolor );
            int midlen = rhd - lhd + 1;
            XX_CHECK( prelen >= 0 );
            if ( midlen > 0 ) {
               QString midstr;
               if ( midlen > 0 ) {
                  midstr.setLatin1( renderedText + lhd, midlen );
               }
               int midwidth = fm.width( midstr, midlen );
               p.fillRect( lx, y, midwidth, fm.lineSpacing(), brush );
               p.drawText( lx, y + fm.ascent(), midstr, midlen );
               lx += midwidth;
            }

            // Post-part.
            p.setPen( fcolorSup );
            int postlen = rlength - rhd - 1;
            XX_CHECK( prelen >= 0 );
            if ( postlen > 0 ) {
               QString poststr;
               if ( postlen > 0 ) {
                  poststr.setLatin1( renderedText + rhd + 1, postlen );
               }
               int postwidth = fm.width( poststr, postlen );
               p.fillRect( lx, y, postwidth, fm.lineSpacing(), brushSup );
               p.drawText( lx, y + fm.ascent(), poststr, postlen );
               lx += postwidth;
           }

            // Filler part.
            int fillerwidth = w - lx;
            p.fillRect( lx, y, fillerwidth, fm.lineSpacing(), brushSup );
         }
         else {
            //
            // Render without horizontal diffs.
            //            
            QString str( renderedText );

            p.fillRect( 0, y, w, fm.lineSpacing(), brush );
            p.drawText( x, y + fm.ascent(), str );
         }
      }
      else {
         // The line is empty, just fill in the background.
         p.fillRect( 0, y, w, fm.lineSpacing(), brush );
      }
   }

   // Fill in at the bottom if necessary (at end of text).
   if ( y < h ) {
      QColor backgroundColor = 
         resources.getColor( COLOR_BACKGROUND );
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

      QColor vlineColor =
         resources.getColor( COLOR_VERTICAL_LINE );
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

   const QFont& font = _app->getResources().getFontText();
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


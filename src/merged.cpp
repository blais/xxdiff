/******************************************************************************\
 * $Id: merged.cpp 419 2001-11-27 02:14:11Z blais $
 * $Date: 2001-11-26 21:14:11 -0500 (Mon, 26 Nov 2001) $
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

#include <merged.h>
#include <text.h>
#include <app.h>
#include <resources.h>
#include <diffs.h>
#include <buffer.h>

#include <qscrollview.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qlayout.h>

#include <qapplication.h>
#include <qclipboard.h>

#include <math.h>
#include <stdio.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxMergedText
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxMergedText::XxMergedText( 
   XxMergedFrame* main, 
   XxApp*         app, 
   QWidget*       parent, 
   const char*    name 
) :
   BaseClass( parent, name, WResizeNoErase ),
   _main( main ),
   _app( app ),
   _topLine( 1 ),
   _grab( false )
{
   setFrameStyle( QFrame::Panel | QFrame::Sunken );
   setLineWidth( 2 );
   setBackgroundMode( NoBackground );

   const XxDiffs* diffs = _app->getDiffs();
   if ( diffs != 0 ) {
      // Note: what to do on rediff, when the diffs object changes?
      connect( diffs, SIGNAL(changed()), this, SLOT(adjustVertically()) );
   }
}

// OK ----- This is the same as XxText.
//------------------------------------------------------------------------------
//
XxMergedText::~XxMergedText()
{
}

// OK ----- This is the same as XxText.
//------------------------------------------------------------------------------
//
QSizePolicy XxMergedText::sizePolicy() const
{
   return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

//------------------------------------------------------------------------------
//
void XxMergedText::drawContents( QPainter* pp )
{
   //XX_TRACE( "painting!" );

// OK ----- This is the same as XxText.
   // QPainter p;
   // p.begin( this );
   QPainter& p = *pp;
   QRect rect = contentsRect();

// OK ----- This is the same as XxText.
   // We want 1:1 pixel/coord ratio.
   p.setViewport( rect );
   rect.moveBy( -rect.x(), -rect.y() );
   p.setWindow( rect );
   int w = rect.width();
   int h = rect.height();

// NOT ---- This is different from XxText.
   XxBuffer* file[3];
   for ( uint ii = 0; ii < _app->getNbFiles(); ++ii ) {
      file[ii] = _app->getBuffer( ii );
   }
// OK ----- This is the same as XxText.
   const XxDiffs* diffs = _app->getDiffs();
   const XxResources& resources = _app->getResources();

// OK ----- This is the same as XxText.
   // If it is empty, erase the whole widget with blank color.
   if ( diffs == 0 ) {
      QColor backgroundColor = resources.getColor( COLOR_BACKGROUND );
      QBrush brush( backgroundColor );
      p.fillRect( rect, brush );

      p.end();
      return;
   }

   //
   // Draw appropriate content.
   //
   XxDln topLine = getTopLine();
   XxDln cursorLine = _app->getCursorLine();
   uint horizontalPos = _main->getHorizontalScrollbar()->value();
   uint tabWidth = resources.getTabWidth();

   // Should we set the clip region?

   // Font.
   p.setFont( resources.getFontText() );
   QFontMetrics fm = p.fontMetrics();

   // Don't draw background of chars since we'll draw first.
   p.setBackgroundMode( TransparentMode );
   QPen pen;

   // uint displayLines = _nbDisplayLines;

   const int x = 0 - horizontalPos;
   int prevy = 0;
   int y = 0;
   uint nbLines = 0;
   XxDln curLine = topLine;
   bool prevKnown = true;
   XxFln prevfline = -1;
   while ( y <= h && curLine <= XxDln( diffs->getNbLines() ) ) {
      
      // Get line to display.
      const XxLine& line = diffs->getLine( curLine );

      int no;
      XxFln fline;
      bool known = line.getSelectedText( no, fline );
      if ( known == true ) {
         if ( fline != -1 ) {

            // Set background and foreground colors.
            QColor bcolor;
            QColor fcolor;
            if ( line.getType() == XxLine::SAME ) {
               resources.getRegionColor( COLOR_SAME, bcolor, fcolor );
            }
            else {
               XxColor res = COLOR_BACKGROUND;
               // prevent warning

               switch ( no ) {
                  case 0: res = COLOR_MERGED_DECIDED_1; break;
                  case 1: res = COLOR_MERGED_DECIDED_2; break;
                  case 2: res = COLOR_MERGED_DECIDED_3; break;
               }
               resources.getRegionColor( res, bcolor, fcolor );
            }
            QBrush brush( bcolor );
            p.setPen( fcolor );
            
            uint length;
            const char* lineText = file[no]->getTextLine( fline, length );

            int rlength;
            const char* renderedText = file[no]->renderTextWithTabs( 
               lineText, length, tabWidth, rlength, 0
            );
            
            //
            // Render without horizontal diffs.
            //
            QString str( renderedText );
            
            p.fillRect( 0, y, w, fm.lineSpacing(), brush );
            p.drawText( x, y + fm.ascent(), str );
            
            prevy = y;
            y += fm.lineSpacing();
            nbLines++;
         }
         else if ( prevfline != -1 ) {

            QColor bcolor;
            QColor fcolor;
            XxColor res = COLOR_BACKGROUND;
            // warning

            switch ( no ) {
               case 0: res = COLOR_MERGED_DECIDED_1; break;
               case 1: res = COLOR_MERGED_DECIDED_2; break;
               case 2: res = COLOR_MERGED_DECIDED_3; break;
            }
            resources.getRegionColor( res, bcolor, fcolor );
            QBrush brush( bcolor );
            p.setPen( fcolor );

            p.fillRect( 0, y, w, HEIGHT_DELETED_REGION, brush );

            prevy = y;
            y += HEIGHT_DELETED_REGION;
         }
      }
      prevfline = fline;

      // Detect changes and insert marker.
      if ( prevKnown != known && known == false ) {
         
         QColor bcolor;
         QColor fcolor;
         resources.getRegionColor( COLOR_MERGED_UNDECIDED, bcolor, fcolor );
         QBrush brush( bcolor );
         p.setPen( fcolor );

         int hch = fm.lineSpacing();
         p.fillRect( 0, y, w, hch, brush );

         brush.setColor( fcolor );
         brush.setStyle( QBrush::BDiagPattern );
         p.fillRect( 0, y, w, hch, brush );

         prevy = y;
         y += hch;
         nbLines++;
      }
      prevKnown = known;
      
      // Draw line cursor.
      if ( curLine == cursorLine ) {
         QColor cursorColor = 
            resources.getColor( COLOR_CURSOR );
         p.setPen( cursorColor );
         p.drawRect( 0, prevy, w, y - prevy );
      }

      curLine++;
   }

   // Fill in at the bottom if necessary (at end of text).
   if ( y < h ) {
      QColor backgroundColor = 
         resources.getColor( COLOR_BACKGROUND );
      QBrush brush( backgroundColor );
      p.fillRect( x, y, w, h - y, brush );
   }

   // p.end();
}

//------------------------------------------------------------------------------
//
void XxMergedText::verticalScroll( int )
{
   update();
}

//------------------------------------------------------------------------------
//
void XxMergedText::horizontalScroll( int )
{
   // Note: todo fix adjustment correctly.
   // Hide it when necessary. 
   // We need to implement this one like for the other text views.
   // Perhaps we can derive this widget from the text view widget?
   // They should definitely share some commonalities.
   update();
}

//------------------------------------------------------------------------------
//
void XxMergedText::adjustVertically()
{
   // Adjust vertical scrollbar.
   XxDln topLine = getTopLine();

   const XxDiffs* diffs = _app->getDiffs();
   QScrollBar* vscroll = _main->getVerticalScrollbar();
   if ( diffs == 0 ) {
      vscroll->setRange( 0,0 );
   }
   else {
      uint safetyMargin = 4;
      // Leave some extra space for a screenful of alternating ignore and
      // deleted markers.
      
      XxDln maxLine = diffs->moveBackwardsVisibleLines(
         diffs->getNbLines(), _nbDisplayLines, 1
      );
      vscroll->setRange( 0, maxLine + safetyMargin );
   }

   setTopLine( topLine );
}

//------------------------------------------------------------------------------
//
XxDln XxMergedText::getTopLine() const
{
   return XxDln( _main->getVerticalScrollbar()->value() + 1 );
}

//------------------------------------------------------------------------------
//
void XxMergedText::setTopLine( XxDln lineNo ) const
{
   const XxDiffs* diffs = _app->getDiffs();

   XxDln validLine = std::max(
      XxDln(1),
      std::min( XxDln( diffs->getNbLines() ), lineNo )
   );
   
   XxDln maxLine = diffs->moveBackwardsVisibleLines(
      diffs->getNbLines(), _nbDisplayLines, 1
   );
   XxDln displayableLine = 
      std::min( validLine, std::max( XxDln(1), maxLine ) );
   _main->getVerticalScrollbar()->setValue( displayableLine - 1 );
}

//------------------------------------------------------------------------------
//
void XxMergedText::setCenterLine( XxDln lineNo )
{
   const XxDiffs* diffs = _app->getDiffs();
   XxDln newTopLine = diffs->moveBackwardsVisibleLines(
      lineNo, _nbDisplayLines / 2, 1
   );
   setTopLine( newTopLine );
}

//------------------------------------------------------------------------------
//
void XxMergedText::mousePressEvent( QMouseEvent* event )
{
   const QFont& font = _app->getResources().getFontText();
   QFontMetrics fm( font );
   XxDln dlineno = event->y() / fm.lineSpacing();

   // Activate popup in third button.
   if ( event->button() == RightButton &&
        event->state() & ControlButton ) {
         _grab = true;
         _grabTopLine = getTopLine();
         _grabDeltaLineNo = dlineno;
   }

   // Note: Todo: clicking in the merged view should also map the cursor in the
   // main view.
}

//------------------------------------------------------------------------------
//
void XxMergedText::mouseMoveEvent( QMouseEvent* event )
{
   if ( _grab ) {
      const QFont& font = _app->getResources().getFontText();
      QFontMetrics fm( font );
      XxDln dlineno = event->y() / fm.lineSpacing();
      setTopLine( _grabTopLine + (_grabDeltaLineNo - dlineno) );
   }
}

//------------------------------------------------------------------------------
//
void XxMergedText::mouseReleaseEvent( QMouseEvent* /*event*/ )
{
   // Release grab in all case. It won't hurt.
   _grab = false;
}

//------------------------------------------------------------------------------
//
void XxMergedText::resizeEvent( QResizeEvent* /*ev*/ )
{
   // Compute nb. display lines.
   const QFont& font = _app->getResources().getFontText();
   QFontMetrics fm( font );
   _nbDisplayLines = contentsRect().height() / fm.lineSpacing() + 1;

   // Adjust horizontal scrollbar.
   int textWidth = _app->getTextWidth();

   // Leave a pixel margin to the right of text.
   textWidth += 4; // 4 pixels.
   QScrollBar* hscroll = _main->getHorizontalScrollbar();
   if ( textWidth <= width() ) {
      hscroll->setRange( 0, 0 );
   }
   else {
      hscroll->setSteps( 1, width() );
      hscroll->setRange( 0, textWidth - width() );
   }
   
   if ( int( textWidth - width() ) < hscroll->value() ) {
      hscroll->setValue( textWidth - width() );
      // Note: this will indirectly trigger a redraw.
   }

   adjustVertically();
}

/*==============================================================================
 * CLASS XxMergedFrame
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxMergedFrame::XxMergedFrame( 
   XxApp*      app, 
   QWidget*    parent, 
   const char* name 
) :
   BaseClass( parent, name )
{
   QVBoxLayout* vlayout = new QVBoxLayout( this );
   QHBoxLayout* hlayout = new QHBoxLayout( vlayout );

   _merged = new XxMergedText( this, app, this );
   hlayout->addWidget( _merged );

   _vscroll = new QScrollBar( this );
   _vscroll->setFixedWidth( 20 );
   hlayout->addWidget( _vscroll );

   _hscroll = new QScrollBar( Qt::Horizontal, this );
   _hscroll->setFixedHeight( 20 );

   vlayout->addWidget( _hscroll );

   connect( _vscroll, SIGNAL(valueChanged(int)),
            _merged, SLOT(verticalScroll(int)) );
   connect( _hscroll, SIGNAL(valueChanged(int)),
            _merged, SLOT(horizontalScroll(int)) );

   // Track application's scrolling window.
   connect( app, SIGNAL(cursorChanged(int)),
            this, SLOT(appCursorChanged(int)) );
   connect( app, SIGNAL(scrolled(int)), this, SLOT(appScrolled(int)) );
}

//------------------------------------------------------------------------------
//
QScrollBar* XxMergedFrame::getHorizontalScrollbar()
{
   return _hscroll;
}

//------------------------------------------------------------------------------
//
QScrollBar* XxMergedFrame::getVerticalScrollbar()
{
   return _vscroll;
}

//------------------------------------------------------------------------------
//
void XxMergedFrame::update()
{
   BaseClass::update();
   _merged->update();  // We need this (verified Mon Nov 26 21:12:10 EST 2001),
                       // otherwise the merged view does not update when the
                       // cursor does not change but the selection changes.
}

//------------------------------------------------------------------------------
//
void XxMergedFrame::appCursorChanged( int cursorLine )
{
   // When cursor changes, try to track it at the center line of the merged
   // view.  Important note: this will try to set as the center line what SHOULD
   // be the center line if the diffs were all expanded.  Thus the region where
   // the cursor would show up in the merged view will most of the time be
   // higher than the center, but never above the top of the window.  This is a
   // good heuristic, since it allows you to see more of the region when
   // collapsing/expanding (IOW if the merged view was really centered that
   // wouldn't be very good because the top half wouldn't be very useful (you do
   // need some though, for context).  If you don't understand all this shtuff,
   // just think that this is why the merged view isn't really centered on the
   // cursor.
   _merged->setCenterLine( cursorLine );
}

//------------------------------------------------------------------------------
//
void XxMergedFrame::appScrolled( int /*topLine*/ )
{
   // Use this to track the top line of the application.
   // _merged->setTopLine( topLine );
}

/*==============================================================================
 * CLASS XxMergedWindow
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxMergedWindow::XxMergedWindow( 
   XxApp*      app, 
   QWidget*    parent, 
   const char* name 
) :
   BaseClass( parent, name ),
   _app( app )
{
   const XxResources& resources = app->getResources();

   QPopupMenu* menu = new QPopupMenu;
   menu->insertItem( 
      "Close", this, SLOT(hide()),
      resources.getAccelerator( ACCEL_MERGED_CLOSE )
   );

   QMenuBar* m = menuBar();
   m->setSeparator( QMenuBar::InWindowsStyle );
   m->insertItem( "W&indow", menu );

   _frame = new XxMergedFrame( app, this, "merged_frame" );

   setCentralWidget( _frame );
}

//------------------------------------------------------------------------------
//
void XxMergedWindow::update()
{
   BaseClass::update();
   _frame->update(); // FIXME do we really need this?
}

//------------------------------------------------------------------------------
//
void XxMergedWindow::show()
{
   BaseClass::show();
   // Nop for now.
}

//------------------------------------------------------------------------------
//
void XxMergedWindow::hide()
{
   BaseClass::hide();
   _app->synchronizeUI();
}

XX_NAMESPACE_END

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

#include <copyLabel.h>

#include <kdeSupport.h>

#include <QtGui/QToolTip>

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QResizeEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QLabel>


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxCopyLabel
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxCopyLabel::XxCopyLabel( QWidget* parent ) :
   QLabel( parent )
{
   setAlignment( (alignment() & !Qt::AlignLeft) | Qt::AlignCenter );
}

//------------------------------------------------------------------------------
//
XxCopyLabel::~XxCopyLabel()
{
}

//------------------------------------------------------------------------------
//
void XxCopyLabel::setText( const QString& s )
{
   _fulltext = s;
   QLabel::setText( s );
}

//------------------------------------------------------------------------------
//
const QString& XxCopyLabel::getFullText() const
{
   return _fulltext;
}

//------------------------------------------------------------------------------
//
void XxCopyLabel::mousePressEvent( QMouseEvent* event )
{
   QClipboard *cb = QkApplication::clipboard();
   cb->setText( _fulltext, cb->supportsSelection() ? 
                       QClipboard::Selection :
                       QClipboard::Clipboard );
   QLabel::mousePressEvent( event );
}

//------------------------------------------------------------------------------
//
QSize XxCopyLabel::sizeHint() const
{
   QSize hint = QLabel::sizeHint();
   hint.setWidth( 0 );
   return hint;
}

//------------------------------------------------------------------------------
//
void XxCopyLabel::resizeEvent( QResizeEvent* event )
{
   setText( _fulltext );
   QString tex = text();
   QFontMetrics fm = fontMetrics();
   QRect br = fm.boundingRect( tex );
   while ( br.width() + SAFETY_OFFSET > width() ) {

      // Remove beginning part
      //
      // Note: also check for '\' in case we ever port to Windoze.
      int pos = -1;
      if ( tex.indexOf( "[...]/", 0 ) == 0 ) {
         pos = tex.indexOf( '/', 6 );
      }
      else {
         pos = tex.indexOf( '/', 0 );
      }

      if ( pos == -1 ) {
         break;
      }
      
      tex.replace( 0, pos + 1, "[...]/" );
      br = fm.boundingRect( tex );
   }
   
   QLabel::setText( tex );
   QLabel::resizeEvent( event );
}

//------------------------------------------------------------------------------
//
bool XxCopyLabel::event( QEvent* event )
{
   if ( event->type() == QEvent::ToolTip ) {
      QHelpEvent *helpEvent = static_cast<QHelpEvent *>( event );
      QRect br = fontMetrics().boundingRect( _fulltext );
      if (  br.width() + SAFETY_OFFSET > size().width() ) {
         QToolTip::showText( helpEvent->globalPos(), _fulltext );
      } else {
         event->ignore();
      }
      return true;
   }
   return QWidget::event(event);
}


XX_NAMESPACE_END

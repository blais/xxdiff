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

#include <qtooltip.h>

#include <qapplication.h>
#include <qclipboard.h>


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

XX_NAMESPACE_BEGIN

/*==============================================================================
 * LOCAL CLASS XxCopyLabel::Tip
 *============================================================================*/

// <summary> a tooltip that only tips when the string of an associated label is
// too long.  </summary>

class XxCopyLabel::Tip : public QToolTip {

public:

   /*----- member functions -----*/

   // Constructor.
   Tip( QLabel* parent );

   // See base class.
   virtual void maybeTip( const QPoint& );

};

//------------------------------------------------------------------------------
//
XxCopyLabel::Tip::Tip( QLabel* parent ) :
   QToolTip( parent )
{
}

//------------------------------------------------------------------------------
//
void XxCopyLabel::Tip::maybeTip( const QPoint& )
{
   XxCopyLabel* clabel = static_cast<XxCopyLabel*>( parentWidget() );
   XX_CHECK( clabel != 0 );
   QString tex = clabel->getFullText();
   QFontMetrics fm = clabel->fontMetrics();
   QRect br = fm.boundingRect( tex );
   if ( br.width() + XxCopyLabel::SAFETY_OFFSET > clabel->size().width() ) {
      tip( clabel->rect(), tex );
   }
}

/*==============================================================================
 * CLASS XxCopyLabel
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxCopyLabel::XxCopyLabel( QWidget* parent ) :
   QLabel( parent )
{
   setAlignment( (alignment() & !Qt::AlignLeft) | Qt::AlignCenter );
   _tip = new Tip( this );
}

//------------------------------------------------------------------------------
//
XxCopyLabel::~XxCopyLabel()
{
   delete _tip;
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
   cb->setText( _fulltext );
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
      if ( tex.find( "[...]/", 0 ) == 0 ) {
         pos = tex.find( '/', 6 );
      }
      else {
         pos = tex.find( '/', 0 );
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

XX_NAMESPACE_END

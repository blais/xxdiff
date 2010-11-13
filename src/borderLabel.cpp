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

#include <borderLabel.h>
#include <app.h>

#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QColor>

#include <QtGui/QApplication>
#include <QtGui/QLabel>

#include <stdio.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxBorderLabel
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxBorderLabel::XxBorderLabel( 
   BorderFlags     bf,
   QWidget*        parent,
   Qt::WindowFlags wf
) :
   QLabel( parent, wf ),
   _borders( bf )
{
}

//------------------------------------------------------------------------------
//
XxBorderLabel::XxBorderLabel( 
   const QString& text,
   BorderFlags     bf,
   QWidget*        parent,
   Qt::WindowFlags wf
) :
   QLabel( text, parent, wf ),
   _borders( bf )
{
}

//------------------------------------------------------------------------------
//
XxBorderLabel::~XxBorderLabel()
{
}

//------------------------------------------------------------------------------
//
void XxBorderLabel::paintEvent( QPaintEvent *e )
{
   QLabel::paintEvent(e);

   QPainter p( this );
   QRect rect = contentsRect();

   p.setPen( palette().color( backgroundRole() ).darker( 135 ) );

   if ( _borders & BorderLeft ) {
      p.drawLine( rect.topLeft(), rect.bottomLeft() );
   }
   if ( _borders & BorderRight ) {
      p.drawLine( rect.topRight(), rect.bottomRight() );
   }
   if ( _borders & BorderBottom ) {
      p.drawLine( rect.bottomLeft(), rect.bottomRight() );
   }
   if ( _borders & BorderTop ) {
      p.drawLine( rect.topLeft(), rect.topRight() );
   }
}

XX_NAMESPACE_END

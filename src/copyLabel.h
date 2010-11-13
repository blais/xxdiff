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

#ifndef INCL_XXDIFF_COPYLABEL
#define INCL_XXDIFF_COPYLABEL

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_BORDERLABEL
#include <borderLabel.h>
#endif

#ifndef INCL_QT_QLABEL
#include <QtGui/QLabel>
#define INCL_QT_QLABEL
#endif

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QScrollBar;
class QResizeEvent;
class QMouseEvent;
class QEvent;

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxCopyLabel
 *============================================================================*/

// <summary> a label that puts its contents into the clipboard when clicked on
// </summary>

class XxCopyLabel : public XxBorderLabel {

   Q_OBJECT

public:

   /*----- types and enumerations -----*/

   enum {
      SAFETY_OFFSET = 10
   };

   /*----- member functions -----*/

   // Constructor.
   XxCopyLabel( QWidget* parent = 0 );

   // Destructor.
   virtual ~XxCopyLabel();

   // See base class.
   // <group>
   virtual QSize sizeHint() const;
   void setText( const QString& );
   virtual void mousePressEvent( QMouseEvent* );
   virtual void resizeEvent( QResizeEvent* );
   // </group>

   // Returns the full text of the label.
   const QString& getFullText() const;

protected:

   bool event( QEvent* );

private:

   /*----- data members -----*/

   QString _fulltext;

};

XX_NAMESPACE_END

#endif

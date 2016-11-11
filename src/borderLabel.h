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

#ifndef INCL_XXDIFF_BORDERLABEL
#define INCL_XXDIFF_BORDERLABEL

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_TYPES
#include <types.h>
#endif

#ifndef INCL_QT_QLABEL
#include <QLabel>
#define INCL_QT_QLABEL
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxBorderLabel
 *============================================================================*/

// <summary> a widget that can display line numbers </summary>

class XxBorderLabel : public QLabel {

public:

   /*----- types and enumerations -----*/

   enum BorderFlag { 
      BorderNone   = 0,
      BorderLeft   = (1 << 0),
      BorderRight  = (1 << 1),
      BorderTop    = (1 << 2),
      BorderBottom = (1 << 3)
   };
   Q_DECLARE_FLAGS(BorderFlags, BorderFlag)

   /*----- member functions -----*/

   // Constructors.
   XxBorderLabel(
      BorderFlags bf = BorderNone,
      QWidget* parent = 0,
      Qt::WindowFlags wf = 0
   );
   XxBorderLabel( 
      const QString& text,
      BorderFlags bf = BorderNone,
      QWidget* parent = 0,
      Qt::WindowFlags wf = 0
   );

   // Destructor.
   virtual ~XxBorderLabel();

   // See base class.
   virtual void paintEvent( QPaintEvent* );

private:

   /*----- data members -----*/

    BorderFlags _borders;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(XxBorderLabel::BorderFlags)


XX_NAMESPACE_END

#endif

/******************************************************************************\
 * $Id: suicideMessageBox.h 432 2001-11-30 07:21:57Z blais $
 * $Date: 2001-11-30 02:21:57 -0500 (Fri, 30 Nov 2001) $
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
 ******************************************************************************/

#ifndef INCL_XXDIFF_SUICIDEMESSAGEBOX
#define INCL_XXDIFF_SUICIDEMESSAGEBOX

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_QT_QMESSAGEBOX
#include <qmessagebox.h>
#define INCL_QT_QMESSAGEBOX
#endif

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

XX_NAMESPACE_BEGIN

/*==============================================================================
 * LOCAL CLASS XxSuicideMessageBox
 *============================================================================*/

// <summary> simple non-modal dialog that cares after itself </summary>

class XxSuicideMessageBox : public QMessageBox {

public:

   /*----- member functions -----*/

   // Constructor.
   XxSuicideMessageBox( 
      QWidget*       parent,
      const QString& caption, 
      const QString& text, 
      Icon           icon 
   );

   // See base class.
   virtual void done( int r );

};

XX_NAMESPACE_END

#endif

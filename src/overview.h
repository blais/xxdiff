/******************************************************************************\
 * $Id: overview.h 138 2001-05-20 18:08:45Z blais $
 * $Date: 2001-05-20 14:08:45 -0400 (Sun, 20 May 2001) $
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

#ifndef INCL_XXDIFF_OVERVIEW
#define INCL_XXDIFF_OVERVIEW

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_QT_QFRAME
#include <qframe.h>
#define INCL_QT_QFRAME
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;


/*==============================================================================
 * CLASS XxOverview
 *============================================================================*/

// <summary> overview area </summary>

class XxOverview : public QFrame {

public:

   /*----- member functions -----*/

   // Constructor.
   XxOverview( XxApp* app, QWidget * parent = 0, const char* name = 0 );

   // Destructor.
   virtual ~XxOverview();

   // See base class.
   virtual void drawContents( QPainter* );

   // Override to expand.
   virtual QSizePolicy sizePolicy() const;

private:

   /*----- data members -----*/

   XxApp*	_app;

};


XX_NAMESPACE_END

#endif

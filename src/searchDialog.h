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

#ifndef INCL_XXDIFF_SEARCHDIALOG
#define INCL_XXDIFF_SEARCHDIALOG

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_RESOURCES
#include <resources.h>
#endif

#ifndef INCL_SEARCHDIALOGBASE
#include <searchDialogBase.h>
#define INCL_SEARCHDIALOGBASE
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;

/*==============================================================================
 * CLASS XxSearchDialog
 *============================================================================*/

// <summary> dialog for diff search </summary>

class XxSearchDialog : public XxSearchDialogBase {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef XxSearchDialogBase BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxSearchDialog( XxApp* app, QWidget* parent );

   // See base class.
   virtual void show();

public slots:

   /*----- member functions -----*/

   void onApply();
   void onGotoLine();

private:

   /*----- data members -----*/

   XxApp* _app;

};

XX_NAMESPACE_END

#endif

/******************************************************************************\
 * $Id: dialogs.h 2 2000-09-15 02:19:22Z blais $
 * $Date: 2000-09-14 22:19:22 -0400 (Thu, 14 Sep 2000) $
 *
 * Copyright (C) 1999, 2000  Martin Blais <blais@iro.umontreal.ca>
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

#ifndef INCL_XXDIFF_DIALOGS
#define INCL_XXDIFF_DIALOGS

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_QT_QDIALOG
#include <qdialog.h>
#define INCL_QT_QDIALOG
#endif


/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QLineEdit;


XX_NAMESPACE_BEGIN


/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;


/*==============================================================================
 * CLASS XxSearchDialog
 *============================================================================*/

// <summary> simple non-modal dialog that cares after itself </summary>

class XxSearchDialog : public QDialog {

   Q_OBJECT

public:

   /*----- member functions -----*/

   // Constructor.
   XxSearchDialog( XxApp* app, QWidget* parent );

public slots:

   /*----- member functions -----*/

   void onSearch();
   void onCancel();

private:

   /*----- data members -----*/

   XxApp* 	_app;
   QLineEdit* 	_e;

};

/*==============================================================================
 * CLASS XxTabWidthDialog
 *============================================================================*/

// <summary> dialog to change tab width </summary>

class XxTabWidthDialog : public QDialog {

   Q_OBJECT

public:

   /*----- member functions -----*/

   // Constructor.
   XxTabWidthDialog( XxApp* app, QWidget* parent );

   // See base class.
   virtual void show();

public slots:

   /*----- member functions -----*/

   void onSet();
   void onClose();

private:

   /*----- data members -----*/

   XxApp* 	_app;
   QLineEdit* 	_e;

};


XX_NAMESPACE_END

#endif

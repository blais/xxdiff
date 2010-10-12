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

#ifndef INCL_XXDIFF_MARKERS
#define INCL_XXDIFF_MARKERS

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_MARKERSWIDGETBASE
#include <markersWidgetBase.h>
#define INCL_MARKERSWIDGETBASE
#endif

#ifndef INCL_STD_QFILEDIALOG
#include <QFileDialog>
#define INCL_STD_QFILEDIALOG
#endif

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QPushButton;

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxMarkersWidget
 *============================================================================*/

// <summary> widget for diff markers </summary>

// Note: although it should not be visible, this class must be present in the .h
// because of moc generation requirements.

class XxMarkersWidget : public XxMarkersWidgetBase {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef XxMarkersWidgetBase BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxMarkersWidget( 
      bool     threeWay,
      QWidget* parent = 0
   );

   // Queries.
   // <group>
   bool useConditionals() const;
   bool removeEmptyConditionals() const;
   QString getConditionalVariable1() const;
   QString getConditionalVariable2() const;
   QString getConditionalVariable3() const;
   // </group>


public slots:

   /*----- member functions -----*/

   void onConflicts();
   void onConditionals();

};

/*==============================================================================
 * CLASS XxMarkersDialog
 *============================================================================*/

// <summary> a dialog for the markers </summary>

class XxMarkersDialog : public QDialog {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef QDialog BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxMarkersDialog(
      const bool  threeWay,
      QWidget*    parent = 0
   );

   // Disable the cancel button.
   void disableCancel();

   // Method to override the rejection.
   virtual void reject();

   /*----- static member functions -----*/

   // Run the dialog and get the markers.
   // Returns true on success.
   static bool getMarkers(
      bool        threeWay,
      bool&       useConditionals,
      bool&       removeEmptyConditionals,
      QString     conditionals[3],
      bool        noCancel,
      QWidget*    parent = 0
   );

private:

   /*----- data members -----*/

   XxMarkersWidget* _markersWidget;
   QPushButton*     _buttonOk;
   QPushButton*     _buttonCancel;

};


/*==============================================================================
 * CLASS XxMarkersFileDialog
 *============================================================================*/

// <summary> a file dialog with markers added to it </summary>

class XxMarkersFileDialog : public QFileDialog {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef QFileDialog BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxMarkersFileDialog( 
      const QString& dirName,
      const QString& filter,
      bool           modal,
      bool           threeWay,
      QWidget*       parent = 0
   );

   /*----- static member functions -----*/

   // Special version of getSaveFileName which add marker selection widgets.
   static QString getSaveFileName( 
      const QString& startWith,
      const QString& filter,
      bool           threeWay,
      bool&          useConditionals,
      bool&          removeEmptyConditionals,
      QString        conditionals[3],
      QWidget*       parent = 0
   );

private:

   /*----- data members -----*/

   XxMarkersWidget* _markersWidget;

};

XX_NAMESPACE_END

#endif

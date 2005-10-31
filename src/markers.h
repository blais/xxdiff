/******************************************************************************\
 * $Id: markers.h 56 2000-12-25 20:15:47Z  $
 * $Date: 2000-12-25 15:15:47 -0500 (Mon, 25 Dec 2000) $
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

#ifndef INCL_XXDIFF_MARKERSFILEDIALOG
#define INCL_XXDIFF_MARKERSFILEDIALOG

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
#include <qfiledialog.h>
#define INCL_STD_QFILEDIALOG
#endif

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
      QWidget* parent,
      bool     threeWay
   );

   // Queries.
   // <group>
   bool isUseConditionals() const;
   QString getConditionalVariable1() const;
   QString getConditionalVariable2() const;
   // </group>


public slots:

   /*----- member functions -----*/

   void onConflicts();
   void onConditionals();

};

/*==============================================================================
 * CLASS XxMarkersFileDialog
 *============================================================================*/

// <summary> widget for diff markers </summary>

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
      QWidget*       parent,
      const char*    name,
      bool           modal,
      bool           threeWay
   );

   /*----- static member functions -----*/

   // Special version of getSaveFileName which add marker selection widgets.
   static QString getSaveFileName( 
      const QString& startWith,
      const QString& filter,
      QWidget*       parent,
      bool           threeWay,
      bool&          useConditionals,
      std::string&   conditionalVariable1,
      std::string&   conditionalVariable2
   );

private:

   /*----- data members -----*/

   XxMarkersWidget* _markersWidget;

};

XX_NAMESPACE_END

#endif

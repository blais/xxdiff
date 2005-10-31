/******************************************************************************\
 * $Id: help.h 2 2000-09-15 02:19:22Z blais $
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

#ifndef INCL_XXDIFF_HELP
#define INCL_XXDIFF_HELP

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

#ifndef INCL_STD_IOSFWD
#include <iosfwd>
#define INCL_STD_IOSFWD
#endif


/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QWidget;


XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxResources;


/*==============================================================================
 * CLASS XxHelp
 *============================================================================*/

// <summary> help methods </summary>

class XxHelp {

public:

   /*----- types and enumerations -----*/

   enum WhatsThisTextType {
      VSCROLL              = 0,
      HSCROLL              = 1,
      OVERVIEW		   = 2,
      FILENAME		   = 3,
      LINENO		   = 4,
      TEXT_VIEW		   = 5,
      MERGED_VIEW	   = 6
   };

   /*----- static member functions -----*/

   // Creates a new, self-destructive about dialog.  Just show it.
   static QDialog* getAboutDialog( QWidget* parent = 0 );

   // Creates a new, self-destructive color legend.  Just show it.
   static QDialog* getColorLegend( 
      QWidget*     parent = 0
   );

   // Creates a new, self-destructive man page dialog.  Just show it.
   static QDialog* getManPageDialog(
      QWidget*     parent = 0
   );

   // Returns the help text for specific widgets.
   static const char* getWhatsThisText( WhatsThisTextType type );

   // Print out usage message.
   static void dumpUsage( std::ostream& );

   // Print out version message.
   static void dumpVersion( std::ostream& );

   // Print out the resources and associated documentation.
   static void dumpResources( std::ostream& );

};


XX_NAMESPACE_END

#endif

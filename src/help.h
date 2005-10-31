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

   // Returns help strings.
   // <group>

   // Returns the help text for specific widgets.
   static const QString& getWhatsThisText( WhatsThisTextType type );

   // Returns version string.
   static QString getVersion();

   // Returns usage text. If `plain' is set to true, filter out formatting tags.
   static QString getUsage( int helpMask, bool plain = false );

   // Returns user's manual, formatted in HTML.
   static QString getManual();

   // Returns the invocation section, as plain text.
   static QString getInvocationTxt();

   // </group>


   // Creates dialogs.
   // <group>

   // Creates a new, self-destructive about dialog.  Just show it.
   static QDialog* getAboutDialog( QWidget* parent = 0 );

   // Creates a new, self-destructive man page dialog.  Just show it.
   static QDialog* getManPageDialog( QWidget* parent = 0 );

   // </group>


   // Utility function to XMLize output of resources.
   static QString xmlize( const QString& in );

};

XX_NAMESPACE_END

#endif


/******************************************************************************\
 * $Id: optionsDialog.h 48 2000-10-03 04:43:36Z  $
 * $Date: 2000-10-03 00:43:36 -0400 (Tue, 03 Oct 2000) $
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

#ifndef INCL_XXDIFF_OPTIONSDIALOG
#define INCL_XXDIFF_OPTIONSDIALOG

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_RESOURCES
#include <resources.h>
#endif

#ifndef INCL_OPTIONSDIALOGBASE
#include <optionsDialogBase.h>
#define INCL_OPTIONSDIALOGBASE
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxApp;

/*==============================================================================
 * CLASS XxOptionsDialog
 *============================================================================*/

// <summary> dialog for diff options </summary>

class XxOptionsDialog : public XxOptionsDialogBase {

   Q_OBJECT

   /*----- types and enumerations -----*/

   typedef XxOptionsDialogBase BaseClass;

public:

   /*----- member functions -----*/

   // Constructor.
   XxOptionsDialog( XxApp* app, QWidget* parent );

   // See base class.
   virtual void show();

   // Asks the dialog to synchronize itself.
   void synchronize();

   // Selects a particular tab. 
   // <group>
   void selectDiffOptions();
   void selectDisplayOptions();
   // </group>

   /*----- static member functions -----*/

   // Methods to manipulate command strings.
   // <group>
   static bool isInCommand(
      const std::string& command,
      const char*        option
   );

   static void addToCommand(
      std::string& command,
      const char*  option
   );
   static void removeFromCommand( 
      std::string& command,
      const char*  option
   );

   // Look for one of three options. If found, return 1, 2 or 3. If neither is
   // found, return 0. However, if one of the options is the empty string, if
   // none of the non-empty options is found, return the index of the empty
   // string option.
   static int isInCommand(
      const std::string& command,
      const char*        option1,
      const char*        option2,
      const char*        option3
   );

   // Set one option among three. One of the options may be the empty string.
   static void setOneOfInCommand(
      std::string& command,
      const char*  optionNew,
      const char*  optionOther1,
      const char*  optionOther2
   );
   // </group>

public slots:

   /*----- member functions -----*/

   void onApply();
   void onClose();

   //---------------------------------------------------------------------------
   // Files 2

   void checkboxIgnoreTrailingBlanks( int );
   void checkboxIgnoreWhitespace( int );
   void checkboxIgnoreCase( int );
   void checkboxIgnoreBlankLines( int );
   void radiobuttonQualityNormal( int );
   void radiobuttonQualityFastest( int );
   void radiobuttonQualityHighest( int );

   //---------------------------------------------------------------------------
   // Files 3

   //---------------------------------------------------------------------------
   // Directories

   void checkboxRecursive( int );

   //---------------------------------------------------------------------------
   // Display

   void checkboxHorizontalDiffs( int );
   void checkboxIgnoreHorizontalWhitespace( int );
   void checkboxHideCarriageReturns( int );
   void spinboxTabWidth( int );
   void checkboxIgnoreFileChanges( int );
   void checkboxCutAndPasteAnnotations( int );

   //---------------------------------------------------------------------------
   // Colors

   void listboxColors( const QString& );
   void editColor();


private:

   /*----- types and enumerationse -----*/

   typedef std::map< int, QColor > ColorModMap;

   /*----- member functions -----*/

   int getDiffPageIndex() const;

   void setFileDiffOptions(
      XxResources::Resource cmdOptionId,
      bool                  state
   ) const;

   void setFileDiffQuality(
      XxResources::Quality quality
   ) const;

   bool maybeSetCommand( 
      XxResources::Resource commandId,
      const char*           commandString
   );


   /*----- data members -----*/

   XxApp*      _app;
   ColorModMap _colorModMap;

};

XX_NAMESPACE_END

#endif

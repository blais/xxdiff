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

   // Destructor.
   virtual ~XxOptionsDialog();

   // See base class.
   virtual void show();

   // Selects a particular tab. 
   // <group>
   void selectDiffOptions();
   void selectDisplayOptions();
   // </group>

   /*----- static member functions -----*/

   // Methods to manipulate command strings.
   // <group>
   static bool isInCommand(
      const QString& command,
      const QString& option
   );

   static void addToCommand(
      QString&       command,
      const QString& option
   );
   static void removeFromCommand( 
      QString&       command,
      const QString& option
   );

   // Look for one of three options. If found, return 1, 2 or 3. If neither is
   // found, return 0. However, if one of the options is the empty string, if
   // none of the non-empty options is found, return the index of the empty
   // string option.
   static int isInCommand(
      const QString& command,
      const QString& option1,
      const QString& option2,
      const QString& option3
   );

   // Set one option among three. One of the options may be the empty string.
   static void setOneOfInCommand(
      QString&       command,
      const QString& optionNew,
      const QString& optionOther1,
      const QString& optionOther2
   );
   // </group>

public slots:

   /*----- member functions -----*/

   // Asks the dialog to synchronize itself.
   void synchronize();

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

   //     void checkboxRecursive( int );

   //---------------------------------------------------------------------------
   // Display

   //     void checkboxHorizontalDiffs( int );
   //     void checkboxIgnoreHorizontalWhitespace( int );
   //     void checkboxHideCarriageReturns( int );
   //     void spinboxTabWidth( int );
   //     void checkboxIgnoreFileChanges( int );
   //     void checkboxFormatClipboardText( int );
   //     void checkboxDrawVerticalLine( int );
   //     void comboIgnoreFile( const QString& );

   void editFontApp();
   void editFontText();

   //---------------------------------------------------------------------------
   // Colors

   void listboxColors( const QString& );
   void editColorFore();
   void editColorBack();


private:

   /*----- member functions -----*/

   int getDiffPageIndex() const;

   void setFileDiffOptions(
      XxCommandSwitch cmdOptionId,
      bool         state
   ) const;

   void setFileDiffQuality(
      XxQuality quality
   ) const;

   bool maybeSetCommand( 
      XxCommand      commandId,
      const QString& commandString
   );

   QString getFontDisplayText(
      const QFont& font
   ) const;
   
   /*----- data members -----*/

   XxApp* _app;

   // Note: perhaps we should keep a whole resources object for the dialog.
   QFont  _fontApp;
   QFont  _fontText;

};

XX_NAMESPACE_END

#endif


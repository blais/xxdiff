/******************************************************************************\
 * $Id: optionsDialog.cpp 250 2001-10-04 19:56:59Z blais $
 * $Date: 2001-10-04 15:56:59 -0400 (Thu, 04 Oct 2001) $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <optionsDialog.h>
#include <resources.h>
#include <diffs.h>
#include <app.h>

#include <qtabwidget.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstring.h>

#include <map>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxOptionsDialog
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxOptionsDialog::XxOptionsDialog( 
   XxApp*   app,
   QWidget* parent
) :
   BaseClass( parent ),
   _app( app )
{
   XX_CHECK( _app != 0 );

   XxDiffs* diffs = _app->getDiffs();
   if ( diffs != 0 ) {
      int cpage = getDiffPageIndex();

      // Disable other modes' options.
      for ( int ii = 0; ii < 3; ++ii ) {
#if QT_VERSION >= 220
         _tabWidget->setCurrentPage( ii );
#endif
         QWidget* widget = _tabWidget->currentPage();
         if ( ii != cpage && widget ) {
            widget->setEnabled( false );
         }
      }

#if QT_VERSION >= 220
      _tabWidget->setCurrentPage( cpage );
#endif
   }

   if ( diffs->isDirectoryDiff() == true ) {
      _groupboxFileDiffs->setEnabled( false );
   }
   else {
      _groupboxDirectoryDiffs->setEnabled( false );
   }

   // Make connections.

   connect( _buttonApply, SIGNAL( clicked() ), this, SLOT( onApply() ) );
   connect( _buttonOk, SIGNAL( clicked() ), this, SLOT( onApply() ) );

   //---------------------------------------------------------------------------
   // Files 2

   connect( _checkboxIgnoreTrailingBlanks, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreTrailingBlanks(int) ) );
   connect( _checkboxIgnoreWhitespace, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreWhitespace(int) ) );
   connect( _checkboxIgnoreCase, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreCase(int) ) );
   connect( _checkboxIgnoreBlankLines, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreBlankLines(int) ) );

   connect( _radiobuttonQualityNormal, SIGNAL( stateChanged(int) ), 
            this, SLOT( radiobuttonQualityNormal(int) ) );
   connect( _radiobuttonQualityFastest, SIGNAL( stateChanged(int) ), 
            this, SLOT( radiobuttonQualityFastest(int) ) );
   connect( _radiobuttonQualityHighest, SIGNAL( stateChanged(int) ), 
            this, SLOT( radiobuttonQualityHighest(int) ) );

   //---------------------------------------------------------------------------
   // Files 3

   //---------------------------------------------------------------------------
   // Directories

   connect( _checkboxRecursive, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxRecursive(int) ) );

   //---------------------------------------------------------------------------
   // Display

   connect( _checkboxHorizontalDiffs, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxHorizontalDiffs(int) ) );
   connect( _checkboxIgnoreHorizontalWhitespace, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreHorizontalWhitespace(int) ) );
   connect( _checkboxHideCarriageReturns, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxHideCarriageReturns(int) ) );
   connect( _spinboxTabWidth, SIGNAL( valueChanged(int) ), 
            this, SLOT( spinboxTabWidth(int) ) );
   connect( _checkboxDrawVerticalLine, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxDrawVerticalLine(int) ) );
   connect( _comboIgnoreFile, SIGNAL( textChanged(const QString&) ), 
            this, SLOT( comboIgnoreFile(const QString&) ) );

   connect( _checkboxIgnoreFileChanges, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreFileChanges(int) ) );

   connect( _checkboxFormatClipboardText, SIGNAL( stateChanged(int) ),
            this, SLOT( checkboxFormatClipboardText(int) ) );

   //---------------------------------------------------------------------------
   // Colors

   // Fill up listbox with color names.
   for ( int ii = int(XxResources::COLOR_FIRST);
         ii < int(XxResources::COLOR_LAST);
         ++ii ) {
      const char* resname = XxResources::getResourceName( 
         XxResources::Resource(ii) 
      );

      _listboxColors->insertItem( QString( resname ) );
   }

   connect( _listboxColors, SIGNAL( highlighted(const QString&) ), 
            this, SLOT( listboxColors(const QString&) ) );

   connect( _buttonEditColor, SIGNAL( clicked() ), 
            this, SLOT( editColor() ) );

}

//------------------------------------------------------------------------------
//
int XxOptionsDialog::getDiffPageIndex() const
{
   XxDiffs* diffs = _app->getDiffs();
   if ( diffs->isDirectoryDiff() == true ) {
      return 2;
   }
   else if ( _app->getNbFiles() == 2 ) {
      return 0;
   }
   // else {
   return 1;
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::show()
{
   synchronize();
   BaseClass::show();
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::synchronize()
{
   const XxResources* resources = XxResources::getInstance();
   XX_CHECK( resources != 0 );

   //---------------------------------------------------------------------------
   // Files 2

   XxResources::Resource cmdResId = XxResources::COMMAND_DIFF_FILES_2;

   _lineeditCommandFiles2->setText( 
      QString( resources->getCommand( cmdResId ) )
   );

   _checkboxIgnoreTrailingBlanks->setChecked( 
      resources->isCommandOption( cmdResId, 
                                  XxResources::CMDOPT_FILES_IGNORE_TRAILING )
   );
   _checkboxIgnoreWhitespace->setChecked( 
      resources->isCommandOption( cmdResId, 
                                  XxResources::CMDOPT_FILES_IGNORE_WHITESPACE )
   );
   _checkboxIgnoreCase->setChecked( 
      resources->isCommandOption( cmdResId, 
                                  XxResources::CMDOPT_FILES_IGNORE_CASE )
   );
   _checkboxIgnoreBlankLines->setChecked( 
      resources->isCommandOption( cmdResId, 
                                  XxResources::CMDOPT_FILES_IGNORE_BLANK_LINES )
   );

   QString rcmd = resources->getCommand( cmdResId );
   XxResources::Quality quality = resources->getQuality( rcmd );
   _radiobuttonQualityNormal->setChecked( 
      quality == XxResources::QUALITY_NORMAL 
   );
   _radiobuttonQualityFastest->setChecked( 
      quality == XxResources::QUALITY_FASTEST 
   );
   _radiobuttonQualityHighest->setChecked( 
      quality == XxResources::QUALITY_HIGHEST 
   );

   //---------------------------------------------------------------------------
   // Files 3

   _lineeditCommandFiles3->setText(
      QString( resources->getCommand( XxResources::COMMAND_DIFF_FILES_3 ) )
   );

   //---------------------------------------------------------------------------
   // Directories

   _lineeditCommandDirs->setText(
      QString( resources->getCommand( XxResources::COMMAND_DIFF_DIRECTORIES ) )
   );

   _lineeditCommandDirsRecursive->setText(
      QString( resources->getCommand( 
         XxResources::COMMAND_DIFF_DIRECTORIES_REC 
      ) )
   );

   _checkboxRecursive->setChecked(
      resources->getBoolOpt( XxResources::DIRDIFF_RECURSIVE )
   );

   //---------------------------------------------------------------------------
   // Display

   _checkboxHorizontalDiffs->setChecked(
      resources->getBoolOpt( XxResources::HORIZONTAL_DIFFS )
   );
   _checkboxIgnoreHorizontalWhitespace->setChecked(
      resources->getBoolOpt( XxResources::IGNORE_HORIZONTAL_WS )
   );
   _checkboxHideCarriageReturns->setChecked(
      resources->getBoolOpt( XxResources::HIDE_CR )
   );

   _spinboxTabWidth->setValue( resources->getTabWidth() );

   _checkboxDrawVerticalLine->setChecked(
      resources->getBoolOpt( XxResources::SHOW_VERTICAL_LINE )
   );

   _comboIgnoreFile->setCurrentItem( int( resources->getIgnoreFile() ) );


   _checkboxIgnoreFileChanges->setChecked(
      resources->getBoolOpt( XxResources::DIRDIFF_IGNORE_FILE_CHANGES )
   );

   _checkboxFormatClipboardText->setChecked(
      resources->getBoolOpt( XxResources::FORMAT_CLIPBOARD_TEXT )
   );

   _lineeditClipboardTextFormat->setText(
      resources->getClipboardTextFormat()
   );

   //---------------------------------------------------------------------------
   // Colors

}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::selectDiffOptions()
{
#if QT_VERSION >= 220
   _tabWidget->setCurrentPage( getDiffPageIndex() );
#endif
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::selectDisplayOptions()
{
#if QT_VERSION >= 220
   _tabWidget->setCurrentPage( 3 );
#endif
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::onApply()
{
   XxResources* resources = _app->getResourcesNC();
   XX_CHECK( resources );

   bool redoDiff = false;

   //---------------------------------------------------------------------------
   // Files 2

   // First set resources, then redodiff.
   redoDiff |= maybeSetCommand( 
      XxResources::COMMAND_DIFF_FILES_2,
      _lineeditCommandFiles2->text() 
   );

   //---------------------------------------------------------------------------
   // Files 3

   redoDiff |= maybeSetCommand( 
      XxResources::COMMAND_DIFF_FILES_3,
      _lineeditCommandFiles3->text()
   );

   //---------------------------------------------------------------------------
   // Directories

   redoDiff |= maybeSetCommand( 
      XxResources::COMMAND_DIFF_DIRECTORIES,
      _lineeditCommandDirs->text()
   );
   redoDiff |= maybeSetCommand( 
      XxResources::COMMAND_DIFF_DIRECTORIES_REC,
      _lineeditCommandDirsRecursive->text()
   );

   resources->setBoolOpt( XxResources::DIRDIFF_RECURSIVE, 
                          _checkboxRecursive->isChecked() );
                          
   //---------------------------------------------------------------------------
   // Display

   bool reinitHorizontalDiffs = false;
   if ( resources->getBoolOpt( XxResources::HORIZONTAL_DIFFS ) !=
        _checkboxHorizontalDiffs->isChecked() ) {
      resources->setBoolOpt( XxResources::HORIZONTAL_DIFFS, 
                             _checkboxHorizontalDiffs->isChecked() );
      reinitHorizontalDiffs = true;
   }

   if ( resources->getBoolOpt( XxResources::IGNORE_HORIZONTAL_WS ) !=
        _checkboxIgnoreHorizontalWhitespace->isChecked() ) {
      resources->setBoolOpt( XxResources::IGNORE_HORIZONTAL_WS, 
                             _checkboxIgnoreHorizontalWhitespace->isChecked() );
      reinitHorizontalDiffs = true;
   }

   resources->setBoolOpt( XxResources::HIDE_CR, 
                          _checkboxHideCarriageReturns->isChecked() );

   resources->setTabWidth( _spinboxTabWidth->value() );

   resources->setBoolOpt( XxResources::SHOW_VERTICAL_LINE, 
                          _checkboxDrawVerticalLine->isChecked() );

   resources->setVerticalLinePos( _spinboxVlinePos->value() );

   resources->setIgnoreFile(
      XxResources::IgnoreFile(_comboIgnoreFile->currentItem())
   );

   resources->setBoolOpt( XxResources::DIRDIFF_IGNORE_FILE_CHANGES, 
                          _checkboxIgnoreFileChanges->isChecked() );
   if ( _checkboxIgnoreFileChanges->isChecked() == true ) {
      redoDiff = true;
   }

   resources->setBoolOpt( XxResources::FORMAT_CLIPBOARD_TEXT, 
                          _checkboxFormatClipboardText->isChecked() );

   QString oldClipboardFormat = resources->getClipboardTextFormat();
   QString newClipboardFormat = _lineeditClipboardTextFormat->text();
   if ( newClipboardFormat != oldClipboardFormat ) {
      resources->setClipboardTextFormat( newClipboardFormat );
   }

   //---------------------------------------------------------------------------
   // Colors

   
   for ( ColorModMap::const_iterator iter = _colorModMap.begin();
         iter != _colorModMap.end();
         ++iter ) {
      resources->setColor( XxResources::Resource( (*iter).first ),
                           (*iter).second );
   }
   _colorModMap.clear();


   //---------------------------------------------------------------------------

   _app->synchronizeUI();
   
   if ( reinitHorizontalDiffs == true ) {
      XxDiffs* diffs = _app->getDiffs();
      diffs->initializeHorizontalDiffs( _app->getFiles(), true );
   }

   if ( redoDiff == true ) {
      _app->onRedoDiff();
   }
   else {
      _app->repaintTexts();
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::onClose()
{
   hide();
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreTrailingBlanks( int state )
{
   setFileDiffOptions( XxResources::CMDOPT_FILES_IGNORE_TRAILING, 
                       state != 0 );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreWhitespace( int state )
{
   setFileDiffOptions( XxResources::CMDOPT_FILES_IGNORE_WHITESPACE, 
                       state != 0 );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreCase( int state )
{
   setFileDiffOptions( XxResources::CMDOPT_FILES_IGNORE_CASE, 
                       state != 0 );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreBlankLines( int state )
{
   setFileDiffOptions( XxResources::CMDOPT_FILES_IGNORE_BLANK_LINES, 
                       state != 0 );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::setFileDiffOptions(
   XxResources::Resource cmdOptionId,
   bool                  state
) const
{
   XxResources* resources = _app->getResourcesNC();
   XX_CHECK( resources );

   QLineEdit* le = 0;
   if ( _app->getNbFiles() == 2 ) {
      le = _lineeditCommandFiles2;
   }
   else {
      le = _lineeditCommandFiles3;
   }
   QString cmd = le->text();
   
   const QString opt = resources->getCommandOption( cmdOptionId );

   if ( state == true ) {
      XxOptionsDialog::addToCommand( cmd, opt );
   }
   else {
      XxOptionsDialog::removeFromCommand( cmd, opt );
   }

   le->setText( cmd );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::radiobuttonQualityNormal( int state )
{
   if ( state != 0 ) {
      setFileDiffQuality( XxResources::QUALITY_NORMAL );
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::radiobuttonQualityFastest( int state )
{
   if ( state != 0 ) {
      setFileDiffQuality( XxResources::QUALITY_FASTEST );
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::radiobuttonQualityHighest( int state )
{
   if ( state != 0 ) {
      setFileDiffQuality( XxResources::QUALITY_HIGHEST );
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::setFileDiffQuality(
   XxResources::Quality quality
) const
{
   XxResources* resources = _app->getResourcesNC();
   XX_CHECK( resources );

   QLineEdit* le = 0;
   if ( _app->getNbFiles() == 2 ) {
      le = _lineeditCommandFiles2;
   }
   else {
      le = _lineeditCommandFiles3;
   }
   QString cmd = le->text();
   
   resources->setQuality( cmd, quality );

   le->setText( cmd );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxRecursive( int /*state*/ )
{
   // nop.
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxHorizontalDiffs( int /*state*/ )
{
   // nop.
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreHorizontalWhitespace( int /*state*/ )
{
   // nop.
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxHideCarriageReturns( int /*state*/ )
{
   // nop.
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::spinboxTabWidth( int /*state*/ )
{
   // nop.
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreFileChanges( int /*state*/ )
{
   // nop.
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxFormatClipboardText( int /*state*/ )
{
   // nop.
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxDrawVerticalLine( int /*state*/ )
{
   // nop.
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::comboIgnoreFile( const QString& )
{
   // nop.
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::listboxColors( const QString& colorString )
{
   XxResources* resources = _app->getResourcesNC();
   XX_CHECK( resources );

   XxResources::Resource colorId = 
      XxResources::getResourceId( colorString );
   
   QColor color = resources->getColor( colorId );
   _labelColor->setBackgroundColor( color );

   const char* doc = XxResources::getResourceDoc( colorId );
   _labelDescription->setText( QString( doc ) );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::editColor()
{
   XxResources* resources = _app->getResourcesNC();
   XX_CHECK( resources );

   QString colorString = _listboxColors->currentText();
   XxResources::Resource colorId = 
      XxResources::getResourceId( colorString );

   QColor curColor;
   ColorModMap::const_iterator iter = _colorModMap.find( colorId );
   if ( iter != _colorModMap.end() ) {
      curColor = (*iter).second;
   }
   else {
      curColor = resources->getColor( colorId );
   }

   QColor newColor = QColorDialog::getColor( curColor, this, "color_dialog" );
   
   if ( newColor != curColor ) {
      _colorModMap[ colorId ] = newColor;

      _labelColor->setBackgroundColor( newColor );
   }
}

//------------------------------------------------------------------------------
//
bool XxOptionsDialog::isInCommand(
   const QString& command,
   const QString& option
)
{
   return command.find( option ) != -1;
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::addToCommand(
   QString&       command,
   const QString& option
)
{
   // Look for option in command, if not found, add it to command.
   int pos = command.find( option );
   if ( pos == -1 ) {
      command.append( " " );
      command.append( option );
   }
   command = command.simplifyWhiteSpace();
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::removeFromCommand( 
   QString&       command,
   const QString& option
)
{
   // Look for option in command, if found, remove it from command.
   int pos = command.find( option );
   if ( pos != -1 ) {
      command.remove( pos, option.length() );
   }
   command = command.simplifyWhiteSpace();
}

//------------------------------------------------------------------------------
//
int XxOptionsDialog::isInCommand(
   const QString& command,
   const QString& option1,
   const QString& option2,
   const QString& option3
)
{
   int l1 = option1.length();
   int l2 = option2.length();
   int l3 = option3.length();

   bool f1 = l1 > 0 && XxOptionsDialog::isInCommand( command, option1 );
   bool f2 = l2 > 0 && XxOptionsDialog::isInCommand( command, option2 );
   bool f3 = l3 > 0 && XxOptionsDialog::isInCommand( command, option3 );

   if ( f1 || f2 || f3 ) {
      if ( f1 ) {
         return 1;
      }
      if ( f2 ) {
         return 2;
      }
      if ( f3 ) {
         return 3;
      }
   }
   else {
      if ( l1 == 0 ) {
         return 1;
      }
      if ( l2 == 0 ) {
         return 2;
      }
      if ( l3 == 0 ) {
         return 3;
      }
   }

   return 0;
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::setOneOfInCommand(
   QString&       command,
   const QString& optionNew,
   const QString& optionOther1,
   const QString& optionOther2
)
{
   int lNew = optionNew.length();
   int lOther1 = optionOther1.length();
   int lOther2 = optionOther2.length();

   if ( lOther1 > 0 ) {
      XxOptionsDialog::removeFromCommand( command, optionOther1 );
   }
   if ( lOther2 > 0 ) {
      XxOptionsDialog::removeFromCommand( command, optionOther2 );
   }

   if ( lNew > 0 ) {
      XxOptionsDialog::addToCommand( command, optionNew );
   }
}

//------------------------------------------------------------------------------
//
bool XxOptionsDialog::maybeSetCommand( 
   XxResources::Resource commandId,
   const QString&        commandString
)
{
   XxResources* resources = _app->getResourcesNC();
   XX_CHECK( resources );

   if ( resources->getCommand( commandId ) == commandString ) {
      resources->setCommand( commandId, commandString );
      return true;
   }
   return false;
}

XX_NAMESPACE_END


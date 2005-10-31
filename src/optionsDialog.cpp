/******************************************************************************\
 * $Id: optionsDialog.cpp 2 2000-09-15 02:19:22Z blais $
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

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

//------------------------------------------------------------------------------
//
void tidyUpSpace( std::string& command )
{
   // Remove the extra spaces, leaving exactly one space between each option.
   using namespace std;

   string::size_type bpos = string::npos;
   std::string outcommand;
   for ( string::iterator iter = command.begin();
         iter != command.end();
         ++iter ) {

      if ( *iter == ' ' || *iter == '\t' ) {
         if ( bpos == string::npos ) {
            bpos = iter - command.begin();
         }
      }
      else {
         if ( bpos != string::npos ) {
            bpos = string::npos;
            outcommand.append( " ", 1 );
         }
         outcommand.append( &( *iter ), 1 );
      }
   }

   command = outcommand;
}

}

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
      int cpage;
      if ( diffs->isDirectoryDiff() == true ) {
         cpage = 2;
      }
      else if ( _app->getNbFiles() == 2 ) {
         cpage = 0;
      }
      else {
         cpage = 1;
      }

      // Disable other modes' options.
      for ( int ii = 0; ii < 3; ++ii ) {
         _tabWidget->setCurrentPage( ii );
         QWidget* widget = _tabWidget->currentPage();
         if ( ii != cpage && widget ) {
            widget->setEnabled( false );
         }
      }
      _tabWidget->setCurrentPage( cpage );
   }

   // Make connections.
   connect( _buttonApply, SIGNAL( clicked() ), this, SLOT( onApply() ) );
   connect( _buttonOk, SIGNAL( clicked() ), this, SLOT( onApply() ) );

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

   connect( _checkboxRecursive, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxRecursive(int) ) );
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
      QString( resources->getCommand( XxResources::COMMAND_DIFF_FILES_2 ) )
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

   const std::string& rcmd = resources->getCommand( cmdResId );
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
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::onApply()
{
   XxResources* resources = _app->getResourcesNC();
   XX_CHECK( resources );

   //---------------------------------------------------------------------------
   // Files 2

   // First set resources, then redodiff.
   resources->setCommand( 
      XxResources::COMMAND_DIFF_FILES_2,
      _lineeditCommandFiles2->text().ascii() 
   );

   XxResources::Resource cmdResId = _app->getNbFiles() == 2 ? 
      XxResources::COMMAND_DIFF_FILES_2 : 
      XxResources::COMMAND_DIFF_FILES_3;

   resources->setCommandOption( cmdResId, 
                                XxResources::CMDOPT_FILES_IGNORE_TRAILING,
                                _checkboxIgnoreTrailingBlanks->isChecked() );
   
   resources->setCommandOption( cmdResId, 
                                XxResources::CMDOPT_FILES_IGNORE_WHITESPACE,
                                _checkboxIgnoreWhitespace->isChecked() );
   resources->setCommandOption( cmdResId, 
                                XxResources::CMDOPT_FILES_IGNORE_CASE,
                                _checkboxIgnoreCase->isChecked() );
   resources->setCommandOption( cmdResId, 
                                XxResources::CMDOPT_FILES_IGNORE_BLANK_LINES,
                                _checkboxIgnoreBlankLines->isChecked() );

   std::string cmd = resources->getCommand( cmdResId );
   if ( _radiobuttonQualityNormal->isChecked() == true ) {
      resources->setQuality( cmd, XxResources::QUALITY_NORMAL );
   }
   else if ( _radiobuttonQualityFastest->isChecked() == true ) {
      resources->setQuality( cmd, XxResources::QUALITY_FASTEST );
   }
   else if ( _radiobuttonQualityHighest->isChecked() == true ) {
      resources->setQuality( cmd, XxResources::QUALITY_HIGHEST );
   }
   resources->setCommand( cmdResId, cmd.c_str() );


   //---------------------------------------------------------------------------
   // Files 3

   resources->setCommand( 
      XxResources::COMMAND_DIFF_FILES_3,
      _lineeditCommandFiles3->text().ascii() 
   );

   //---------------------------------------------------------------------------
   // Directories

   resources->setCommand( 
      XxResources::COMMAND_DIFF_DIRECTORIES,
      _lineeditCommandDirs->text().ascii() 
   );
   resources->setCommand( 
      XxResources::COMMAND_DIFF_DIRECTORIES_REC,
      _lineeditCommandDirsRecursive->text().ascii() 
   );

   resources->setBoolOpt( XxResources::DIRDIFF_RECURSIVE, 
                          _checkboxRecursive->isChecked() );
                          
   _app->synchronizeUI();
   _app->onRedoDiff();
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
   std::string cmd( le->text().ascii() );
   
   const char* opt = resources->getCommandOption( cmdOptionId );

   if ( state == true ) {
      XxOptionsDialog::addToCommand( cmd, opt );
   }
   else {
      XxOptionsDialog::removeFromCommand( cmd, opt );
   }

   le->setText( QString( cmd.c_str() ) );
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
   std::string cmd( le->text().ascii() );
   
   resources->setQuality( cmd, quality );

   le->setText( QString( cmd.c_str() ) );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxRecursive( int state )
{
   // nop.
}

//------------------------------------------------------------------------------
//
bool XxOptionsDialog::isInCommand(
   const std::string& command,
   const char*        option
)
{
   using namespace std;

   string::size_type xpos = command.find( option );
   if ( xpos != string::npos ) {
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::addToCommand(
   std::string& command,
   const char*  option
)
{
   using namespace std;

   // Look for option in command, if not found, add it to command.
   string::size_type pos = command.find( option );
   if ( pos == string::npos ) {
      command.append( " " );
      command.append( option );
   }

   tidyUpSpace( command );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::removeFromCommand( 
   std::string& command,
   const char*  option
)
{
   using namespace std;

   // Look for option in command, if found, remove it from command.
   string::size_type pos = command.find( option );
   if ( pos != string::npos ) {
      command.erase( pos, ::strlen( option ) );
   }

   tidyUpSpace( command );
}

//------------------------------------------------------------------------------
//
int XxOptionsDialog::isInCommand(
   const std::string& command,
   const char*        option1,
   const char*        option2,
   const char*        option3
)
{
   int l1 = ::strlen( option1 );
   int l2 = ::strlen( option2 );
   int l3 = ::strlen( option3 );

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
   std::string& command,
   const char*  optionNew,
   const char*  optionOther1,
   const char*  optionOther2
)
{
   int lNew = ::strlen( optionNew );
   int lOther1 = ::strlen( optionOther1 );
   int lOther2 = ::strlen( optionOther2 );

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

XX_NAMESPACE_END

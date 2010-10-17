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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <app.h>
#include <cmdline.h>
#include <exceptions.h>
#include <overview.h>
#include <text.h>
#include <merged.h>
#include <lineNumbers.h>
#include <util.h>
#include <help.h>
#include <builderFiles2.h>
#include <builderFiles3.h>
#include <builderDirs2.h>
#include <builderUnmerge.h>
#include <builderSingle.h>
#include <diffs.h>
#include <buffer.h>
#include <resources.h>
#include <suicideMessageBox.h>
#include <optionsDialog.h>
#include <searchDialog.h>
#include <markers.h>
#include <resParser.h>
#include <central.h>

#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QLayout>
#include <QtGui/QScrollBar>
#include <QtGui/QLabel>
#if (QT_VERSION >= 0x030000)
#include <QtGui/QStyleFactory>
#endif
#include <QtGui/QFont>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QShortcut>
#include <QtGui/QLabel>
#include <QtGui/QWhatsThis>
#include <QtGui/QClipboard>
#include <QtCore/QSocketNotifier>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtGui/QSplitter>
#include <QtCore/QRegexp>
#include <QtGui/QCheckBox>
#include <QtCore/QDateTime>
#include <QtGui/QPixmap>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>


#ifdef XX_KDE
#include <kmessagebox.h>
#endif

#include <exception>
#include <fstream>
#include <iostream>
#include <memory>

#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>

#ifndef WINDOWS
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <unistd.h>
#else
#  include <io.h>
#endif


// Pixmaps
#include "pixmaps/next_difference.xpm"
#include "pixmaps/next_unselected_difference.xpm"
#include "pixmaps/previous_difference.xpm"
#include "pixmaps/previous_unselected_difference.xpm"
#include "pixmaps/search.xpm"
#include "pixmaps/search_backward.xpm"
#include "pixmaps/search_forward.xpm"
#include "pixmaps/select_region_left.xpm"
#include "pixmaps/select_region_middle.xpm"
#include "pixmaps/select_region_neither.xpm"
#include "pixmaps/select_region_right.xpm"
#include "pixmaps/select_region_unselect.xpm"
#include "pixmaps/split_swap_join.xpm"
#include "pixmaps/diff_files.xpm"
#include "pixmaps/diff_files_next.xpm"
#include "pixmaps/save_as_left.xpm"
#include "pixmaps/save_as_middle.xpm"
#include "pixmaps/save_as_right.xpm"
#include "pixmaps/save_as_merged.xpm"
#include "pixmaps/save_as.xpm"
#include "pixmaps/return_accept.xpm"
#include "pixmaps/return_reject.xpm"
#include "pixmaps/return_merged.xpm"

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

enum MenuIds {
   ID_TogglePaneMergedView,
   ID_TogglePopupMergedView,
   ID_ToggleToolbar,
   ID_ToggleLineNumbers,
   ID_ToggleVerticalLine,
   ID_ToggleHorizNullMarkers,
   ID_ToggleOverview,
   ID_ToggleShowFilenames,
   ID_Hordiff_None,
   ID_Hordiff_Single,
   ID_Hordiff_Multiple,
   ID_ToggleIgnoreHorizontalWs,
   ID_ToggleIgnorePerHunkWs,
   ID_ToggleFormatClipboardText,
   ID_ToggleIgnoreTrailing,
   ID_ToggleIgnoreWhitespace,
   ID_ToggleIgnoreCase,
   ID_ToggleIgnoreBlankLines,
   ID_ToggleHideCarriageReturns,
   ID_ToggleDirDiffsRecursive,
   ID_ToggleDirDiffsIgnoreFileChanges,
   ID_ToggleQualityNormal,
   ID_ToggleQualityFastest,
   ID_ToggleQualityHighest,
   ID_TabsAtThree,
   ID_TabsAtFour,
   ID_TabsAtEight,
   ID_IgnoreFileNone,
   ID_IgnoreFileLeft,
   ID_IgnoreFileMiddle,
   ID_IgnoreFileRight,
   ID_View_DiffFilesAtCursor,
   ID_View_NextAndDiffFiles,
   ID_View_CopyRightToLeft,
   ID_View_CopyLeftToRight,
   ID_View_RemoveLeft,
   ID_View_RemoveRight
};


/*==============================================================================
 * CLASS XxMainWindow
 *============================================================================*/

// This object used to handle the resize event for the application, but there
// was a bug with computing the number of display lines (done by the text
// widget), so the first text widget now handles the resize event for the app.
// We keep this object around because we might eventually override some of the
// main window methods.

class XxMainWindow : public QkMainWindow {

public:

   /*----- member functions -----*/

   // Constructor.
   XxMainWindow(
      XxApp*      app,
      QWidget*    parent = 0,
      Qt::WFlags      f = Qt::Window
   );


private:

   /*----- data members -----*/

   XxApp* _app;

};
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
XxMainWindow::XxMainWindow(
   XxApp*      app,
   QWidget*    parent,
   Qt::WFlags      f
) :
   QkMainWindow( parent, f ),
   _app( app )
{}

}

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxApp
 *============================================================================*/

/*----- static data members -----*/

int XxApp::_sockfd = -1;
QSocketNotifier* XxApp::_socketNotifier = 0;

//------------------------------------------------------------------------------
//
XxApp::XxApp( int& argc, char** argv, XxCmdline& cmdline ) :
#ifdef XX_KDE
   KApplication( argc, argv, "xxdiff" ),
#else
   QApplication( argc, argv ),
#endif
   _isUICreated( false ),
   _dontShow( false ),
   _returnValue( 0 ),
   _mainWindow( 0 ),
   _paneMergedView( 0 ),
   _popupMergedView( 0 ),
   _searchDialog( 0 ),
   _optionsDialog( 0 ),
   _textWidth( 0 ),
   _cursorLine( 1 ), // disallowed value on purpose
   _filesAreDirectories( false ),
   _resources( 0 ),
   _cmdline( cmdline ),
   _newlineChar( '\n' )
{
   if ( _cmdline._macNewlines ) {
      _newlineChar = '\015';
   }

   // Read in the resources and create resources object.
   _resources = buildResources();

#ifndef XX_KDE
#if (QT_VERSION >= 0x030000)
   // By default, if not specified, force SGI style.
   if ( _cmdline._forceStyle == false ) {
      _style = QStyleFactory::create( _resources->getStyleKey() );
      setStyle( _style );
   }
#endif
#endif

#ifndef XX_KDE
   if ( _cmdline._forceFont == false ) {
      setFont( _resources->getFontApp() );
   }
#endif

   if ( _cmdline._promptForFiles ) {
      promptForFiles( _cmdline );
   }

   // Read in the file names.
   QString filenames[3];
   QString displayFilenames[3];
   QFileInfo fileInfos[3];
   bool isTemporary[3] = { false, false, false };
   _nbFiles = processFileNames(
      _cmdline,
      filenames,
      displayFilenames,
      fileInfos,
      isTemporary,
      _filesAreDirectories
   );

   // Note: this is already pretty much validated in XxCmdline.
   // Old code here stays anyway.
   if ( _nbFiles == 0 ) {
      throw XxUsageError( XX_EXC_PARAMS, "No files specified." );
   }
   // Check for odd number of files.
   if ( ( ( _cmdline._unmerge == false && _cmdline._single == false )
          && ( _nbFiles < 1 || _nbFiles > 3 ) ) ||
        ( ( _cmdline._unmerge == true || _cmdline._single == true )
          && _nbFiles < 1 ) ) {
      throw XxUsageError( XX_EXC_PARAMS, "Wrong number of files specified." );
   }

   // Here is where in unmerge mode we make xxdiff think that it has two
   // files. There will be other special cases for this throughout the code too.
   if ( _cmdline._unmerge == true ) {
      _nbFiles = _cmdline._unmergeNbFiles;
   }
   else if ( _cmdline._single == true ) {
      _nbFiles = 2;
   }

   // Add the extra diff arguments to all the command strings, we don't want to
   // bother with finding out which command, because it may change at runtime
   // (perhaps not yet supported but might in the future).
   XX_ASSERT( _resources != 0 );
   for ( int ii = 0; ii < 4; ++ii ) {
      QString cmd = _resources->getCommand( XxCommand(ii) );
      XxOptionsDialog::addToCommand( cmd, _cmdline._extraDiffArgs );
      _resources->setCommand( XxCommand(ii), cmd );
   }

   bool succ = false;

   if ( _nbFiles != 0 ) {

      try {
         // Note: perhaps this should be replaced by a simple call to
         // onRedoDiff().
         if ( _cmdline._unmerge == false && _cmdline._single == false ) {
            for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
               std::auto_ptr<XxBuffer> newbuf(
                  readFile( ii,
                            filenames[ii],
                            displayFilenames[ii],
                            fileInfos[ii],
                            isTemporary[ii] )
               );
               _files[ii] = newbuf;
            }
         }
         else if ( _cmdline._single == true ) {

            // Read the one single file.
            std::auto_ptr<XxBuffer> newbuf(
               readFile( 0,
                         filenames[0],
                         displayFilenames[0],
                         fileInfos[0],
                         isTemporary[0] )
            );
            _files[0] = newbuf;

            // Make an empty buffer.
            std::auto_ptr<XxBuffer> emptybuf(
               new XxBuffer( false, QString("/dev/null"), QString("(empty)") )
            );
            _files[1] = emptybuf;
         }
         else {
            std::auto_ptr<XxBuffer> newbuf(
               readFile( 0,
                         filenames[0],
                         displayFilenames[0],
                         fileInfos[0],
                         isTemporary[0] )
            );
            _files[0] = newbuf;

            // Make a proxy for the other buffer.
            std::auto_ptr<XxBuffer> newbuf2(
               new XxBuffer(
                  (*_files[0]), filenames[0], displayFilenames[0], fileInfos[0]
               )
            );
            _files[1] = newbuf2;

            if ( _cmdline._unmergeNbFiles == 3 ) {
               // Make a proxy for the other buffer.
               std::auto_ptr<XxBuffer> newbuf2(
                  new XxBuffer(
                     (*_files[0]), filenames[0],
                     displayFilenames[0], fileInfos[0]
                  )
               );
               _files[2] = newbuf2;
            }
         }
      }
      catch ( const std::exception& ex ) {
         throw XxIoError( XX_EXC_PARAMS, "Error opening input file." );
      }

      succ = processDiff();

      // Here we just finished reading all the files and processing the diffs,
      // we're ready to run without the input files at all. Spit out a note
      // about that if requested.
      if ( _cmdline._indicateInputProcessed == true ) {
         std::cout << "INPUT-PROCESSED" << std::endl << std::flush;
      }

      // Initialize the horizontal diffs if necessary.
      if ( succ == true ) {

         if ( !_filesAreDirectories &&
              _resources->getBoolOpt( BOOL_IGNORE_PERHUNK_WS ) ) {
            _diffs->computeIgnoreDisplay( _nbFiles, getBuffers() );
         }
         _diffs->initializeHorizontalDiffs( *_resources, getBuffers() );
      }
   }

   // Create the interface.
   createUI();

   if ( succ == true ) {
      // Set filename label.
      for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
         if ( _files[ii].get() != 0 ) {
            _central->setFilename( ii, _files[ii]->getDisplayName() );
         }
      }
   }

   // Add some context help.
   createOnContextHelp();

   // Synchronize the created UI with the resources.
   synchronizeUI();

   // Resize and show/hide line numbers.
   adjustLineNumbers();

   // Sets the title bar.
   if ( _nbFiles == 2 ) {
      QString str =
         QString(_files[0]->getDisplayName()) + " <-> " +
         QString(_files[1]->getDisplayName());
      _mainWindow->setWindowTitle( str );
   }
   else if ( _nbFiles == 3 ) {
      QString str =
         QString(_files[0]->getDisplayName()) + " <-> " +
         QString(_files[1]->getDisplayName()) + " <-> " +
         QString(_files[2]->getDisplayName());
      _mainWindow->setWindowTitle( str );
   }
   else {
      _mainWindow->setWindowTitle( "xxdiff" );
   }

   // Call post creation actions before showing.
   if ( _resources->getBoolOpt( BOOL_SELECT_MERGE ) ) {
      selectGlobalMerge();
   }

   bool allSelected = true;
   if ( _diffs.get() != 0 ) {
      allSelected = _diffs->isAllSelected();
   }

   // Determine if we should not show up.
   _dontShow =
      ( _resources->getBoolOpt( BOOL_EXIT_ON_SAME ) && _returnValue == 0 )
      ||
      ( _resources->getBoolOpt( BOOL_EXIT_IF_NO_CONFLICTS ) &&
        allSelected == true );

   if ( !_dontShow ) {

      // Resize the main window before showing it.
      const QRect& psize = _resources->getPreferredGeometry();
      if ( _cmdline._forceGeometry == true ) {
         _mainWindow->show();
      }
      else if ( _resources->getMaximize() ) {
         _mainWindow->showMaximized();
         // Don't make fullscreen, rather maximized.
      }
      else {
         _mainWindow->resize( psize.size() );
         _mainWindow->show();

         // Note: positioning has to be done after show().  This results in some
         // flickering, but there's nothing we can do about it, this is a Qt
         // limitation.
         if ( psize.topLeft() != QPoint( -1, -1 ) ) {
            _mainWindow->move( psize.topLeft() );
         }
      }
   }

   if ( !_dontShow ) {
      // Update the widgets the first time.
      updateWidgets();
#ifdef Q_OS_MAC
      // Or the window would appear under other application windows
      _mainWindow->raise();
#endif
   }
}

//------------------------------------------------------------------------------
//
XxApp::~XxApp()
{
   if ( _socketNotifier != 0 ) {
      ::close( _sockfd );
   }
   delete _resources;
}

//------------------------------------------------------------------------------
// Side-effect: This modifies the cmdline.
void XxApp::promptForFiles( XxCmdline& cmdline )
{
	cmdline._filenames[0] = QFileDialog::getOpenFileName(
      0, "Choose Left File" );

	if ( ! cmdline._filenames[0].isNull() ) {

		cmdline._filenames[1] = QFileDialog::getOpenFileName(
          0, "Choose Right File" );

		if ( ! cmdline._filenames[1].isNull() ) {
			cmdline._nbFilenames = 2;
		}
	}
}

//------------------------------------------------------------------------------
//
uint XxApp::processFileNames(
   const XxCmdline& cmdline,
   QString          filenames[3],
   QString          displayFilenames[3],
   QFileInfo        fileInfos[3],
   bool             isTemporary[3],
   bool&            filesAreDirectories
)
{
   uint nbFiles = 0;
   QString bn;
   bool isDirectory[3] = { false, false, false };

   if ( cmdline._unmerge == false && cmdline._single == false ) {
      //
      // First find if all paths are directories.  If this is the case, then
      // this is a directory diff.  If ONLY ONE path is a file, then we append
      // the basename to the other ones which are directories.
      //
      // If stdin is encountered, then directory diffs are not allowed (you
      // can't cat a directory into stdin!) so if all filenames are directories
      // bail out.
      //
      // We aspire to do our best but only in cases where there is no ambiguity.
      //

      bool stdinUsed = false;
      uint nbPathFiles = 0;
      int filePathIndex = -1;
      for ( XxFno ii = 0; ii < cmdline._nbFilenames; ++ii ) {
         if ( cmdline._filenames[ii] == "-" ) {
            if ( stdinUsed == true ) {
               throw XxUsageError(
                  XX_EXC_PARAMS, "Cannot accept more than one file from stdin."
               );
            }
            stdinUsed = true;
            isDirectory[ii] = false;
         }
         else {
            fileInfos[ii] = QFileInfo( cmdline._filenames[ii] );
            isDirectory[ii] = fileInfos[ii].isDir();
            if ( ! isDirectory[ii] ) {
               filePathIndex = ii;
               nbPathFiles++;
            }
         }
         nbFiles++;
      }

      if ( stdinUsed == true && nbPathFiles == 0 ) {
         throw XxUsageError(
            XX_EXC_PARAMS, "Cannot use stdin as only file with all directories."
         );
      }

      if ( nbPathFiles > 1 &&
           nbFiles > ( nbPathFiles + (stdinUsed ? 1 : 0) ) ) {
         throw XxUsageError( XX_EXC_PARAMS,
                             "There are more than one named file AND "
                             "one of the files is a directory." );
         // Note: we could check if both files basenames are equal, in that
         // case, it would be valid.
      }

      filesAreDirectories = ( nbFiles > 0 && nbPathFiles == 0 );

      // Find basename and append to directories.
      if ( !filesAreDirectories && nbPathFiles < nbFiles ) {
         XX_ASSERT( filePathIndex != -1 );
         bn = fileInfos[filePathIndex].fileName();
      }

   }
   else { // Parsing filenames for unmerge.

      if ( cmdline._nbFilenames > 1 ) {
         throw XxUsageError(
            XX_EXC_PARAMS,
            "Cannot accept more than one file in unmerge or single mode."
         );
      }

      filesAreDirectories = false;

      if ( cmdline._nbFilenames == 1 ) {
         fileInfos[0] = QFileInfo( cmdline._filenames[0] );
         if ( fileInfos[0].isDir() ) {
            throw XxUsageError(
               XX_EXC_PARAMS,
               "Input in unmerge or single mode must be a file."
            );
         }
         nbFiles++;
      }
   }

   int newest = -1;
   unsigned int newTime = 0;
   bool stdinSeen = false;
   for ( XxFno iii = 0; iii < cmdline._nbFilenames; ++iii ) {
      if ( cmdline._filenames[iii] == "-" ) {
         XX_ASSERT( stdinSeen == false );
         stdinSeen = true;
         fileInfos[iii] = QFileInfo(); // also to mark that it's stdin.
         filenames[iii] = cmdline._filenames[iii];
         displayFilenames[iii] = cmdline._stdinFilename;
         isTemporary[iii] = true;
         newest = true;
         newTime = XxUtil::toTime_t( QDateTime::currentDateTime() );
      }
      else {
         QString fname = cmdline._filenames[iii];
         if ( !filesAreDirectories && isDirectory[iii] == true ) {
            // Append basename to directory.
            fname.append( "/" );
            XX_ASSERT( !bn.isEmpty() );
            fname.append( bn );
            fileInfos[iii] = QFileInfo(fname);
         }
         displayFilenames[iii] = fname;
         filenames[iii] = fname;
         bool isdir;
         XxUtil::testFile( filenames[iii], fileInfos[iii], false, isdir );
         if ( XxUtil::toTime_t( fileInfos[iii].lastModified() ) > newTime ) {
             newTime = XxUtil::toTime_t( fileInfos[iii].lastModified() );
             newest = iii;
         }
      }
   }

   for ( XxFno iii = 0; iii < cmdline._nbFilenames; ++iii ) {
      if ( ! cmdline._userFilenames[iii].isEmpty() ) {
         if ( 0 > cmdline._userFilenames[iii].indexOf('%') ) {
            displayFilenames[iii] = cmdline._userFilenames[iii];
         }
         else {
            if ( ! XxUtil::formatFilename( displayFilenames[iii],
                                           cmdline._userFilenames[iii],
                                           fileInfos[iii],
                                           isTemporary[iii],
                                           filenames[iii],
                                           newest == iii ) ) {
               throw XxUsageError( XX_EXC_PARAMS,
                                   "Malformed %-format sequence in titles" );
            }
         }
      }
   }

   return nbFiles;
}

//------------------------------------------------------------------------------
//
void XxApp::createUI()
{
   XX_ASSERT( _resources != 0 );

   //
   // Create widgets.
   //

   _mainWindow = new XxMainWindow( this, 0 );

   // Central widget.
   //
   QWidget* topCentralWidget = new QWidget( _mainWindow );
   QHBoxLayout* topLayout =
      new QHBoxLayout( topCentralWidget );
   topLayout->setMargin( 0 );
   topLayout->setSpacing( 0 );

   // Pane merged widget.
   //
   _splitter = new QSplitter;
   _splitter->setOrientation( Qt::Vertical );

   topLayout->addWidget( _splitter );

   _paneMergedView = new XxMergedFrame( this, _splitter, "paneMergedView" );
   if ( !_resources->getShowOpt( SHOW_PANE_MERGED_VIEW ) ) {
      _paneMergedView->hide();
   }

   // Texts widget (texts).
   //
   _central = new XxCentralFrame( this, _splitter, "centralFrame" );
   _central->showFilenames( _resources->getShowOpt( SHOW_FILENAMES ) );

   // Hide filename widgets on startup if requested.
   if ( _resources->getShowOpt( SHOW_FILENAMES ) == false ) {
      _central->showFilenames( _resources->getShowOpt( SHOW_FILENAMES ) );
   }

   // Adjust splitter (after we've create two children for it to contain).
   uint smpp = _resources->getShowPaneMergedViewPercent();
   _splitter->setStretchFactor( 0, smpp );
   _splitter->setStretchFactor( 1, 100-smpp );


   // Overview and remaining unselected regions indicator area.
   //
   _overviewArea = new QWidget;
   QVBoxLayout* overviewLayout =
      new QVBoxLayout( _overviewArea );
   overviewLayout->setMargin( 0 );
   overviewLayout->setSpacing( 0 );

   //QHBoxLayout* hlayout = new QHBoxLayout;
   _remUnselView = new QLabel( "remaining unselected" );
   _remUnselView->setAlignment( Qt::AlignCenter );
   _remUnselView->setMaximumHeight( _remUnselView->sizeHint().height() );
   //hlayout->addWidget( _remUnselView, 100 );

   //QCheckBox* _equalCheckBox = new QCheckBox( _overviewArea );
   //hlayout->addWidget( _equalCheckBox, 1 );

   //overviewLayout->addLayout( hlayout );
   overviewLayout->addWidget( _remUnselView );

   _overview = new XxOverview( this, _central );
   _overviewArea->setFixedWidth( _overview->width() );
   overviewLayout->addWidget( _overview );

   topLayout->addWidget( _overviewArea );

   if ( _cmdline._originalXdiff ) {
      // FIXME We cannot undo this once xxdiff has started. Not that we mind so
      // much.
      _remUnselView->hide();
   }

   if ( _resources->getShowOpt( SHOW_OVERVIEW ) == false ) {
      _overviewArea->hide();
   }

   //
   // Create menus.
   //
   createMenus();

   // Add some extra accelerators.
   //
   // Note: these have effect at the top level only, they are not bound specific
   // to a particular widget.

   connect ( new QShortcut( Qt::Key_Delete, _mainWindow ), SIGNAL(activated()), SLOT(selectRegionNeither()) );

   //
   // Cursor motion.
   //
   connect ( new QShortcut( Qt::Key_Down, _mainWindow ), SIGNAL(activated()), SLOT(cursorDown()) );
   connect ( new QShortcut( Qt::Key_Up,   _mainWindow ), SIGNAL(activated()), SLOT(cursorUp()) );

   connect ( new QShortcut( Qt::Key_Home, _mainWindow ), SIGNAL(activated()), SLOT(cursorTop()) );
   connect ( new QShortcut( Qt::Key_End,  _mainWindow ), SIGNAL(activated()), SLOT(cursorBottom()) );

   connect ( new QShortcut( _resources->getAccelerator( ACCEL_CURSOR_DOWN ),   _mainWindow ), SIGNAL(activated()), SLOT(cursorDown()) );
   connect ( new QShortcut( _resources->getAccelerator( ACCEL_CURSOR_UP ),     _mainWindow ), SIGNAL(activated()), SLOT(cursorUp()) );
   connect ( new QShortcut( _resources->getAccelerator( ACCEL_CURSOR_TOP ),    _mainWindow ), SIGNAL(activated()), SLOT(cursorTop()) );
   connect ( new QShortcut( _resources->getAccelerator( ACCEL_CURSOR_BOTTOM ), _mainWindow ), SIGNAL(activated()), SLOT(cursorBottom()) );

   //
   // Page motion.
   //

   // Don't bind these, SPC is used for the dirdiffs.
   // a->connectItem( a->insertItem(Key_Space), this, SLOT(pageDown()) );
   // a->connectItem( a->insertItem(Key_Backspace), this, SLOT(pageUp()) );

   connect ( new QShortcut( Qt::Key_PageDown, _mainWindow ), SIGNAL(activated()), SLOT(pageDown()) );
   connect ( new QShortcut( Qt::Key_PageUp,   _mainWindow ), SIGNAL(activated()), SLOT(pageUp()) );

   connect ( new QShortcut( _resources->getAccelerator( ACCEL_PAGE_DOWN ), _mainWindow ), SIGNAL(activated()), SLOT(pageDown()) );
   connect ( new QShortcut( _resources->getAccelerator( ACCEL_PAGE_UP ),   _mainWindow ), SIGNAL(activated()), SLOT(pageUp()) );

   //
   // Interactive font resize.
   //
   connect ( new QShortcut( _resources->getAccelerator( ACCEL_FONT_RESIZE_BIGGER ),  _mainWindow ), SIGNAL(activated()), SLOT(fontSizeIncrease()) );
   connect ( new QShortcut( _resources->getAccelerator( ACCEL_FONT_RESIZE_SMALLER ), _mainWindow ), SIGNAL(activated()), SLOT(fontSizeDecrease()) );

   // Make some connections.
   connect( this, SIGNAL(cursorChanged(int)),
            this, SLOT(updateWidgets()) );
   connect( this, SIGNAL(cursorChanged(int)),
            _central, SLOT(updateLineNumberLabels(int)) );

   //
   // Create toolbar
   //
   _toolbar = createToolbar();
   XX_ASSERT( _toolbar != 0 );
   if ( !_resources->getShowOpt( SHOW_TOOLBAR ) ) {
      _toolbar->hide();
   }

   // Create popup merged view upon startup if asked for.
   if ( _resources->getShowOpt( SHOW_POPUP_MERGED_VIEW ) ) {
      togglePopupMergedView();
   }

   // Connect closing window to quit().
#if (QT_VERSION >= 0x030000)
   connect( this, SIGNAL(lastWindowClosed()), this, SLOT(quit()) );
#endif

   //
   // Show it!
   //
   topCentralWidget->updateGeometry();
   _mainWindow->setCentralWidget( topCentralWidget );

   _isUICreated = true;
}

//------------------------------------------------------------------------------
//
QkToolBar* XxApp::createToolbar()
{
#ifdef XX_KDE
   KToolBar* toolbar = _mainWindow->addToolBar( "Tools" );
#else
   QToolBar* toolbar = _mainWindow->addToolBar( "Tools" );
#endif

   QPixmap pmSaveAsMerged(
      const_cast<const char**>( save_as_merged_xpm )
   );
  toolbar->addAction(
     pmSaveAsMerged,
     "Save as merged",
     this, SLOT(saveAsMerged())
  );

   QAction* actSaveAsRight = 0;
   if ( _cmdline._unmerge == false ) {

      QPixmap pmSaveAsLeft(
         const_cast<const char**>( save_as_left_xpm )
      );
      toolbar->addAction(
         pmSaveAsLeft,
         "Save as left",
         this, SLOT(saveAsLeft())
      );

      if ( _nbFiles == 3 ) {
         QPixmap pmSaveAsMiddle(
            const_cast<const char**>( save_as_middle_xpm )
         );
         toolbar->addAction(
            pmSaveAsMiddle,
            "Save as middle",
            this, SLOT(saveAsMiddle())
         );
      }

      QPixmap pmSaveAsRight(
         const_cast<const char**>( save_as_right_xpm )
      );
      actSaveAsRight = toolbar->addAction(
         pmSaveAsRight,
         "Save as right",
         this, SLOT(saveAsRight())
      );

   }
   else {

      QPixmap pmSaveAsMiddle(
         const_cast<const char**>( save_as_middle_xpm )
      );
      toolbar->addAction(
         pmSaveAsMiddle,
         "Save as original",
         this, SLOT(saveAsLeft())
      );

   }

   QPixmap pmSaveAs(
      const_cast<const char**>( save_as_xpm )
   );
   toolbar->addAction(
      pmSaveAs,
      "Save as ...",
      this, SLOT(saveAs())
   );

   if ( _cmdline._single == true ) {
      XX_CHECK( actSaveAsRight );
      actSaveAsRight->setEnabled( false );
   }

   toolbar->addSeparator();

   QPixmap pmPreviousUnselectedDifference(
      const_cast<const char**>( previous_unselected_difference_xpm )
   );
   toolbar->addAction(
      pmPreviousUnselectedDifference,
      "Previous unselected difference",
      this, SLOT(previousUnselected())
   );

   QPixmap pmPreviousDifference(
      const_cast<const char**>( previous_difference_xpm )
   );
   toolbar->addAction(
      pmPreviousDifference,
      "Previous difference",
      this, SLOT(previousDifference())
   );

   QPixmap pmNextDifference(
      const_cast<const char**>( next_difference_xpm )
   );
   toolbar->addAction(
      pmNextDifference,
      "Next difference",
      this, SLOT(nextDifference())
   );

   QPixmap pmNextUnselectedDifference(
      const_cast<const char**>( next_unselected_difference_xpm )
   );
   toolbar->addAction(
      pmNextUnselectedDifference,
      "Next unselected difference",
      this, SLOT(nextUnselected())
   );

   toolbar->addSeparator();

   QPixmap pmSearch(
      const_cast<const char**>( search_xpm )
   );
   toolbar->addAction(
      pmSearch,
      "Search",
      this, SLOT(search())
   );

   QPixmap pmSearchBackward(
      const_cast<const char**>( search_backward_xpm )
   );
   toolbar->addAction(
      pmSearchBackward,
      "Search backward",
      this, SLOT(searchBackward())
   );

   QPixmap pmSearchForward(
      const_cast<const char**>( search_forward_xpm )
   );
   toolbar->addAction(
      pmSearchForward,
      "Search forward",
      this, SLOT(searchForward())
   );

   if ( _filesAreDirectories == false ) {
      toolbar->addSeparator();

      QPixmap pmSelectRegionLeft(
         const_cast<const char**>( select_region_left_xpm )
      );
      toolbar->addAction(
         pmSelectRegionLeft,
         "Select region left",
         this, SLOT(selectRegionLeft())
      );

      if ( _nbFiles == 3 ) {
         QPixmap pmSelectRegionMiddle(
            const_cast<const char**>( select_region_middle_xpm )
         );
         toolbar->addAction(
            pmSelectRegionMiddle,
            "Select region middle",
            this, SLOT(selectRegionMiddle())
         );
      }

      QPixmap pmSelectRegionRight(
         const_cast<const char**>( select_region_right_xpm )
      );
      toolbar->addAction(
         pmSelectRegionRight,
         "Select region right",
         this, SLOT(selectRegionRight())
      );

      QPixmap pmSelectRegionNeither(
         const_cast<const char**>( select_region_neither_xpm )
      );
      toolbar->addAction(
         pmSelectRegionNeither,
         "Select region neither",
         this, SLOT(selectRegionNeither())
      );

      QPixmap pmSelectRegionUnselect(
         const_cast<const char**>( select_region_unselect_xpm )
      );
      toolbar->addAction(
         pmSelectRegionUnselect,
         "Unselect region",
         this, SLOT(selectRegionUnselect())
      );

      toolbar->addSeparator();

      QPixmap pmSplitSwapJoin(
         const_cast<const char**>( split_swap_join_xpm )
      );
      toolbar->addAction(
         pmSplitSwapJoin,
         "Split/swap/join",
         this, SLOT(regionSplitSwapJoin())
      );
   }
   else {
      toolbar->addSeparator();

      QPixmap pmDiffFiles(
         const_cast<const char**>( diff_files_xpm )
      );
      toolbar->addAction(
         pmDiffFiles,
         "Diff files at cursor",
         this, SLOT(diffFilesAtCursor())
      );

      QPixmap pmNextAndDiffFiles(
         const_cast<const char**>( diff_files_next_xpm )
      );
      toolbar->addAction(
         pmNextAndDiffFiles,
         "Move to next difference and diff files at cursor",
         this, SLOT(nextAndDiffFiles())
      );
   }

   toolbar->addSeparator();

   // Review status.
   if ( _cmdline._forceDecision == true ) {

      QPixmap pmQuitReject(
         const_cast<const char**>( return_reject_xpm )
      );
      toolbar->addAction(
         pmQuitReject,
         "Exit with output indicating rejection of the change.",
         this, SLOT(quitReject())
      )->setStatusTip( "Quit Reject" );

      QPixmap pmQuitMerged(
         const_cast<const char**>( return_merged_xpm )
      );
      toolbar->addAction(
         pmQuitMerged,
         "Exit with output indicating changes have been merged manually.",
         this, SLOT(quitMerged())
      )->setStatusTip( "Quit Merged" );

      QPixmap pmQuitAccept(
         const_cast<const char**>( return_accept_xpm )
      );
      toolbar->addAction(
         pmQuitAccept,
         "Exit with output indicating acceptance of the change.",
         this, SLOT(quitAccept())
      )->setStatusTip( "Quit Accept" );
   }

   return toolbar;
}

//------------------------------------------------------------------------------
//
void XxApp::createOnContextHelp()
{
   _overview->setWhatsThis( XxHelp::getWhatsThisText( XxHelp::OVERVIEW ) );
}

//------------------------------------------------------------------------------
//
void XxApp::createMenus()
{
    QkMenuBar* menubar = _mainWindow->menuBar();
    
   //---------------------------------------------------------------------------
    
   // File menu
   QkMenu* fileMenu = menubar->addMenu( "&File" );
   fileMenu->addAction(
      "Replace left file...", this, SLOT(openLeft()),
      _resources->getAccelerator( ACCEL_OPEN_LEFT )
   );
   if ( _nbFiles == 3 ) {
      fileMenu->addAction(
         "Replace middle file...", this, SLOT(openMiddle()),
         _resources->getAccelerator( ACCEL_OPEN_MIDDLE )
      );
   }
   fileMenu->addAction(
      "Replace right file...", this, SLOT(openRight()),
      _resources->getAccelerator( ACCEL_OPEN_RIGHT )
   );
   fileMenu->addSeparator();

   QAction* ids[9];
   if ( _cmdline._unmerge == false ) {
      /*ids[0] = */fileMenu->addAction(
         "Save as left", this, SLOT(saveAsLeft()),
         _resources->getAccelerator( ACCEL_SAVE_AS_LEFT )
      );
      if ( _nbFiles == 3 ) {
         /*ids[1] = */fileMenu->addAction(
            "Save as middle", this, SLOT(saveAsMiddle()),
            _resources->getAccelerator( ACCEL_SAVE_AS_MIDDLE )
         );
      }
      ids[2] = fileMenu->addAction(
         "Save as right", this, SLOT(saveAsRight()),
         _resources->getAccelerator( ACCEL_SAVE_AS_RIGHT )
      );
      if ( _cmdline._single == true ) {
          ids[2]->setEnabled( false );
      }
   }
   else {
      /*ids[0] = */fileMenu->addAction(
         "Save as original", this, SLOT(saveAsLeft()),
         _resources->getAccelerator( ACCEL_SAVE_AS_LEFT )
      );
   }

   fileMenu->addAction(
      "Save as merged", this, SLOT(saveAsMerged()),
      _resources->getAccelerator( ACCEL_SAVE_AS_MERGED )
   );
   /*ids[3] = */fileMenu->addAction(
      "Save as...", this, SLOT(saveAs()),
      _resources->getAccelerator( ACCEL_SAVE_AS )
   );
   /*ids[4] = */fileMenu->addAction(
      "Save selected only...", this, SLOT(saveSelectedOnly()),
      _resources->getAccelerator( ACCEL_SAVE_SELECTED_ONLY )
   );

   fileMenu->addSeparator();

   if ( _cmdline._unmerge == false ) {
      /*ids[6] = */fileMenu->addAction(
         "Generate patch against left", this, SLOT(generatePatchFromLeft()),
         _resources->getAccelerator( ACCEL_PATCH_FROM_LEFT )
      );
      if ( _nbFiles == 3 ) {
         /*ids[7] = */fileMenu->addAction(
            "Generate patch against middle", this,
            SLOT(generatePatchFromMiddle()),
            _resources->getAccelerator( ACCEL_PATCH_FROM_MIDDLE )
         );
      }
      ids[8] = fileMenu->addAction(
         "Generate patch against right", this, SLOT(generatePatchFromRight()),
         _resources->getAccelerator( ACCEL_PATCH_FROM_RIGHT )
      );

   }
   else {
      /*ids[0] = */fileMenu->addAction(
         "Generate patch against original", this, SLOT(generatePatchFromLeft()),
         _resources->getAccelerator( ACCEL_PATCH_FROM_LEFT )
      );
   }

   fileMenu->addSeparator();

   fileMenu->addAction(
      "Redo diff", this, SLOT(redoDiff()),
      _resources->getAccelerator( ACCEL_REDO_DIFF )
   );
   fileMenu->addSeparator();
   fileMenu->addAction(
      "Edit left file", this, SLOT(editLeft()),
      _resources->getAccelerator( ACCEL_EDIT_LEFT )
   );
   if ( _nbFiles == 3 ) {
      fileMenu->addAction(
         "Edit middle file", this, SLOT(editMiddle()),
         _resources->getAccelerator( ACCEL_EDIT_MIDDLE )
      );
   }
   ids[5] = fileMenu->addAction(
      "Edit right file", this, SLOT(editRight()),
      _resources->getAccelerator( ACCEL_EDIT_RIGHT )
   );
   if ( _cmdline._unmerge == true || _cmdline._single == true ) {
       ids[5]->setEnabled( false );
   }

   fileMenu->addAction(
      "Save Options...", this, SLOT(saveOptions()),
      _resources->getAccelerator( ACCEL_SAVE_OPTIONS )
   );

   fileMenu->addSeparator();

   if ( _cmdline._forceDecision == false ) {
      fileMenu->addAction(
         "Exit", this, SLOT(quit()),
         _resources->getAccelerator( ACCEL_EXIT )
      );
   }
   else {

      fileMenu->addAction(
         "Exit with ACCEPT", this, SLOT(quitAccept()),
         _resources->getAccelerator( ACCEL_EXIT_ACCEPT )
      );
      fileMenu->addAction(
         "Exit with MERGED", this, SLOT(quitMerged()),
         _resources->getAccelerator( ACCEL_EXIT_MERGED )
      );
      fileMenu->addAction(
         "Exit with REJECT", this, SLOT(quitReject()),
         _resources->getAccelerator( ACCEL_EXIT_REJECT )
      );
   }

   //---------------------------------------------------------------------------

   // Edit menu
   QkMenu* editMenu = menubar->addMenu( "&Edit" );
   editMenu->addAction(
      "Search...", this, SLOT(search()),
      _resources->getAccelerator( ACCEL_SEARCH )
   );
   editMenu->addAction(
      "Search forward", this, SLOT(searchForward()),
      _resources->getAccelerator( ACCEL_SEARCH_FORWARD )
   );
   editMenu->addAction(
      "Search backward", this, SLOT(searchBackward()),
      _resources->getAccelerator( ACCEL_SEARCH_BACKWARD )
   );
   editMenu->addSeparator();
   editMenu->addAction(
      "Beginning of file", this, SLOT(cursorTop()),
      _resources->getAccelerator( ACCEL_CURSOR_TOP )
   );
   editMenu->addAction(
      "End of file", this, SLOT(cursorBottom()),
      _resources->getAccelerator( ACCEL_CURSOR_BOTTOM )
   );

   //---------------------------------------------------------------------------

   // View menu
   QkMenu* viewMenu = menubar->addMenu( "V&iew" );
   if ( _filesAreDirectories == true ) {
      _menuactions[ ID_View_DiffFilesAtCursor ] = viewMenu->addAction(
         "Diff files at cursor", this, SLOT(diffFilesAtCursor()),
         _resources->getAccelerator( ACCEL_DIFF_FILES_AT_CURSOR )
      );
      _menuactions[ ID_View_NextAndDiffFiles ] = viewMenu->addAction(
         "Next and diff files", this, SLOT(nextAndDiffFiles()),
         _resources->getAccelerator( ACCEL_NEXT_AND_DIFF_FILES )
      );
      _menuactions[ ID_View_CopyLeftToRight ] = viewMenu->addAction(
         "Copy left file to right", this, SLOT(copyFileLeftToRight()),
         _resources->getAccelerator( ACCEL_COPY_LEFT_TO_RIGHT )
      );
      _menuactions[ ID_View_CopyRightToLeft ] = viewMenu->addAction(
         "Copy right file to left", this, SLOT(copyFileRightToLeft()),
         _resources->getAccelerator( ACCEL_COPY_RIGHT_TO_LEFT )
      );
      _menuactions[ ID_View_RemoveLeft ] = viewMenu->addAction(
         "Remove left file", this, SLOT(removeFileLeft()),
         _resources->getAccelerator( ACCEL_REMOVE_LEFT )
      );
      _menuactions[ ID_View_RemoveRight ] = viewMenu->addAction(
         "Remove right file", this, SLOT(removeFileRight()),
         _resources->getAccelerator( ACCEL_REMOVE_RIGHT )
      );
      viewMenu->addSeparator();
   }
   viewMenu->addAction(
      "Next difference", this, SLOT(nextDifference()),
      _resources->getAccelerator( ACCEL_NEXT_DIFFERENCE )
   );
   viewMenu->addAction(
      "Previous difference", this, SLOT(previousDifference()),
      _resources->getAccelerator( ACCEL_PREVIOUS_DIFFERENCE )
   );
   viewMenu->addSeparator();
   viewMenu->addAction(
      "Next unselected", this, SLOT(nextUnselected()),
      _resources->getAccelerator( ACCEL_NEXT_UNSELECTED )
   );
   viewMenu->addAction(
      "Previous unselected", this, SLOT(previousUnselected()),
      _resources->getAccelerator( ACCEL_PREVIOUS_UNSELECTED )
   );

   //---------------------------------------------------------------------------

   // Right click View menu for directories
   _viewPopup[0] = new QkMenu;
   if ( _filesAreDirectories == true ) {
      _menuactions[ ID_View_DiffFilesAtCursor ] = _viewPopup[0]->addAction(
         "Diff files at cursor", this, SLOT(diffFilesAtCursor()),
         _resources->getAccelerator( ACCEL_DIFF_FILES_AT_CURSOR )
      );
      _menuactions[ ID_View_NextAndDiffFiles ] = _viewPopup[0]->addAction(
         "Next and diff files", this, SLOT(nextAndDiffFiles()),
         _resources->getAccelerator( ACCEL_NEXT_AND_DIFF_FILES )
      );
      _menuactions[ ID_View_CopyLeftToRight ] = _viewPopup[0]->addAction(
         "Copy left file to right", this, SLOT(copyFileLeftToRight()),
         _resources->getAccelerator( ACCEL_COPY_LEFT_TO_RIGHT )
      );
      _menuactions[ ID_View_CopyRightToLeft ] = _viewPopup[0]->addAction(
         "Copy right file to left", this, SLOT(copyFileRightToLeft()),
         _resources->getAccelerator( ACCEL_COPY_RIGHT_TO_LEFT )
      );
      _menuactions[ ID_View_RemoveLeft ] = _viewPopup[0]->addAction(
         "Remove left file", this, SLOT(removeFileLeft()),
         _resources->getAccelerator( ACCEL_REMOVE_LEFT )
      );
      _menuactions[ ID_View_RemoveRight ] = _viewPopup[0]->addAction(
         "Remove right file", this, SLOT(removeFileRight()),
         _resources->getAccelerator( ACCEL_REMOVE_RIGHT )
      );
      _viewPopup[0]->addSeparator();
   }

      _viewPopup[0]->addAction(
         "Next difference", this, SLOT(nextDifference()),
         _resources->getAccelerator( ACCEL_NEXT_DIFFERENCE )
      );
      _viewPopup[0]->addAction(
         "Previous difference", this, SLOT(previousDifference()),
         _resources->getAccelerator( ACCEL_PREVIOUS_DIFFERENCE )
      );
      _viewPopup[0]->addSeparator();
      _viewPopup[0]->addAction(
         "Next unselected", this, SLOT(nextUnselected()),
         _resources->getAccelerator( ACCEL_NEXT_UNSELECTED )
      );
      _viewPopup[0]->addAction(
         "Previous unselected", this, SLOT(previousUnselected()),
         _resources->getAccelerator( ACCEL_PREVIOUS_UNSELECTED )
      );

   // Right click View menu for the left file
   _viewPopup[1] = new QkMenu;
   _viewPopup[1]->addAction(
      "Replace left file...", this, SLOT(openLeft()),
      _resources->getAccelerator( ACCEL_OPEN_LEFT )
   );
   _viewPopup[1]->addAction(
      "Edit left file", this, SLOT(editLeft()),
      _resources->getAccelerator( ACCEL_EDIT_LEFT )
   );
   _viewPopup[1]->addAction(
         "Save as left", this, SLOT(saveAsLeft()),
         _resources->getAccelerator( ACCEL_SAVE_AS_LEFT )
      );
   _viewPopup[1]->addSeparator();

   _viewPopup[1]->addAction(
      "Next difference", this, SLOT(nextDifference()),
      _resources->getAccelerator( ACCEL_NEXT_DIFFERENCE )
   );
   _viewPopup[1]->addAction(
      "Previous difference", this, SLOT(previousDifference()),
      _resources->getAccelerator( ACCEL_PREVIOUS_DIFFERENCE )
   );
   _viewPopup[1]->addSeparator();
   _viewPopup[1]->addAction(
      "Next unselected", this, SLOT(nextUnselected()),
      _resources->getAccelerator( ACCEL_NEXT_UNSELECTED )
   );
   _viewPopup[1]->addAction(
      "Previous unselected", this, SLOT(previousUnselected()),
      _resources->getAccelerator( ACCEL_PREVIOUS_UNSELECTED )
   );
   _viewPopup[1]->addSeparator();

   _viewPopup[1]->addAction(
      "Select left line", this, SLOT(selectLineLeft()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_LEFT )
   );
   if ( _nbFiles == 3 ) {
      _viewPopup[1]->addAction(
         "Select middle line", this, SLOT(selectLineMiddle()),
         _resources->getAccelerator( ACCEL_SELECT_LINE_MIDDLE )
      );
   }
   _viewPopup[1]->addAction(
      "Select right line", this, SLOT(selectLineRight()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_RIGHT )
   );
   _viewPopup[1]->addAction(
      "Select neither", this, SLOT(selectLineNeither()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_NEITHER )
   );
   _viewPopup[1]->addAction(
      "Unselect", this, SLOT(selectLineUnselect()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_UNSELECT)
   );

   // Right click View menu for the middle or right file
   _viewPopup[2] = new QkMenu;
   if ( _nbFiles == 3 ) {
     _viewPopup[2]->addAction(
        "Replace middle file...", this, SLOT(openMiddle()),
        _resources->getAccelerator( ACCEL_OPEN_MIDDLE )
     );
     _viewPopup[2]->addAction(
        "Edit middle file", this, SLOT(editMiddle()),
        _resources->getAccelerator( ACCEL_EDIT_MIDDLE )
     );
     _viewPopup[2]->addAction(
        "Save as middle", this, SLOT(saveAsMiddle()),
        _resources->getAccelerator( ACCEL_SAVE_AS_MIDDLE )
     );
   }
   else
   {
     _viewPopup[2]->addAction(
        "Replace right file...", this, SLOT(openRight()),
        _resources->getAccelerator( ACCEL_OPEN_RIGHT )
     );
     _viewPopup[2]->addAction(
        "Edit right file", this, SLOT(editRight()),
        _resources->getAccelerator( ACCEL_EDIT_RIGHT )
     );
     _viewPopup[2]->addAction(
        "Save as right", this, SLOT(saveAsRight()),
        _resources->getAccelerator( ACCEL_SAVE_AS_RIGHT )
     );
   }
   _viewPopup[2]->addSeparator();

   _viewPopup[2]->addAction(
      "Next difference", this, SLOT(nextDifference()),
      _resources->getAccelerator( ACCEL_NEXT_DIFFERENCE )
   );
   _viewPopup[2]->addAction(
      "Previous difference", this, SLOT(previousDifference()),
      _resources->getAccelerator( ACCEL_PREVIOUS_DIFFERENCE )
   );
   _viewPopup[2]->addSeparator();
   _viewPopup[2]->addAction(
      "Next unselected", this, SLOT(nextUnselected()),
      _resources->getAccelerator( ACCEL_NEXT_UNSELECTED )
   );
   _viewPopup[2]->addAction(
      "Previous unselected", this, SLOT(previousUnselected()),
      _resources->getAccelerator( ACCEL_PREVIOUS_UNSELECTED )
   );
   _viewPopup[2]->addSeparator();

   _viewPopup[2]->addAction(
      "Select left line", this, SLOT(selectLineLeft()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_LEFT )
   );
   if ( _nbFiles == 3 ) {
      _viewPopup[2]->addAction(
         "Select middle line", this, SLOT(selectLineMiddle()),
         _resources->getAccelerator( ACCEL_SELECT_LINE_MIDDLE )
      );
   }
   _viewPopup[2]->addAction(
      "Select right line", this, SLOT(selectLineRight()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_RIGHT )
   );
   _viewPopup[2]->addAction(
      "Select neither", this, SLOT(selectLineNeither()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_NEITHER )
   );
   _viewPopup[2]->addAction(
      "Unselect", this, SLOT(selectLineUnselect()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_UNSELECT)
   );

   // Right click View menu for right file
   _viewPopup[3] = new QkMenu;
   _viewPopup[3]->addAction(
      "Replace right file...", this, SLOT(openRight()),
      _resources->getAccelerator( ACCEL_OPEN_RIGHT )
   );
   _viewPopup[3]->addAction(
      "Edit right file", this, SLOT(editRight()),
      _resources->getAccelerator( ACCEL_EDIT_RIGHT )
   );
   _viewPopup[3]->addAction(
      "Save as right", this, SLOT(saveAsRight()),
      _resources->getAccelerator( ACCEL_SAVE_AS_RIGHT )
   );
   _viewPopup[3]->addSeparator();

   _viewPopup[3]->addAction(
      "Next difference", this, SLOT(nextDifference()),
      _resources->getAccelerator( ACCEL_NEXT_DIFFERENCE )
   );
   _viewPopup[3]->addAction(
      "Previous difference", this, SLOT(previousDifference()),
      _resources->getAccelerator( ACCEL_PREVIOUS_DIFFERENCE )
   );
   _viewPopup[3]->addSeparator();
   _viewPopup[3]->addAction(
      "Next unselected", this, SLOT(nextUnselected()),
      _resources->getAccelerator( ACCEL_NEXT_UNSELECTED )
   );
   _viewPopup[3]->addAction(
      "Previous unselected", this, SLOT(previousUnselected()),
      _resources->getAccelerator( ACCEL_PREVIOUS_UNSELECTED )
   );
   _viewPopup[3]->addSeparator();

   _viewPopup[3]->addAction(
      "Select left line", this, SLOT(selectLineLeft()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_LEFT )
   );
   if ( _nbFiles == 3 ) {
      _viewPopup[3]->addAction(
         "Select middle line", this, SLOT(selectLineMiddle()),
         _resources->getAccelerator( ACCEL_SELECT_LINE_MIDDLE )
      );
   }
   _viewPopup[3]->addAction(
      "Select right line", this, SLOT(selectLineRight()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_RIGHT )
   );
   _viewPopup[3]->addAction(
      "Select neither", this, SLOT(selectLineNeither()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_NEITHER )
   );
   _viewPopup[3]->addAction(
      "Unselect", this, SLOT(selectLineUnselect()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_UNSELECT)
   );

   //---------------------------------------------------------------------------

   if ( _filesAreDirectories == false ) {
   
      // Global menu
      QkMenu* globalMenu = menubar->addMenu( "&Global" );
      globalMenu->addAction(
         "Select left", this, SLOT(selectGlobalLeft()),
         _resources->getAccelerator( ACCEL_SELECT_GLOBAL_LEFT )
      );
      if ( _nbFiles == 3 ) {
         globalMenu->addAction(
            "Select middle", this, SLOT(selectGlobalMiddle()),
            _resources->getAccelerator( ACCEL_SELECT_GLOBAL_MIDDLE )
         );
      }
      globalMenu->addAction(
         "Select right", this, SLOT(selectGlobalRight()),
         _resources->getAccelerator( ACCEL_SELECT_GLOBAL_RIGHT )
      );
      globalMenu->addAction(
         "Select neither", this, SLOT(selectGlobalNeither()),
         _resources->getAccelerator( ACCEL_SELECT_GLOBAL_NEITHER )
      );
      globalMenu->addAction(
         "Unselect", this, SLOT(selectGlobalUnselect()),
         _resources->getAccelerator( ACCEL_SELECT_GLOBAL_UNSELECT)
      );
      globalMenu->addSeparator();
      globalMenu->addAction(
         "Select unselected left", this, SLOT(selectGlobalUnselectedLeft()),
         _resources->getAccelerator(
            ACCEL_SELECT_GLOBAL_UNSELECTED_LEFT
         )
      );
      if ( _nbFiles == 3 ) {
         globalMenu->addAction(
            "Select unselected middle", this, SLOT(selectGlobalUnselectedMiddle()),
            _resources->getAccelerator(
               ACCEL_SELECT_GLOBAL_UNSELECTED_MIDDLE
            )
         );
      }
      globalMenu->addAction(
         "Select unselected right", this, SLOT(selectGlobalUnselectedRight()),
         _resources->getAccelerator(
            ACCEL_SELECT_GLOBAL_UNSELECTED_RIGHT
         )
      );
      globalMenu->addAction(
         "Select unselected neither", this, SLOT(selectGlobalUnselectedNeither()),
         _resources->getAccelerator(
            ACCEL_SELECT_GLOBAL_UNSELECTED_NEITHER
         )
      );
      globalMenu->addSeparator();
      globalMenu->addAction(
         "Merge", this, SLOT(selectGlobalMerge()),
         _resources->getAccelerator(
            ACCEL_SELECT_GLOBAL_MERGE
         )
      );

      //---------------------------------------------------------------------------

      // Region menu
      QkMenu* regionMenu = menubar->addMenu( "&Region" );
      regionMenu->addAction(
         "Select left", this, SLOT(selectRegionLeft()),
         _resources->getAccelerator( ACCEL_SELECT_REGION_LEFT )
      );
      if ( _nbFiles == 3 ) {
         regionMenu->addAction(
            "Select middle", this, SLOT(selectRegionMiddle()),
            _resources->getAccelerator( ACCEL_SELECT_REGION_MIDDLE )
         );
      }
      regionMenu->addAction(
         "Select right", this, SLOT(selectRegionRight()),
         _resources->getAccelerator( ACCEL_SELECT_REGION_RIGHT )
      );
      regionMenu->addAction(
         "Select neither", this, SLOT(selectRegionNeither()),
         _resources->getAccelerator( ACCEL_SELECT_REGION_NEITHER )
      );
      regionMenu->addAction(
         "Unselect", this, SLOT(selectRegionUnselect()),
         _resources->getAccelerator( ACCEL_SELECT_REGION_UNSELECT)
      );
      regionMenu->addSeparator();
      regionMenu->addAction(
         "Select left and next", this,
         SLOT(selectRegionLeftAndNext()),
         _resources->getAccelerator(
            ACCEL_SELECT_REGION_LEFT_AND_NEXT
         )
      );
      if ( _nbFiles == 3 ) {
         regionMenu->addAction(
            "Select middle and next", this,
            SLOT(selectRegionMiddleAndNext()),
            _resources->getAccelerator(
               ACCEL_SELECT_REGION_MIDDLE_AND_NEXT
            )
         );
      }
      regionMenu->addAction(
         "Select right and next", this,
         SLOT(selectRegionRightAndNext()),
         _resources->getAccelerator(
            ACCEL_SELECT_REGION_RIGHT_AND_NEXT
         )
      );
      regionMenu->addAction(
         "Select neither and next", this,
         SLOT(selectRegionNeitherAndNext()),
         _resources->getAccelerator(
            ACCEL_SELECT_REGION_NEITHER_AND_NEXT
         )
      );
      regionMenu->addSeparator();
      regionMenu->addAction(
         "Split/swap/join", this, SLOT(regionSplitSwapJoin()),
         _resources->getAccelerator(
            ACCEL_SELECT_REGION_SPLIT_SWAP_JOIN
         )
      );

      //---------------------------------------------------------------------------

      // Line menu
      QkMenu* lineMenu = menubar->addMenu( "Li&ne" );
      lineMenu->addAction(
         "Select left", this, SLOT(selectLineLeft()),
         _resources->getAccelerator( ACCEL_SELECT_LINE_LEFT )
      );
      if ( _nbFiles == 3 ) {
         lineMenu->addAction(
            "Select middle", this, SLOT(selectLineMiddle()),
            _resources->getAccelerator( ACCEL_SELECT_LINE_MIDDLE )
         );
      }
      lineMenu->addAction(
         "Select right", this, SLOT(selectLineRight()),
         _resources->getAccelerator( ACCEL_SELECT_LINE_RIGHT )
      );
      lineMenu->addAction(
         "Select neither", this, SLOT(selectLineNeither()),
         _resources->getAccelerator( ACCEL_SELECT_LINE_NEITHER )
      );
      lineMenu->addAction(
         "Unselect", this, SLOT(selectLineUnselect()),
         _resources->getAccelerator( ACCEL_SELECT_LINE_UNSELECT)
      );
      
  } //  _filesAreDirectories == false

   //---------------------------------------------------------------------------

   // Options menu
   _optionsMenu = menubar->addMenu( "O&ptions" );

   _optionsMenu->addAction(
      "Edit diff options...", this, SLOT(editDiffOptions()),
      _resources->getAccelerator( ACCEL_EDIT_DIFF_OPTIONS )
   );

   if ( _filesAreDirectories == false ) {

      if ( _nbFiles == 2 &&
           _cmdline._unmerge == false &&
           _cmdline._single == false ) {
         _optionsMenu->addSeparator();

         _menuactions[ ID_ToggleIgnoreTrailing ] = _optionsMenu->addAction(
            "Ignore trailing blanks", this, SLOT(ignoreTrailing()),
            _resources->getAccelerator( ACCEL_IGNORE_TRAILING )
         );
         _menuactions[ ID_ToggleIgnoreTrailing ]->setCheckable( true );
         
         _menuactions[ ID_ToggleIgnoreWhitespace ] = _optionsMenu->addAction(
            "Ignore whitespace", this, SLOT(ignoreWhitespace()),
            _resources->getAccelerator( ACCEL_IGNORE_WHITESPACE )
         );
         _menuactions[ ID_ToggleIgnoreWhitespace ]->setCheckable( true );
         
         _menuactions[ ID_ToggleIgnoreCase ] = _optionsMenu->addAction(
            "Ignore case", this, SLOT(ignoreCase()),
            _resources->getAccelerator( ACCEL_IGNORE_CASE )
         );
         _menuactions[ ID_ToggleIgnoreCase ]->setCheckable( true );
         
         _menuactions[ ID_ToggleIgnoreBlankLines ] = _optionsMenu->addAction(
            "Ignore blank lines", this, SLOT(ignoreBlankLines()),
            _resources->getAccelerator( ACCEL_IGNORE_BLANK_LINES )
         );
         _menuactions[ ID_ToggleIgnoreBlankLines ]->setCheckable( true );

         _optionsMenu->addSeparator();

         _menuactions[ ID_ToggleQualityNormal ] = _optionsMenu->addAction(
            "Quality: normal", this, SLOT(qualityNormal()),
            _resources->getAccelerator( ACCEL_QUALITY_NORMAL )
         );
         _menuactions[ ID_ToggleQualityNormal ]->setCheckable( true );
         
         _menuactions[ ID_ToggleQualityFastest ] = _optionsMenu->addAction(
            "Quality: fastest", this, SLOT(qualityFastest()),
            _resources->getAccelerator( ACCEL_QUALITY_FASTEST )
         );
         _menuactions[ ID_ToggleQualityFastest ]->setCheckable( true );
         
         _menuactions[ ID_ToggleQualityHighest ] = _optionsMenu->addAction(
            "Quality: highest", this, SLOT(qualityHighest()),
            _resources->getAccelerator( ACCEL_QUALITY_HIGHEST )
         );
         _menuactions[ ID_ToggleQualityHighest ]->setCheckable( true );
      }
   }
   else {
      _optionsMenu->addSeparator();

      _menuactions[ ID_ToggleDirDiffsRecursive ] = _optionsMenu->addAction(
         "Recursive", this, SLOT(dirDiffRecursive()),
         _resources->getAccelerator(
            ACCEL_DIRDIFF_RECURSIVE
         )
      );
      _menuactions[ ID_ToggleDirDiffsRecursive ]->setCheckable( true );
   }

   //---------------------------------------------------------------------------

   _displayMenu = menubar->addMenu( "&Display" );

   _displayMenu->addAction(
      "Edit display options...", this, SLOT(editDisplayOptions()),
      _resources->getAccelerator( ACCEL_EDIT_DISPLAY_OPTIONS )
   );
   _displayMenu->addSeparator();

   _hordiffMenu = 0;
   if ( _filesAreDirectories == false ) {

      {
         _hordiffMenu = _displayMenu->addMenu( "Horizontal diffs" );

         _menuactions[ ID_Hordiff_None ] = _hordiffMenu->addAction(
            "None", this, SLOT(hordiffTypeNone()),
            _resources->getAccelerator( ACCEL_HORDIFF_NONE )
         );
         _menuactions[ ID_Hordiff_None ]->setCheckable( true );

         _menuactions[ ID_Hordiff_Single ] = _hordiffMenu->addAction(
            "Single", this, SLOT(hordiffTypeSingle()),
            _resources->getAccelerator( ACCEL_HORDIFF_SINGLE )
         );
         _menuactions[ ID_Hordiff_Single ]->setCheckable( true );

         _menuactions[ ID_Hordiff_Multiple ] = _hordiffMenu->addAction(
            "Multiple", this, SLOT(hordiffTypeMultiple()),
            _resources->getAccelerator( ACCEL_HORDIFF_MULTIPLE )
         );
         _menuactions[ ID_Hordiff_Multiple ]->setCheckable( true );
      }

      _menuactions[ ID_ToggleIgnoreHorizontalWs ] = _displayMenu->addAction(
         "Ignore horizontal whitespace", this, SLOT(toggleIgnoreHorizontalWs()),
         _resources->getAccelerator( ACCEL_TOGGLE_IGNORE_HORIZONTAL_WS )
      );
      _menuactions[ ID_ToggleIgnoreHorizontalWs ]->setCheckable( true );

      _menuactions[ ID_ToggleIgnorePerHunkWs ] = _displayMenu->addAction(
         "Ignore per-hunk whitespace", this, SLOT(toggleIgnorePerHunkWs()),
         _resources->getAccelerator( ACCEL_TOGGLE_IGNORE_PERHUNK_WS )
      );
      _menuactions[ ID_ToggleIgnorePerHunkWs ]->setCheckable( true );

      _menuactions[ ID_ToggleHideCarriageReturns ] = _displayMenu->addAction(
         "Hide carriage returns", this, SLOT(hideCarriageReturns()),
         _resources->getAccelerator( ACCEL_HIDE_CR )
      );
      _menuactions[ ID_ToggleHideCarriageReturns ]->setCheckable( true );

      _menuactions[ ID_ToggleVerticalLine ] = _displayMenu->addAction(
         "Draw vertical line", this, SLOT(toggleVerticalLine()),
         _resources->getAccelerator( ACCEL_TOGGLE_VERTICAL_LINE )
      );
      _menuactions[ ID_ToggleVerticalLine ]->setCheckable( true );

      _menuactions[ ID_ToggleHorizNullMarkers ] = _displayMenu->addAction(
         "Horizontal Null Markers", this, SLOT(toggleHorizNullMarkers()),
         _resources->getAccelerator( ACCEL_TOGGLE_NULL_HORIZONTAL_MARKERS )
      );
      _menuactions[ ID_ToggleHorizNullMarkers ]->setCheckable( true );

   }
   else {
      _menuactions[ ID_ToggleDirDiffsIgnoreFileChanges ] = _displayMenu->addAction(
         "Ignore file changes", this, SLOT(ignoreFileChanges()),
         _resources->getAccelerator(
            ACCEL_DIRDIFF_IGNORE_FILE_CHANGES
         )
      );
      _menuactions[ ID_ToggleDirDiffsIgnoreFileChanges ]->setCheckable( true );
   }

   _displayMenu->addSeparator();
   
   _menuactions[ ID_ToggleFormatClipboardText ] = _displayMenu->addAction(
      "Format clipboard text", this,
      SLOT(toggleFormatClipboardText()),
      _resources->getAccelerator(
         ACCEL_TOGGLE_FORMAT_CLIPBOARD_TEXT
      )
   );
   _menuactions[ ID_ToggleFormatClipboardText ]->setCheckable( true );

   if ( _filesAreDirectories == false ) {

      _displayMenu->addSeparator();

      _menuactions[ ID_TabsAtThree ] = _displayMenu->addAction(
         "Tabs at 3", this, SLOT(tabsAt3()),
         _resources->getAccelerator( ACCEL_TABS_AT_3 )
      );
      _menuactions[ ID_TabsAtThree ]->setCheckable( true );
      
      _menuactions[ ID_TabsAtFour ] = _displayMenu->addAction(
         "Tabs at 4", this, SLOT(tabsAt4()),
         _resources->getAccelerator( ACCEL_TABS_AT_4 )
      );
      _menuactions[ ID_TabsAtFour ]->setCheckable( true );
      
      _menuactions[ ID_TabsAtEight ] = _displayMenu->addAction(
         "Tabs at 8", this, SLOT(tabsAt8()),
         _resources->getAccelerator( ACCEL_TABS_AT_8 )
      );
      _menuactions[ ID_TabsAtEight ]->setCheckable( true );
   }

   _displayMenu->addSeparator();
   _menuactions[ ID_ToggleLineNumbers ] = _displayMenu->addAction(
      "Toggle line numbers", this, SLOT(toggleLineNumbers()),
      _resources->getAccelerator( ACCEL_TOGGLE_LINE_NUMBERS )
   );
   _menuactions[ ID_ToggleLineNumbers ]->setCheckable( true );

   if ( _filesAreDirectories == false && _nbFiles == 3 ) {

      _displayMenu->addSeparator();

      _menuactions[ ID_IgnoreFileNone ] = _displayMenu->addAction(
         "No ignore", this, SLOT(ignoreFileNone()),
         _resources->getAccelerator( ACCEL_IGNORE_FILE_NONE )
      );
      _menuactions[ ID_IgnoreFileNone ]->setCheckable( true );
      
      _menuactions[ ID_IgnoreFileLeft ] = _displayMenu->addAction(
         "Ignore left file", this, SLOT(ignoreFileLeft()),
         _resources->getAccelerator( ACCEL_IGNORE_FILE_LEFT )
      );
      _menuactions[ ID_IgnoreFileLeft ]->setCheckable( true );
      
      _menuactions[ ID_IgnoreFileMiddle ] = _displayMenu->addAction(
         "Ignore middle file", this, SLOT(ignoreFileMiddle()),
         _resources->getAccelerator( ACCEL_IGNORE_FILE_MIDDLE )
      );
      _menuactions[ ID_IgnoreFileMiddle ]->setCheckable( true );
      
      _menuactions[ ID_IgnoreFileRight ] = _displayMenu->addAction(
         "Ignore right file", this, SLOT(ignoreFileRight()),
         _resources->getAccelerator( ACCEL_IGNORE_FILE_RIGHT )
      );
      _menuactions[ ID_IgnoreFileRight ]->setCheckable( true );
   }

   //---------------------------------------------------------------------------

   // Windows menu
   _windowsMenu = menubar->addMenu( "W&indows" );
   if ( _filesAreDirectories == false ) {
      _menuactions[ ID_TogglePaneMergedView ] = _windowsMenu->addAction(
         "Toggle pane merged view", this, SLOT(togglePaneMergedView()),
         _resources->getAccelerator( ACCEL_TOGGLE_PANE_MERGED_VIEW )
      );
      _menuactions[ ID_TogglePaneMergedView ]->setCheckable( true );
      
      _menuactions[ ID_TogglePopupMergedView ] = _windowsMenu->addAction(
         "Toggle popup merged view", this, SLOT(togglePopupMergedView()),
         _resources->getAccelerator( ACCEL_TOGGLE_POPUP_MERGED_VIEW )
      );
      _menuactions[ ID_TogglePopupMergedView ]->setCheckable( true );
      
      _windowsMenu->addSeparator();
   }
   _menuactions[ ID_ToggleToolbar ] = _windowsMenu->addAction(
      "Toggle toolbar", this,
      SLOT(toggleToolbar()),
      _resources->getAccelerator( ACCEL_TOGGLE_TOOLBAR )
   );
   _menuactions[ ID_ToggleToolbar ]->setCheckable( true );
   
   _menuactions[ ID_ToggleOverview ] = _windowsMenu->addAction(
      "Toggle overview", this, SLOT(toggleOverview()),
      _resources->getAccelerator( ACCEL_TOGGLE_OVERVIEW )
   );
   _menuactions[ ID_ToggleOverview ]->setCheckable( true );
   
   _menuactions[ ID_ToggleShowFilenames ] = _windowsMenu->addAction(
      "Toggle show filename", this, SLOT(toggleShowFilenames()),
      _resources->getAccelerator( ACCEL_TOGGLE_SHOW_FILENAMES )
   );
   _menuactions[ ID_ToggleShowFilenames ]->setCheckable( true );

   //---------------------------------------------------------------------------

   // Help menu
   menubar->addSeparator();
   QkMenu* helpMenu = menubar->addMenu( "&Help" );
   helpMenu->addAction(
      "User's manual...", this, SLOT(helpManPage()),
      _resources->getAccelerator( ACCEL_HELP_MAN_PAGE )
   );

   QAction * actOnContext = QWhatsThis::createAction();
   actOnContext->setShortcut( _resources->getAccelerator( ACCEL_HELP_ON_CONTEXT ) );
   helpMenu->addAction( actOnContext );

   helpMenu->addSeparator();
   helpMenu->addAction(
      "About...", this, SLOT(helpAbout()),
      _resources->getAccelerator( ACCEL_HELP_ABOUT )
   );
}

//------------------------------------------------------------------------------
//
std::auto_ptr<XxBuffer> XxApp::readFile(
   const XxFno      no,
   const QString&   filename,
   const QString&   displayFilename,
   const QFileInfo& fileInfo,
   bool             isTemporary
)
{
   XX_ASSERT( 0 <= no && no <= 2 );
   XX_ASSERT( filename.length() != 0 && displayFilename.length() != 0 );

   //
   // Read in the file.
   //
   std::auto_ptr<XxBuffer> newbuf;

   {
      if ( _filesAreDirectories &&
           _resources->getBoolOpt( BOOL_DIRDIFF_BUILD_FROM_OUTPUT ) ) {
         // Assign an empty buffer. The directory diffs builder will fill it in
         // with whatever contents are read from directory diffs command output.
         std::auto_ptr<XxBuffer> tmp(
            new XxBuffer( false, filename, displayFilename, _newlineChar )
         );
         newbuf = tmp;
      }
      else {
         std::auto_ptr<XxBuffer> tmp(
            new XxBuffer(
               filename,
               displayFilename,
               fileInfo,
               isTemporary,
               _newlineChar
            )
         );

         if ( _cmdline._useTemporaryFiles ) {
            // Copy the file contents to a temporary file and use that as our
            // buffer.
            tmp->makeTemporary();
         }

         newbuf = tmp;
      }
   }

   return newbuf;
}

//------------------------------------------------------------------------------
//
bool XxApp::processDiff()
{
   _isThereAnyDifference = false;

   // Important note: this method assumes that the nbFiles have already been
   // loaded.
   for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
      XX_ASSERT( _files[ii].get() != 0 );
   }

   // Note: disconnect when deleting _diffs.
   if ( _diffs.get() != 0 ) {
      disconnect( _diffs.get() );
   }
   _diffs.release();


   //
   // If this is a single, create the diffs directly without running anything
   // special.
   //
   std::auto_ptr<XxBuilder> builder;
   if ( _cmdline._single == true ) {

      XxBuilderSingle* builderSingle = new XxBuilderSingle;
      std::auto_ptr<XxBuilder> builderTmp( builderSingle );
      builder = builderTmp;
      try {

         QString leftName, rightName;
         std::auto_ptr<XxDiffs> tmp(
            builderSingle->process( *(_files[0]) )
         );
         _diffs = tmp;
         XX_ASSERT( _diffs.get() != 0 );
      }
      catch ( const XxError& ex ) {
         QString str;
         QTextStream oss( &str );
         oss << "Error processing single file:" << ex.getMsg() << endl;
         outputDiffErrors( str );
         _returnValue = builder->getStatus();
         return false;
      }
   }
   //
   // If this is an unmerge, create the diffs directly from the buffer in
   // memory (i.e. we do not need to spawn an external process).
   //
   else if ( _cmdline._unmerge == true ) {

      XxBuilderUnmerge* builderUnmerge = new XxBuilderUnmerge;
      std::auto_ptr<XxBuilder> builderTmp( builderUnmerge );
      builder = builderTmp;
      try {

         QString leftName, middleName, rightName;
         if ( _cmdline._unmergeNbFiles == 2 ) {
            std::auto_ptr<XxDiffs> tmp(
               builderUnmerge->process(
                  *(_files[0]), _resources, leftName, rightName
               )
            );
            _diffs = tmp;
         }
         else {
            std::auto_ptr<XxDiffs> tmp(
               builderUnmerge->process(
                  *(_files[0]), _resources, leftName, middleName, rightName
               )
            );
            _diffs = tmp;
         }
         XX_ASSERT( _diffs.get() != 0 );

         // Reindexing. We have chosen to use a different strategy because we'd
         // really like to keep the line numbers from the original conflict
         // file. This works and fools all the code into thinking it's a normal
         // two-file diff, but we'd lose the real line numbers.
         _diffs->reindex( _files[0], _files[1], _files[2] );

         // Add names from tags to display filenames.
         if ( !leftName.isEmpty() ) {
            _files[0]->setDisplayName(
               _files[0]->getDisplayName() + " (" + leftName + ")"
            );
         }
         if ( _cmdline._unmergeNbFiles == 2 ) {
            if ( !rightName.isEmpty() ) {
               _files[1]->setDisplayName(
                  _files[1]->getDisplayName() + " (" + rightName + ")"
               );
            }
         }
         else {
            if ( !middleName.isEmpty() ) {
               _files[1]->setDisplayName(
                  _files[1]->getDisplayName() + " (" + middleName + ")"
               );
            }
            if ( !rightName.isEmpty() ) {
               _files[2]->setDisplayName(
                  _files[2]->getDisplayName() + " (" + rightName + ")"
               );
            }
         }
      }
      catch ( const XxError& ex ) {
         QString str;
         QTextStream oss( &str );
         oss << "Error unmerging file:" << ex.getMsg() << endl;
         outputDiffErrors( str );
         _returnValue = builder->getStatus();
         return false;
      }
   }
   //
   // else, normal diffs, run the diff command (this is the usual case).
   //
   else if ( _filesAreDirectories == false ) {
      if ( _nbFiles == 2 ) {
         XxBuilderFiles2* filesBuilder = new XxBuilderFiles2(
            _resources->getBoolOpt( BOOL_USE_INTERNAL_DIFF )
         );
         std::auto_ptr<XxBuilder> builderTmp( filesBuilder );
         builder = builderTmp;
         try {
            std::auto_ptr<XxDiffs> tmp(
               filesBuilder->process(
                  _resources->getCommand( CMD_DIFF_FILES_2 ),
                  *_files[0],
                  *_files[1]
               )
            );
            _diffs = tmp;
            XX_ASSERT( _diffs.get() != 0 );
         }
         catch ( const XxError& ex ) {
            QString str;
            QTextStream oss( &str );
            oss << "Error executing \""
                << _resources->getCommand( CMD_DIFF_FILES_2 )
                << "\" command, could not build diffs:" << endl
                << ex.getMsg() << endl
                << "Maybe try --text option." << endl
                << endl;
            outputDiffErrors( str );
            _returnValue = builder->getStatus();
            return false;
         }
      }
      else if ( _nbFiles == 3 ) {
         XxBuilderFiles3* filesBuilder = new XxBuilderFiles3;
         std::auto_ptr<XxBuilder> builderTmp( filesBuilder );
         builder = builderTmp;
         try {
            std::auto_ptr<XxDiffs> tmp(
               filesBuilder->process(
                  _resources->getCommand( CMD_DIFF_FILES_3 ),
                  *_files[0],
                  *_files[1],
                  *_files[2]
               )
            );
            _diffs = tmp;
            XX_ASSERT( _diffs.get() != 0 );
         }
         catch ( const XxError& ex ) {
            QString str;
            QTextStream oss( &str );
            oss << "Error executing \""
                << _resources->getCommand( CMD_DIFF_FILES_3 )
                << "\" command, could not build diffs:" << endl
                << ex.getMsg() << endl
                << "Maybe try --text option." << endl
                << endl;
            outputDiffErrors( str );
            _returnValue = builder->getStatus();
            return false;
         }
      }
   }
   else {
      if ( _nbFiles != 2 ) {
         throw XxUsageError(
            XX_EXC_PARAMS,
            "Directory diffs can only be performed with two files."
         );
      }

      if ( _cmdline._forceDecision == true ) {
         throw XxUsageError( XX_EXC_PARAMS,
                             "Cannot force decision on directory diffs." );
      }

      XxBuilderDirs2* dirsBuilder = new XxBuilderDirs2(
         _resources->getBoolOpt( BOOL_DIRDIFF_BUILD_FROM_OUTPUT ),
         _resources->getBoolOpt( BOOL_DIRDIFF_RECURSIVE ),
         _resources->getBoolOpt( BOOL_DIRDIFF_IGNORE_FILE_CHANGES )
      );
      std::auto_ptr<XxBuilder> builderTmp( dirsBuilder );
      builder = builderTmp;
      const char* dirdiff_command = 0;
      try {
         if ( _resources->getBoolOpt( BOOL_DIRDIFF_RECURSIVE ) ) {
            dirdiff_command = _resources->getCommand(
               CMD_DIFF_DIRECTORIES_REC
            ).toAscii().constData();
         }
         else {
            dirdiff_command = _resources->getCommand(
               CMD_DIFF_DIRECTORIES
            ).toAscii().constData();
         }
         std::auto_ptr<XxDiffs> tmp(
            dirsBuilder->process( dirdiff_command, *_files[0], *_files[1] )
         );
         _diffs = tmp;
         XX_ASSERT( _diffs.get() != 0 );
      }
      catch ( const XxError& ex ) {
         QString str;
         QTextStream oss( &str );
         oss << "Error executing \""
             << dirdiff_command
             << "\" command, could not build diffs:" << endl
             << ex.getMsg() << endl;
         outputDiffErrors( str );
         _returnValue = builder->getStatus();
         return false;
      }
   }

   // Note: the return value of xxdiff matches the one that the spawned
   // diff returns.  e.g. a return value of 1 does not indicate an error.
   _returnValue = builder->getStatus();

   // If there were some warnings, output them.
   if ( !_resources->getBoolOpt( BOOL_IGNORE_ERRORS ) &&
        builder->hasErrors() ) {
      outputDiffErrors( builder->getErrors() );
   }

   //
   // Compute ignore-display algorithms on top of the diff process.
   //
   if ( _diffs.get() != 0 ) {
      if ( !_filesAreDirectories &&
           _resources->getBoolOpt( BOOL_IGNORE_PERHUNK_WS ) ) {
         _diffs->computeIgnoreDisplay( _nbFiles, getBuffers() );
      }
   }

   //
   // Adjust UI elements
   //
   if ( _diffs.get() != 0 ) {

      connect( _diffs.get(), SIGNAL(changed()), this, SLOT(updateWidgets()) );
      connect( _diffs.get(), SIGNAL(nbLinesChanged()),
               this, SLOT(onNbLinesChanged()) );

      // Sanity check: check that the number of file lines are smaller than the
      // number of diff lines.
      if ( !( _cmdline._unmerge || _cmdline._single ) ) {
         for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
#if 1
// FIXME this is not necessarily true with -B, depending on the implementation
// (if you display the empty lines then it should be true)
            if ( XxDln( _files[ii]->getNbLines() ) > _diffs->getNbLines() ) {
               throw XxInternalError( XX_EXC_PARAMS );
            }
#endif
         }
      }

      // Reset update connections and adjust scrollbars.
      if ( _diffs.get() != 0 ) {
         connect( _diffs.get(), SIGNAL(changed()),
                  this, SLOT(updateWidgets()) );
      }

      // Note: this should also be done, but more efficiently for each loaded
      // file separately.
      emit textSizeChanged();
   }

   _isThereAnyDifference = computeAbsoluteDifference();

   return _returnValue == 2 || _diffs.get() != 0;
}

//------------------------------------------------------------------------------
//
XxResources* XxApp::buildResources() const
{
   XxResources* resources = new XxResources( _cmdline._originalXdiff );
   XxResParser resParser;

   // Note: the UI hasn't been built at this point.
   if ( _cmdline._useRcfile == true ) {
      try {
         QString rcfilename = XxResParser::getRcFilename();
         resParser.parse( rcfilename, *resources );
      }
      catch ( const XxIoError& ioerr ) {
         QMessageBox::critical(
            _mainWindow, "xxdiff", ioerr.getMsg()
         );
      }
   }

   if ( !_cmdline._cmdlineResources.isEmpty() ) {
      try {
         QTextStream cmdlineStream( &_cmdline._cmdlineResources );
         resParser.parse( cmdlineStream, *resources );
      }
      catch ( const XxIoError& ioerr ) {
         QMessageBox::critical(
            _mainWindow, "xxdiff cmdline resources", ioerr.getMsg()
         );
      }
   }

   return resources;
}

//------------------------------------------------------------------------------
//
void XxApp::outputDiffErrors( const QString& errors )
{
   // _mainWindow could be 0 here, if this is called from the constructor,
   // that's ok.

   QMessageBox* diffErrorsMsgBox = new XxSuicideMessageBox(
      _mainWindow, "Diff errors.", errors, QMessageBox::Warning
   );

   diffErrorsMsgBox->show();

   // Forget about it, it'll be closed and destroy itself, not a leak.
   diffErrorsMsgBox->exec();
}

//------------------------------------------------------------------------------
//
uint XxApp::computeTextWidth() const
{
   uint textWidth = 0;
   for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
      textWidth =
         std::max( textWidth,
                   _files[ii]->computeTextWidth(
                      _resources->getFontText(),
                      _resources->getTabWidth(),
                      _resources->getBoolOpt( BOOL_HIDE_CR )
                   ) );
   }
   return textWidth;
}

//------------------------------------------------------------------------------
//
uint XxApp::getTextWidth() const
{
   if ( _textWidth == 0 ) {
      _textWidth = computeTextWidth();
   }
   return _textWidth;
}

//------------------------------------------------------------------------------
//
void XxApp::invalidateTextWidth()
{
   _textWidth = 0;
   emit textSizeChanged();
   adjustLineNumbers();
}

//------------------------------------------------------------------------------
//
void XxApp::adjustLineNumbers()
{
   _central->adjustLineNumbers(
      _resources->getShowOpt( SHOW_LINE_NUMBERS ), _resources->getFontText()
   );
}

//------------------------------------------------------------------------------
//
QRect XxApp::getMainWindowGeometry() const
{
   return _mainWindow->geometry();
}

//------------------------------------------------------------------------------
//
QkMenu* XxApp::getViewPopup( const int no, const XxLine& /*line*/ ) const
{
   if ( _filesAreDirectories == true ) {
      XxDln cursorLine = getCursorLine();
      const XxLine& line = _diffs->getLine( cursorLine );

      XxFln no1 = line.getLineNo( 0 );
      XxFln no2 = line.getLineNo( 1 );
      // Note: can only have two files.
      bool dirs = line.getType() == XxLine::DIRECTORIES;

      _menuactions[ ID_View_DiffFilesAtCursor ]->setEnabled( ( no1 != -1 && no2 != -1 ) );
      _menuactions[ ID_View_CopyRightToLeft ]->setEnabled(   ( no2 != -1 && !dirs ) );
      _menuactions[ ID_View_CopyLeftToRight ]->setEnabled(   ( no1 != -1 && !dirs ) );
      _menuactions[ ID_View_RemoveLeft ]->setEnabled(        ( no1 != -1 && !dirs ) );
      _menuactions[ ID_View_RemoveRight ]->setEnabled(       ( no2 != -1 && !dirs ) );
      return _viewPopup[0];
   }
   else
   {
      return _viewPopup[no + 1];
   }

}

//------------------------------------------------------------------------------
//
void XxApp::updateWidgets()
{
   _central->update();

   if ( _remUnselView->isVisible() ) {
      uint remNb = 0;
      if ( _diffs.get() ) {
         remNb = _diffs->countRemainingUnselected();
      }

// FIXME error do this on diff change not on update
      _remUnselView->setNum( int(remNb) );

      // Little marker if the files are EXACTLY equal.
      if ( ! _isThereAnyDifference ) {
         _remUnselView->setText( _remUnselView->text() + QString(" (=)") );
      }
   }
   if ( _overview->isVisible() ) {
      _overview->update();
   }
   if ( _popupMergedView != 0 && _popupMergedView->isVisible() ) {
      _popupMergedView->update();
   }
   if ( _paneMergedView != 0 && _paneMergedView->isVisible() ) {
      _paneMergedView->update();
   }
}

//------------------------------------------------------------------------------
//
void XxApp::onNbLinesChanged()
{
   emit textSizeChanged();
}

//------------------------------------------------------------------------------
//
bool XxApp::askOverwrite( const QString& filename ) const
{
   // Check for file existence.
   QFileInfo finfo( filename );
   if ( finfo.exists() ) {
      QString msg;
      {
         QTextStream oss( &msg );
         oss << "File \"" << filename << "\" exists";
         if ( ! finfo.isWritable() ) {
            oss << " (AND IS NOT WRITABLE)";
         }
         oss << ", overwrite?";
      }

      int resp = QMessageBox::warning(
         _mainWindow, "xxdiff", msg, "Ok", "Cancel", QString(), 0, 1
      );
      if ( resp == 1 ) {
         // User has canceled.
         return false;
      }
      // Continue anyway, overwrite file accepted.
   }
   return true;
}

//------------------------------------------------------------------------------
//
bool XxApp::saveMergedToFile(
   const QString& filename,
   const bool     ask,
   const bool     noCancel,
   const bool     overwrite
)
{
   if ( _diffs.get() == 0 ) {
      return false;
   }

   // Check if there are some unselected regions remaining.
   bool allSelected = _diffs->isAllSelected();
   if ( !allSelected ) {

      // Bring the user to the first unselected region.
      XxDln nextNo = _diffs->findNextUnselected( 0 );
      XX_ASSERT( nextNo != -1 );
      setCursorLine( nextNo, true );
   }

   QString cleanname = XxUtil::removeClearCaseExt( filename );

   QString f;
   bool useConditionals = false;
   bool removeEmptyConditionals = false;
   QString conditionals[3];
   if ( ask == true ) {
      if ( !allSelected ) {
         f = XxMarkersFileDialog::getSaveFileName(
            cleanname, QString(),
            _nbFiles == 3,
            useConditionals,
            removeEmptyConditionals,
            conditionals,
            _mainWindow
         );
         if ( f.isEmpty() ) {
            // The user cancelled the dialog.
            return false;
         }
      }
      else {
         f = QkFileDialog::getSaveFileName(
            _mainWindow, QString(), cleanname
         );
         if ( f.isEmpty() ) {
            // The user cancelled the dialog.
            return false;
         }
      }
   }
   else {
      f = cleanname;

      if ( !allSelected ) {
         bool res = XxMarkersDialog::getMarkers(
            _nbFiles == 3,
            useConditionals,
            removeEmptyConditionals,
            conditionals,
            noCancel,
            _mainWindow
         );
         if ( !res ) {
            // The user cancelled the dialog.
            return false;
         }
      }
   }
   XX_ASSERT( !f.isEmpty() );

   if ( !overwrite ) {
      if ( ! askOverwrite( f ) ) {
         return false;
      }
   }

   // Open a file.
   try {
      QFile outfile( f );
      bool succ = outfile.open( QIODevice::Truncate | QIODevice::WriteOnly );
      if ( !succ ) {
         throw XxIoError( XX_EXC_PARAMS, "Error opening output file." );
      }

      // Save to the file.
      {
         QTextStream osstream( &outfile );
         //osstream.setEncoding( QTextStream::Locale ); not necessary
         _diffs->save( getResources(), osstream, getBuffers(),
                       useConditionals, removeEmptyConditionals,
                       conditionals );
      }

      outfile.close();
      if ( outfile.error() != QFile::NoError ) {
         throw XxIoError( XX_EXC_PARAMS, "Error closing output file." );
      }
   }
   catch ( const XxIoError& ioerr ) {
      QMessageBox::critical(
         _mainWindow, "xxdiff", ioerr.getMsg()
      );
   }

   _diffs->clearDirty();
   return true;
}

//------------------------------------------------------------------------------
//
void XxApp::editFile( const QString& filename )
{
   if ( _diffs.get() == 0 ) {
      return;
   }

   QString command = _resources->getCommand( CMD_EDIT );
   if ( command.isEmpty() ) {
      return;
   }

#ifndef WINDOWS

   //
   // Open socket and create notifier if not yet done.  We're using a socket
   // notifier for synchronization reasons.
   //
   if ( _socketNotifier == 0 ) {

#ifdef FILE_NAMESPACE_SOCKET
      /* Create the socket.   */
      _sockfd = ::socket( PF_UNIX, SOCK_DGRAM, 0 );
      if ( _sockfd < 0 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }

      /* set socket reuse. */
      int reuse = 1;
      ::setsockopt( _sockfd, SOL_SOCKET, SO_REUSEADDR,
                    (const char*)&reuse, sizeof(int) );

      struct sockaddr_un name;
      size_t size;
      /* Bind a name to the socket.   */
      name.sun_family = AF_FILE;
      ::strncpy( name.sun_path, "/tmp/xxdiff_socket", sizeof(name.sun_path) );

      /* The size of the address is
         the offset of the start of the filename,
         plus its length,
         plus one for the terminating null byte.  */
      size = ( offsetof( struct sockaddr_un, sun_path )
               + strlen( name.sun_path ) + 1 );

      if ( ::bind( _sockfd, (struct sockaddr*)&name, size ) < 0 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }
#else
      /* Create the socket.   */
      _sockfd = ::socket( PF_INET, SOCK_STREAM, 0 );
      if ( _sockfd < 0 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }

      /* set socket reuse. */
      /* Note: the const char* cast is necessary for SUNWspro. */
      int reuse = 1;
      ::setsockopt( _sockfd, SOL_SOCKET, SO_REUSEADDR,
                    (const char *)&reuse, sizeof(int) );

      /* Give the socket a name.  */
      struct sockaddr_in name;
      name.sin_family = AF_INET;
      name.sin_port = 0; // unique port, otherwise: htons( port );
      name.sin_addr.s_addr = htonl( INADDR_ANY );
      if ( ::bind( _sockfd, (struct sockaddr*)&name, sizeof(name) ) < 0 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }
#endif

      // connect to host
      ::connect( _sockfd, (struct sockaddr*)&name, sizeof(name) );

      // set non-blocking
      ::fcntl( _sockfd, F_SETFL, O_NONBLOCK );

      // Create a new socket notifier.
      _socketNotifier =
         new QSocketNotifier( _sockfd, QSocketNotifier::Read, _mainWindow );
      QObject::connect(
         _socketNotifier, SIGNAL(activated(int)),
         this, SLOT(editDone())
      );
   }

   QStringList filenames;
   filenames.append( filename );
   const char** args;
   XxUtil::splitArgs( command, filenames, args );

   try {
      XxUtil::spawnCommand( args, handlerSIGCHLD );
   }
   catch ( const XxIoError& ioerr ) {
      QString text;
      {
         QTextStream oss( &text );
         oss << "There has been an error spawning the editor:"
             << ioerr.getMsg() << endl;
      }
      QMessageBox* box = new XxSuicideMessageBox(
         _mainWindow, "Error.", text, QMessageBox::Warning
      );
      box->show();
   }

   XxUtil::freeArgs( args );
#endif /* !WINDOWS */
}

//------------------------------------------------------------------------------
//
void XxApp::editDone()
{
   // Read all the garbage from the socket.
   char buf[1024];
   while ( read( _sockfd, buf, sizeof(char) ) > 0 ) {
   }

   delete _socketNotifier;
   _socketNotifier = 0;
   ::close( _sockfd );

   redoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::openFile( const XxFno no )
{
   // Read in the file names.
   QString filenames[3];
   QString displayFilenames[3];
   bool isTemporary[3] = { false, false, false };
   for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
      filenames[ii] = _files[ii]->getName();
      displayFilenames[ii] = _files[ii]->getDisplayName();
      isTemporary[ii] = _files[ii]->isTemporary();
   }

   QString startWith;
   XX_ASSERT( _files[no].get() != 0 );
   if ( !isTemporary[no] ) {
      startWith = filenames[no];
   }
   QString f =
      QkFileDialog::getOpenFileName( _mainWindow, QString(), startWith );
   if ( f.isEmpty() ) {
      // The user cancelled the dialog.
      return;
   }
   else {
      bool isDirectory;
      XxUtil::testFile( f, false, isDirectory );
      if ( isDirectory == true ) {
         QString text( "Cannot open a new directory" );
         QMessageBox* box = new XxSuicideMessageBox(
            _mainWindow, "Error.", text, QMessageBox::Warning
         );
         box->show();
         return;
      }

      QFileInfo fileInfo( f );
      std::auto_ptr<XxBuffer> newbuf(
         readFile( no, f, f, fileInfo, false )
      );
      if ( newbuf.get() != 0 ) {
         _files[no] = newbuf;

         bool succ = processDiff();

         // Adjust UI elements
         // Set filename label.
         if ( _files[no].get() != 0 ) {
            _central->setFilename( no, _files[no]->getDisplayName() );
         }

         // Initialize the horizontal diffs if necessary.
         if ( succ == true ) {
            if ( !_filesAreDirectories &&
                 _resources->getBoolOpt( BOOL_IGNORE_PERHUNK_WS ) ) {
               _diffs->computeIgnoreDisplay( _nbFiles, getBuffers() );
            }
            _diffs->initializeHorizontalDiffs( *_resources, getBuffers() );
         }

         // Reset the cursor line.
         setCursorLine( 1 );
         updateWidgets();
      }
      else {
         QString str;
         QTextStream oss( &str );
         oss << "Error: could not open file." << endl;
         outputDiffErrors( str );
      }
   }
}

//------------------------------------------------------------------------------
//
int XxApp::getReturnValue() const
{
   return _returnValue;
}

//------------------------------------------------------------------------------
//
void XxApp::selectAndNext( XxLine::Selection selection )
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectRegion( getCursorLine(), selection );
      XxDln nextNo = _diffs->findNextUnselected( _cursorLine );
      if ( nextNo == -1 ) {
         // Otherwise look from beginning of file.
         nextNo = _diffs->findNextUnselected( 1 );
      }
      if ( nextNo != -1 ) {
         setCursorLine( nextNo, true );
         return;
      }
      // Otherwise we're done, ask user to save file.
      saveAs();
   }
}

//------------------------------------------------------------------------------
//
void XxApp::onRedoDiff()
{
   XxFno nbFiles = getNbFiles();
   if ( nbFiles > 0 ) {
      XxDln cursorLine = getCursorLine();
      XxDln centerLine = _central->getCenterLine();

      bool succ = false; // Do nothing.

      // Make backup in case we need to recover.
      XxBuffer* backup[3];
      for ( int iii = 0; iii < 3; ++iii ) {
         backup[iii] = _files[iii].get();
      }

      try {
         // Reread the files.
         if ( _cmdline._unmerge == false && _cmdline._single == false ) {
            for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
               const XxBuffer& buffer = *(_files[ii].get());
               if ( buffer.isTemporary() == false ) {
                  // We need to get the fileinfo again here because the size
                  // might have changed.
                  QFileInfo fileInfo( buffer.getName() );

                  std::auto_ptr<XxBuffer> newbuf(
                     readFile( ii,
                               buffer.getName(),
                               buffer.getDisplayName(),
                               fileInfo,
                               buffer.isTemporary() )
                  );
                  _files[ii] = newbuf;
               }
            }

            // Do the diff.
            succ = processDiff();
         }
         else if ( _cmdline._single == true ) {

            // Read the one single file.
            std::auto_ptr<XxBuffer> newbuf(
               readFile( 0,
                         _files[0]->getName(),
                         _files[0]->getDisplayName(),
                         _files[0]->getFileInfo(),
                         _files[0]->isTemporary() )
            );
            _files[0] = newbuf;

            // Make an empty buffer.
            std::auto_ptr<XxBuffer> emptybuf(
               new XxBuffer( false, QString("/dev/null"), QString("(empty)") )
            );
            _files[1] = emptybuf;

         }
         else { // unmerge
            const XxBuffer& buffer1 = *(_files[0].get());
            if ( buffer1.isTemporary() == false ) {
               std::auto_ptr<XxBuffer> newbuf(
                  readFile( 0,
                            _files[0]->getName(),
                            _files[0]->getDisplayName(),
                            // FIXME not sure if we should redo fileinfo here...
                            _files[0]->getFileInfo(),
                            _files[0]->isTemporary() )
               );
               _files[0] = newbuf;

               // Make a proxy for the other buffer.
               std::auto_ptr<XxBuffer> newbuf2(
                  new XxBuffer( (*_files[0]),
                                _files[0]->getName(),
                                _files[0]->getDisplayName(),
                                _files[0]->getFileInfo() )
               );
               _files[1] = newbuf2;

               if ( _cmdline._unmergeNbFiles == 3 ) {
                  // Make a proxy for the other buffer.
                  std::auto_ptr<XxBuffer> newbuf2(
                     new XxBuffer( (*_files[0]),
                                   _files[0]->getName(),
                                   _files[0]->getDisplayName(),
                                   _files[0]->getFileInfo() )
                  );
                  _files[2] = newbuf2;
               }

               // Do the diff.
               succ = processDiff();
            }
         }

         // Initialize the horizontal diffs if necessary.
         if ( succ == true ) {
            if ( !_filesAreDirectories &&
                 _resources->getBoolOpt( BOOL_IGNORE_PERHUNK_WS ) ) {
               _diffs->computeIgnoreDisplay( _nbFiles, getBuffers() );
            }
            _diffs->initializeHorizontalDiffs( *_resources, getBuffers() );
         }

         // Try to set the same lines as it used to have before the redo.
         // Reset the cursor line.
         setCursorLine( cursorLine );
         _central->setCenterLine( centerLine );
         updateWidgets();
      }
      catch ( const std::exception& ex ) {
         outputDiffErrors( ex.what() );

         // Recover from backup.
         for ( int iii = 0; iii < 3; ++iii ) {
            _files[iii].reset( backup[iii] );
         }

         return;
      }
   }
}

/*==============================================================================
 * MENU COMMANDS
 *============================================================================*/

//------------------------------------------------------------------------------
//
void XxApp::openLeft()
{
   if ( _nbFiles == 0 ) {
      return;
   }
   openFile( 0 );
}

//------------------------------------------------------------------------------
//
void XxApp::openMiddle()
{
   if ( _nbFiles == 0 ) {
      return;
   }
   openFile( 1 );
}

//------------------------------------------------------------------------------
//
void XxApp::openRight()
{
   if ( _nbFiles == 0 ) {
      return;
   }
   openFile( _nbFiles == 2 ? 1 : 2 );
}

//------------------------------------------------------------------------------
//
void XxApp::saveAsLeft()
{
   XxBuffer* file = getBuffer( 0 );
   if ( file != 0 && file->isTemporary() == false ) {
      if ( validateNeedToSave( 0 ) == true ) {
         saveMergedToFile( file->getName(), false );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::saveAsMiddle()
{
   XxBuffer* file = getBuffer( 1 );
   if ( file != 0 && file->isTemporary() == false ) {
      if ( validateNeedToSave( 1 ) == true ) {
         saveMergedToFile( file->getName(), false );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::saveAsRight()
{
   XxBuffer* file = getBuffer( _nbFiles == 2 ? 1 : 2 );
   if ( file != 0 && file->isTemporary() == false ) {
      if ( validateNeedToSave( _nbFiles == 2 ? 1 : 2 ) == true ) {
         saveMergedToFile( file->getName(), false );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::saveAsMerged()
{
   QString mergedName = getMergedFilename();

   // Note: overwrite automatically if merged filename as on the cmdline.
   saveMergedToFile( mergedName,
               false,
               false /*default*/,
               ! _cmdline._mergedFilename.isEmpty() );
}

//------------------------------------------------------------------------------
//
void XxApp::saveAs()
{
   QString mergedName = getMergedFilename();
   saveMergedToFile( mergedName, true );
}

//------------------------------------------------------------------------------
//
void XxApp::generatePatchFromLeft()
{
   XxBuffer* buf = getBuffer( 0 );
   if ( buf != 0 ) {

      // Save the merged file.
      //
      // Note: we don't really need to have the temporary file opened here, we
      // just need its filename.
      char temporaryFilename1[64] = "/var/tmp/xxdiff-tmp-patch.XXXXXX";
      FILE* fout1 = XxUtil::tempfile( temporaryFilename1 );
      saveMergedToFile( temporaryFilename1, false, false, true );
      ::fclose( fout1 );

      // Save the original file.
      char temporaryFilename2[64] = "/var/tmp/xxdiff-tmp-patch.XXXXXX";
      FILE* fout2 = XxUtil::tempfile( temporaryFilename2 );
      uint sz;
      const char* charbuf = buf->getBuffer( sz );
      {
         QTextStream osstream( fout2 );
         osstream << QString::fromLocal8Bit( charbuf, sz );
      }
      ::fclose( fout2 );

      // Run diff on the two temporary files.
      QStringList filenames;
      filenames.append( temporaryFilename1 );
      filenames.append( temporaryFilename2 );
      const char** out_args;
      XxUtil::splitArgs( "diff -Naur", filenames, out_args );

      FILE* fout;
      FILE* ferr;
      int pid = XxUtil::spawnCommand( out_args, &fout, &ferr );
      if ( fout == 0 || ferr == 0 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }

// FIXME: TODO read the output and write it to some file.

      ::fclose( fout );
      ::fclose( ferr );

      XxUtil::freeArgs( out_args );

      if ( pid >= 0 ) {
         int status;
         ::waitpid( pid, &status, 0 );
      }

      // Delete the temporary files.
      XxUtil::removeFile( temporaryFilename1 );
      XxUtil::removeFile( temporaryFilename2 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::generatePatchFromMiddle()
{
// FIXME: todo !!!!!!!!!!!!!!!!!!!!!!!!!!


}

//------------------------------------------------------------------------------
//
void XxApp::generatePatchFromRight()
{
// FIXME: todo !!!!!!!!!!!!!!!!!!!!!!!!!!


}

//------------------------------------------------------------------------------
//
bool XxApp::validateNeedToSave( uint no ) const
{
   if ( _diffs.get() == 0 ) {
      return false;
   }

   // We always need to save an unmerged file.
   if ( _cmdline._unmerge == true ) {
      return true;
   }

   if ( _diffs->checkSelections( XxLine::Selection(no) ) == true ) {
      // Pop a dialog.
      QString text;
      {
         QTextStream oss( &text );
         oss << "The selections are all on this file." << endl
             << "Save anyway?";
      }

      QMessageBox::StandardButton resp = QMessageBox::warning(
         _mainWindow, "xxdiff", text,
         QMessageBox::Ok | QMessageBox::Cancel
      );
      if ( resp == QMessageBox::Cancel ) {
         // User has canceled.
         return false;
      }
      // Continue anyway.
   }
   return true;
}

//------------------------------------------------------------------------------
//
QString XxApp::getMergedFilename() const
{
   QString name;
   if ( !_cmdline._mergedFilename.isEmpty() ) {
      name = _cmdline._mergedFilename;
   }
   else {
      name = _resources->getMergedFilename();
   }

   QString left, middle, right;
   XxBuffer* leftbuf = getBuffer( 0 );
   if ( leftbuf ) {
      left = leftbuf->getName();
   }
   if ( _nbFiles == 2 ) {
      XxBuffer* rightbuf = getBuffer( 1 );
      if ( rightbuf ) {
         right = rightbuf->getName();
      }
   }
   else {
      XxBuffer* middlebuf = getBuffer( 1 );
      if ( middlebuf ) {
         middle = middlebuf->getName();
      }
      XxBuffer* rightbuf = getBuffer( 2 );
      if ( rightbuf ) {
         right = rightbuf->getName();
      }
   }

   name.replace( QRegExp("%L"), left );
   name.replace( QRegExp("%M"), middle );
   name.replace( QRegExp("%R"), right );

   return name;
}

//------------------------------------------------------------------------------
//
void XxApp::saveSelectedOnly()
{
   if ( _diffs.get() == 0 ) {
      return;
   }

   QString f = QkFileDialog::getSaveFileName(
      _mainWindow
   );
   if ( f.isEmpty() ) {
      // The user cancelled the dialog.
      return;
   }

   // Check for file existence.
   if ( ! askOverwrite( f ) ) {
      return;
   }

   QFile outfile( f );
   bool succ = outfile.open( QIODevice::Truncate | QIODevice::WriteOnly );
   if ( !succ ) {
      throw XxIoError( XX_EXC_PARAMS, "Error opening output file." );
   }

   // Save to the file.
   {
      QTextStream osstream( &outfile );
      //osstream.setEncoding( QTextStream::Locale ); not necessary
      _diffs->saveSelectedOnly( osstream, getBuffers() );
   }

   outfile.close();
   if ( outfile.error() != QFile::NoError ) {
      throw XxIoError( XX_EXC_PARAMS, "Error opening output file." );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::editLeft()
{
   XxBuffer* file = getBuffer( 0 );
   if ( file != 0 ) {
      editFile( file->getName() );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::editMiddle()
{
   XxBuffer* file = getBuffer( 1 );
   if ( file != 0 ) {
      editFile( file->getName() );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::editRight()
{
   XxBuffer* file = getBuffer( _nbFiles == 2 ? 1 : 2 );
   if ( file != 0 ) {
      editFile( file->getName() );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::saveOptions()
{
   // Generate it into a string to check if there are some contents.
   QString outstr;
   {
      // Save to the stream the differences with the default resources.
      QTextStream outs( &outstr, QIODevice::WriteOnly );
      std::auto_ptr<XxResources> defres(
         new XxResources( _cmdline._originalXdiff )
      );
      XxResParser::genInitFile( *_resources, *defres, outs );
   }
   if ( outstr.isEmpty() ) {
      QMessageBox::warning(
         _mainWindow, "xxdiff",
         "There is nothing to write as init file\n(nothing has been changed).",
         QMessageBox::Ok
      );
      return;
   }
   // Ok, there is something to write, get filename and save to file.

   QString f;
   f = QkFileDialog::getSaveFileName(
      _mainWindow, QString(), QString( getenv("HOME") ) + QString( "/.xxdiffrc" ) 
   );
   if ( f.isEmpty() ) {
      // The user cancelled the dialog.
      return;
   }
   XX_ASSERT( !f.isEmpty() );

   if ( ! askOverwrite( f ) ) {
      return;
   }

   // Open a file.
   try {
      QFile outfile( f );
      bool succ = outfile.open( QIODevice::Truncate | QIODevice::WriteOnly );
      if ( !succ ) {
         throw XxIoError( XX_EXC_PARAMS, "Error opening output file." );
      }

      // Save to the file.
      {
         QTextStream osstream( &outfile );
         osstream << outstr;
      }

      outfile.close();
      if ( outfile.error() != QFile::NoError ) {
         throw XxIoError( XX_EXC_PARAMS, "Error closing output file." );
      }
   }
   catch ( const XxIoError& ioerr ) {
      QMessageBox::critical(
         _mainWindow, "xxdiff", ioerr.getMsg()
      );
   }
}

//------------------------------------------------------------------------------
//
int XxApp::exec()
{
   if ( _dontShow ) {
      // If we asked for a decision, at least output something.
      // FIXME: maybe this should become NODIFF?
      if ( _cmdline._forceDecision == true ) {
         std::cout << "NODECISION" << std::endl;
      }

      // Exit program.
      return _returnValue;
   }
   return QkApplication::exec();
}

//------------------------------------------------------------------------------
//
void XxApp::quit()
{
   exit( _returnValue );
}

//------------------------------------------------------------------------------
//
void XxApp::exit( int retcode, const char* decisionString )
{
   static bool exited = false;
   if ( exited == false ) {
      exited = true;
   }
   else {
      // Avoid lastWindowClosed() to be called again and again.
      return;
   }

   // If an output file is request upon exiting, do it here.
   // This can only be requested from the cmdline.
   if ( _cmdline._forceDecision == true ) {
      if ( decisionString != 0 ) {
         std::cout << decisionString << std::endl;
      }
      else {
         std::cout << "NODECISION" << std::endl;
      }
   }
   else {
      // If there are some selected regions, confirm with user (optional).
      if ( _diffs.get() != 0 ) {
         if ( _resources->getBoolOpt( BOOL_WARN_ABOUT_UNSAVED ) == true &&
              _diffs->isDirty() == true &&
              _diffs->isSomeSelected() == true ) {
            QMessageBox::StandardButton resp = QMessageBox::warning(
               _mainWindow,
               "xxdiff",
               "Some selections changed and unsaved, quit anyway?",
               QMessageBox::Yes | QMessageBox::No
            );
            if ( resp == QMessageBox::No ) {
               // User has canceled.
               exited = false;
               return;
            }
            // Continue anyway.
         }
      }
   }

   if ( _resources->getBoolOpt( BOOL_EXIT_WITH_MERGE_STATUS ) == true) {
     // Determine Merge Status
      //
     // If NOT merges were selected, OR NOT all merge selection saved,
     // merge is considered incomplete.
     if ((_diffs->isAllSelected() == false) ||
         ((_diffs.get() != 0) &&
          ((_diffs->isDirty() == true) &&
           (_diffs->isSomeSelected() == true)))) {
         _returnValue = 3; // diff uses 1,2 so use 3 to be 'diff'erent!
     }
     else {
         _returnValue = 0;
     }
   }

   QkApplication::exit( retcode );
}

//------------------------------------------------------------------------------
//
void XxApp::search()
{
   if ( _diffs.get() != 0 ) {
      // Create search dialog if non-existent.
      if ( _searchDialog == 0 ) {
         _searchDialog = new XxSearchDialog( this, _mainWindow );
      }
      _searchDialog->show();
   }
}

//------------------------------------------------------------------------------
//
void XxApp::searchForward()
{
   if ( _diffs.get() != 0 ) {
      XxDiffs::SearchResult sres =
         _diffs->findNextSearch( getCursorLine() );
      if ( sres.isValid() ) {
	 setCursorLine( sres._lineNo, true );
      }
      else {
	 setCursorLine( _diffs->getNbLines(), true );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::searchBackward()
{
   if ( _diffs.get() != 0 ) {
      XxDiffs::SearchResult sres =
         _diffs->findPreviousSearch( getCursorLine() );
      if ( sres.isValid() ) {
         setCursorLine( sres._lineNo, true );
      }
      else {
         setCursorLine( 1, true );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::redoDiff()
{
   if ( _nbFiles == 0 || _diffs.get() == 0 ) {
      return;
   }

   onRedoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::editDiffOptions()
{
   // Popup name and arguments box.
   if ( _optionsDialog == 0 ) {
      _optionsDialog = new XxOptionsDialog( this, _mainWindow );
   }
   _optionsDialog->selectDiffOptions();
   _optionsDialog->show();
}

//------------------------------------------------------------------------------
//
void XxApp::editDisplayOptions()
{
   // Popup name and arguments box.
   if ( _optionsDialog == 0 ) {
      _optionsDialog = new XxOptionsDialog( this, _mainWindow );
   }
   _optionsDialog->selectDisplayOptions();
   _optionsDialog->show();
}

//------------------------------------------------------------------------------
//
void XxApp::diffFilesAtCursor()
{
   XX_ASSERT( _filesAreDirectories == true );
   if ( _nbFiles != 2 || _diffs.get() == 0 ) {
      return;
   }

   // Get filenames.
   QStringList filenames;
   const QString * titles[3] = { NULL, NULL, NULL };

   XxDln cursorLine = getCursorLine();
   const XxLine& line = _diffs->getLine( cursorLine );
   if ( line.getType() == XxLine::SAME ||
        line.getType() == XxLine::DIFF_ALL ||
        line.getType() == XxLine::DIRECTORIES ) {

      for ( XxFno ii = 0; ii < 2; ++ii ) {
         bool empty;
         XxFln fline = _diffs->getBufferLine( ii, cursorLine, empty );
         XX_ASSERT( empty == false );

         filenames.append( _files[ii]->getBufferAtLine( fline ) );

         // Add the title there if we have it.
         if ( 0 > _cmdline._userFilenames[ii].indexOf('%') ) {
             QString * tmpTitle = new QString();
             tmpTitle->sprintf( "--title%d=%s",
                                ii+1,
                                _cmdline._userFilenames[ii].toLatin1().constData() );
             titles[ii] = tmpTitle;
         }
      }

   }
   else if ( line.getType() == XxLine::INSERT_1 ) {

      bool empty;
      XxFln fline = _diffs->getBufferLine( 0, cursorLine, empty );
      XX_ASSERT( empty == false );

      filenames.append( _files[0]->getBufferAtLine( fline ) );
   }
   else if ( line.getType() == XxLine::INSERT_2 ) {

      bool empty;
      XxFln fline = _diffs->getBufferLine( 1, cursorLine, empty );
      XX_ASSERT( empty == false );

      filenames.append( _files[1]->getBufferAtLine( fline ) );
   }

   if ( filenames.count() > 0 ) {
      // Spawn a diff.
      QString command = argv()[0];

      if ( filenames.count() == 1 ) {
         command += QString(" --single ");
         // FIXME: it would be nice here to be able to say that the file should
         // display on the right side if this is an insert hunk in a directory
         // diff.

         // If the file is a directory, do nothing.
         QFileInfo finfo( filenames[0] );
         if ( finfo.isDir() ) {
            return;
         }
      }

      const char** args;
      XxUtil::splitArgs( command, titles, filenames, args );

      try {
         XxUtil::spawnCommand( args );
      }
      catch ( const XxIoError& ioerr ) {
         QString text;
         {
            QTextStream oss( &text );
            oss << "There has been an error spawning diff program:"
                << ioerr.getMsg() << endl;
         }
         QMessageBox* box = new XxSuicideMessageBox(
            _mainWindow, "Error.", text, QMessageBox::Warning
         );
         box->show();
      }
      for ( XxFno ii = 0; ii < 2; ++ii)
         delete titles[ii];

      XxUtil::freeArgs( args );
   }

}

//------------------------------------------------------------------------------
//
void XxApp::nextAndDiffFiles()
{
   if ( _diffs.get() != 0 ) {
      XxDln nextNo = _diffs->findNextNonSameLine( _cursorLine );
      if ( nextNo != -1 ) {
         setCursorLine( nextNo, true );
         diffFilesAtCursor();
      }
      else {
         setCursorLine( _diffs->getNbLines(), true );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::copyFileLeftToRight()
{
   copyFile( 0 );
}

//------------------------------------------------------------------------------
//
void XxApp::copyFileRightToLeft()
{
   copyFile( 1 );
}

//------------------------------------------------------------------------------
//
void XxApp::copyFile( XxFno nnno ) const
{
   XX_ASSERT( _filesAreDirectories == true );
   if ( _nbFiles != 2 || _diffs.get() == 0 ) {
      return;
   }

   const XxFno nosrc = nnno;
   const XxFno nodst = ( nnno + 1 ) % 2;

   // First check if the file on the right exists, both by diff contents and by
   // stat'ing file.
   XxDln cursorLine = getCursorLine();
   const XxLine& line = _diffs->getLine( cursorLine );

   // Get filenames.
   XxFln fline = line.getLineNo( nosrc );
   if ( fline == -1 ) {
      QMessageBox* box = new XxSuicideMessageBox(
         _mainWindow, "Error.", "File is empty.", QMessageBox::Warning
      );
      box->show();
      return;
   }
   QString filesrc = _files[nosrc]->getBufferAtLine( fline );

   // If the destination file is empty, the filename should be the
   // destination directory with the source filename appended.
   fline = line.getLineNo( nodst );
   QString filedst;
   if ( fline == -1 ) {
      filedst = _files[nodst]->getName();

      // Append source filename with root of source directory removed
      filedst.append( "/" ).append( filesrc.mid( _files[nosrc]->getName().length() ) );
   }
   else {
      filedst = _files[nodst]->getBufferAtLine( fline );

      if ( ! askOverwrite( filedst ) ) {
         return;
      }
   }

   // Copy file.
   if ( XxUtil::copyFile( filesrc, filedst ) != 0 ) {
      QMessageBox* box = new XxSuicideMessageBox(
         _mainWindow, "Error.", "Error copying file.", QMessageBox::Warning
      );
      box->show();
      return;
   }
}

//------------------------------------------------------------------------------
//
void XxApp::removeFileLeft()
{
   removeFile( 0 );
}

//------------------------------------------------------------------------------
//
void XxApp::removeFileRight()
{
   removeFile( 1 );
}

//------------------------------------------------------------------------------
//
void XxApp::removeFile( XxFno nnno ) const
{
   XX_ASSERT( _filesAreDirectories == true );
   if ( _nbFiles != 2 || _diffs.get() == 0 ) {
      return;
   }

   const XxFno nosrc = nnno;

   // First check if the file on the right exists, both by diff contents and by
   // stat'ing file.
   XxDln cursorLine = getCursorLine();
   const XxLine& line = _diffs->getLine( cursorLine );

   // Get filenames.
   XxFln fline = line.getLineNo( nosrc );
   if ( fline == -1 ) {
      QMessageBox* box = new XxSuicideMessageBox(
         _mainWindow, "Error.", "File is empty.", QMessageBox::Warning
      );
      box->show();
      return;
   }
   QString filesrc = _files[nosrc]->getBufferAtLine( fline );

   QMessageBox::StandardButton resp = QMessageBox::warning(
      _mainWindow, "xxdiff", "Delete file... are you sure?",
      QMessageBox::Yes | QMessageBox::No
   );
   if ( resp == QMessageBox::No ) {
      // User has canceled.
      return;
   }
   // Continue anyway.

   // Remove file.
   if ( XxUtil::removeFile( filesrc ) != 0 ) {
      QMessageBox* box = new XxSuicideMessageBox(
         _mainWindow, "Error.", "Error deleting file.", QMessageBox::Warning
      );
      box->show();
      return;
   }
}

//------------------------------------------------------------------------------
//
void XxApp::nextDifference()
{
   if ( _diffs.get() != 0 ) {
      XxDln nextNo = _diffs->findNextDifference( _cursorLine );
      if ( nextNo != -1 ) {
         setCursorLine( nextNo, true );
      }
      else {
         setCursorLine( _diffs->getNbLines(), true );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::previousDifference()
{
   if ( _diffs.get() != 0 ) {
      XxDln nextNo = _diffs->findPreviousDifference( _cursorLine );
      if ( nextNo != -1 ) {
         setCursorLine( nextNo, true );
      }
      else {
         setCursorLine( 1, true );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::nextUnselected()
{
   if ( _diffs.get() != 0 ) {
      XxDln nextNo = _diffs->findNextUnselected( _cursorLine );
      if ( nextNo != -1 ) {
         setCursorLine( nextNo, true );
      }
      else {
         setCursorLine( _diffs->getNbLines(), true );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::previousUnselected()
{
   if ( _diffs.get() != 0 ) {
      XxDln nextNo = _diffs->findPreviousUnselected( _cursorLine );
      if ( nextNo != -1 ) {
         setCursorLine( nextNo, true );
      }
      else {
         setCursorLine( 1, true );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectGlobalLeft()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectGlobal( XxLine::SEL1 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectGlobalMiddle()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectGlobal( XxLine::SEL2 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectGlobalRight()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectGlobal( _nbFiles == 2 ? XxLine::SEL2 : XxLine::SEL3 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectGlobalNeither()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectGlobal( XxLine::NEITHER );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectGlobalUnselect()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectGlobal( XxLine::UNSELECTED );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectGlobalUnselectedLeft()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectGlobalUnselected( XxLine::SEL1 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectGlobalUnselectedMiddle()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectGlobalUnselected( XxLine::SEL2 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectGlobalUnselectedRight()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectGlobalUnselected(
         _nbFiles == 2 ? XxLine::SEL2 : XxLine::SEL3
      );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectGlobalUnselectedNeither()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectGlobalUnselected( XxLine::NEITHER );
   }
}

//------------------------------------------------------------------------------
//
int XxApp::selectGlobalMerge()
{
   if ( _diffs.get() != 0 ) {
      return _diffs->merge( _nbFiles );
   }
   return 2;
}

//------------------------------------------------------------------------------
//
void XxApp::selectRegionLeft()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectRegion( getCursorLine(), XxLine::SEL1 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectRegionMiddle()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectRegion( getCursorLine(), XxLine::SEL2 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectRegionRight()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectRegion( getCursorLine(),
                            _nbFiles == 2 ? XxLine::SEL2 : XxLine::SEL3  );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectRegionNeither()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectRegion( getCursorLine(), XxLine::NEITHER );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectRegionUnselect()
{
   if ( _diffs.get() != 0 ) {
      _diffs->selectRegion( getCursorLine(), XxLine::UNSELECTED );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectRegionLeftAndNext()
{
   selectAndNext( XxLine::SEL1 );
}

//------------------------------------------------------------------------------
//
void XxApp::selectRegionMiddleAndNext()
{
   selectAndNext( XxLine::SEL2 );
}

//------------------------------------------------------------------------------
//
void XxApp::selectRegionRightAndNext()
{
   selectAndNext( _nbFiles == 2 ? XxLine::SEL2 : XxLine::SEL3 );
}

//------------------------------------------------------------------------------
//
void XxApp::selectRegionNeitherAndNext()
{
   selectAndNext( XxLine::NEITHER );
}

//------------------------------------------------------------------------------
//
void XxApp::regionSplitSwapJoin()
{
   if ( _diffs.get() != 0 ) {
      bool joined = _diffs->splitSwapJoin( getCursorLine(), getNbFiles() );

      // Redo the horizontal diffs if required.
      // Note: you could optimize and do only the required lines.
      if ( joined == true ) {
         _diffs->initializeHorizontalDiffs( *_resources, getBuffers() );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineLeft()
{
   if ( _diffs.get() != 0 ) {
      XxDln lineNo = getCursorLine();
      _diffs->selectLine( lineNo, XxLine::SEL1 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineMiddle()
{
   if ( _diffs.get() != 0 ) {
      XxDln lineNo = getCursorLine();
      _diffs->selectLine( lineNo, XxLine::SEL2 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineRight()
{
   if ( _diffs.get() != 0 ) {
      XxDln lineNo = getCursorLine();
      _diffs->selectLine( lineNo,
                          _nbFiles == 2 ? XxLine::SEL2 : XxLine::SEL3 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineNeither()
{
   if ( _diffs.get() != 0 ) {
      XxDln lineNo = getCursorLine();
      _diffs->selectLine( lineNo, XxLine::NEITHER );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineUnselect()
{
   if ( _diffs.get() != 0 ) {
      XxDln lineNo = getCursorLine();
      _diffs->selectLine( lineNo, XxLine::UNSELECTED );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::setTabWidth( const int newTabWidth )
{
   _resources->setTabWidth( newTabWidth );

   emit textSizeChanged();
   synchronizeUI();
   updateWidgets();
}

//------------------------------------------------------------------------------
//
void XxApp::tabsAt3()
{
   setTabWidth( 3 );
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::tabsAt4()
{
   setTabWidth( 4 );
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::tabsAt8()
{
   setTabWidth( 8 );
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreTrailing()
{
   setFileDiffOptions( CMDSW_FILES_IGNORE_TRAILING );
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreWhitespace()
{
   setFileDiffOptions( CMDSW_FILES_IGNORE_WHITESPACE );
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreCase()
{
   setFileDiffOptions( CMDSW_FILES_IGNORE_CASE );
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreBlankLines()
{
   setFileDiffOptions( CMDSW_FILES_IGNORE_BLANK_LINES );
}

//------------------------------------------------------------------------------
//
void XxApp::setFileDiffOptions( XxCommandSwitch option )
{
   XxCommand cmdResId = (_nbFiles == 2) ? CMD_DIFF_FILES_2 : CMD_DIFF_FILES_3;
   _resources->toggleCommandSwitch( cmdResId, option );

   synchronizeUI();
   if ( _optionsDialog != 0 ) {
      _optionsDialog->synchronize();
   }
   onRedoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::hideCarriageReturns()
{
   _resources->toggleBoolOpt( BOOL_HIDE_CR );
   synchronizeUI();
   updateWidgets();
}

//------------------------------------------------------------------------------
//
void XxApp::dirDiffRecursive()
{
   _resources->toggleBoolOpt( BOOL_DIRDIFF_RECURSIVE );
   synchronizeUI();
   onRedoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreFileChanges()
{
   _resources->toggleBoolOpt( BOOL_DIRDIFF_IGNORE_FILE_CHANGES );
   synchronizeUI();
   onRedoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::qualityNormal()
{
   setQuality( QUALITY_NORMAL );
}

//------------------------------------------------------------------------------
//
void XxApp::qualityFastest()
{
   setQuality( QUALITY_FASTEST );
}

//------------------------------------------------------------------------------
//
void XxApp::qualityHighest()
{
   setQuality( QUALITY_HIGHEST );
}

//------------------------------------------------------------------------------
//
void XxApp::setQuality( XxQuality quality )
{
   XxCommand cmdResId = (_nbFiles == 2) ? CMD_DIFF_FILES_2 : CMD_DIFF_FILES_3;
   QString cmd = _resources->getCommand( cmdResId );
   _resources->setQuality( cmd, quality );
   _resources->setCommand( cmdResId, cmd );

   synchronizeUI();
   if ( _optionsDialog != 0 ) {
      _optionsDialog->synchronize();
   }
   onRedoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleShowFilenames()
{
   _central->showFilenames( _resources->toggleShowOpt( SHOW_FILENAMES ) );
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::togglePaneMergedView()
{
   if ( _nbFiles == 0 || _diffs.get() == 0 ) {
      return;
   }

   if ( _paneMergedView->isVisible() == false ) {
      _paneMergedView->show();
   }
   else {
      _paneMergedView->hide();
   }

   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::togglePopupMergedView()
{
   if ( _nbFiles == 0 || _diffs.get() == 0 ) {
      return;
   }

   // Create popup merge view if it does not exist.
   if ( _popupMergedView == 0 ) {
      _popupMergedView = new XxMergedWindow( this, _mainWindow );
   }

   // Show it or hide it.
   if ( _popupMergedView->isVisible() == false ) {
      // Popup merged view.

      // Resize the merged window to have the same size as the text widgets.
      int mh = _mainWindow->menuBar()->height();
      _popupMergedView->resize( QSize( _central->getOneWidth(),
                                       _central->getOneHeight() + mh ) );
      _popupMergedView->show();
   }
   else {
      // Hide merged view.

      _popupMergedView->hide();
   }

   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleToolbar()
{
   _resources->toggleShowOpt( SHOW_TOOLBAR );
   synchronizeUI();

   if ( _resources->getShowOpt( SHOW_TOOLBAR ) ) {
      _toolbar->show();
   }
   else {
      _toolbar->hide();
   }
}

//------------------------------------------------------------------------------
//
void XxApp::toggleLineNumbers()
{
   _resources->toggleShowOpt( SHOW_LINE_NUMBERS );

   adjustLineNumbers();
   synchronizeUI();
   updateWidgets();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleVerticalLine()
{
   _resources->toggleShowOpt( SHOW_VERTICAL_LINE );
   updateWidgets();
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleHorizNullMarkers()
{
   _resources->toggleBoolOpt( BOOL_NULL_HORIZONTAL_MARKERS );
   updateWidgets();
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleOverview()
{
   _resources->toggleShowOpt( SHOW_OVERVIEW );

   if ( _resources->getShowOpt( SHOW_OVERVIEW ) ) {
      _overviewArea->show();
   }
   else {
      _overviewArea->hide();
   }

   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::hordiffTypeNone()
{
   _resources->setHordiffType( HD_NONE );
   hordiffTypeUpdate();
}

//------------------------------------------------------------------------------
//
void XxApp::hordiffTypeSingle()
{
   _resources->setHordiffType( HD_SINGLE );
   hordiffTypeUpdate();
}

//------------------------------------------------------------------------------
//
void XxApp::hordiffTypeMultiple()
{
   _resources->setHordiffType( HD_MULTIPLE );
   hordiffTypeUpdate();
}

//------------------------------------------------------------------------------
//
void XxApp::hordiffTypeUpdate()
{
   if ( _diffs.get() != 0 ) {
      _diffs->initializeHorizontalDiffs( *_resources, getBuffers() );
      updateWidgets();
   }
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleIgnoreHorizontalWs()
{
   _resources->toggleBoolOpt( BOOL_IGNORE_HORIZONTAL_WS );

   if ( _diffs.get() != 0 ) {
      _diffs->initializeHorizontalDiffs( *_resources, getBuffers(), true );
      updateWidgets();
   }
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleIgnorePerHunkWs()
{
   _resources->toggleBoolOpt( BOOL_IGNORE_PERHUNK_WS );

   if ( _diffs.get() != 0 ) {
      if ( !_filesAreDirectories &&
           _resources->getBoolOpt( BOOL_IGNORE_PERHUNK_WS ) ) {
         _diffs->computeIgnoreDisplay( _nbFiles, getBuffers() );
      }
      updateWidgets();
   }
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleFormatClipboardText()
{
   _resources->toggleBoolOpt( BOOL_FORMAT_CLIPBOARD_TEXT );
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreFileNone()
{
   _resources->setIgnoreFile( IGNORE_NONE );
   synchronizeUI();
   updateWidgets();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreFileLeft()
{
   _resources->setIgnoreFile( IGNORE_LEFT );
   synchronizeUI();
   updateWidgets();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreFileMiddle()
{
   _resources->setIgnoreFile( IGNORE_MIDDLE );
   synchronizeUI();
   updateWidgets();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreFileRight()
{
   _resources->setIgnoreFile( IGNORE_RIGHT );
   synchronizeUI();
   updateWidgets();
}

//------------------------------------------------------------------------------
//
void XxApp::helpManPage()
{
   QDialog* manPage = XxHelp::getManPageDialog( _mainWindow );
   if ( manPage != 0 ) {
      manPage->show();
   }
}

//------------------------------------------------------------------------------
//
void XxApp::helpAbout()
{
   QDialog* box = XxHelp::getAboutDialog( _mainWindow );
   box->show();
}

//------------------------------------------------------------------------------
//
void XxApp::synchronizeUI()
{
   if ( _filesAreDirectories == false ) {
      if ( _nbFiles == 2 &&
           _cmdline._unmerge == false &&
           _cmdline._single == false ) {
         XxCommand cmdResId =
            (_nbFiles == 2) ? CMD_DIFF_FILES_2 : CMD_DIFF_FILES_3;
         // Note: useless, this is only valid for two-way file diffs.

         _menuactions[ ID_ToggleIgnoreTrailing ]->setChecked( 
            _resources->isCommandSwitch(
               cmdResId,
               CMDSW_FILES_IGNORE_TRAILING
            )
         );
         _menuactions[ ID_ToggleIgnoreWhitespace ]->setChecked( 
            _resources->isCommandSwitch(
               cmdResId,
               CMDSW_FILES_IGNORE_WHITESPACE
            )
         );
         _menuactions[ ID_ToggleIgnoreCase ]->setChecked( 
            _resources->isCommandSwitch(
               cmdResId,
               CMDSW_FILES_IGNORE_CASE
            )
         );
         _menuactions[ ID_ToggleIgnoreBlankLines ]->setChecked( 
            _resources->isCommandSwitch(
               cmdResId,
               CMDSW_FILES_IGNORE_BLANK_LINES
            )
         );

         QString cmd = _resources->getCommand( cmdResId );
         XxQuality quality = _resources->getQuality( cmd );

         _menuactions[ ID_ToggleQualityNormal ]->setChecked( 
            quality == QUALITY_NORMAL
         );
         _menuactions[ ID_ToggleQualityFastest ]->setChecked( 
            quality == QUALITY_FASTEST
         );
         _menuactions[ ID_ToggleQualityHighest ]->setChecked( 
            quality == QUALITY_HIGHEST
         );
      }
   }
   else {
      _menuactions[ ID_ToggleDirDiffsRecursive ]->setChecked( 
         _resources->getBoolOpt( BOOL_DIRDIFF_RECURSIVE )
      );
   }

   //---------------------------------------------------------------------------

   if ( _filesAreDirectories == false ) {
      {
         _menuactions[ ID_Hordiff_None ]->setChecked(
            _resources->getHordiffType() == HD_NONE
         );
         _menuactions[ ID_Hordiff_Single ]->setChecked( 
            _resources->getHordiffType() == HD_SINGLE
         );
         _menuactions[ ID_Hordiff_Multiple ]->setChecked( 
            _resources->getHordiffType() == HD_MULTIPLE
         );
      }
      _menuactions[ ID_ToggleIgnoreHorizontalWs ]->setChecked( 
         _resources->getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS )
      );
      _menuactions[ ID_ToggleIgnorePerHunkWs ]->setChecked( 
         _resources->getBoolOpt( BOOL_IGNORE_PERHUNK_WS )
      );
      _menuactions[ ID_ToggleHideCarriageReturns ]->setChecked( 
         _resources->getBoolOpt( BOOL_HIDE_CR )
      );
      _menuactions[ ID_ToggleVerticalLine ]->setChecked( 
         _resources->getShowOpt( SHOW_VERTICAL_LINE )
      );
      _menuactions[ ID_ToggleHorizNullMarkers ]->setChecked( 
         _resources->getBoolOpt( BOOL_NULL_HORIZONTAL_MARKERS )
      );
   }
   else {
      _menuactions[ ID_ToggleDirDiffsIgnoreFileChanges ]->setChecked( 
         _resources->getBoolOpt( BOOL_DIRDIFF_IGNORE_FILE_CHANGES )
      );
   }

   _menuactions[ ID_ToggleFormatClipboardText ]->setChecked( 
      _resources->getBoolOpt( BOOL_FORMAT_CLIPBOARD_TEXT )
   );

   if ( _filesAreDirectories == false ) {

      uint tabWidth = _resources->getTabWidth();
      _menuactions[ ID_TabsAtThree ]->setChecked( tabWidth == 3 );
      _menuactions[ ID_TabsAtFour ]->setChecked( tabWidth == 4 );
      _menuactions[ ID_TabsAtEight ]->setChecked( tabWidth == 8 );
   }

   _menuactions[ ID_ToggleLineNumbers ]->setChecked( 
      _resources->getShowOpt( SHOW_LINE_NUMBERS )
   );

   if ( _filesAreDirectories == false && _nbFiles == 3 ) {

      XxIgnoreFile ignoreFile = _resources->getIgnoreFile();
      _menuactions[ ID_IgnoreFileNone ]->setChecked( ignoreFile == IGNORE_NONE );
      _menuactions[ ID_IgnoreFileLeft ]->setChecked( ignoreFile == IGNORE_LEFT );
      _menuactions[ ID_IgnoreFileMiddle ]->setChecked( ignoreFile == IGNORE_MIDDLE );
      _menuactions[ ID_IgnoreFileRight ]->setChecked( ignoreFile == IGNORE_RIGHT );
   }

   //---------------------------------------------------------------------------

   if ( _filesAreDirectories == false ) {
      _menuactions[ ID_TogglePaneMergedView ]->setChecked( 
         ( _paneMergedView != 0 && _paneMergedView->isVisible() )
      );
      _menuactions[ ID_TogglePopupMergedView ]->setChecked( 
         ( _popupMergedView != 0 && _popupMergedView->isVisible() )
      );
  }
   _menuactions[ ID_ToggleToolbar ]->setChecked( 
      _resources->getShowOpt( SHOW_TOOLBAR )
   );
   _menuactions[ ID_ToggleOverview ]->setChecked( 
      _resources->getShowOpt( SHOW_OVERVIEW )
   );
   _menuactions[ ID_ToggleShowFilenames ]->setChecked( 
      _resources->getShowOpt( SHOW_FILENAMES )
   );
}

//------------------------------------------------------------------------------
//
void XxApp::handlerSIGCHLD( int )
{
   XX_ASSERT( _socketNotifier != 0 );
   XX_TRACE( "XxApp::handlerSIGCHLD called" );

   // Wake up the socket notifier.
   // This should get caught by a select() call from the main event loop.
   static const char* buf = "garbage";
   write( _sockfd, &buf, sizeof(char) );
}

//==============================================================================
// Scroll mediator, cursor manager, etc.
//==============================================================================

//------------------------------------------------------------------------------
//
XxDln XxApp::getCursorLine() const
{
   if ( _diffs.get() == 0 ) {
      return 0;
   }
   return _cursorLine;
}

//------------------------------------------------------------------------------
//
XxDln XxApp::setCursorLine( const XxDln lineNo, const bool recenter )
{
   XxDln oldLine = _cursorLine;

   // Validate and set.
   XxDln validLine =
      std::max( XxDln(1), std::min( XxDln(getNbLines()), lineNo ) );
   _cursorLine = validLine;

   if ( _cursorLine != oldLine ) {
      emit cursorChanged( _cursorLine );
   }

   if ( recenter ) {
      _central->recenter();
   }

   return oldLine;
}

//------------------------------------------------------------------------------
//
bool XxApp::adjustCursor( const int topLine, const int bottomLine )
{
   // Insures that the cursor lies within the boundaries given in parameters.
   XxDln cursorLine = getCursorLine();
   if ( cursorLine < topLine ) {
      setCursorLine( topLine );
      return true;
   }
   else if ( cursorLine > bottomLine ) {
      setCursorLine( bottomLine );
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
//
void XxApp::pageDown()
{
   // Note: we leave 5 lines in common, and we skip pages the size of the
   // central texts.
   XxDln diff = _central->getNbDisplayLines() - 5;

   setCursorLine( getCursorLine() + diff, true );
}

//------------------------------------------------------------------------------
//
void XxApp::pageUp()
{
   // Note: we leave 5 lines in common, and we skip pages the size of the
   // central texts.
   XxDln diff = _central->getNbDisplayLines() - 5;

   setCursorLine( getCursorLine() - diff, true );
}

//------------------------------------------------------------------------------
//
void XxApp::cursorDown()
{
   setCursorLine( getCursorLine() + 1, true );
}

//------------------------------------------------------------------------------
//
void XxApp::cursorUp()
{
   setCursorLine( getCursorLine() - 1, true );
}

//------------------------------------------------------------------------------
//
void XxApp::cursorTop()
{
   setCursorLine( 1, true );
}

//------------------------------------------------------------------------------
//
void XxApp::cursorBottom()
{
   setCursorLine( getNbLines(), true );
}

//------------------------------------------------------------------------------
//
bool XxApp::computeAbsoluteDifference() const
{
   const std::auto_ptr<XxBuffer>* buffers = getBuffers();
   if ( _nbFiles == 2 ) {
      uint bs1;
      const char* b1 = buffers[0]->getBuffer( bs1 );
      uint bs2;
      const char* b2 = buffers[1]->getBuffer( bs2 );
      if ( bs1 != bs2 ) {
         return true;
      }
      return bool( ::memcmp( b1, b2, bs1 ) );
   }
   else { // _nbFiles == 3
      uint bs1;
      const char* b1 = buffers[0]->getBuffer( bs1 );
      uint bs2;
      const char* b2 = buffers[1]->getBuffer( bs2 );
      uint bs3;
      const char* b3 = buffers[2]->getBuffer( bs3 );
      if ( bs1 != bs2 || bs2 != bs3 ) {
         return true;
      }
      return bool( ::memcmp( b1, b2, bs1 ) ) || bool( ::memcmp( b1, b3, bs1 ) );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::quitAccept()
{
   // Confirm with the user if some incompatible selections were made.
   if ( _diffs->hasSelectionsOtherThan(
           _nbFiles == 2 ? XxLine::SEL2 : XxLine::SEL3 ) ) {

      int resp = QMessageBox::warning(
         _mainWindow,
         "xxdiff",
         "Some selections are incompatible with your decision, confirm action.",
         "Discard selections", "Cancel", QString(), 0, 1
      );
      if ( resp == 1 ) {
         // User has canceled.
         return;
      }
      // Discard selections.
   }

   exit( _returnValue, "ACCEPT" );
   selectGlobalRight();
   bool saved =
      saveMergedToFile( getMergedFilename(), false, !_mainWindow->isVisible(), true );
   XX_ASSERT( saved );
}

//------------------------------------------------------------------------------
//
void XxApp::quitReject()
{
   // Confirm with the user if some incompatible selections were made.
   if ( _diffs->hasSelectionsOtherThan( XxLine::SEL1 ) ) {

      int resp = QMessageBox::warning(
         _mainWindow,
         "xxdiff",
         "Some selections are incompatible with your decision, confirm action.",
         "Accept, discard selections", "Cancel", QString(), 0, 1
      );
      if ( resp == 1 ) {
         // User has canceled.
         return;
      }
      // Discard selections.
   }

   exit( _returnValue, "REJECT" );
   selectGlobalLeft();
   bool saved =
      saveMergedToFile( getMergedFilename(), false, !_mainWindow->isVisible(), true );
   XX_ASSERT( saved );
}

//------------------------------------------------------------------------------
//
void XxApp::quitMerged()
{
   // Force saving a merged file.
   QString mergedName = getMergedFilename();
   if ( ! saveMergedToFile(
           mergedName, false, !_mainWindow->isVisible(), true
        ) ) {
      return; // Don't quit! Output *IS* required.
   }

   exit( _returnValue, "MERGED" );
}

//------------------------------------------------------------------------------
//
void XxApp::fontSizeIncrease()
{
   fontSizeChange( +1 );
}

//------------------------------------------------------------------------------
//
void XxApp::fontSizeDecrease()
{
   fontSizeChange( -1 );
}

//------------------------------------------------------------------------------
//
void XxApp::fontSizeChange( int increment )
{
   //
   // Interactive font resize feature with mouse wheel.
   //

   QFont font = _resources->getFontText();
   QFontInfo finfo( font );

   // Search for a font with a different pixelsize than our currently used
   // font.
   int pixels = finfo.pixelSize();
   while ( true ) {
      pixels += increment;
      if ( pixels <= 0 || pixels > 512 ) {
         return;
      }

      font.setPixelSize( pixels );
      QFontInfo finfoNew( font );
      if ( finfoNew.pixelSize() != finfo.pixelSize() ) {
         break;
      }
   }

   _resources->setFontText( font );
   invalidateTextWidth(); // FIXME is this needed?
   updateWidgets();

}

XX_NAMESPACE_END

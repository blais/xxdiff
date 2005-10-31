/******************************************************************************\
 * $Id: app.cpp 347 2001-11-06 06:30:32Z blais $
 * $Date: 2001-11-06 01:30:32 -0500 (Tue, 06 Nov 2001) $
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
#include <diffs.h>
#include <buffer.h>
#include <resources.h>
#include <suicideMessageBox.h>
#include <optionsDialog.h>
#include <searchDialog.h>
#include <markersFileDialog.h>
#include <resParser.h>

#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qlayout.h>
#include <qscrollbar.h>
#include <qlabel.h>
#include <qsgistyle.h>
#include <qfont.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qaccel.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qclipboard.h>
#include <qsocketnotifier.h>
#include <qtooltip.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qtextstream.h>
#include <qfile.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <memory>

#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

#include <stdio.h>

#ifndef WINDOWS
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <unistd.h>
#  include <libgen.h>
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

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

enum MenuIds {
   ID_ToggleToolbar,
   ID_ToggleLineNumbers,
   ID_ToggleVerticalLine,
   ID_ToggleOverview,
   ID_ToggleShowFilenames,
   ID_ToggleHorizontalDiffs,
   ID_ToggleIgnoreHorizontalWs,
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

class XxMainWindow : public QMainWindow {

public:

   /*----- member functions -----*/

   // Constructor.
   XxMainWindow( 
      XxApp*      app,
      QWidget*    parent = 0, 
      const char* name = 0, 
      WFlags      f = WType_TopLevel 
   );


private:

   /*----- data members -----*/

   XxApp* _app;

};
 
//------------------------------------------------------------------------------
//
XxMainWindow::XxMainWindow( 
   XxApp*      app,
   QWidget*    parent,
   const char* name, 
   WFlags      f
) :
   QMainWindow( parent, name, f ),
   _app( app )
{}

/*==============================================================================
 * LOCAL CLASS XxCopyLabelTip
 *============================================================================*/

// <summary> a tooltip that only tips when the string of an associated label is
// too long.  </summary>

class XxCopyLabelTip : public QToolTip {

public:

   /*----- member functions -----*/

   // Constructor.
   XxCopyLabelTip( QLabel* parent );

   // See base class.
   virtual void maybeTip( const QPoint& );

};

/*==============================================================================
 * LOCAL CLASS XxCopyLabel
 *============================================================================*/

// <summary> a label that puts its contents into the clipboard when clicked on
// </summary>

class XxCopyLabel : public QLabel {

public:

   /*----- types and enumerations -----*/

   enum {
      SAFETY_OFFSET = 10
   };

   /*----- member functions -----*/

   // Constructor.
   XxCopyLabel( QWidget* parent );

   // Destructor.
   ~XxCopyLabel();

   // See base class.
   // <group>
   virtual QSize sizeHint() const;
   virtual void setText( const QString& );
   virtual void mousePressEvent( QMouseEvent* );
   virtual void resizeEvent( QResizeEvent* );
   // </group>

   // Returns the full text of the label.
   const QString& getFullText() const;

private:

   XxCopyLabelTip* _tip;
   QString         _fulltext;
};


/*==============================================================================
 * LOCAL CLASS XxCopyLabelTip
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxCopyLabelTip::XxCopyLabelTip( QLabel* parent ) :
   QToolTip( parent )
{
}

//------------------------------------------------------------------------------
//
void XxCopyLabelTip::maybeTip( const QPoint& )
{
   XxCopyLabel* clabel = static_cast<XxCopyLabel*>( parentWidget() );
   XX_CHECK( clabel != 0 );
   QString tex = clabel->getFullText();
   QFontMetrics fm = clabel->fontMetrics();
   QRect br = fm.boundingRect( tex );
   if ( br.width() + XxCopyLabel::SAFETY_OFFSET > clabel->size().width() ) {
      tip( clabel->rect(), tex );
   }
}

/*==============================================================================
 * LOCAL CLASS XxCopyLabel
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxCopyLabel::XxCopyLabel( QWidget* parent ) :
   QLabel( parent )
{
   setAlignment( (alignment() & !Qt::AlignLeft) | Qt::AlignCenter );
   _tip = new XxCopyLabelTip( this );
}

//------------------------------------------------------------------------------
//
XxCopyLabel::~XxCopyLabel()
{
   delete _tip;
}

//------------------------------------------------------------------------------
//
void XxCopyLabel::setText( const QString& s )
{
   _fulltext = s;
   QLabel::setText( s );
}

//------------------------------------------------------------------------------
//
const QString& XxCopyLabel::getFullText() const
{
   return _fulltext;
}

//------------------------------------------------------------------------------
//
void XxCopyLabel::mousePressEvent( QMouseEvent* event )
{
   QClipboard *cb = QApplication::clipboard();
   cb->setText( _fulltext );
   QLabel::mousePressEvent( event );
}

//------------------------------------------------------------------------------
//
QSize XxCopyLabel::sizeHint() const
{
   QSize hint = QLabel::sizeHint();
   hint.setWidth( 0 );
   return hint;
}

//------------------------------------------------------------------------------
//
void XxCopyLabel::resizeEvent( QResizeEvent* event )
{
   setText( _fulltext );
   QString tex = text();
   QFontMetrics fm = fontMetrics();
   QRect br = fm.boundingRect( tex );
   while ( br.width() + SAFETY_OFFSET > width() ) {

      // Remove beginning part
      //
      // Note: also check for '\' in case we ever port to Windoze.
      int pos = -1;
      if ( tex.find( "[...]/", 0 ) == 0 ) {
         pos = tex.find( '/', 6 );
      }
      else {
         pos = tex.find( '/', 0 );
      }

      if ( pos == -1 ) {
         break;
      }
      
      tex.replace( 0, pos + 1, "[...]/" );
      br = fm.boundingRect( tex );
   }
   
   QLabel::setText( tex );
   QLabel::resizeEvent( event );
}

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
XxApp::XxApp( int argc, char** argv, const XxCmdline& cmdline ) :
   QApplication( argc, argv ),
   _isUICreated( false ),
   _returnValue( 0 ),
   _diffErrorsMsgBox( 0 ),
   _searchDialog( 0 ),
   _optionsDialog( 0 ),
   _mergedWindow( 0 ),
   _textWidth( 0 ),
   _displayWidth( 0 ),
   _displayHeight( 0 ),
   _displayLines( 0 ),
   _cursorLine( 0 ), // disallowed value on purpose
   _filesAreDirectories( false ),
   _resources( 0 ),
   _cmdline( cmdline )
{
   _vscroll[0] = _vscroll[1] = 0;

   if ( _cmdline._forceStyle == true ) {
      _style = new QSGIStyle;
      setStyle( _style );
   }

   // Read in the resources and create resources object.
   _resources = buildResources();

   setFont( _resources->getFontApp(), true );
   
   // Read in the file names.
   QString filenames[3];
   QString displayFilenames[3];
   bool isTemporary[3] = { false, false, false };
   _nbFiles = processFileNames(
      _cmdline, filenames, displayFilenames, isTemporary, _filesAreDirectories
   );
   _nbTextWidgets = _nbFiles;
   if ( _nbFiles == 0 ) {
      throw XxUsageError( XX_EXC_PARAMS );
   }

   // Check for odd number of files.
   if ( _nbFiles != 0 && _nbFiles != 2 && _nbFiles != 3 ) {
      throw XxUsageError( XX_EXC_PARAMS );
   }

   // Add extra diff arguments.
   XX_ASSERT( _resources != 0 );
   XxCommand cmdResId;
   if ( _filesAreDirectories == false ) {
      cmdResId = _nbFiles == 2 ? 
         CMD_DIFF_FILES_2 : 
         CMD_DIFF_FILES_3;
   }
   else {
      cmdResId = CMD_DIFF_DIRECTORIES;
   }

   QString cmd = _resources->getCommand( cmdResId );
   XxOptionsDialog::addToCommand( cmd, _cmdline._extraDiffArgs );
   _resources->setCommand( cmdResId, cmd );

   // Create the interface.
   createUI( _nbTextWidgets );

   // Add some context help.
   createOnContextHelp();

   // Synchronize the created UI with the resources.
   synchronizeUI();

   if ( _nbFiles != 0 ) {
      for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
         readFile(
            ii,
            filenames[ii],
            displayFilenames[ii],
            isTemporary[ii] 
         );
      }
      bool succ = processDiff();

      // Initialize the horizontal diffs if necessary.
      if ( succ == true &&
           _resources->getBoolOpt( BOOL_HORIZONTAL_DIFFS ) == true ) {
         _diffs->initializeHorizontalDiffs( 
            _resources->getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS ), getBuffers()
         );
      }
   }

   // Resize and show/hide line numbers.
   adjustLineNumbers();

   // Sets the title bar.
   if ( _nbFiles == 2 ) {
      QString str = 
         QString(_files[0]->getDisplayName()) + " <-> " + 
         QString(_files[1]->getDisplayName());
      _mainWindow->setCaption( str );
   }
   else if ( _nbFiles == 3 ) {
      QString str = 
         QString(_files[0]->getDisplayName()) + " <-> " + 
         QString(_files[1]->getDisplayName()) + " <-> " + 
         QString(_files[2]->getDisplayName());
      _mainWindow->setCaption( str );
   }
   else {
      _mainWindow->setCaption( "xxdiff" );
   }

   // Resize the main window before showing.
   const QRect& psize = _resources->getPreferredGeometry();
   if ( !_resources->getMaximize() ) {
      _mainWindow->resize( psize.size() );
   }

   // Call post creation actions.
   if ( _cmdline._mergeRequested ) {
      selectGlobalMerge();
   }

   // Show the main window.
   if ( _resources->getMaximize() ) {
      _mainWindow->showMaximized();
      // Don't make fullscreen, rather maximized.
   }
   else {
      _mainWindow->show();

      // Note: positioning has to be done after show().  This results in some
      // flickering, but there's nothing we can do about it.
      if ( psize.topLeft() != QPoint( -1, -1 ) ) {
         _mainWindow->move( psize.topLeft() );
      }
   }

   if ( _diffErrorsMsgBox != 0 ) {
      _diffErrorsMsgBox->show();
      _diffErrorsMsgBox = 0; // forget about it, it'll get sad and suicide.
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
//
uint XxApp::processFileNames(
   const XxCmdline& cmdline,
   QString filenames[3],
   QString displayFilenames[3],
   bool    created[3],
   bool&   filesAreDirectories
)
{
   uint nbFiles = 0;

   //
   // First find if all paths are directories.  If this is the case, then this
   // is a directory diff.  If ONLY ONE path is a file, then we append the
   // basename to the other ones which are directories.
   //
   // If stdin is encountered, then directory diffs are not allowed (you can't
   // cat a directory into stdin!) so if all filenames are directories bail out.
   //
   // We aspire to do our best but only in cases where there is no ambiguity.
   //

   bool isDirectory[3] = { false, false, false };
   bool stdinUsed = false;
   uint nbPathFiles = 0;
   int filePathIndex = -1;
   for ( XxFno ii = 0; ii < cmdline._nbFilenames; ++ii ) {
      if ( cmdline._filenames[ii] == "-" ) {
         if ( stdinUsed == true ) {
            // Cannot have two filenames from stdin.
            throw XxUsageError( XX_EXC_PARAMS );
         }
         stdinUsed = true;
         isDirectory[ii] = false;
      }
      else {
         QFileInfo finfo( cmdline._filenames[ii] );
         isDirectory[ii] = finfo.isDir();
         if ( ! isDirectory[ii] ) {
            filePathIndex = ii;
            nbPathFiles++;
         }
      }
      nbFiles++;
   }

   if ( stdinUsed == true && nbPathFiles == 0 ) {
      // stdin was specified and all other paths are directories.
      throw XxUsageError( XX_EXC_PARAMS );
   }

   if ( nbPathFiles > 1 && nbFiles > ( nbPathFiles + (stdinUsed ? 1 : 0) ) ) {
      // There are more than one named file AND one of the files is a directory.
      throw XxUsageError( XX_EXC_PARAMS );
   }

   filesAreDirectories = ( nbFiles > 0 && nbPathFiles == 0 );

   // Find basename and append to directories.
   QString bn;
   if ( !filesAreDirectories && nbPathFiles < nbFiles ) {
      XX_ASSERT( filePathIndex != -1 );
      bn = XxUtil::baseName( cmdline._filenames[ filePathIndex ] );
   }

   bool stdinCopied = false;
   for ( XxFno iii = 0; iii < cmdline._nbFilenames; ++iii ) {
      if ( cmdline._filenames[iii] == "-" ) {
         XX_ASSERT( stdinCopied == false );
         stdinCopied = true;
            
         displayFilenames[iii] = cmdline._stdinFilename;
         char temporaryFilename[32] = "/var/tmp/xxdiff-tmp.XXXXXX";
         int tfd = mkstemp( temporaryFilename );
         FILE *fout;
         if ( ( fout = fdopen( tfd, "w") ) == NULL ) {
            throw XxIoError( XX_EXC_PARAMS, 
                             "Error opening temporary file." );
         }
   
         filenames[iii] = temporaryFilename;
         created[iii] = true;
         XxUtil::copyToFile( stdin, fout );

         // Important comment: unfortunately we cannot use the unlink() trick
         // here since we will call the subordinate diff on this file, so it
         // needs to be visible in the directory.  Perhaps we could relink it
         // just before calling the subordinate diff and then unlinking it right
         // after.  Something to think about.

         if ( fclose( fout ) != 0 ) {
            throw XxIoError( XX_EXC_PARAMS,
                             "Error closing temporary file." );
         }
      }
      else {
         QString fname = cmdline._filenames[iii];
         if ( !filesAreDirectories && isDirectory[iii] == true ) {
            // Append basename to directory.
            fname.append( "/" );
            XX_ASSERT( !bn.isEmpty() );
            fname.append( bn );
         }
         displayFilenames[iii] = fname;
         filenames[iii] = fname;
         bool isdir;
         XxUtil::testFile( filenames[iii], false, isdir );
      }

      if ( ! cmdline._userFilenames[iii].isEmpty() ) {
         displayFilenames[iii] = cmdline._userFilenames[iii];
      }
   }

   return nbFiles;
}

//------------------------------------------------------------------------------
//
void XxApp::createUI( uint nbTextWidgets )
{
   XX_ASSERT( _resources != 0 );

   _mainWindow = new XxMainWindow( this, 0, "xxdiff main window" );

   //
   // Create widgets
   //
   _centralWidget = new QWidget( _mainWindow );
   QHBoxLayout* topLayout = new QHBoxLayout( _centralWidget );

   QVBoxLayout* textAndSbLayout = new QVBoxLayout( topLayout );
   QHBoxLayout* textLayout = new QHBoxLayout( textAndSbLayout );

   QFont smaller = _resources->getFontApp();
   smaller.setPointSize( smaller.pointSize() - 2 );

   for ( uint ii = 0; ii < nbTextWidgets; ++ii ) { 
      if ( ii == 1 ) {
         _vscroll[0] = new QScrollBar( _centralWidget );
         _vscroll[0]->setFixedWidth( 20 );
         textLayout->addWidget( _vscroll[0] );
         connect( _vscroll[0], SIGNAL(valueChanged(int)),
                  this, SLOT(verticalScroll(int)) );
      }
      if ( ii == 2 ) {
         _vscroll[1] = new QScrollBar( _centralWidget );
         _vscroll[1]->setFixedWidth( 20 );
         textLayout->addWidget( _vscroll[1] );
         connect( _vscroll[1], SIGNAL(valueChanged(int)),
                  this, SLOT(verticalScroll2(int)) );
      }

      QVBoxLayout* layout = new QVBoxLayout( textLayout );
      //textLayout->setStretchFactor( layout, 1 ); useless to make equal

      // Create filename and line number labels.
      QHBoxLayout* fnLayout = new QHBoxLayout( layout );
      _filenameLabel[ii] = new XxCopyLabel( _centralWidget );
      _filenameLabel[ii]->setFont( smaller );
      _filenameLabel[ii]->setFrameStyle( QFrame::Panel | QFrame::Raised );
      _filenameLabel[ii]->setMinimumWidth( 1 );
      _filenameLabel[ii]->setLineWidth( 2 );

      _lineNumberLabel[ii] = new QLabel( "9999", _centralWidget );
      _lineNumberLabel[ii]->setAlignment( AlignCenter );
      _lineNumberLabel[ii]->setFrameStyle( QFrame::Panel | QFrame::Raised );
      _lineNumberLabel[ii]->setLineWidth( 2 );
      _lineNumberLabel[ii]->setMinimumSize( _lineNumberLabel[ii]->sizeHint() );
      _lineNumberLabel[ii]->setText( "" );

      if ( _resources->getShowOpt( SHOW_FILENAMES ) == false ) {
         _filenameLabel[ii]->hide();
         _lineNumberLabel[ii]->hide();
      }
      fnLayout->addWidget( _filenameLabel[ii], 10 );
      fnLayout->addWidget( _lineNumberLabel[ii], 1 );

      // Create linenumbers widget and text widget.
      fnLayout = new QHBoxLayout( layout );
      _text[ii] = new XxText( this, ii, _centralWidget );
      _lineNumbers[ii] = new XxLineNumbers( this, ii, _centralWidget );

      fnLayout->addWidget( _lineNumbers[ii] );
      fnLayout->addWidget( _text[ii] );
   }

   _hscroll = new QScrollBar( Qt::Horizontal, _centralWidget );
   _hscroll->setFixedHeight( 20 );
   connect( _hscroll, SIGNAL(valueChanged(int)), 
            this, SLOT(horizontalScroll(int)) );
   textAndSbLayout->addWidget( _hscroll );

   _overview = new XxOverview( this, _centralWidget, "overview_area" );
   if ( _resources->getShowOpt( SHOW_OVERVIEW ) == false ) {
      _overview->hide();
   }
   topLayout->addWidget( _overview );

   // Create menus.
   createMenus();

   // Add some extra accelerators.
   QAccel* a = new QAccel( _mainWindow );
   a->connectItem( a->insertItem(Key_Space), this, SLOT(scrollDown()) ); 
   a->connectItem( a->insertItem(Key_Backspace), this, SLOT(scrollUp()) ); 

   a->connectItem( a->insertItem(Key_Prior), this, SLOT(scrollUp()) ); 
   a->connectItem( a->insertItem(Key_Next), this, SLOT(scrollDown()) ); 

   a->connectItem( a->insertItem(Key_Down), this, SLOT(cursorDown()) ); 
   a->connectItem( a->insertItem(Key_Up), this, SLOT(cursorUp()) ); 

   a->connectItem( a->insertItem(Key_Home), this, SLOT(cursorTop()) ); 
   a->connectItem( a->insertItem(Key_End), this, SLOT(cursorBottom()) ); 

   a->connectItem( a->insertItem(Key_Right), this, SLOT(scrollRight()) ); 
   a->connectItem( a->insertItem(Key_Left), this, SLOT(scrollLeft()) ); 

   a->connectItem( a->insertItem(Key_Delete), this, SLOT(selectRegionNeither()) ); 

   // Make some connections.
   connect( this, SIGNAL(cursorChanged(int)), this, SLOT(repaintTexts()) );
   connect( this, SIGNAL(cursorChanged(int)), 
            this, SLOT(updateLineNumberLabels(int)) );

   //
   // Create toolbar
   //
   _toolbar = createToolbar();
   XX_ASSERT( _toolbar != 0 );
   if ( !_resources->getShowOpt( SHOW_TOOLBAR ) ) {
      _toolbar->hide();
   }

   //
   // Show it!
   //
   _centralWidget->updateGeometry();
   _mainWindow->setCentralWidget( _centralWidget );
   setMainWidget( _mainWindow );

   _isUICreated = true;
}

//------------------------------------------------------------------------------
//
QToolBar* XxApp::createToolbar()
{
   QToolBar* toolbar = new QToolBar( "Tools", _mainWindow );
   //QToolButton* tb;
   
   QPixmap pm_previous_unselected_difference( 
      const_cast<const char**>( previous_unselected_difference_xpm )
   );
   new QToolButton( 
      pm_previous_unselected_difference, 
      "Previous unselected difference", 
      "Previous unselected difference", 
      this, SLOT(previousUnselected()), toolbar 
   );
   
   QPixmap pm_previous_difference( 
      const_cast<const char**>( previous_difference_xpm )
   );
   new QToolButton( 
      pm_previous_difference, 
      "Previous difference", 
      "Previous difference", 
      this, SLOT(previousDifference()), toolbar 
   );

   QPixmap pm_next_difference( 
      const_cast<const char**>( next_difference_xpm )
   );
   new QToolButton(
      pm_next_difference, 
      "Next difference",
      "Next difference", 
      this, SLOT(nextDifference()), toolbar
   );

   QPixmap pm_next_unselected_difference( 
      const_cast<const char**>( next_unselected_difference_xpm )
   );
   new QToolButton( 
      pm_next_unselected_difference, 
      "Next unselected difference", 
      "Next unselected difference", 
      this, SLOT(nextUnselected()), toolbar 
   );

   toolbar->addSeparator();

   QPixmap pm_search( 
      const_cast<const char**>( search_xpm )
   );
   new QToolButton( 
      pm_search, 
      "Search", 
      "Search", 
      this, SLOT(search()), toolbar 
   );
      
   QPixmap pm_search_backward( 
      const_cast<const char**>( search_backward_xpm )
   );
   new QToolButton( 
      pm_search_backward,
      "Search backward", 
      "Search backward", 
      this, SLOT(searchBackward()), toolbar 
   );
      
   QPixmap pm_search_forward( 
      const_cast<const char**>( search_forward_xpm )
   );
   new QToolButton( 
      pm_search_forward,
      "Search forward", 
      "Search forward", 
      this, SLOT(searchForward()), toolbar 
   );
      
   if ( _filesAreDirectories == false ) {
      toolbar->addSeparator();
      
      QPixmap pm_select_region_left( 
         const_cast<const char**>( select_region_left_xpm )
      );
      new QToolButton( 
         pm_select_region_left,
         "Select region left", 
         "Select region left", 
         this, SLOT(selectRegionLeft()), toolbar 
      );
      
      if ( _nbFiles == 3 ) {
         QPixmap pm_select_region_middle( 
            const_cast<const char**>( select_region_middle_xpm )
         );
         new QToolButton( 
            pm_select_region_middle,
            "Select region middle", 
            "Select region middle", 
            this, SLOT(selectRegionMiddle()), toolbar 
         );
      }

      QPixmap pm_select_region_right( 
         const_cast<const char**>( select_region_right_xpm )
      );
      new QToolButton( 
         pm_select_region_right,
         "Select region right", 
         "Select region right", 
         this, SLOT(selectRegionRight()), toolbar 
      );

      QPixmap pm_select_region_neither( 
         const_cast<const char**>( select_region_neither_xpm )
      );
      new QToolButton( 
         pm_select_region_neither,
         "Select region neither", 
         "Select region neither", 
         this, SLOT(selectRegionNeither()), toolbar 
      );

      QPixmap pm_select_region_unselect( 
         const_cast<const char**>( select_region_unselect_xpm )
      );
      new QToolButton( 
         pm_select_region_unselect,
         "Unselect region", 
         "Unselect region", 
         this, SLOT(selectRegionUnselect()), toolbar 
      );

      toolbar->addSeparator();

      QPixmap pm_split_swap_join( 
         const_cast<const char**>( split_swap_join_xpm )
      );
      new QToolButton( 
         pm_split_swap_join,
         "Split/swap/join", 
         "Split/swap/join", 
         this, SLOT(regionSplitSwapJoin()), toolbar 
      );
   }
   else {
      toolbar->addSeparator();
      
      QPixmap pm_diff_files( 
         const_cast<const char**>( diff_files_xpm )
      );
      new QToolButton( 
         pm_diff_files,
         "Diff files at cursor", 
         "Diff files at cursor", 
         this, SLOT(diffFilesAtCursor()), toolbar 
      );
   }

   return toolbar;
}

//------------------------------------------------------------------------------
//
void XxApp::createOnContextHelp()
{
   QWhatsThis::add( _vscroll[0],
                    XxHelp::getWhatsThisText( XxHelp::VSCROLL ) );
   if ( _vscroll[1] != 0 ) {
      QWhatsThis::add( _vscroll[1],
                       XxHelp::getWhatsThisText( XxHelp::VSCROLL ) );
   }
   QWhatsThis::add( _hscroll, XxHelp::getWhatsThisText( XxHelp::HSCROLL ) );
   QWhatsThis::add( _overview, XxHelp::getWhatsThisText( XxHelp::OVERVIEW ) );

   for ( int ii = 0; ii < _nbTextWidgets; ++ii ) {
      QWhatsThis::add(
         _filenameLabel[ii], XxHelp::getWhatsThisText( XxHelp::FILENAME )
      );
      QWhatsThis::add(
         _lineNumberLabel[ii], XxHelp::getWhatsThisText( XxHelp::LINENO )
      );
      QWhatsThis::add(
         _text[ii], XxHelp::getWhatsThisText( XxHelp::TEXT_VIEW )
      );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::createMenus()
{
   // File menu
   QPopupMenu* fileMenu = new QPopupMenu;
   fileMenu->insertItem( 
      "Replace left file...", this, SLOT(openLeft()), 
      _resources->getAccelerator( ACCEL_OPEN_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      fileMenu->insertItem( 
         "Replace middle file...", this, SLOT(openMiddle()), 
         _resources->getAccelerator( ACCEL_OPEN_MIDDLE ) 
      );
   }
   fileMenu->insertItem( 
      "Replace right file...", this, SLOT(openRight()), 
      _resources->getAccelerator( ACCEL_OPEN_RIGHT ) 
   );
   fileMenu->insertSeparator();
   fileMenu->insertItem( 
      "Save as left", this, SLOT(saveAsLeft()), 
      _resources->getAccelerator( ACCEL_SAVE_AS_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      fileMenu->insertItem( 
         "Save as middle", this, SLOT(saveAsMiddle()), 
         _resources->getAccelerator( ACCEL_SAVE_AS_MIDDLE ) 
      );
   }
   fileMenu->insertItem( 
      "Save as right", this, SLOT(saveAsRight()), 
      _resources->getAccelerator( ACCEL_SAVE_AS_RIGHT ) 
   );
   fileMenu->insertItem( 
      "Save as...", this, SLOT(saveAs()), 
      _resources->getAccelerator( ACCEL_SAVE_AS ) 
   );
   fileMenu->insertItem( 
      "Save selected only...", this, SLOT(saveSelectedOnly()), 
      _resources->getAccelerator( ACCEL_SAVE_SELECTED_ONLY ) 
   );
   fileMenu->insertSeparator();
   fileMenu->insertItem( 
      "Redo diff", this, SLOT(redoDiff()), 
      _resources->getAccelerator( ACCEL_REDO_DIFF ) 
   );
   fileMenu->insertSeparator();
   fileMenu->insertItem( 
      "Edit left file", this, SLOT(editLeft()), 
     _resources->getAccelerator( ACCEL_EDIT_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      fileMenu->insertItem( 
         "Edit middle file", this, SLOT(editMiddle()), 
         _resources->getAccelerator( ACCEL_EDIT_MIDDLE ) 
      );
   }
   fileMenu->insertItem( 
      "Edit right file", this, SLOT(editRight()), 
      _resources->getAccelerator( ACCEL_EDIT_RIGHT ) 
   );
   fileMenu->insertSeparator();

   fileMenu->insertItem( 
      "Exit", this, SLOT(quit()),
      _resources->getAccelerator( ACCEL_EXIT )
   );

   //---------------------------------------------------------------------------

   // Edit menu
   QPopupMenu* editMenu = new QPopupMenu;
   editMenu->insertItem( 
      "Search...", this, SLOT(search()),
      _resources->getAccelerator( ACCEL_SEARCH ) 
   );
   editMenu->insertItem( 
      "Search forward", this, SLOT(searchForward()), 
      _resources->getAccelerator( ACCEL_SEARCH_FORWARD ) 
   );
   editMenu->insertItem( 
      "Search backward", this, SLOT(searchBackward()), 
      _resources->getAccelerator( ACCEL_SEARCH_BACKWARD ) 
   );
   editMenu->insertSeparator();
   editMenu->insertItem( 
      "Scroll down", this, SLOT(scrollDown()), 
      _resources->getAccelerator( ACCEL_SCROLL_DOWN ) 
   );
   editMenu->insertItem( 
      "Scroll up", this, SLOT(scrollUp()), 
      _resources->getAccelerator( ACCEL_SCROLL_UP ) 
   );
   editMenu->insertItem( 
      "Cursor down", this, SLOT(cursorDown()), 
      _resources->getAccelerator( ACCEL_CURSOR_DOWN ) 
   );
   editMenu->insertItem( 
      "Cursor up", this, SLOT(cursorUp()), 
      _resources->getAccelerator( ACCEL_CURSOR_UP ) 
   );
   editMenu->insertItem( 
      "Cursor top", this, SLOT(cursorTop()), 
      _resources->getAccelerator( ACCEL_CURSOR_TOP ) 
   );
   editMenu->insertItem( 
      "Cursor bottom", this, SLOT(cursorBottom()), 
      _resources->getAccelerator( ACCEL_CURSOR_BOTTOM ) 
   );

   //---------------------------------------------------------------------------

   // View menu
   QPopupMenu* viewMenu = new QPopupMenu;
   if ( _filesAreDirectories == true ) {
      _menuids[ ID_View_DiffFilesAtCursor ] = viewMenu->insertItem( 
         "Diff files at cursor", this, SLOT(diffFilesAtCursor()),
         _resources->getAccelerator( ACCEL_DIFF_FILES_AT_CURSOR )
      );
      _menuids[ ID_View_CopyLeftToRight ] = viewMenu->insertItem( 
         "Copy left file to right", this, SLOT(copyFileLeftToRight()),
         _resources->getAccelerator( ACCEL_COPY_LEFT_TO_RIGHT )
      );
      _menuids[ ID_View_CopyRightToLeft ] = viewMenu->insertItem( 
         "Copy right file to left", this, SLOT(copyFileRightToLeft()),
         _resources->getAccelerator( ACCEL_COPY_RIGHT_TO_LEFT )
      );
      _menuids[ ID_View_RemoveLeft ] = viewMenu->insertItem( 
         "Remove left file", this, SLOT(removeFileLeft()),
         _resources->getAccelerator( ACCEL_REMOVE_LEFT )
      );
      _menuids[ ID_View_RemoveRight ] = viewMenu->insertItem( 
         "Remove right file", this, SLOT(removeFileRight()),
         _resources->getAccelerator( ACCEL_REMOVE_RIGHT )
      );
      viewMenu->insertSeparator();
   }
   viewMenu->insertItem( 
      "Next difference", this, SLOT(nextDifference()),
      _resources->getAccelerator( ACCEL_NEXT_DIFFERENCE ) 
   );
   viewMenu->insertItem( 
      "Previous difference", this, SLOT(previousDifference()),
      _resources->getAccelerator( ACCEL_PREVIOUS_DIFFERENCE ) 
   );
   viewMenu->insertSeparator();
   viewMenu->insertItem( 
      "Next unselected", this, SLOT(nextUnselected()),
      _resources->getAccelerator( ACCEL_NEXT_UNSELECTED ) 
   );
   viewMenu->insertItem( 
      "Previous unselected", this, SLOT(previousUnselected()),
      _resources->getAccelerator( ACCEL_PREVIOUS_UNSELECTED ) 
   );
   _viewPopup = viewMenu;

   //---------------------------------------------------------------------------

   // Global menu
   QPopupMenu* globalMenu = new QPopupMenu;
   globalMenu->insertItem( 
      "Select left", this, SLOT(selectGlobalLeft()),
      _resources->getAccelerator( ACCEL_SELECT_GLOBAL_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      globalMenu->insertItem( 
         "Select middle", this, SLOT(selectGlobalMiddle()),
         _resources->getAccelerator( ACCEL_SELECT_GLOBAL_MIDDLE ) 
      );
   }
   globalMenu->insertItem( 
      "Select right", this, SLOT(selectGlobalRight()),
      _resources->getAccelerator( ACCEL_SELECT_GLOBAL_RIGHT ) 
   );
   globalMenu->insertItem( 
      "Select neither", this, SLOT(selectGlobalNeither()),
      _resources->getAccelerator( ACCEL_SELECT_GLOBAL_NEITHER )
   );
   globalMenu->insertItem( 
      "Unselect", this, SLOT(selectGlobalUnselect()),
      _resources->getAccelerator( ACCEL_SELECT_GLOBAL_UNSELECT)
   );
   globalMenu->insertSeparator();
   globalMenu->insertItem( 
      "Select unselected left", this, SLOT(selectGlobalUnselectedLeft()),
      _resources->getAccelerator( 
         ACCEL_SELECT_GLOBAL_UNSELECTED_LEFT 
      ) 
   );
   if ( _nbFiles == 3 ) {
      globalMenu->insertItem( 
         "Select unselected middle", this, SLOT(selectGlobalUnselectedMiddle()),
         _resources->getAccelerator( 
            ACCEL_SELECT_GLOBAL_UNSELECTED_MIDDLE 
         ) 
      );
   }
   globalMenu->insertItem( 
      "Select unselected right", this, SLOT(selectGlobalUnselectedRight()),
      _resources->getAccelerator( 
         ACCEL_SELECT_GLOBAL_UNSELECTED_RIGHT 
      ) 
   );
   globalMenu->insertItem( 
      "Select unselected neither", this, SLOT(selectGlobalUnselectedNeither()),
      _resources->getAccelerator(
         ACCEL_SELECT_GLOBAL_UNSELECTED_NEITHER 
      )
   );
   globalMenu->insertSeparator();
   globalMenu->insertItem( 
      "Merge", this, SLOT(selectGlobalMerge()),
      _resources->getAccelerator(
         ACCEL_SELECT_GLOBAL_MERGE
      ) 
   );

   //---------------------------------------------------------------------------

   // Region menu
   QPopupMenu* regionMenu = new QPopupMenu;
   regionMenu->insertItem( 
      "Select left", this, SLOT(selectRegionLeft()),
      _resources->getAccelerator( ACCEL_SELECT_REGION_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      regionMenu->insertItem( 
         "Select middle", this, SLOT(selectRegionMiddle()),
         _resources->getAccelerator( ACCEL_SELECT_REGION_MIDDLE ) 
      );
   }
   regionMenu->insertItem( 
      "Select right", this, SLOT(selectRegionRight()),
      _resources->getAccelerator( ACCEL_SELECT_REGION_RIGHT ) 
   );
   regionMenu->insertItem( 
      "Select neither", this, SLOT(selectRegionNeither()),
      _resources->getAccelerator( ACCEL_SELECT_REGION_NEITHER )
   );
   regionMenu->insertItem( 
      "Unselect", this, SLOT(selectRegionUnselect()),
      _resources->getAccelerator( ACCEL_SELECT_REGION_UNSELECT)
   );
   regionMenu->insertSeparator();
   regionMenu->insertItem( 
      "Select left and next", this,
      SLOT(selectRegionLeftAndNext()),
      _resources->getAccelerator(
         ACCEL_SELECT_REGION_LEFT_AND_NEXT
      ) 
   );
   if ( _nbFiles == 3 ) {
      regionMenu->insertItem( 
         "Select middle and next", this,
         SLOT(selectRegionMiddleAndNext()),
         _resources->getAccelerator(
            ACCEL_SELECT_REGION_MIDDLE_AND_NEXT
         ) 
      );
   }
   regionMenu->insertItem( 
      "Select right and next", this, 
      SLOT(selectRegionRightAndNext()),
      _resources->getAccelerator(
         ACCEL_SELECT_REGION_RIGHT_AND_NEXT
      ) 
   );
   regionMenu->insertItem( 
      "Select neither and next", this, 
      SLOT(selectRegionNeitherAndNext()),
      _resources->getAccelerator(
         ACCEL_SELECT_REGION_NEITHER_AND_NEXT
      )
   );
   regionMenu->insertSeparator();
   regionMenu->insertItem( 
      "Split/swap/join", this, SLOT(regionSplitSwapJoin()),
      _resources->getAccelerator( 
         ACCEL_SELECT_REGION_SPLIT_SWAP_JOIN
      )
   );

   //---------------------------------------------------------------------------

   // Line menu
   QPopupMenu* lineMenu = new QPopupMenu;
   lineMenu->insertItem( 
      "Select left", this, SLOT(selectLineLeft()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      lineMenu->insertItem( 
         "Select middle", this, SLOT(selectLineMiddle()),
         _resources->getAccelerator( ACCEL_SELECT_LINE_MIDDLE ) 
      );
   }
   lineMenu->insertItem( 
      "Select right", this, SLOT(selectLineRight()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_RIGHT ) 
   );
   lineMenu->insertItem( 
      "Select neither", this, SLOT(selectLineNeither()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_NEITHER )
   );
   lineMenu->insertItem( 
      "Unselect", this, SLOT(selectLineUnselect()),
      _resources->getAccelerator( ACCEL_SELECT_LINE_UNSELECT)
   );

   //---------------------------------------------------------------------------

   // Options menu
   _optionsMenu = new QPopupMenu;

   _optionsMenu->insertItem( 
      "Edit diff options...", this, SLOT(editDiffOptions()), 
      _resources->getAccelerator( ACCEL_EDIT_DIFF_OPTIONS ) 
   );

   if ( _filesAreDirectories == false ) {

      if ( _nbFiles == 2 ) {
         _optionsMenu->insertSeparator();

         _menuids[ ID_ToggleIgnoreTrailing ] = _optionsMenu->insertItem( 
            "Ignore trailing blanks", this, SLOT(ignoreTrailing()),
            _resources->getAccelerator( ACCEL_IGNORE_TRAILING )
         );
         _menuids[ ID_ToggleIgnoreWhitespace ] = _optionsMenu->insertItem( 
            "Ignore whitespace", this, SLOT(ignoreWhitespace()),
            _resources->getAccelerator( ACCEL_IGNORE_WHITESPACE )
         );
         _menuids[ ID_ToggleIgnoreCase ] = _optionsMenu->insertItem( 
            "Ignore case", this, SLOT(ignoreCase()),
            _resources->getAccelerator( ACCEL_IGNORE_CASE )
         ); 
         _menuids[ ID_ToggleIgnoreBlankLines ] = _optionsMenu->insertItem( 
            "Ignore blank lines", this, SLOT(ignoreBlankLines()),
            _resources->getAccelerator( ACCEL_IGNORE_BLANK_LINES )
         );
         _optionsMenu->setItemEnabled( 
            _menuids[ ID_ToggleIgnoreBlankLines ], false 
         );

         _optionsMenu->insertSeparator();

         _menuids[ ID_ToggleQualityNormal ] = _optionsMenu->insertItem( 
            "Quality: normal", this, SLOT(qualityNormal()),
            _resources->getAccelerator( ACCEL_QUALITY_NORMAL )
         );
         _menuids[ ID_ToggleQualityFastest ] = _optionsMenu->insertItem( 
            "Quality: fastest", this, SLOT(qualityFastest()),
            _resources->getAccelerator( ACCEL_QUALITY_FASTEST )
         );
         _menuids[ ID_ToggleQualityHighest ] = _optionsMenu->insertItem( 
            "Quality: highest", this, SLOT(qualityHighest()),
            _resources->getAccelerator( ACCEL_QUALITY_HIGHEST )
         );
      }
   }
   else {
      _optionsMenu->insertSeparator();

      _menuids[ ID_ToggleDirDiffsRecursive ] = _optionsMenu->insertItem(
         "Recursive", this, SLOT(dirDiffRecursive()),
         _resources->getAccelerator( 
            ACCEL_DIRDIFF_RECURSIVE
         )
      );
   }

   _optionsMenu->setCheckable( true );

   //---------------------------------------------------------------------------

   _displayMenu = new QPopupMenu;

   _displayMenu->insertItem( 
      "Edit display options...", this, SLOT(editDisplayOptions()), 
      _resources->getAccelerator( ACCEL_EDIT_DISPLAY_OPTIONS ) 
   );
   _displayMenu->insertSeparator();

   if ( _filesAreDirectories == false ) {
      _menuids[ ID_ToggleHorizontalDiffs ] = _displayMenu->insertItem( 
         "Horizontal diffs", this, SLOT(toggleHorizontalDiffs()),
         _resources->getAccelerator( 
            ACCEL_TOGGLE_HORIZONTAL_DIFFS 
         )
      );
      
      _menuids[ ID_ToggleIgnoreHorizontalWs ] = _displayMenu->insertItem( 
         "Ignore horizontal whitespace", this, SLOT(toggleIgnoreHorizontalWs()),
         _resources->getAccelerator( 
            ACCEL_TOGGLE_IGNORE_HORIZONTAL_WS
         )
      );
      
      _menuids[ ID_ToggleHideCarriageReturns ] = _displayMenu->insertItem( 
         "Hide carriage returns", this, SLOT(hideCarriageReturns()),
         _resources->getAccelerator( ACCEL_HIDE_CR )
      );

      _menuids[ ID_ToggleVerticalLine ] = _displayMenu->insertItem( 
         "Draw vertical line", this, SLOT(toggleVerticalLine()),
         _resources->getAccelerator( ACCEL_TOGGLE_VERTICAL_LINE )
      );
   }
   else {
      _menuids[ ID_ToggleDirDiffsIgnoreFileChanges ] = _displayMenu->insertItem(
         "Ignore file changes", this, SLOT(ignoreFileChanges()),
         _resources->getAccelerator( 
            ACCEL_DIRDIFF_IGNORE_FILE_CHANGES
         )
      );
   }

   _displayMenu->insertSeparator();
   _menuids[ ID_ToggleFormatClipboardText ] = _displayMenu->insertItem( 
      "Format clipboard text", this, 
      SLOT(toggleFormatClipboardText()),
      _resources->getAccelerator( 
         ACCEL_TOGGLE_FORMAT_CLIPBOARD_TEXT
      )
   );

   if ( _filesAreDirectories == false ) {

      _displayMenu->insertSeparator();
      
      _menuids[ ID_TabsAtThree ] = _displayMenu->insertItem( 
         "Tabs at 3", this, SLOT(tabsAt3()),
         _resources->getAccelerator( ACCEL_TABS_AT_3 )
      );
      _menuids[ ID_TabsAtFour ] = _displayMenu->insertItem( 
         "Tabs at 4", this, SLOT(tabsAt4()),
         _resources->getAccelerator( ACCEL_TABS_AT_4 )
      );
      _menuids[ ID_TabsAtEight ] = _displayMenu->insertItem( 
         "Tabs at 8", this, SLOT(tabsAt8()),
         _resources->getAccelerator( ACCEL_TABS_AT_8 )
      );
   }

   _displayMenu->insertSeparator();
   _menuids[ ID_ToggleLineNumbers ] = _displayMenu->insertItem( 
      "Toggle line numbers", this, SLOT(toggleLineNumbers()),
      _resources->getAccelerator( ACCEL_TOGGLE_LINE_NUMBERS )
   );


   if ( _filesAreDirectories == false && _nbFiles == 3 ) {

      _displayMenu->insertSeparator();
      
      _menuids[ ID_IgnoreFileNone ] = _displayMenu->insertItem( 
         "No ignore", this, SLOT(ignoreFileNone()),
         _resources->getAccelerator( ACCEL_IGNORE_FILE_NONE )
      );
      _menuids[ ID_IgnoreFileLeft ] = _displayMenu->insertItem( 
         "Ignore left file", this, SLOT(ignoreFileLeft()),
         _resources->getAccelerator( ACCEL_IGNORE_FILE_LEFT )
      );
      _menuids[ ID_IgnoreFileMiddle ] = _displayMenu->insertItem( 
         "Ignore middle file", this, SLOT(ignoreFileMiddle()),
         _resources->getAccelerator( ACCEL_IGNORE_FILE_MIDDLE )
      );
      _menuids[ ID_IgnoreFileRight ] = _displayMenu->insertItem( 
         "Ignore right file", this, SLOT(ignoreFileRight()),
         _resources->getAccelerator( ACCEL_IGNORE_FILE_RIGHT )
      );
   }


   _displayMenu->setCheckable( true );

   //---------------------------------------------------------------------------

   // Windows menu
   _windowsMenu = new QPopupMenu;
   if ( _filesAreDirectories == false ) {
      _windowsMenu->insertItem( 
         "Merged view...", this, SLOT(mergedView()),
         _resources->getAccelerator( ACCEL_MERGED_VIEW )
      );
      _windowsMenu->insertSeparator();
   }
   _menuids[ ID_ToggleToolbar ] = _windowsMenu->insertItem( 
      "Toggle toolbar", this, 
      SLOT(toggleToolbar()),
      _resources->getAccelerator( ACCEL_TOGGLE_TOOLBAR )
   );
   _menuids[ ID_ToggleOverview ] = _windowsMenu->insertItem( 
      "Toggle overview", this, SLOT(toggleOverview()),
      _resources->getAccelerator( ACCEL_TOGGLE_OVERVIEW )
   );
   _menuids[ ID_ToggleShowFilenames ] = _windowsMenu->insertItem( 
      "Toggle show filename", this, SLOT(toggleShowFilenames()),
      _resources->getAccelerator( ACCEL_TOGGLE_SHOW_FILENAMES )
   );
   _windowsMenu->setCheckable( true );

   //---------------------------------------------------------------------------

   // Help menu
   QPopupMenu* helpMenu = new QPopupMenu;
   helpMenu->insertItem( 
      "User's manual...", this, SLOT(helpManPage()),
      _resources->getAccelerator( ACCEL_HELP_MAN_PAGE )
   );
   helpMenu->insertItem( 
      "On context", _mainWindow, SLOT(whatsThis()),
      _resources->getAccelerator( ACCEL_HELP_ON_CONTEXT )
   );
   helpMenu->insertItem( 
      "Generate init file...", this, SLOT(helpGenInitFile()),
      _resources->getAccelerator( ACCEL_HELP_GEN_INIT_FILE )
   );
   helpMenu->insertSeparator();
   helpMenu->insertItem( 
      "About...", this, SLOT(helpAbout()),
      _resources->getAccelerator( ACCEL_HELP_ABOUT )
   );

   //---------------------------------------------------------------------------

   QMenuBar* m = _mainWindow->menuBar();
   m->setSeparator( QMenuBar::InWindowsStyle );
   m->insertItem( "&File", fileMenu );
   m->insertItem( "&Edit", editMenu );
   m->insertItem( "V&iew", viewMenu );
   if ( _filesAreDirectories == false ) {
      m->insertItem( "&Global", globalMenu );
      m->insertItem( "&Region", regionMenu );
      m->insertItem( "Li&ne", lineMenu );
   }
   m->insertItem( "O&ptions", _optionsMenu );
   m->insertItem( "&Display", _displayMenu );
   m->insertItem( "W&indows", _windowsMenu );
   m->insertSeparator();
   m->insertItem( "&Help", helpMenu );
}

//------------------------------------------------------------------------------
//
void XxApp::readFile( 
   const XxFno    no, 
   const QString& filename,
   const QString& displayFilename,
   bool           isTemporary
)
{
   XX_ASSERT( 0 <= no && no <= 2 );
   XX_ASSERT( filename && displayFilename );

   //
   // Read in the file.
   //
   try {
      if ( _filesAreDirectories &&
           _resources->getBoolOpt( BOOL_DIRDIFF_BUILD_FROM_OUTPUT ) ) {
         // Assign an empty buffer. The directory diffs builder will fill it in
         // with whatever contents are read from directory diffs command output.
         _files[no].reset( new XxBuffer( false, filename, displayFilename ) );
      }
      else {
         _files[no].reset( 
            new XxBuffer( 
               filename, 
               displayFilename, 
               _resources->getBoolOpt( BOOL_HIDE_CR ),
               isTemporary 
            )
         );
      }
   }
   catch ( const std::exception& ex ) {
      QString str;
      QTextOStream oss( &str );
      oss << "Error loading in file:" << endl << ex.what() << endl;
      outputDiffErrors( str );
      _files[no].release();
      return;
   }

   //
   // Adjust UI elements
   //

   // Set filename label.
   if ( _files[no].get() != 0 ) {
      _filenameLabel[no]->setText( _files[no]->getDisplayName() );
   }

   // Note: we should also readjust the width and hor. scrollbar here, really,
   // but since the diff builder is always called after (and it does all that),
   // it is more efficient not to.
}

//------------------------------------------------------------------------------
//
void XxApp::reReadFile( const XxFno no )
{
   XX_ASSERT( 0 <= no && no <= 2 );

   //
   // Reread the file.
   //
   try {      
      std::auto_ptr<XxBuffer> oldfile = _files[no];

      if ( _filesAreDirectories &&
           _resources->getBoolOpt( BOOL_DIRDIFF_BUILD_FROM_OUTPUT ) ) {
         // Assign an empty buffer. The directory diffs builder will fill it in
         // with whatever contents are read from directory diffs command output.
         _files[no].reset( new XxBuffer( 
            false,
            oldfile->getName(),
            oldfile->getDisplayName()
         ) );
      }
      else {
         _files[no].reset( 
            new XxBuffer( 
               oldfile->getName(),
               oldfile->getDisplayName(),
               _resources->getBoolOpt( BOOL_HIDE_CR ),
               oldfile->isTemporary()
            )
         );
      }
   }
   catch ( const std::exception& ex ) {
      QString str;
      QTextOStream oss( &str );
      oss << "Error loading in file:" << endl << ex.what() << endl;
      outputDiffErrors( str );
      _files[no].release();
      return;
   }

   // Note: we should also readjust the width and hor. scrollbar here, really,
   // but since the diff builder is always called after (and it does all that),
   // it is more efficient not to.
}

//------------------------------------------------------------------------------
//
bool XxApp::processDiff()
{
   // Important note: this method assumes the UI has been created.
   XX_ASSERT( _isUICreated == true );
   
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
   // Run the diff command
   //
   if ( _filesAreDirectories == false ) {
      if ( _nbFiles == 2 ) {
         XxBuilderFiles2 builder(
            _resources->getBoolOpt( BOOL_USE_INTERNAL_DIFF )
         );
         try {
            std::auto_ptr<XxDiffs> tmp(
               builder.process(
                  _resources->getCommand( CMD_DIFF_FILES_2 ),
                  _files[0]->getName(), _files[0]->getNbLines(),
                  _files[1]->getName(), _files[1]->getNbLines()
               )
            );
            _diffs = tmp;
            XX_ASSERT( _diffs.get() != 0 );
         }
         catch ( const XxError& ex ) {
            QString str;
            QTextOStream oss( &str );
            oss << "Error executing \"" 
                << _resources->getCommand( CMD_DIFF_FILES_2 )
                << "\" command, couldn't build diffs:" << endl
                << ex.getMsg() << endl;
            outputDiffErrors( str );
            return false;
         }
      
         // Note: the return value of xxdiff matches the one that the spawned
         // diff returns.  e.g. a return value of 1 does not indicate an error.
         _returnValue = builder.getStatus();

         // If there were some warnings, output them.
         if ( !_resources->getBoolOpt( BOOL_IGNORE_ERRORS ) &&
              builder.hasErrors() ) {
            outputDiffErrors( builder.getErrors() );
         }
      }
      else if ( _nbFiles == 3 ) {
         XxBuilderFiles3 builder;
         try {
            std::auto_ptr<XxDiffs> tmp(
               builder.process(
                  _resources->getCommand( CMD_DIFF_FILES_3 ),
                  _files[0]->getName(), _files[0]->getNbLines(),
                  _files[1]->getName(), _files[1]->getNbLines(),
                  _files[2]->getName(), _files[2]->getNbLines()
               )
            );
            _diffs = tmp;
            XX_ASSERT( _diffs.get() != 0 );
         }
         catch ( const XxError& ex ) {
            QString str;
            QTextOStream oss( &str );
            oss << "Error executing \"" 
                << _resources->getCommand( CMD_DIFF_FILES_3 )
                << "\" command, couldn't build diffs:" << endl
                << ex.getMsg() << endl;
            outputDiffErrors( str );
            return false;
         }
      
         // Note: the return value of xxdiff matches the one that the spawned
         // diff returns.  e.g. a return value of 1 does not indicate an error.
         _returnValue = builder.getStatus();

         // If there were some errors, output them.
         if ( !_resources->getBoolOpt( BOOL_IGNORE_ERRORS ) &&
              builder.hasErrors() ) {
            outputDiffErrors( builder.getErrors() );
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

      XxBuilderDirs2 builder(
         _resources->getBoolOpt( BOOL_DIRDIFF_BUILD_FROM_OUTPUT ),
         _resources->getBoolOpt( BOOL_DIRDIFF_RECURSIVE ),
         _resources->getBoolOpt( BOOL_DIRDIFF_IGNORE_FILE_CHANGES )
      );
      const char* dirdiff_command = 0;
      try {
         if ( _resources->getBoolOpt( BOOL_DIRDIFF_RECURSIVE ) ) {
            dirdiff_command = _resources->getCommand(
               CMD_DIFF_DIRECTORIES_REC
            );
         }
         else {
            dirdiff_command = _resources->getCommand(
               CMD_DIFF_DIRECTORIES
            );
         }
         std::auto_ptr<XxDiffs> tmp(
            builder.process(
               dirdiff_command,
               _files[0]->getName(),
               _files[0].get(),
               _files[1]->getName(),
               _files[1].get()
            )
         );
         _diffs = tmp;
         XX_ASSERT( _diffs.get() != 0 );
      }
      catch ( const XxError& ex ) {
         QString str;
         QTextOStream oss( &str );
         oss << "Error executing \"" 
             << dirdiff_command
             << "\" command, couldn't build diffs:" << endl
             << ex.getMsg() << endl;
         outputDiffErrors( str );
         return false;
      }
      
      // Note: the return value of xxdiff matches the one that the spawned
      // diff returns.  e.g. a return value of 1 does not indicate an error.
      _returnValue = builder.getStatus();

         // If there were some warnings, output them.
      if ( !_resources->getBoolOpt( BOOL_IGNORE_ERRORS ) &&
           builder.hasErrors() ) {
         outputDiffErrors( builder.getErrors() );
      }
   }

   //
   // Adjust UI elements
   //
   if ( _diffs.get() != 0 ) {

      connect( _diffs.get(), SIGNAL(changed()), this, SLOT(repaintTexts()) );
      connect( _diffs.get(), SIGNAL(nbLinesChanged()), 
               this, SLOT(onNbLinesChanged()) );
      
      // Sanity check: check that the number of file lines are smaller than the
      // number of diff lines.
      for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
         if ( _files[ii]->getNbLines() > _diffs->getNbLines() ) {
            throw XxInternalError( XX_EXC_PARAMS );
         }
      }
      
      // Reset update connections and adjust scrollbars.
      if ( _diffs.get() != 0 ) {
         connect( _diffs.get(), SIGNAL(changed()), this, SLOT(repaintTexts()) );
      }

      // Note: this should also be done, but more efficiently for each loaded
      // file separately.
      adjustScrollbars( true );
   }

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
            _mainWindow, "xxdiff", ioerr.getMsg(), 1,0,0
         );
      }
   }

   if ( !_cmdline._cmdlineResources.isEmpty() ) {
      try {
         QTextIStream cmdlineStream( _cmdline._cmdlineResources );
         resParser.parse( cmdlineStream, *resources );
      }
      catch ( const XxIoError& ioerr ) {
         QMessageBox::critical( 
            _mainWindow, "xxdiff cmdline resources", ioerr.getMsg(), 1,0,0
         );
      }
   }

   return resources;
}

//------------------------------------------------------------------------------
//
void XxApp::outputDiffErrors( const QString& errors )
{
   _diffErrorsMsgBox = new XxSuicideMessageBox( 
      _mainWindow, "Diff errors.", errors, QMessageBox::Warning 
   );
}

//------------------------------------------------------------------------------
//
uint XxApp::computeTextWidth() const
{
   uint textWidth = 0;
   for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
      textWidth = std::max( textWidth, _files[ii]->computeTextWidth( 
         _resources->getFontText(), _resources->getTabWidth()
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
}

//------------------------------------------------------------------------------
//
void XxApp::adjustComponents()
{
   // The first text widget calls this.
   adjustScrollbars();
   adjustCursor();
   adjustLineNumbers();
}

//------------------------------------------------------------------------------
//
QRect XxApp::getMainWindowGeometry() const
{
   return _mainWindow->geometry();
}

//------------------------------------------------------------------------------
//
QPopupMenu* XxApp::getViewPopup( const XxLine& /*line*/ ) const
{
   if ( _filesAreDirectories == true ) {
      XxDln cursorLine = getCursorLine();
      const XxLine& line = _diffs->getLine( cursorLine );

      XxFln no1 = line.getLineNo( 0 );
      XxFln no2 = line.getLineNo( 1 );
      // Note: can only have two files.
      bool dirs = line.getType() == XxLine::DIRECTORIES;

      _viewPopup->setItemEnabled( _menuids[ ID_View_DiffFilesAtCursor ],
                                  ( no1 != -1 && no2 != -1 ) );
      _viewPopup->setItemEnabled( _menuids[ ID_View_CopyRightToLeft ],
                                  ( no2 != -1 && !dirs ) );
      _viewPopup->setItemEnabled( _menuids[ ID_View_CopyLeftToRight ],
                                  ( no1 != -1 && !dirs ) );

      _viewPopup->setItemEnabled( _menuids[ ID_View_RemoveLeft ],
                                  ( no1 != -1 && !dirs ) );
      _viewPopup->setItemEnabled( _menuids[ ID_View_RemoveRight ],
                                  ( no2 != -1 && !dirs ) );
   }

   return _viewPopup;
}

//------------------------------------------------------------------------------
//
void XxApp::adjustScrollbars( bool force )
{
   // Horizontal scrollbar.

   // Compute the minimum of the text widths here, because the text widths may
   // vary depending on the width of the lineNumbers widget.
   uint displayWidth = 0;
   for ( int ii = 0; ii < _nbTextWidgets; ++ii ) {
      displayWidth = std::max( displayWidth, _text[ii]->getDisplayWidth() );
   }

   if ( force == true || 
        _displayWidth != displayWidth ) {
      _displayWidth = displayWidth;

      // Leave a pixel margin to the right of text.
      uint textWidth = getTextWidth() + 16; // 10 pixels. right margin.
      if ( textWidth <= _displayWidth ) {
         _hscroll->setRange( 0, 0 );
         _hscroll->hide();
      }
      else {
         _hscroll->setSteps( 1, _displayWidth );
         _hscroll->setRange( 0, textWidth - _displayWidth );
         _hscroll->show();
      }

      if ( int( textWidth - _displayWidth ) < _hscroll->value() ) {
         _hscroll->setValue( textWidth - _displayWidth );
         // Note: this will indirectly trigger a redraw.
      }
   }

   // Vertical scrollbar.
   if ( force == true || 
        _displayHeight != (uint)_text[0]->height() ) {
      XxDln displayLines = _text[0]->computeDisplayLines();
      if ( force == true || 
           _displayLines != displayLines ) {
         _displayLines = displayLines;
         _vscroll[0]->setSteps( 1, _displayLines );
         if ( _vscroll[1] != 0 ) {
            _vscroll[1]->setSteps( 1, _displayLines );
         }

         XxDln topLine = getTopLine();

         if ( _diffs.get() == 0 ) {
            _vscroll[0]->setRange( 0,0 );
            if ( _vscroll[1] != 0 ) {
               _vscroll[1]->setRange( 0,0 );
            }
         }
         else {
            XxDln maxLine = 
               std::max( 0, XxDln( _diffs->getNbLines() - _displayLines + 1 ) );
            _vscroll[0]->setRange( 0, maxLine );
            if ( _vscroll[1] != 0 ) {
               _vscroll[1]->setRange( 0, maxLine );
            }
         }

         setTopLine( topLine );
         // Note: this will indirectly trigger a redraw.
      }
      else {
         // We need to redraw the lower portion, that's why we redraw here.
         repaintTexts();
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::horizontalScroll( int /*value*/ )
{
   repaintTexts();
}

//------------------------------------------------------------------------------
//
void XxApp::scrollRight()
{
   _hscroll->setValue( _hscroll->value() + _hscroll->lineStep() * 10 );
}

//------------------------------------------------------------------------------
//
void XxApp::scrollLeft()
{
   _hscroll->setValue( _hscroll->value() - _hscroll->lineStep() * 10 );
}

//------------------------------------------------------------------------------
//
void XxApp::verticalScroll( int value )
{
   adjustCursor();
   repaintTexts();
   emit scrolled( getTopLine() );
   if ( _vscroll[1] != 0 ) {
      // Will only change if different.
      _vscroll[1]->setValue( value );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::verticalScroll2( int value )
{
   _vscroll[0]->setValue( value );
   verticalScroll( value );
}

//------------------------------------------------------------------------------
//
uint XxApp::getHorizontalPos() const
{
   return _hscroll->value();
}


//------------------------------------------------------------------------------
//
XxDln XxApp::setTopLine( const XxDln lineNo )
{
   if ( _diffs.get() == 0 ) {
      return 1;
   }

   XxDln oldLine = getTopLine();
   XxDln validLine = 
      std::max( XxDln(1), std::min( XxDln(_diffs->getNbLines()), lineNo ) );
   XxDln displayableLine = 
      std::min(
         validLine, 
         std::max( XxDln(1), 
                   XxDln(_diffs->getNbLines() - (getNbDisplayLines() - 2)) )
      );
   _vscroll[0]->setValue( displayableLine - 1 );
   if ( _vscroll[1] != 0 ) {
      _vscroll[1]->setValue( displayableLine - 1 );
   }
   // Note: this will trigger the scroll signal indirectly.
   
   adjustCursor();

   return oldLine;
}

//------------------------------------------------------------------------------
//
XxDln XxApp::setCenterLine( const XxDln lineNo )
{
   if ( _diffs.get() == 0 ) {
      return 1;
   }

   XxDln oldLine = getCenterLine();

   setTopLine( lineNo - getNbDisplayLines() / 2 );

   return oldLine;
}

//------------------------------------------------------------------------------
//
XxDln XxApp::setBottomLine( const XxDln lineNo )
{
   if ( _diffs.get() == 0 ) {
      return 1;
   }

   XxDln oldLine = getBottomLine();

   setTopLine( lineNo - (getNbDisplayLines() - 2) );

   return oldLine;
}

//------------------------------------------------------------------------------
//
XxDln XxApp::setCursorLine( const XxDln lineNo )
{
   XxDln oldLine = _cursorLine;

   // Validate and set.
   XxDln validLine = 
      std::max( XxDln(1), std::min( XxDln(getNbLines()), lineNo ) );
   _cursorLine = validLine;

   // Scroll to display.
   if ( _cursorLine < getTopLine() ) {
      setTopLine( _cursorLine );
   }
   else if ( _cursorLine > getBottomLine() ) {
      setBottomLine( _cursorLine );
   }

   if ( _cursorLine != oldLine ) {
      emit cursorChanged( _cursorLine );
   }
   return oldLine;
}

//------------------------------------------------------------------------------
//
bool XxApp::adjustCursor()
{
   XxDln oldLine = _cursorLine;
   _cursorLine = 
      std::max( getTopLine(), std::min( getBottomLine(), oldLine ) );
   if ( _cursorLine != oldLine ) {
      emit cursorChanged( _cursorLine );
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
//
void XxApp::updateLineNumberLabels( int cursorLine )
{
   if ( _diffs.get() != 0 ) {
      for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
         bool aempty;

         XxFln fline = _diffs->getBufferLine( ii, cursorLine, aempty );
         _lineNumberLabel[ii]->setNum( fline );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::repaintTexts()
{
   for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
      _lineNumbers[ii]->update();
      _text[ii]->update();
   }
   if ( _overview->isVisible() ) {
      _overview->update();
   }
   if ( _mergedWindow != 0 && _mergedWindow->isVisible() ) {
      _mergedWindow->update();
   }
}

//------------------------------------------------------------------------------
//
void XxApp::onNbLinesChanged()
{
   adjustScrollbars( true );
}

//------------------------------------------------------------------------------
//
void XxApp::saveToFile( const QString& filename, const bool ask ) 
{
   if ( _diffs.get() == 0 ) {
      return;
   }

   // Check if there are some unselected regions remaining.
   bool allSelected = _diffs->isAllSelected();
   if ( !allSelected ) {
      
      // Bring the user to the first unselected region.
      XxDln nextNo = _diffs->findNextUnselected( 0 );
      XX_ASSERT( nextNo != -1 );
      setCursorLine( nextNo );
      setCenterLine( nextNo );
   }

   // Remove ClearCase extended syntax if it is there.
   int bpos = filename.find( "@@" );
   QString cleanname;
   if ( bpos != -1 ) {
      cleanname = filename.mid( 0, bpos );
   }
   else {
      cleanname = filename;
   }
   
   QString f;
   bool useConditionals = false;
   bool removeEmptyConditionals = false;
   QString conditionals[3];
   if ( !allSelected ) {

      f = XxMarkersFileDialog::getSaveFileName( 
         cleanname, QString::null, _mainWindow, "xxdiff save file",
         _nbFiles == 3,
         useConditionals, 
         removeEmptyConditionals,
         conditionals
      );
      if ( f.isEmpty() ) {
         // The user cancelled the dialog.
         return;
      }
   }
   else if ( ask == true || !allSelected ) {
      f = QFileDialog::getSaveFileName( 
         cleanname, QString::null, _mainWindow, "xxdiff save file"
      );
      if ( f.isEmpty() ) {
         // The user cancelled the dialog.
         return;
      }
   }
   else {
      f = cleanname;
   }
   XX_ASSERT( !f.isEmpty() );
   
   // Check for file existence.
   QFileInfo finfo( f );
   if ( finfo.exists() ) {        
      QString msg;
      if ( finfo.isWritable() ) {
         msg = QString("File exists, overwrite?");
      }
      else {
         msg = QString("File exists (AND IS NOT WRITABLE), overwrite?");
      }

      int resp = QMessageBox::warning( 
         _mainWindow, "xxdiff", msg, "Ok", "Cancel", QString::null, 0, 1
      );
      if ( resp == 1 ) {
         // User has canceled.
         return;
      }
      // Continue anyway.
   }
   
   // Open a file.
   try {
      QFile outfile( f );
      bool succ = outfile.open( IO_Truncate | IO_WriteOnly );
      if ( !succ ) {
         throw XxIoError( XX_EXC_PARAMS, "Error opening output file." );
      }
      
      // Save to the file.
      {
         QTextStream osstream( &outfile );
         _diffs->save( getResources(), osstream, getBuffers(),
                       useConditionals, removeEmptyConditionals,
                       conditionals );
      }
      
      outfile.close();
      if ( outfile.status() != IO_Ok ) {
         throw XxIoError( XX_EXC_PARAMS, "Error closing output file." );
      }
   }
   catch ( const XxIoError& ioerr ) {
      QMessageBox::critical( 
         _mainWindow, "xxdiff", ioerr.getMsg(), 1,0,0
      );
   }

   _diffs->clearDirty();
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
      ::setsockopt( _sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int) );

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
      int reuse = 1;
      ::setsockopt( _sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int) );

      /* Give the socket a name.  */
      struct sockaddr_in name;
      name.sin_family = AF_INET;
      name.sin_port = htons( 12793 ); // How can I get a unique port?
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

   command += QString(" ") + filename;
   const char** args;
   XxUtil::splitArgs( command, args );

   try {
      XxUtil::spawnCommand( args, handlerSIGCHLD );
   }
   catch ( const XxIoError& ioerr ) {
      QString text;
      {
         QTextOStream oss( &text );
         oss << "There has been an error spawning the editor:" 
             << ioerr.getMsg() << endl << flush;
      }
      QMessageBox* box = new XxSuicideMessageBox( 
         _mainWindow, "Error.", text, QMessageBox::Warning 
      );
      box->show();
   }

   XxUtil::freeArgs( args );
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

   QString startWith = QString::null;
   XX_ASSERT( _files[no].get() != 0 );
   if ( !isTemporary[no] ) {
      startWith = filenames[no];
   }
   QString f = QFileDialog::getOpenFileName( 
      startWith, QString::null, _mainWindow, "xxdiff open file"
   );
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
      readFile( no, f, f, false );
      bool succ = processDiff();
      
      // Initialize the horizontal diffs if necessary.
      if ( succ == true &&
           _resources->getBoolOpt( BOOL_HORIZONTAL_DIFFS ) == true ) {
         _diffs->initializeHorizontalDiffs( 
            _resources->getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS ), getBuffers()
         );
      }

      // Reset the cursor line.
      setCursorLine( 1 );
      repaintTexts();
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
         setCursorLine( nextNo );
         setCenterLine( nextNo );
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
      XxDln centerLine = getCenterLine();

      // Reread the files.
      for ( XxFno ii = 0; ii < nbFiles; ++ii ) {
         reReadFile( ii );
      }

      // Do the diff.
      bool succ = processDiff();

      // Initialize the horizontal diffs if necessary.
      if ( succ == true &&
           _resources->getBoolOpt( BOOL_HORIZONTAL_DIFFS ) == true ) {
         _diffs->initializeHorizontalDiffs(
            _resources->getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS ), getBuffers()
         );
      }

      // Try to set the same lines as it used to have before the redo.
      // Reset the cursor line.
      setCursorLine( cursorLine );
      setCenterLine( centerLine );
      repaintTexts();
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
         saveToFile( file->getName(), false );
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
         saveToFile( file->getName(), false );
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
         saveToFile( file->getName(), false );
      }
   }
}

//------------------------------------------------------------------------------
//
bool XxApp::validateNeedToSave( uint no ) const
{
   if ( _diffs.get() == 0 ) {
      return false;
   }

   if ( _diffs->checkSelections( XxLine::Selection(no) ) == true ) {
      // Pop a dialog.
      QString text;
      QTextOStream oss( &text );
      oss << "The selections are all on this file." << endl
          << "Save anyway?" << flush;

      int resp = QMessageBox::warning( 
         _mainWindow, "xxdiff", text, 
         "Ok", "Cancel", QString::null, 0, 1
      );
      if ( resp == 1 ) {
         // User has canceled.
         return false;
      }
      // Continue anyway.
   }
   return true;
}

//------------------------------------------------------------------------------
//
void XxApp::saveAs()
{
   XxBuffer* file = getBuffer( 0 );
   QString filename;
   if ( file != 0 && file->isTemporary() == false ) {
      filename = file->getName();
   }
   saveToFile( filename, true );
}

//------------------------------------------------------------------------------
//
void XxApp::saveSelectedOnly()
{
   if ( _diffs.get() == 0 ) {
      return;
   }

   QString f = QFileDialog::getSaveFileName( 
      QString::null, QString::null, _mainWindow, "xxdiff save selected"
   );
   if ( f.isEmpty() ) {
      // The user cancelled the dialog.
      return;
   }
   
   // Check for file existence.
   QFileInfo finfo( f );
   if ( finfo.exists() ) {     
      QString msg;
      if ( finfo.isWritable() ) {
         msg = QString("File exists, overwrite?");
      }
      else {
         msg = QString("File exists (AND IS NOT WRITABLE), overwrite?");
      }

      int resp = QMessageBox::warning( 
         _mainWindow, "xxdiff", msg, "Ok", "Cancel", QString::null, 0, 1
      );
      if ( resp == 1 ) {
         // User has canceled.
         return;
      }
      // Continue anyway.
   }
   
   QFile outfile( f );
   bool succ = outfile.open( IO_Truncate | IO_WriteOnly );
   if ( !succ ) {
      throw XxIoError( XX_EXC_PARAMS, "Error opening output file." );
   }
   
   // Save to the file.
   {
      QTextStream osstream( &outfile );
      _diffs->saveSelectedOnly( osstream, getBuffers() );
   }
   
   outfile.close();
   if ( outfile.status() != IO_Ok ) {
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
int XxApp::exec()
{
   if ( _resources->getBoolOpt( BOOL_EXIT_ON_SAME ) && _returnValue == 0 ) {
      return _returnValue;
   }
   return QApplication::exec();
}

//------------------------------------------------------------------------------
//
void XxApp::quit()
{
   // If there are some selected regions, confirm with user (optional).
   if ( _diffs.get() != 0 ) {
      if ( _resources->getBoolOpt( BOOL_WARN_ABOUT_UNSAVED ) == true &&
           _diffs->isDirty() == true &&
           _diffs->isSomeSelected() == true ) {
         int resp = QMessageBox::warning( 
            _mainWindow,
            "xxdiff",
            "Some selections changed and unsaved, quit anyway?",
            "Ok", "Cancel", QString::null, 0, 1
         );
         if ( resp == 1 ) {
            // User has canceled.
            return;
         }
         // Continue anyway.
      }
   }

   QApplication::exit( _returnValue );
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
	 setCursorLine( sres._lineNo );
	 setCenterLine( sres._lineNo );
      }
      else {
	 setCursorLine( _diffs->getNbLines() );
         setCenterLine( _diffs->getNbLines() );
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
         setCursorLine( sres._lineNo );
         setCenterLine( sres._lineNo );
      }
      else {
         setCursorLine( 1 );
         setCenterLine( 1 );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::scrollDown()
{
   XxDln diff = (getNbDisplayLines() - 3);
   // Note: we leave 3 lines in common.

   // Scroll.
   XxDln topLine = getTopLine();
   setTopLine( topLine + diff );

   // Keep cursor in center.
   if ( topLine == getTopLine() ) {
      setCursorLine( getNbLines() );
   }
   else {
      setCursorLine( getCenterLine() );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::scrollUp()
{
   XxDln diff = (getNbDisplayLines() - 3);
   // Note: we leave 3 lines in common.

   // Scroll.
   XxDln topLine = getTopLine();
   setTopLine( topLine - diff );

   // Keep cursor in center.
   if ( topLine == getTopLine() ) {
      setCursorLine( 1 );
   }
   else {
      setCursorLine( getCenterLine() );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::cursorDown()
{
   setCursorLine( getCursorLine() + 1 );
   setCenterLine( getCursorLine() );
}

//------------------------------------------------------------------------------
//
void XxApp::cursorUp()
{
   setCursorLine( getCursorLine() - 1 );
   setCenterLine( getCursorLine() );
}

//------------------------------------------------------------------------------
//
void XxApp::cursorTop()
{
   setCursorLine( 1 );
}

//------------------------------------------------------------------------------
//
void XxApp::cursorBottom()
{
   setCursorLine( getNbLines() );
}

//------------------------------------------------------------------------------
//
void XxApp::redoDiff()
{ 
   if ( _nbFiles == 0 || _diffs.get() == 0 ) {
      return;
   }

   // Popup name and arguments box.
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

   XxDln cursorLine = getCursorLine();
   const XxLine& line = _diffs->getLine( cursorLine );
   if ( line.getType() == XxLine::SAME ||
        line.getType() == XxLine::DIFF_ALL || 
        line.getType() == XxLine::DIRECTORIES ) {

      // Get filenames.
      QString filenames[2];
      for ( XxFno ii = 0; ii < 2; ++ii ) {
         bool empty;
         XxFln fline = _diffs->getBufferLine( ii, cursorLine, empty );
         XX_ASSERT( empty == false );

         filenames[ii] = _files[ii]->getBufferAtLine( fline );
      }

      // Spawn a diff.
      QString command = argv()[0] + 
         QString(" ") + filenames[0] + 
         QString (" ") + filenames[1];
      const char** args;
      XxUtil::splitArgs( command, args );

      try { 
         XxUtil::spawnCommand( args );
      }
      catch ( const XxIoError& ioerr ) {
         QString text;
         {
            QTextOStream oss( &text );
            oss << "There has been an error spawning diff program:" 
                << ioerr.getMsg() << endl << flush;
         }
         QMessageBox* box = new XxSuicideMessageBox( 
            _mainWindow, "Error.", text, QMessageBox::Warning 
         );
         box->show();
      }

      XxUtil::freeArgs( args );
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
   // directory name only.
   fline = line.getLineNo( nodst );
   QString filedst;
   if ( fline == -1 ) {
      filedst = _files[nodst]->getName();
   }
   else {
      filedst = _files[nodst]->getBufferAtLine( fline );

      QFileInfo finfo( filedst );
      if ( finfo.exists() ) {
         int resp = QMessageBox::warning( 
            _mainWindow, "xxdiff", "File exists, overwrite?",
            "Ok", "Cancel", QString::null, 0, 1
         );
         if ( resp == 1 ) {
            // User has canceled.
            return;
         }
         // Continue anyway.
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

   int resp = QMessageBox::warning( 
      _mainWindow, "xxdiff", "Delete file... are you sure?",
      "Ok", "Cancel", QString::null, 0, 1
   );
   if ( resp == 1 ) {
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
         setCursorLine( nextNo );
         setCenterLine( nextNo );
      }
      else {
         setCursorLine( _diffs->getNbLines() );
         setCenterLine( _diffs->getNbLines() );
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
         setCursorLine( nextNo );
         setCenterLine( nextNo );
      }
      else {
         setCursorLine( 1 );
         setCenterLine( 1 );
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
         setCursorLine( nextNo );
         setCenterLine( nextNo );
      }
      else {
         setCursorLine( _diffs->getNbLines() );
         setCenterLine( _diffs->getNbLines() );
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
         setCursorLine( nextNo );
         setCenterLine( nextNo );
      }
      else {
         setCursorLine( 1 );
         setCenterLine( 1 );
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
void XxApp::selectGlobalMerge()
{
   if ( _diffs.get() != 0 ) {
      _diffs->merge();
   }
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
         if ( _resources->getBoolOpt( BOOL_HORIZONTAL_DIFFS ) ) {
            _diffs->initializeHorizontalDiffs(
               _resources->getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS ), getBuffers()
            );
         }
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

   _displayWidth = 0; // This is a hack to force adjustment of hscrollbar.
   adjustScrollbars();
   synchronizeUI();
   repaintTexts();   
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
   onRedoDiff();
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
void XxApp::mergedView()
{
   if ( _nbFiles == 0 || _diffs.get() == 0 ) {
      return;
   }

   // Popup merged view.
   if ( _mergedWindow == 0 ) {
      _mergedWindow = new XxMergedWindow(
         this, _mainWindow, "xxdiff merged window"
      );
   }
   if ( _mergedWindow->isVisible() == false ) {
      // Resize the merged window to have the same size as the text widgets.
      int mh = _mainWindow->menuBar()->height();
      int sw = _vscroll[0]->width();
      int sh = _hscroll->height();
      _mergedWindow->resize( QSize( _text[0]->width() + sw,
                                    _text[0]->height() + mh + sh ) );
   }
   _mergedWindow->show();
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

   // Let the resizing occur, process all events, for when adjustScrollbars gets
   // called the sizes have been updated.  Yes, this is a horrible hack but it
   // works (unless your window takes >50 secs. to resize).
   processEvents( 50 );

   adjustScrollbars( true );
   repaintTexts();
}

//------------------------------------------------------------------------------
//
void XxApp::adjustLineNumbers()
{
   if ( _resources->getShowOpt( SHOW_LINE_NUMBERS ) ) {
      // Compute the maximum line numbers width.  This has to be the same for
      // all the texts, in order to have a consistent horizontal scrollbar.
      uint lnw = 0;
      for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
         lnw = std::max(
            lnw,
            _files[ii]->computeLineNumbersWidth( _resources->getFontText() ) +
            ( _lineNumbers[ii]->width() - 
              _lineNumbers[ii]->contentsRect().width() + 2 ) );
         
      }
      for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
         _lineNumbers[ii]->setFixedWidth( lnw );
         _lineNumbers[ii]->show();
      }
   }
   else {
      for ( XxFno ii = 0; ii < _nbTextWidgets; ++ii ) {
         _lineNumbers[ii]->hide();
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::toggleVerticalLine()
{
   _resources->toggleShowOpt( SHOW_VERTICAL_LINE );
   repaintTexts();
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleOverview()
{
   _resources->toggleShowOpt( SHOW_OVERVIEW );

   if ( _resources->getShowOpt( SHOW_OVERVIEW ) ) {
      _overview->show();
   }
   else {
      _overview->hide();
   }
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleShowFilenames()
{
   _resources->toggleShowOpt( SHOW_FILENAMES );

   for ( int ii = 0; ii < _nbTextWidgets; ++ii ) {
      if ( _resources->getShowOpt( SHOW_FILENAMES ) ) {
         _filenameLabel[ii]->show();
         _lineNumberLabel[ii]->show();
      }
      else {
         _filenameLabel[ii]->hide();
         _lineNumberLabel[ii]->hide();
      }
   }
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleHorizontalDiffs()
{
   _resources->toggleBoolOpt( BOOL_HORIZONTAL_DIFFS );

   if ( _diffs.get() != 0 ) {
      _diffs->initializeHorizontalDiffs(
         _resources->getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS ), getBuffers()
      );
      repaintTexts();
   }
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleIgnoreHorizontalWs()
{
   _resources->toggleBoolOpt( BOOL_IGNORE_HORIZONTAL_WS );

   if ( _diffs.get() != 0 ) {
      _diffs->initializeHorizontalDiffs(
         _resources->getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS ),  
         getBuffers(),
         true
      );
      repaintTexts();
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
   repaintTexts();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreFileLeft()
{
   _resources->setIgnoreFile( IGNORE_LEFT );
   synchronizeUI();
   repaintTexts();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreFileMiddle()
{
   _resources->setIgnoreFile( IGNORE_MIDDLE );
   synchronizeUI();
   repaintTexts();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreFileRight()
{
   _resources->setIgnoreFile( IGNORE_RIGHT );
   synchronizeUI();
   repaintTexts();
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
void XxApp::helpGenInitFile()
{
   // Generate it into a string to check if there are some contents.
   QString outstr;
   {
      // Save to the stream the differences with the default resources.
      QTextStream outs( &outstr, IO_WriteOnly );
      std::auto_ptr<XxResources> defres(
         new XxResources( _cmdline._originalXdiff )
      );
      XxResParser::genInitFile( *_resources, *defres, outs );
   }
   if ( outstr.isEmpty() ) {
      QMessageBox::warning( 
         _mainWindow, "xxdiff",
         "There is nothing to write as init file\n(nothing has been changed).",
         "Ok"
      );
      return;
   }
   // Ok, there is something to write, get filename and save to file.

   QString f;
   f = QFileDialog::getSaveFileName( QString( "xxdiffrc" ),
                                     QString::null,
                                     _mainWindow,
                                     "xxdiff save sample" );
   if ( f.isEmpty() ) {
      // The user cancelled the dialog.
      return;
   }
   XX_ASSERT( !f.isEmpty() );

   QFileInfo finfo( f );
   if ( finfo.exists() ) {        
      int resp = QMessageBox::warning( 
         _mainWindow, "xxdiff", "File exists, overwrite?",
         "Ok", "Cancel", QString::null, 0, 1
      );
      if ( resp == 1 ) {
         // User has canceled.
         return;
      }
      // Continue anyway.
   }
   
   // Open a file.
   try {
      QFile outfile( f );
      bool succ = outfile.open( IO_Truncate | IO_WriteOnly );
      if ( !succ ) {
         throw XxIoError( XX_EXC_PARAMS, "Error opening output file." );
      }
      
      // Save to the file.
      {
         QTextStream osstream( &outfile );
         osstream << outstr;
      }

      outfile.close();
      if ( outfile.status() != IO_Ok ) {
         throw XxIoError( XX_EXC_PARAMS, "Error closing output file." );
      }
   }
   catch ( const XxIoError& ioerr ) {
      QMessageBox::critical( 
         _mainWindow, "xxdiff", ioerr.getMsg(), 1,0,0
      );
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
      XxCommand cmdResId =
         (_nbFiles == 2) ? CMD_DIFF_FILES_2 : CMD_DIFF_FILES_3;
      
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreTrailing ],
         _resources->isCommandSwitch( 
            cmdResId, 
            CMDSW_FILES_IGNORE_TRAILING
         )
      );
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreWhitespace ],
         _resources->isCommandSwitch( 
            cmdResId, 
            CMDSW_FILES_IGNORE_WHITESPACE
         )
      );
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreCase ],
         _resources->isCommandSwitch( 
            cmdResId, 
            CMDSW_FILES_IGNORE_CASE
         )
      );
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreBlankLines ],
         _resources->isCommandSwitch( 
            cmdResId, 
            CMDSW_FILES_IGNORE_BLANK_LINES
         )
      );

      QString cmd = _resources->getCommand( cmdResId );
      XxQuality quality = _resources->getQuality( cmd );

      _optionsMenu->setItemChecked(
         _menuids[ ID_ToggleQualityNormal ],
         quality == QUALITY_NORMAL
      );
      _optionsMenu->setItemChecked(
         _menuids[ ID_ToggleQualityFastest ],
         quality == QUALITY_FASTEST
      );
      _optionsMenu->setItemChecked(
         _menuids[ ID_ToggleQualityHighest ],
         quality == QUALITY_HIGHEST
      );
   }
   else {
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleDirDiffsRecursive ],
         _resources->getBoolOpt( BOOL_DIRDIFF_RECURSIVE )
      );
   }

   //---------------------------------------------------------------------------

   if ( _filesAreDirectories == false ) {
      _displayMenu->setItemChecked( 
         _menuids[ ID_ToggleHorizontalDiffs ],
         _resources->getBoolOpt( BOOL_HORIZONTAL_DIFFS )
      );
      _displayMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreHorizontalWs ],
         _resources->getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS )
      );
      _displayMenu->setItemChecked( 
         _menuids[ ID_ToggleHideCarriageReturns ],
         _resources->getBoolOpt( BOOL_HIDE_CR )
      );
      _displayMenu->setItemChecked( 
         _menuids[ ID_ToggleVerticalLine ],
         _resources->getShowOpt( SHOW_VERTICAL_LINE )
      );
   }
   else {
      _displayMenu->setItemChecked( 
         _menuids[ ID_ToggleDirDiffsIgnoreFileChanges ],
         _resources->getBoolOpt( BOOL_DIRDIFF_IGNORE_FILE_CHANGES )
      );
   }

   _displayMenu->setItemChecked( 
      _menuids[ ID_ToggleFormatClipboardText ],
      _resources->getBoolOpt( BOOL_FORMAT_CLIPBOARD_TEXT )
   );

   if ( _filesAreDirectories == false ) {

      uint tabWidth = _resources->getTabWidth();
      _displayMenu->setItemChecked( _menuids[ ID_TabsAtThree ], tabWidth == 3 );
      _displayMenu->setItemChecked( _menuids[ ID_TabsAtFour ], tabWidth == 4 );
      _displayMenu->setItemChecked( _menuids[ ID_TabsAtEight ], tabWidth == 8 );
   }

   _displayMenu->setItemChecked( 
      _menuids[ ID_ToggleLineNumbers ], 
      _resources->getShowOpt( SHOW_LINE_NUMBERS )
   );

   if ( _filesAreDirectories == false && _nbFiles == 3 ) {

      XxIgnoreFile ignoreFile = _resources->getIgnoreFile();
      _displayMenu->setItemChecked( _menuids[ ID_IgnoreFileNone ], 
                                    ignoreFile == IGNORE_NONE );
      _displayMenu->setItemChecked( _menuids[ ID_IgnoreFileLeft ], 
                                    ignoreFile == IGNORE_LEFT );
      _displayMenu->setItemChecked( _menuids[ ID_IgnoreFileMiddle ], 
                                    ignoreFile == IGNORE_MIDDLE );
      _displayMenu->setItemChecked( _menuids[ ID_IgnoreFileRight ], 
                                    ignoreFile == IGNORE_RIGHT );
   }

   //---------------------------------------------------------------------------

   _windowsMenu->setItemChecked( 
      _menuids[ ID_ToggleToolbar ], 
      _resources->getShowOpt( SHOW_TOOLBAR )
   );
   _windowsMenu->setItemChecked( 
      _menuids[ ID_ToggleOverview ],
      _resources->getShowOpt( SHOW_OVERVIEW )
   );
   _windowsMenu->setItemChecked( 
      _menuids[ ID_ToggleShowFilenames ],
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

XX_NAMESPACE_END

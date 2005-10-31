/******************************************************************************\
 * $Id: app.cpp 2 2000-09-15 02:19:22Z blais $
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

#define QT_2_2

#include <main.h>
#include <app.h>
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
#include <optionsDialog.h>
#include <dialogs.h>
#include <stringResParser.h>
#ifdef XX_USE_XRM
#include <xrmParser.h>
#endif
#ifdef XX_USE_RCFILE
#include <rcfileParser.h>
#endif
#include <getopt.h>

#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qlayout.h>
#include <qscrollbar.h>
#include <qlabel.h>
#if QT_VERSION >= 220
#include <qsgistyle.h>
#else
#include <qmotifstyle.h>
#endif
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

#include <exception>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
using namespace std;

enum MenuIds {
   ID_ToggleToolbar,
   ID_ToggleLineNumbers,
   ID_ToggleShowMarkers,
   ID_ToggleOverview,
   ID_ToggleShowFilenames,
   ID_ToggleHorizontalDiffs,
   ID_ToggleIgnoreHorizontalWs,
   ID_ToggleCutAndPasteAnnotations,
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
   ID_TabsAtFour,
   ID_TabsAtEight
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
      XxApp* app,
      QWidget* parent = 0, 
      const char* name = 0, 
      WFlags f = WType_TopLevel 
   );


private:

   /*----- data members -----*/

   XxApp* _app;

};

//------------------------------------------------------------------------------
//
XxMainWindow::XxMainWindow( 
   XxApp* app,
   QWidget* parent,
   const char* name, 
   WFlags f
) :
   QMainWindow( parent, name, f ),
   _app( app )
{}


/*==============================================================================
 * LOCAL CLASS XxSuicideMessageBox
 *============================================================================*/

// <summary> simple non-modal dialog that cares after itself </summary>

class XxSuicideMessageBox : public QMessageBox {

public:

   /*----- member functions -----*/

   // Constructor.
   XxSuicideMessageBox( 
      QWidget*       parent,
      const QString& caption, 
      const QString& text, 
      Icon           icon 
   );

   // See base class.
   virtual void done( int r );

};

//------------------------------------------------------------------------------
//
XxSuicideMessageBox::XxSuicideMessageBox( 
   QWidget*       parent,
   const QString& caption, 
   const QString& text, 
   Icon           icon 
) :
   QMessageBox( caption, text, icon, 1, 0, 0, parent, 0, false )
{}

//------------------------------------------------------------------------------
//
void XxSuicideMessageBox::done( int r )
{
   delete this;
}

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
      // FIXME also check for '\' in case we ever port to Windoze.
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
using namespace std;

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxApp
 *============================================================================*/

/*----- static data members -----*/

int              XxApp::_sockfd = -1;
QSocketNotifier* XxApp::_socketNotifier = 0;


//------------------------------------------------------------------------------
//
XxApp::XxApp( int argc, char** argv ) :
   QApplication( argc, argv ),
   _isUICreated( false ),
   _searchDialog( 0 ),
   _optionsDialog( 0 ),
   _tabWidthDialog( 0 ),
   _mergedWindow( 0 ),
   _textWidth( 0 ),
   _displayWidth( 0 ),
   _displayHeight( 0 ),
   _displayLines( 0 ),
   _cursorLine( 0 ), // disallowed value on purpose
   _filesAreDirectories( false ),
   _returnValue( 0 ),
   _stringResParser( 0 )
{
   _vscroll[0] = _vscroll[1] = 0;

#if QT_VERSION >= 220
   _style = new QSGIStyle;
#else
   _style = new QMotifStyle;
#endif
   setStyle( _style );

   // Create resources object.
   _resources = XxResources::getInstance();
#ifdef XX_DEBUG
   _resources->checkResourcesDoc();
#endif

   // Parse command line options.
   // This will potentially create a string resources parser.
   parseCommandLine( argc, argv );

   // Read in the resources.
   buildResources();

   // Parse the resources given in the command line.
   parseCommandLineResources();
   
   // Add extra diff arguments.
   XX_ASSERT( _resources != 0 );
   XxResources::Resource cmdResId = _nbFiles == 2 ? 
      XxResources::COMMAND_DIFF_FILES_2 : 
      XxResources::COMMAND_DIFF_FILES_3;

   std::string cmd = _resources->getCommand( cmdResId );
   XxOptionsDialog::addToCommand( cmd, _extraDiffArgs.c_str() );
   _resources->setCommand( cmdResId, cmd.c_str() );


#if QT_VERSION < 220
   setFont( _resources->getAppFont(), true );
#endif
   _font = _resources->getTextFont();
   
   // Read in the file names.
   std::string filenames[3];
   std::string displayFilenames[3];
   bool isTemporary[3] = { false, false, false };
   _nbFiles = readFileNames( 
      argc, argv, filenames, displayFilenames, isTemporary, _filesAreDirectories
   );
   _nbTextWidgets = _nbFiles;
   if ( _nbTextWidgets == 0 ) { // Create the two widgets anyway.
      _nbTextWidgets = 2;
   }

#ifdef DONT_SUPPORT_DIRECTORY_DIFFS
   if ( _filesAreDirectories == true ) {
      std::cout << "Directory diffs detected, not yet supported." << std::endl;
      // Directory diffs not yet supported.
      throw new XxUsageError;
   }
#endif

   // Check for odd number of files.
   if ( _nbFiles != 0 && _nbFiles != 2 && _nbFiles != 3 ) {
      throw new XxUsageError;
   }

   // Create the interface.
   createUI( _nbTextWidgets );

   // Add some context help.
   createOnContextHelp();

   // Synchronize the created UI with the resources.
   synchronizeUI();

   if ( _nbFiles != 0 ) {
      for ( int ii = 0; ii < _nbFiles; ++ii ) {
         readFile(
            ii,
            filenames[ii].c_str(),
            displayFilenames[ii].c_str(),
            isTemporary[ii] 
         );
      }
      bool succ = processDiff();

      // Initialize the horizontal diffs if necessary.
      if ( succ == true &&
           _resources->getBoolOpt( XxResources::HORIZONTAL_DIFFS ) == true ) {
         _diffs->initializeHorizontalDiffs( getFiles() );
      }
   }

   // Sets the title bar.
   if ( _nbFiles == 2 ) {
      QString str = 
         QString(_files[0]->getName()) + " <-> " + 
         QString(_files[1]->getName());
      _mainWindow->setCaption( str );
   }
   else {
      _mainWindow->setCaption( "xxdiff" );
   }

   // Resize the main window before showing.
   const QRect& psize = _resources->getPreferredGeometry();
   _mainWindow->resize( psize.size() );

   // Call post creation actions.
   emit postCreationAction();

   // Show the main window.
   _mainWindow->show();

   // Note: positioning has to be done after show().  This results in some
   // flickering, but there's nothing we can do about it.
   if ( psize.topLeft() != QPoint( -1, -1 ) ) {
      _mainWindow->move( psize.topLeft() );
   }
}

//------------------------------------------------------------------------------
//
XxApp::~XxApp()
{
   if ( _socketNotifier != 0 ) {
      close( _sockfd );
   }
   delete _stringResParser;

   XxResources::releaseInstance();
}

//------------------------------------------------------------------------------
//
uint XxApp::readFileNames( 
   int 		argc, 
   char** 	argv,
   std::string 	filenames[3],
   std::string 	displayFilenames[3],
   bool		created[3],
   bool&	filesAreDirectories
) const
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
   for ( int ii = 0; ii < 3; ++ii ) {
      if ( argc > ii ) {
         if ( strcmp( argv[ii], "-" ) == 0 ) {
            if ( stdinUsed == true ) {
               // Cannot have two filenames from stdin.
               throw new XxUsageError();
            }
            stdinUsed = true;
            isDirectory[ii] = false;
         }
         else {
            isDirectory[ii] = XxUtil::isDirectory( argv[ii] );
            if ( ! isDirectory[ii] ) {
               filePathIndex = ii;
               nbPathFiles++;
            }
         }
         nbFiles++;
      }
   }

   if ( stdinUsed == true && nbPathFiles == 0 ) {
      // stdin was specified and all other paths are directories.
      throw new XxUsageError();
   }

   if ( nbPathFiles > 1 && nbFiles > ( nbPathFiles + (stdinUsed ? 1 : 0) ) ) {
      // There are more than one named file AND one of the files is a directory.
      throw new XxUsageError();
   }

   filesAreDirectories = ( nbFiles > 0 && nbPathFiles == 0 );

   // Find basename and append to directories.
   char* bn = 0;
   if ( !filesAreDirectories && nbPathFiles < nbFiles ) {
      XX_ASSERT( filePathIndex != -1 );
      bn = basename( argv[ filePathIndex ] );
   }

   bool stdinCopied = false;
   for ( int ii = 0; ii < 3; ++ii ) {
      if ( argc > ii ) {
         XX_ASSERT( argv[ii] );

         if ( strcmp( argv[ii], "-" ) == 0 ) {
            XX_ASSERT( stdinCopied == false );
            stdinCopied = true;
            
            displayFilenames[ii] = _stdinFilename;
            char* temporaryFilename = tempnam( 0, "xxdif" );
            filenames[ii] = temporaryFilename;
            free( temporaryFilename );
            created[ii] = true;
            XxUtil::copyToFile( stdin, filenames[ii].c_str() );
         }
         else {
            std::string fname = argv[ii];
            if ( !filesAreDirectories && isDirectory[ii] == true ) {
               // Append basename to directory.
               fname.append( "/" );
               XX_ASSERT( bn != 0 );
               fname.append( bn );
            }
            displayFilenames[ii] = fname;
            filenames[ii] = fname;
            bool isdir;
            XxUtil::testFile( filenames[ii].c_str(), isdir );
         }

         if ( ! _userFilenames[ii].empty() ) {
            displayFilenames[ii] = _userFilenames[ii];
         }
      }
   }

   return nbFiles;
}

//------------------------------------------------------------------------------
//
void XxApp::createUI( uint nbTextWidgets )
{
   XX_ASSERT( _resources != 0 );

   _mainWindow = new XxMainWindow( this );

   //
   // Create widgets
   //
   _centralWidget = new QWidget( _mainWindow );
   QHBoxLayout* topLayout = new QHBoxLayout( _centralWidget );

   QVBoxLayout* textAndSbLayout = new QVBoxLayout( topLayout );
   QHBoxLayout* textLayout = new QHBoxLayout( textAndSbLayout );

   QFont smaller = _resources->getAppFont();
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

      if ( _resources->getBoolOpt( XxResources::SHOW_FILENAMES ) == false ) {
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
   if ( _resources->getBoolOpt( XxResources::SHOW_OVERVIEW ) == false ) {
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

   a->connectItem( a->insertItem(Key_Delete), 
                   this, SLOT(selectRegionNeither()) ); 

   // Make some connections.
   connect( this, SIGNAL(cursorChanged(int)), this, SLOT(repaintTexts()) );
   connect( this, SIGNAL(cursorChanged(int)), 
            this, SLOT(updateLineNumberLabels(int)) );

   //
   // Create toolbar
   //
   _toolbar = createToolbar();
   XX_ASSERT( _toolbar != 0 );
   _toolbar->hide();

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
      (const char**)previous_unselected_difference_xpm 
   );
   new QToolButton( 
      pm_previous_unselected_difference, 
      "Previous unselected difference", 
      "Previous unselected difference", 
      this, SLOT(previousUnselected()), toolbar 
   );
   
   QPixmap pm_previous_difference( 
      (const char**)previous_difference_xpm 
   );
   new QToolButton( 
      pm_previous_difference, 
      "Previous difference", 
      "Previous difference", 
      this, SLOT(previousDifference()), toolbar 
   );

   QPixmap pm_next_difference( 
      (const char**)next_difference_xpm
   );
   new QToolButton(
      pm_next_difference, 
      "Next difference",
      "Next difference", 
      this, SLOT(nextDifference()), toolbar
   );

   QPixmap pm_next_unselected_difference( 
      (const char**) next_unselected_difference_xpm 
   );
   new QToolButton( 
      pm_next_unselected_difference, 
      "Next unselected difference", 
      "Next unselected difference", 
      this, SLOT(nextUnselected()), toolbar 
   );

   toolbar->addSeparator();

   QPixmap pm_search( 
      (const char**) search_xpm 
   );
   new QToolButton( 
      pm_search, 
      "Search", 
      "Search", 
      this, SLOT(search()), toolbar 
   );
      
   QPixmap pm_search_forward( 
      (const char**) search_forward_xpm 
   );
   new QToolButton( 
      pm_search_forward,
      "Search forward", 
      "Search forward", 
      this, SLOT(searchForward()), toolbar 
   );
      
   QPixmap pm_search_backward( 
      (const char**) search_backward_xpm 
   );
   new QToolButton( 
      pm_search_backward,
      "Search backward", 
      "Search backward", 
      this, SLOT(searchBackward()), toolbar 
   );
      
   if ( _filesAreDirectories == false ) {
      toolbar->addSeparator();
      
      QPixmap pm_select_region_left( 
         (const char**) select_region_left_xpm
      );
      new QToolButton( 
         pm_select_region_left,
         "Select region left", 
         "Select region left", 
         this, SLOT(selectRegionLeft()), toolbar 
      );
      
      if ( _nbFiles == 3 ) {
         QPixmap pm_select_region_middle( 
            (const char**) select_region_middle_xpm
         );
         new QToolButton( 
            pm_select_region_middle,
            "Select region middle", 
            "Select region middle", 
            this, SLOT(selectRegionMiddle()), toolbar 
         );
      }

      QPixmap pm_select_region_right( 
         (const char**) select_region_right_xpm
      );
      new QToolButton( 
         pm_select_region_right,
         "Select region right", 
         "Select region right", 
         this, SLOT(selectRegionRight()), toolbar 
      );

      QPixmap pm_select_region_neither( 
         (const char**) select_region_neither_xpm
      );
      new QToolButton( 
         pm_select_region_neither,
         "Select region neither", 
         "Select region neither", 
         this, SLOT(selectRegionNeither()), toolbar 
      );

      QPixmap pm_select_region_unselect( 
         (const char**) select_region_unselect_xpm
      );
      new QToolButton( 
         pm_select_region_unselect,
         "Unselect region", 
         "Unselect region", 
         this, SLOT(selectRegionUnselect()), toolbar 
      );

      toolbar->addSeparator();

      QPixmap pm_split_swap_join( 
         (const char**) split_swap_join_xpm
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
         (const char**) diff_files_xpm
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
      "Open left...", this, SLOT(openLeft()), 
      _resources->getAccelerator( XxResources::ACCEL_OPEN_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      fileMenu->insertItem( 
         "Open middle...", this, SLOT(openMiddle()), 
         _resources->getAccelerator( XxResources::ACCEL_OPEN_MIDDLE ) 
      );
   }
   fileMenu->insertItem( 
      "Open right...", this, SLOT(openRight()), 
      _resources->getAccelerator( XxResources::ACCEL_OPEN_RIGHT ) 
   );
   fileMenu->insertSeparator();
   fileMenu->insertItem( 
      "Save as left", this, SLOT(saveAsLeft()), 
      _resources->getAccelerator( XxResources::ACCEL_SAVE_AS_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      fileMenu->insertItem( 
         "Save as middle", this, SLOT(saveAsMiddle()), 
         _resources->getAccelerator( XxResources::ACCEL_SAVE_AS_MIDDLE ) 
      );
   }
   fileMenu->insertItem( 
      "Save as right", this, SLOT(saveAsRight()), 
      _resources->getAccelerator( XxResources::ACCEL_SAVE_AS_RIGHT ) 
   );
   fileMenu->insertItem( 
      "Save as...", this, SLOT(saveAs()), 
      _resources->getAccelerator( XxResources::ACCEL_SAVE_AS ) 
   );
   fileMenu->insertItem( 
      "Save selected only...", this, SLOT(saveSelectedOnly()), 
      _resources->getAccelerator( XxResources::ACCEL_SAVE_SELECTED_ONLY ) 
   );
   fileMenu->insertSeparator();
   fileMenu->insertItem( 
      "Redo diff", this, SLOT(redoDiff()), 
      _resources->getAccelerator( XxResources::ACCEL_REDO_DIFF ) 
   );
   fileMenu->insertSeparator();
   fileMenu->insertItem( 
      "Edit left file", this, SLOT(editLeft()), 
     _resources->getAccelerator( XxResources::ACCEL_EDIT_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      fileMenu->insertItem( 
         "Edit middle file", this, SLOT(editMiddle()), 
         _resources->getAccelerator( XxResources::ACCEL_EDIT_MIDDLE ) 
      );
   }
   fileMenu->insertItem( 
      "Edit right file", this, SLOT(editRight()), 
      _resources->getAccelerator( XxResources::ACCEL_EDIT_RIGHT ) 
   );
   fileMenu->insertSeparator();

   fileMenu->insertItem( 
      "Exit", this, SLOT(quit()), 
      _resources->getAccelerator( XxResources::ACCEL_EXIT ) 
   );

   //---------------------------------------------------------------------------

   // Edit menu
   QPopupMenu* editMenu = new QPopupMenu;
   editMenu->insertItem( 
      "Search...", this, SLOT(search()),
      _resources->getAccelerator( XxResources::ACCEL_SEARCH ) 
   );
   editMenu->insertItem( 
      "Search forward", this, SLOT(searchForward()), 
      _resources->getAccelerator( XxResources::ACCEL_SEARCH_FORWARD ) 
   );
   editMenu->insertItem( 
      "Search backward", this, SLOT(searchBackward()), 
      _resources->getAccelerator( XxResources::ACCEL_SEARCH_BACKWARD ) 
   );
   editMenu->insertSeparator();
   editMenu->insertItem( 
      "Scroll down", this, SLOT(scrollDown()), 
      _resources->getAccelerator( XxResources::ACCEL_SCROLL_DOWN ) 
   );
   editMenu->insertItem( 
      "Scroll up", this, SLOT(scrollUp()), 
      _resources->getAccelerator( XxResources::ACCEL_SCROLL_UP ) 
   );
   editMenu->insertItem( 
      "Cursor down", this, SLOT(cursorDown()), 
      _resources->getAccelerator( XxResources::ACCEL_CURSOR_DOWN ) 
   );
   editMenu->insertItem( 
      "Cursor up", this, SLOT(cursorUp()), 
      _resources->getAccelerator( XxResources::ACCEL_CURSOR_UP ) 
   );
   editMenu->insertItem( 
      "Cursor top", this, SLOT(cursorTop()), 
      _resources->getAccelerator( XxResources::ACCEL_CURSOR_TOP ) 
   );
   editMenu->insertItem( 
      "Cursor bottom", this, SLOT(cursorBottom()), 
      _resources->getAccelerator( XxResources::ACCEL_CURSOR_BOTTOM ) 
   );

   //---------------------------------------------------------------------------

   // View menu
   QPopupMenu* viewMenu = new QPopupMenu;
   if ( _filesAreDirectories == true ) {
      viewMenu->insertItem( 
         "Diff files at cursor", this, SLOT(diffFilesAtCursor()),
         _resources->getAccelerator( XxResources::ACCEL_DIFF_FILES_AT_CURSOR )
      );
      viewMenu->insertSeparator();
   }
   viewMenu->insertItem( 
      "Next difference", this, SLOT(nextDifference()),
      _resources->getAccelerator( XxResources::ACCEL_NEXT_DIFFERENCE ) 
   );
   viewMenu->insertItem( 
      "Previous difference", this, SLOT(previousDifference()),
      _resources->getAccelerator( XxResources::ACCEL_PREVIOUS_DIFFERENCE ) 
   );
   viewMenu->insertSeparator();
   viewMenu->insertItem( 
      "Next unselected", this, SLOT(nextUnselected()),
      _resources->getAccelerator( XxResources::ACCEL_NEXT_UNSELECTED ) 
   );
   viewMenu->insertItem( 
      "Previous unselected", this, SLOT(previousUnselected()),
      _resources->getAccelerator( XxResources::ACCEL_PREVIOUS_UNSELECTED ) 
   );
   _viewPopup = viewMenu;

   //---------------------------------------------------------------------------

   // Global menu
   QPopupMenu* globalMenu = new QPopupMenu;
   globalMenu->insertItem( 
      "Select left", this, SLOT(selectGlobalLeft()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_GLOBAL_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      globalMenu->insertItem( 
         "Select middle", this, SLOT(selectGlobalMiddle()),
         _resources->getAccelerator( XxResources::ACCEL_SELECT_GLOBAL_MIDDLE ) 
      );
   }
   globalMenu->insertItem( 
      "Select right", this, SLOT(selectGlobalRight()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_GLOBAL_RIGHT ) 
   );
   globalMenu->insertItem( 
      "Select neither", this, SLOT(selectGlobalNeither()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_GLOBAL_NEITHER )
   );
   globalMenu->insertItem( 
      "Unselect", this, SLOT(selectGlobalUnselect()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_GLOBAL_UNSELECT)
   );
   globalMenu->insertSeparator();
   globalMenu->insertItem( 
      "Select unselected left", this, SLOT(selectGlobalUnselectedLeft()),
      _resources->getAccelerator( 
         XxResources::ACCEL_SELECT_GLOBAL_UNSELECTED_LEFT 
      ) 
   );
   if ( _nbFiles == 3 ) {
      globalMenu->insertItem( 
         "Select unselected middle", this, SLOT(selectGlobalUnselectedMiddle()),
         _resources->getAccelerator( 
            XxResources::ACCEL_SELECT_GLOBAL_UNSELECTED_MIDDLE 
         ) 
      );
   }
   globalMenu->insertItem( 
      "Select unselected right", this, SLOT(selectGlobalUnselectedRight()),
      _resources->getAccelerator( 
         XxResources::ACCEL_SELECT_GLOBAL_UNSELECTED_RIGHT 
      ) 
   );
   globalMenu->insertItem( 
      "Select unselected neither", this, SLOT(selectGlobalUnselectedNeither()),
      _resources->getAccelerator(
         XxResources::ACCEL_SELECT_GLOBAL_UNSELECTED_NEITHER 
      )
   );
   globalMenu->insertSeparator();
   globalMenu->insertItem( 
      "Merge", this, SLOT(selectGlobalMerge()),
      _resources->getAccelerator(
         XxResources::ACCEL_SELECT_GLOBAL_MERGE
      ) 
   );

   //---------------------------------------------------------------------------

   // Region menu
   QPopupMenu* regionMenu = new QPopupMenu;
   regionMenu->insertItem( 
      "Select left", this, SLOT(selectRegionLeft()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_REGION_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      regionMenu->insertItem( 
         "Select middle", this, SLOT(selectRegionMiddle()),
         _resources->getAccelerator( XxResources::ACCEL_SELECT_REGION_MIDDLE ) 
      );
   }
   regionMenu->insertItem( 
      "Select right", this, SLOT(selectRegionRight()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_REGION_RIGHT ) 
   );
   regionMenu->insertItem( 
      "Select neither", this, SLOT(selectRegionNeither()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_REGION_NEITHER )
   );
   regionMenu->insertItem( 
      "Unselect", this, SLOT(selectRegionUnselect()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_REGION_UNSELECT)
   );
   regionMenu->insertSeparator();
   regionMenu->insertItem( 
      "Select left and next", this,
      SLOT(selectRegionLeftAndNext()),
      _resources->getAccelerator(
         XxResources::ACCEL_SELECT_REGION_LEFT_AND_NEXT
      ) 
   );
   if ( _nbFiles == 3 ) {
      regionMenu->insertItem( 
         "Select middle and next", this,
         SLOT(selectRegionMiddleAndNext()),
         _resources->getAccelerator(
            XxResources::ACCEL_SELECT_REGION_MIDDLE_AND_NEXT
         ) 
      );
   }
   regionMenu->insertItem( 
      "Select right and next", this, 
      SLOT(selectRegionRightAndNext()),
      _resources->getAccelerator(
         XxResources::ACCEL_SELECT_REGION_RIGHT_AND_NEXT
      ) 
   );
   regionMenu->insertItem( 
      "Select neither and next", this, 
      SLOT(selectRegionNeitherAndNext()),
      _resources->getAccelerator(
         XxResources::ACCEL_SELECT_REGION_NEITHER_AND_NEXT
      )
   );
   regionMenu->insertSeparator();
   regionMenu->insertItem( 
      "Split/swap/join", this, SLOT(regionSplitSwapJoin()),
      _resources->getAccelerator( 
         XxResources::ACCEL_SELECT_REGION_SPLIT_SWAP_JOIN
      )
   );

   //---------------------------------------------------------------------------

   // Line menu
   QPopupMenu* lineMenu = new QPopupMenu;
   lineMenu->insertItem( 
      "Select left", this, SLOT(selectLineLeft()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_LINE_LEFT ) 
   );
   if ( _nbFiles == 3 ) {
      lineMenu->insertItem( 
         "Select middle", this, SLOT(selectLineMiddle()),
         _resources->getAccelerator( XxResources::ACCEL_SELECT_LINE_MIDDLE ) 
      );
   }
   lineMenu->insertItem( 
      "Select right", this, SLOT(selectLineRight()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_LINE_RIGHT ) 
   );
   lineMenu->insertItem( 
      "Select neither", this, SLOT(selectLineNeither()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_LINE_NEITHER )
   );
   lineMenu->insertItem( 
      "Unselect", this, SLOT(selectLineUnselect()),
      _resources->getAccelerator( XxResources::ACCEL_SELECT_LINE_UNSELECT)
   );

   //---------------------------------------------------------------------------

   // Options menu
   _optionsMenu = new QPopupMenu;

   _optionsMenu->insertItem( 
      "Edit options...", this, SLOT(editOptions()), 
      _resources->getAccelerator( XxResources::ACCEL_DIFF_ARGUMENTS ) 
   );

   if ( _filesAreDirectories == false ) {

      if ( _nbFiles == 2 ) {
         _optionsMenu->insertSeparator();

         _menuids[ ID_ToggleIgnoreTrailing ] = _optionsMenu->insertItem( 
            "Ignore trailing blanks", this, SLOT(ignoreTrailing()),
            _resources->getAccelerator( XxResources::ACCEL_IGNORE_TRAILING )
         );
         _menuids[ ID_ToggleIgnoreWhitespace ] = _optionsMenu->insertItem( 
            "Ignore whitespace", this, SLOT(ignoreWhitespace()),
            _resources->getAccelerator( XxResources::ACCEL_IGNORE_WHITESPACE )
         );
         _menuids[ ID_ToggleIgnoreCase ] = _optionsMenu->insertItem( 
            "Ignore case", this, SLOT(ignoreCase()),
            _resources->getAccelerator( XxResources::ACCEL_IGNORE_CASE )
         ); 
         _menuids[ ID_ToggleIgnoreBlankLines ] = _optionsMenu->insertItem( 
            "Ignore blank lines", this, SLOT(ignoreBlankLines()),
            _resources->getAccelerator( XxResources::ACCEL_IGNORE_BLANK_LINES )
         );
         _optionsMenu->setItemEnabled( 
            _menuids[ ID_ToggleIgnoreBlankLines ], false 
         );

         _optionsMenu->insertSeparator();

         _menuids[ ID_ToggleQualityNormal ] = _optionsMenu->insertItem( 
            "Quality: normal", this, SLOT(qualityNormal()),
            _resources->getAccelerator( XxResources::ACCEL_QUALITY_NORMAL )
         );
         _menuids[ ID_ToggleQualityFastest ] = _optionsMenu->insertItem( 
            "Quality: fastest", this, SLOT(qualityFastest()),
            _resources->getAccelerator( XxResources::ACCEL_QUALITY_FASTEST )
         );
         _menuids[ ID_ToggleQualityHighest ] = _optionsMenu->insertItem( 
            "Quality: highest", this, SLOT(qualityHighest()),
            _resources->getAccelerator( XxResources::ACCEL_QUALITY_HIGHEST )
         );
      }
   }
   else {
      _optionsMenu->insertSeparator();

      _menuids[ ID_ToggleDirDiffsRecursive ] = _optionsMenu->insertItem(
         "Recursive", this, SLOT(dirDiffRecursive()),
         _resources->getAccelerator( 
            XxResources::ACCEL_DIRDIFF_RECURSIVE
         )
      );
   }

   _optionsMenu->setCheckable( true );

   //---------------------------------------------------------------------------

   _displayMenu = new QPopupMenu;

   if ( _filesAreDirectories == false ) {
      _menuids[ ID_ToggleHorizontalDiffs ] = _displayMenu->insertItem( 
         "Horizontal diffs", this, SLOT(toggleHorizontalDiffs()),
         _resources->getAccelerator( 
            XxResources::ACCEL_TOGGLE_HORIZONTAL_DIFFS 
         )
      );
      
      _menuids[ ID_ToggleIgnoreHorizontalWs ] = _displayMenu->insertItem( 
         "Ignore horizontal whitespace", this, SLOT(toggleIgnoreHorizontalWs()),
         _resources->getAccelerator( 
            XxResources::ACCEL_TOGGLE_IGNORE_HORIZONTAL_WS
         )
      );
      
      _menuids[ ID_ToggleHideCarriageReturns ] = _displayMenu->insertItem( 
         "Hide carriage returns", this, SLOT(hideCarriageReturns()),
         _resources->getAccelerator( XxResources::ACCEL_HIDE_CR )
      );
   }
   else {
      _menuids[ ID_ToggleDirDiffsIgnoreFileChanges ] = _displayMenu->insertItem(
         "Ignore file changes", this, SLOT(ignoreFileChanges()),
         _resources->getAccelerator( 
            XxResources::ACCEL_DIRDIFF_IGNORE_FILE_CHANGES
         )
      );
   }

   _displayMenu->insertSeparator();
   _menuids[ ID_ToggleCutAndPasteAnnotations ] = _displayMenu->insertItem( 
      "Cut-and-paste annotations", this, 
      SLOT(toggleCutAndPasteAnnotations()),
      _resources->getAccelerator( 
         XxResources::ACCEL_TOGGLE_CUT_AND_PASTE_ANNOTATIONS 
      )
   );

   if ( _filesAreDirectories == false ) {

      _displayMenu->insertSeparator();
      
      _displayMenu->insertItem( 
         "Set tab width...", this, SLOT(setTabWidth()),
         _resources->getAccelerator( XxResources::ACCEL_SET_TAB_WIDTH )
      );
      _menuids[ ID_TabsAtFour ] = _displayMenu->insertItem( 
         "Tabs at 4", this, SLOT(tabsAt4()),
         _resources->getAccelerator( XxResources::ACCEL_TABS_AT_4 )
      );
      _menuids[ ID_TabsAtEight ] = _displayMenu->insertItem( 
         "Tabs at 8", this, SLOT(tabsAt8()),
         _resources->getAccelerator( XxResources::ACCEL_TABS_AT_8 )
      );
   }

   _displayMenu->setCheckable( true );

   //---------------------------------------------------------------------------

   // Windows menu
   _windowsMenu = new QPopupMenu;
   if ( _filesAreDirectories == false ) {
      _windowsMenu->insertItem( 
         "Merged view...", this, SLOT(mergedView()),
         _resources->getAccelerator( XxResources::ACCEL_MERGED_VIEW )
      );
      _windowsMenu->insertSeparator();
   }
   _menuids[ ID_ToggleToolbar ] = _windowsMenu->insertItem( 
      "Toggle toolbar", this, 
      SLOT(toggleToolbar()),
      _resources->getAccelerator( XxResources::ACCEL_TOGGLE_TOOLBAR )
   );
   _menuids[ ID_ToggleLineNumbers ] = _windowsMenu->insertItem( 
      "Toggle line numbers", this, SLOT(toggleLineNumbers()),
      _resources->getAccelerator( XxResources::ACCEL_TOGGLE_LINE_NUMBERS )
   );
   _menuids[ ID_ToggleShowMarkers ] = _windowsMenu->insertItem( 
      "Toggle show markers", this, SLOT(toggleShowMarkers()),
      _resources->getAccelerator( XxResources::ACCEL_TOGGLE_MARKERS )
   );
   _windowsMenu->setItemEnabled( _menuids[ ID_ToggleShowMarkers ], false );
   _menuids[ ID_ToggleOverview ] = _windowsMenu->insertItem( 
      "Toggle overview", this, SLOT(toggleOverview()),
      _resources->getAccelerator( XxResources::ACCEL_TOGGLE_OVERVIEW )
   );
   _menuids[ ID_ToggleShowFilenames ] = _windowsMenu->insertItem( 
      "Toggle show filename", this, SLOT(toggleShowFilenames()),
      _resources->getAccelerator( XxResources::ACCEL_TOGGLE_SHOW_FILENAMES )
   );
   _windowsMenu->setCheckable( true );

   //---------------------------------------------------------------------------

   // Help menu
   QPopupMenu* helpMenu = new QPopupMenu;
   helpMenu->insertItem( 
      "Man page...", this, SLOT(helpManPage()),
      _resources->getAccelerator( XxResources::ACCEL_HELP_MAN_PAGE )
   );
   helpMenu->insertItem( 
      "Color legend...", this, SLOT(helpColorLegend()),
      _resources->getAccelerator( XxResources::ACCEL_HELP_COLOR_LEGEND )
   );
   helpMenu->insertItem( 
      "On context", _mainWindow, SLOT(whatsThis()),
      _resources->getAccelerator( XxResources::ACCEL_HELP_ON_CONTEXT )
   );
   helpMenu->insertItem( 
      "Generate init file...", this, SLOT(helpGenInitFile()),
      _resources->getAccelerator( XxResources::ACCEL_HELP_GEN_INIT_FILE )
   );
   helpMenu->insertSeparator();
   helpMenu->insertItem( 
      "About...", this, SLOT(helpAbout()),
      _resources->getAccelerator( XxResources::ACCEL_HELP_ABOUT )
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
   const int   no, 
   const char* filename,
   const char* displayFilename,
   bool        isTemporary
)
{
   XX_ASSERT( 0 <= no && no <= 2 );
   XX_ASSERT( filename && displayFilename );

   //
   // Read in the file.
   //
   try {
      if ( _filesAreDirectories &&
           _resources->getBoolOpt( XxResources::DIRDIFF_BUILD_FROM_OUTPUT ) ) {
         // Assign an empty buffer. The directory diffs builder will fill it in
         // with whatever contents are read from directory diffs command output.
         _files[no].reset( new XxBuffer( false, filename, displayFilename ) );
      }
      else {
         _files[no].reset( 
            new XxBuffer( 
               filename, 
               displayFilename, 
               _resources->getBoolOpt( XxResources::HIDE_CR ),
               isTemporary 
            )
         );
      }
   }
   catch ( std::exception* ex ) {
      std::ostringstream oss;
      oss << "Error loading in file:" << std::endl
          << ex->what() << std::endl << std::ends;
      outputDiffErrors( oss.str().c_str() );
      delete ex;
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
void XxApp::reReadFile( const int no )
{
   XX_ASSERT( 0 <= no && no <= 2 );

   //
   // Reread the file.
   //
   try {      
      std::auto_ptr<XxBuffer> oldfile = _files[no];

      if ( _filesAreDirectories &&
           _resources->getBoolOpt( XxResources::DIRDIFF_BUILD_FROM_OUTPUT ) ) {
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
               _resources->getBoolOpt( XxResources::HIDE_CR ),
               oldfile->isTemporary()
            )
         );
      }
   }
   catch ( std::exception* ex ) {
      std::ostringstream oss;
      oss << "Error loading in file:" << std::endl
          << ex->what() << std::endl << std::ends;
      outputDiffErrors( oss.str().c_str() );
      delete ex;
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
   for ( int ii = 0; ii < _nbFiles; ++ii ) {
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
         XxBuilderFiles2 builder;
         try {
            std::auto_ptr<XxDiffs> tmp(
               builder.process(
                  _resources->getCommand( XxResources::COMMAND_DIFF_FILES_2 ),
                  _files[0]->getName(), _files[0]->getNbLines(),
                  _files[1]->getName(), _files[1]->getNbLines()
               )
            );
            _diffs = tmp;
            XX_ASSERT( _diffs.get() != 0 );
         }
         catch ( std::exception* ex ) {
            std::ostringstream oss;
            oss << "Error executing \"" 
                << _resources->getCommand( XxResources::COMMAND_DIFF_FILES_2 )
                << "\" command, couldn't build diffs:" << std::endl
                << ex->what() << std::endl << std::ends;
            outputDiffErrors( oss.str().c_str() );
            delete ex;
            return false;
         }
      
         // Note: the return value of xxdiff matches the one that the spawned
         // diff returns.  e.g. a return value of 1 does not indicate an error.
         _returnValue = builder.getStatus();

         // If there were some warnings, output them.
         if ( !_resources->getBoolOpt( XxResources::IGNORE_ERRORS ) &&
              builder.hasErrors() ) {
            outputDiffErrors( builder.getErrors() );
         }
      }
      else if ( _nbFiles == 3 ) {
         XxBuilderFiles3 builder;
         try {
            std::auto_ptr<XxDiffs> tmp(
               builder.process(
                  _resources->getCommand( XxResources::COMMAND_DIFF_FILES_3 ),
                  _files[0]->getName(), _files[0]->getNbLines(),
                  _files[1]->getName(), _files[1]->getNbLines(),
                  _files[2]->getName(), _files[2]->getNbLines()
               )
            );
            _diffs = tmp;
            XX_ASSERT( _diffs.get() != 0 );
         }
         catch ( std::exception* ex ) {
            std::ostringstream oss;
            oss << "Error executing \"" 
                << _resources->getCommand( XxResources::COMMAND_DIFF_FILES_3 )
                << "\" command, couldn't build diffs:" << std::endl
                << ex->what() << std::endl << std::ends;
            outputDiffErrors( oss.str().c_str() );
            delete ex;
            return false;
         }
      
         // Note: the return value of xxdiff matches the one that the spawned
         // diff returns.  e.g. a return value of 1 does not indicate an error.
         _returnValue = builder.getStatus();

         // If there were some errors, output them.
         if ( !_resources->getBoolOpt( XxResources::IGNORE_ERRORS ) &&
              builder.hasErrors() ) {
            outputDiffErrors( builder.getErrors() );
         }
      }
   }
   else {
      XX_ASSERT( _nbFiles == 2 );
      XxBuilderDirs2 builder(
         _resources->getBoolOpt( XxResources::DIRDIFF_BUILD_FROM_OUTPUT ),
         _resources->getBoolOpt( XxResources::DIRDIFF_RECURSIVE ),
         _resources->getBoolOpt( XxResources::DIRDIFF_IGNORE_FILE_CHANGES )
      );
      const char* dirdiff_command = 0;
      try {
         if ( _resources->getBoolOpt( XxResources::DIRDIFF_RECURSIVE ) ) {
            dirdiff_command = _resources->getCommand(
               XxResources::COMMAND_DIFF_DIRECTORIES_REC
            );
         }
         else {
            dirdiff_command = _resources->getCommand(
               XxResources::COMMAND_DIFF_DIRECTORIES
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
      catch ( std::exception* ex ) {
         std::ostringstream oss;
         oss << "Error executing \"" 
             << dirdiff_command
             << "\" command, couldn't build diffs:" << std::endl
             << ex->what() << std::endl << std::ends;
         outputDiffErrors( oss.str().c_str() );
         delete ex;
         return false;
      }
      
      // Note: the return value of xxdiff matches the one that the spawned
      // diff returns.  e.g. a return value of 1 does not indicate an error.
      _returnValue = builder.getStatus();

         // If there were some warnings, output them.
      if ( !_resources->getBoolOpt( XxResources::IGNORE_ERRORS ) &&
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
      for ( int ii = 0; ii < _nbFiles; ++ii ) {
         if ( _files[ii]->getNbLines() > _diffs->getNbLines() ) {
            throw new XxInternalError( XX_INTERROR_PARAMS );
         }
      }
      
      // Reset update connections and adjust scrollbars.
      if ( _diffs.get() != 0 ) {
         connect( _diffs.get(), SIGNAL(changed()), this, SLOT(repaintTexts()) );
      }

      // Note: this should also be done, but more efficiently for each loaded
      // file separately.
      _textWidth = computeTextWidth();
      adjustScrollbars( true );
   }

   return _returnValue == 2 || _diffs.get() != 0;
}

//------------------------------------------------------------------------------
//
void XxApp::parseCommandLine( int& argc, char**& argv )
{
   // The command-line parsing has to be carried out before the resource
   // building because some of the command-line options affect the way resources
   // are built up (e.g. --no-rcfile).  So command-line options that should
   // supersede resources and explicit resources specified on the command-line
   // are stored in the stringResParser, which is a resource parser that is
   // intergrated after the other resource mechanisms, thus taking precedence
   // over those.

   // Initialize cmdline-related variables.
   _userFilenames[0] = "";
   _userFilenames[1] = "";
   _userFilenames[2] = "";
   _stdinFilename = "(stdin)";
   _useXrm = true;
   _useRcfile = true;
   _extraDiffArgs = "";
   _sepDiff = false;

   int optionIndex = 0; 
   struct option longOptions[] = { 
      { "help", 0, 0, 'h' }, 
      { "version", 0, 0, 'v' }, 
      { "no-xrm", 0, 0, 'x' }, 
      { "no-rcfile", 0, 0, 'r' }, 
      { "list-resources", 0, 0, 'l' }, 
      { "exit-on-same", 0, 0, 'D' }, 
      { "ignore-whitespace", 0, 0, 'w' }, 
      { "ignore-trailing-blanks", 0, 0, 'b' }, 
      { "ignore-case", 0, 0, 'i' }, 
      { "merge", 0, 0, 'm' }, 
      { "title1", 1, 0, '1' }, 
      { "title2", 1, 0, '2' }, 
      { "title3", 1, 0, '3' },
      { "titlein", 1, 0, 'N' }, // This is kept for xdiff compatibility only.
      { "resource", 1, 0, 'R' },
      { 0, 0, 0, 0 }
   }; 
   
   // Do the parsing.
   while ( true ) {
      int c = getopt_long( argc, argv, "hvDwbimN:", longOptions, &optionIndex );
      if ( c == -1 ) {
         break;
      }
              
      switch ( c ) {
         case 'h':
            throw new XxUsageError( true );

         case 'v':
            throw new XxUsageError( true, true );

         case 'x':
            _useXrm = false;
            break;

         case 'r':
            _useRcfile = false;
            break;

         case 'l':
            // This lists the default resources, not the ones parsed from the
            // rcfile or xrm or command-line.
            _resources->listResources( std::cout );
            ::exit( 0 );
            break;

         case 'D':
            if ( _stringResParser == 0 ) {
               _stringResParser = new XxStringResParser; 
            }
            _stringResParser->addString( std::string("exitOnSame:true") );
            break;

         case 'w':
         case 'b':
         case 'i':
            _extraDiffArgs.append( " -" );
            char optionString[2];
            optionString[0] = c;
            optionString[1] = 0;
            _extraDiffArgs.append( optionString );
            break;

         case 'm':
            connect( this, SIGNAL(postCreationAction()),
                     this, SLOT(selectGlobalMerge()) );
            break;

         case '1':
            if ( !optarg ) {
               throw new XxUsageError;
            }
            _userFilenames[0] = optarg;
            break;

         case '2':
            if ( !optarg ) {
               throw new XxUsageError;
            }
            _userFilenames[1] = optarg;
            break;

         case '3':
            if ( !optarg ) {
               throw new XxUsageError;
            }
            _userFilenames[2] = optarg;
            break;

         case 'N':
            if ( !optarg ) {
               throw new XxUsageError;
            }
            _stdinFilename = optarg;
            break;

         case 'R':
            if ( !optarg ) {
               throw new XxUsageError;
            }
            if ( _stringResParser == 0 ) {
               _stringResParser = new XxStringResParser; 
            }
            // Keep it for later.
            _stringResParser->addString( optarg );
            break;

         case 0:
            throw new XxInternalError( XX_INTERROR_PARAMS );

         case ':':
         case '?':
            throw new XxUsageError;
            
         default:
            throw new XxUsageError;
      }
   }

   argc -= optind;
   argv += optind;
}


//------------------------------------------------------------------------------
//
void XxApp::buildResources()
{
   // Note: the UI hasn't been built at this point.
#ifdef XX_USE_XRM
   if ( _useXrm == true ) {
      QWidget* dummy = new QWidget;
      XxXrmParser xrmParser( dummy->x11Display(), "Xxdiff", argc, argv );
      delete dummy;
      _resources->parse( xrmParser );
   }
#endif

#ifdef XX_USE_RCFILE
   if ( _useRcfile == true ) {
      XxRcfileParser rcfileParser;
      _resources->parse( rcfileParser );
   }
#endif
}

//------------------------------------------------------------------------------
//
void XxApp::parseCommandLineResources()
{
   if ( _stringResParser != 0 ) {
      _resources->parse( *_stringResParser );
   }
}


//------------------------------------------------------------------------------
//
void XxApp::outputDiffErrors( const char* errors ) const
{
   QString text( errors );
   QMessageBox* box = new XxSuicideMessageBox( 
      _mainWindow, "Diff errors.", text, QMessageBox::Warning 
   );
   box->show();
}

//------------------------------------------------------------------------------
//
uint XxApp::computeTextWidth() const
{
   uint textWidth = 0;
   for ( int ii = 0; ii < _nbFiles; ++ii ) {
      textWidth = std::max( textWidth, _files[ii]->computeTextWidth( 
         _font, _resources->getTabWidth()
      ) );
   }
   return textWidth;
}

//------------------------------------------------------------------------------
//
uint XxApp::getTextWidth() const
{
   return _textWidth;
}

//------------------------------------------------------------------------------
//
void XxApp::resizeEvent()
{
   // The first text widget calls this.
   adjustScrollbars();
   adjustCursor();
}

//------------------------------------------------------------------------------
//
QRect XxApp::getMainWindowGeometry() const
{
   return _mainWindow->geometry();
}

//------------------------------------------------------------------------------
//
QPopupMenu* XxApp::getViewPopup() const
{
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
      uint textWidth = _textWidth + 16; // 10 pixels. right margin.
      if ( textWidth <= _displayWidth ) {
         _hscroll->setRange( 0, 0 );
      }
      else {
         _hscroll->setSteps( 1, _displayWidth );
         _hscroll->setRange( 0, textWidth - _displayWidth );
      }

      if ( int( textWidth - _displayWidth ) < _hscroll->value() ) {
         _hscroll->setValue( textWidth - _displayWidth );
         // Note: this will indirectly trigger a redraw.
      }
   }

   // Vertical scrollbar.
   if ( force == true || 
        _displayHeight != (uint)_text[0]->height() ) {
      uint displayLines = _text[0]->computeDisplayLines();
      if ( force == true || 
           _displayLines != displayLines ) {
         _displayLines = displayLines;
         _vscroll[0]->setSteps( 1, _displayLines );
         if ( _vscroll[1] != 0 ) {
            _vscroll[1]->setSteps( 1, _displayLines );
         }

         uint topLine = getTopLine();

         if ( _diffs.get() == 0 ) {
            _vscroll[0]->setRange( 0,0 );
            if ( _vscroll[1] != 0 ) {
               _vscroll[1]->setRange( 0,0 );
            }
         }
         else {
            uint maxLine = 
               std::max( 0, int( _diffs->getNbLines() - _displayLines + 1 ) );
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
uint XxApp::setTopLine( int lineNo )
{
   if ( _diffs.get() == 0 ) {
      return 1;
   }

   uint oldLine = getTopLine();
   uint validLine = 
      std::max( 1, std::min( (int)_diffs->getNbLines(), lineNo ) );
   uint displayableLine = 
      std::min(
         validLine, 
         std::max( (uint)1, 
                   _diffs->getNbLines() - (getNbDisplayLines() - 2) )
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
uint XxApp::setCenterLine( int lineNo )
{
   if ( _diffs.get() == 0 ) {
      return 1;
   }

   uint oldLine = getCenterLine();

   setTopLine( lineNo - getNbDisplayLines() / 2 );

   return oldLine;
}

//------------------------------------------------------------------------------
//
uint XxApp::setBottomLine( int lineNo )
{
   if ( _diffs.get() == 0 ) {
      return 1;
   }

   uint oldLine = getBottomLine();

   setTopLine( lineNo - (getNbDisplayLines() - 2) );

   return oldLine;
}

//------------------------------------------------------------------------------
//
uint XxApp::setCursorLine( int lineNo )
{
   uint oldLine = _cursorLine;

   // Validate and set.
   uint validLine = 
      std::max( 1, std::min( (int)getNbLines(), lineNo ) );
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
   uint oldLine = _cursorLine;
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
      for ( int ii = 0; ii < _nbFiles; ++ii ) {
         bool aempty;

//           cursorLine = 
//              std::max( getTopLine(), std::min( getBottomLine(), cursorLine ) );
// FIXME remove

         int fline = _diffs->getFileLine( ii, cursorLine, aempty );
         _lineNumberLabel[ii]->setNum( fline );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::repaintTexts()
{
   for ( int ii = 0; ii < _nbFiles; ++ii ) {
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
void XxApp::saveToFile( const char* filename, const bool ask ) 
{
   if ( _diffs.get() == 0 ) {
      return;
   }

   // Check if there are some unselected regions remaining.
   if ( !_diffs->isAllSelected() ) {
      
      // Bring the user to the first unselected region.
      int nextNo = _diffs->findNextUnselected( 0 );
      XX_ASSERT( nextNo != -1 );
      setCursorLine( nextNo );
      setCenterLine( nextNo );

      // Pop a dialog.
      using namespace std;
      ostringstream oss;
      oss << "This region is still unselected.  " 
          << "There are still some unselected regions remaining..." 
          << endl
          << "Save anyway, marking unselected regions as merge conflicts?"
          << ends;
      QString text( oss.str().c_str() );

      int resp = QMessageBox::warning( 
         _mainWindow, "xxdiff", text,
         "Ok", "Cancel", QString::null, 0, 1
      );
      if ( resp == 1 ) {
         // User has canceled.
         return;
      }
      // Continue anyway.
   }

   QString f;
   if ( ask == true ) {
      f = QFileDialog::getSaveFileName( 
         QString( filename ), QString::null, _mainWindow
      );
      if ( f.isEmpty() ) {
         // The user cancelled the dialog.
         return;
      }
   }
   else {
      f = filename;
   }
   XX_ASSERT( !f.isEmpty() );
   
   // Check for file existence.
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
      std::ofstream outfile( f.ascii() );
      if ( outfile.fail() == true ) {
         throw new XxIoError( "Error opening output file." );
      }
      
      // Save to the file.
      _diffs->save( outfile, getFiles() );
      
      outfile.close();
      if ( outfile.fail() == true ) {
         throw new XxIoError( "Error opening output file." );
      }
   }
   catch ( XxIoError* ioerr ) {
      QMessageBox::critical( 
         _mainWindow, "xxdiff", ioerr->what(), 1,0,0
      );
      delete ioerr;
   }

   _diffs->clearDirty();
}

//------------------------------------------------------------------------------
//
void XxApp::editFile( const char* filename ) 
{
   if ( _diffs.get() == 0 ) {
      return;
   }

   const char* command = _resources->getCommand( XxResources::COMMAND_EDIT );
   if ( command == 0 ) {
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
         throw new XxIoError;
      }

      /* set socket reuse. */
      int reuse = 1;
      ::setsockopt( _sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int) );

      struct sockaddr_un name;
      size_t size;
      /* Bind a name to the socket.   */
      name.sun_family = AF_FILE;
      strcpy( name.sun_path, "/tmp/xxdiff_socket" );

      /* The size of the address is
         the offset of the start of the filename,
         plus its length,
         plus one for the terminating null byte.  */
      size = ( offsetof( struct sockaddr_un, sun_path )
               + strlen( name.sun_path ) + 1 );
      
      if ( ::bind( _sockfd, (struct sockaddr*)&name, size ) < 0 ) {
         throw new XxIoError;
      }
#else
      /* Create the socket.   */
      _sockfd = ::socket( PF_INET, SOCK_STREAM, 0 );
      if ( _sockfd < 0 ) {
         throw new XxIoError;
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
         throw new XxIoError;
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

   // Fork and exec.
   switch ( fork() ) {
      case 0: { // the child
         
         /* 
          * split up args passed in into an argument vector for the execvp
          * system call.  this works for an unlimited number of arguments,
          * but fails to do any quote processing.  arguments with embedded
          * spaces will break this.
          */         

         int argc = 0;
         const int BLOCKSIZE = 10;
         int count = BLOCKSIZE;
         const char** argv = (const char**) malloc( sizeof(char*) * count );

         // Make a copy of the args string because strtok is broken under
         // Linux, it modifies the first argument (see BUGS section in man
         // page of strtok).
         char* cargs = strdup( command );
         
         char* ptr;
         for ( ptr = strtok( cargs, " \t" ); 
               ptr; 
               ptr = strtok( 0, " \t" ) ) {
            
            if ( argc >= count ) {
               count += BLOCKSIZE;
               argv = (const char**) realloc( argv, sizeof(char*) * count );
            }
            
            argv[argc++] = strdup( ptr );
         }
         free( cargs );
            
         if ( (argc + 3) >= count ) {
            count += 3;
            argv = (const char**) realloc( argv, sizeof(char*) * count );
         }
         
         argv[argc++] = filename;
         argv[argc] = 0;

         if ( execvp( argv[0], (char**)argv ) == -1 ) {
            exit( 1 ); // signal error.
         }

         // Unreached.

      } break;
      
      case -1: { // fork error
         throw new XxIoError;
      }

      default: { // the parent
         //
         // Register a SIGCHLD handler.
         //
         struct sigaction sa;
         sa.sa_flags = SA_RESETHAND | SA_NOCLDWAIT;
         sa.sa_handler = XxApp::handlerSIGCHLD;
         sigset_t ss;
         sigemptyset( &ss );
         sa.sa_mask = ss;
         sa.sa_sigaction = 0;
         if ( ::sigaction( SIGCHLD, &sa, 0 ) != 0 ) {
            // Ignore error.
            return;
         }
         return;
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::editDone()
{
   // Read all the garbage from the socket.
   char buf;
   while ( read( _sockfd, &buf, sizeof(char) ) > 0 ) {
   }

   delete _socketNotifier;
   _socketNotifier = 0;
   ::close( _sockfd );

   redoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::openFile( const int no )
{
   // Read in the file names.
   std::string filenames[3];
   std::string displayFilenames[3];
   bool isTemporary[3] = { false, false, false };
   for ( int ii = 0; ii < _nbFiles; ++ii ) {
      filenames[ii] = _files[ii]->getName();
      displayFilenames[ii] = _files[ii]->getDisplayName();
      isTemporary[ii] = _files[ii]->isTemporary();
   }

   QString startWith = QString::null;
   XX_ASSERT( _files[no].get() != 0 );
   if ( !isTemporary[no] ) {
      startWith = filenames[no].c_str();
   }
   QString f = QFileDialog::getOpenFileName( 
      startWith, QString::null, _mainWindow 
   );
   if ( f.isEmpty() ) {
      // The user cancelled the dialog.
      return; 
   }
   else {
      bool isDirectory;
      XxUtil::testFile( f.ascii(), isDirectory );
      if ( isDirectory == true ) {
         QString text( "Cannot open a new directory" );
         QMessageBox* box = new XxSuicideMessageBox( 
            _mainWindow, "Error.", text, QMessageBox::Warning 
         );
         box->show();         
         return;
      }
      readFile( no, f.ascii(), f.ascii(), false );
      bool succ = processDiff();
      
      // Initialize the horizontal diffs if necessary.
      if ( succ == true &&
           _resources->getBoolOpt( XxResources::HORIZONTAL_DIFFS ) == true ) {
         _diffs->initializeHorizontalDiffs( getFiles() );
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
      int nextNo = _diffs->findNextUnselected( _cursorLine );
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
   uint nbFiles = getNbFiles();
   if ( nbFiles > 0 ) {
      uint cursorLine = getCursorLine();
      uint centerLine = getCenterLine();

      // Reread the files.
      for ( uint ii = 0; ii < nbFiles; ++ii ) {
         reReadFile( ii );
      }

      // Do the diff.
      bool succ = processDiff();

      // Initialize the horizontal diffs if necessary.
      if ( succ == true &&
           _resources->getBoolOpt( XxResources::HORIZONTAL_DIFFS ) == true ) {
         _diffs->initializeHorizontalDiffs( getFiles() );
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
   XxBuffer* file = getFile( 0 );
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
   XxBuffer* file = getFile( 1 );
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
   XxBuffer* file = getFile( _nbFiles == 2 ? 1 : 2 );
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

   if ( _diffs->checkSelections( no ) == true ) {
      // Pop a dialog.
      using namespace std;
      ostringstream oss;
      oss << "The selections are all on this file." << endl
          << "Save anyway?" << ends;
      QString text( oss.str().c_str() );

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
   XxBuffer* file = getFile( 0 );
   const char* filename = 0;;
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
      QString::null, QString::null, _mainWindow
   );
   if ( f.isEmpty() ) {
      // The user cancelled the dialog.
         return;
   }
   
   // Check for file existence.
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
   std::ofstream outfile( f.ascii() );
   
   // Save to the file.
   _diffs->saveSelectedOnly( outfile, getFiles() );
   
   outfile.close();
}   

//------------------------------------------------------------------------------
//
void XxApp::editLeft()
{
   XxBuffer* file = getFile( 0 );
   if ( file != 0 ) {
      editFile( file->getName() );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::editMiddle()
{
   XxBuffer* file = getFile( 1 );
   if ( file != 0 ) {
      editFile( file->getName() );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::editRight()
{
   XxBuffer* file = getFile( _nbFiles == 2 ? 1 : 2 );
   if ( file != 0 ) {
      editFile( file->getName() );
   }
}

//------------------------------------------------------------------------------
//
int XxApp::exec()
{
   if ( _resources->getBoolOpt( XxResources::EXIT_ON_SAME ) == true &&
        _returnValue == 0 ) {
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
      if ( _resources->getBoolOpt( XxResources::WARN_ABOUT_UNSAVED ) == true &&
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
      XxDiffs::SeaResult sres =
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
      XxDiffs::SeaResult sres = 
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
   uint diff = (getNbDisplayLines() - 3);
   // Note: we leave 3 lines in common.

   // Scroll.
   uint topLine = getTopLine();
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
   uint diff = (getNbDisplayLines() - 3);
   // Note: we leave 3 lines in common.

   // Scroll.
   uint topLine = getTopLine();
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
void XxApp::editOptions()
{ 
   // Popup name and arguments box.
   if ( _optionsDialog == 0 ) {
      _optionsDialog = new XxOptionsDialog( this, _mainWindow );
   }
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

   int cursorLine = getCursorLine();
   const XxLine& line = _diffs->getLine( cursorLine );
   if ( line.getType() == XxLine::SAME ||
        line.getType() == XxLine::DIFF_ALL || 
        line.getType() == XxLine::DIRECTORIES ) {

      std::string filenames[2];
      for ( int ii = 0; ii < 2; ++ii ) {
         bool empty;
         uint fline = _diffs->getFileLine( ii, cursorLine, empty );
         XX_ASSERT( empty == false );

         filenames[ii] = _files[ii]->getFileAtLine( fline );
      }

      // Spawn a diff.
      const char* args[3];
      args[0] = filenames[0].c_str();
      args[1] = filenames[1].c_str();
      args[2] = 0;
      XxUtil::spawnCommand( argv()[0], args );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::nextDifference()
{
   if ( _diffs.get() != 0 ) {
      int nextNo = _diffs->findNextDifference( _cursorLine );
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
      int nextNo = _diffs->findPreviousDifference( _cursorLine );
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
      int nextNo = _diffs->findNextUnselected( _cursorLine );
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
      int nextNo = _diffs->findPreviousUnselected( _cursorLine );
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
      // FIXME do only the required lines.
      if ( joined == true ) {
         if ( _resources->getBoolOpt( XxResources::HORIZONTAL_DIFFS ) ) {
            _diffs->initializeHorizontalDiffs( getFiles() );
         }
      }
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineLeft()
{
   if ( _diffs.get() != 0 ) {
      uint lineNo = getCursorLine();
      _diffs->selectLine( lineNo, XxLine::SEL1 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineMiddle()
{
   if ( _diffs.get() != 0 ) {
      uint lineNo = getCursorLine();
      _diffs->selectLine( lineNo, XxLine::SEL2 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineRight()
{
   if ( _diffs.get() != 0 ) {
      uint lineNo = getCursorLine();
      _diffs->selectLine( lineNo,
                          _nbFiles == 2 ? XxLine::SEL2 : XxLine::SEL3 );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineNeither()
{
   if ( _diffs.get() != 0 ) {
      uint lineNo = getCursorLine();
      _diffs->selectLine( lineNo, XxLine::NEITHER );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::selectLineUnselect()
{
   if ( _diffs.get() != 0 ) {
      uint lineNo = getCursorLine();
      _diffs->selectLine( lineNo, XxLine::UNSELECTED );
   }
}

//------------------------------------------------------------------------------
//
void XxApp::setTabWidth()
{
   // Create search dialog if non-existent.
   if ( _tabWidthDialog == 0 ) {
      _tabWidthDialog = new XxTabWidthDialog( this, _mainWindow );
   }
   _tabWidthDialog->show();
}

//------------------------------------------------------------------------------
//
void XxApp::setTabWidth( const int newTabWidth )
{
   _resources->setTabWidth( newTabWidth );

   _textWidth = computeTextWidth();
   _displayWidth = 0; // This is a hack to force adjustment of hscrollbar.
   adjustScrollbars();
   synchronizeUI();
   repaintTexts();   
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
   setFileDiffOptions( XxResources::CMDOPT_FILES_IGNORE_TRAILING );
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreWhitespace()
{
   setFileDiffOptions( XxResources::CMDOPT_FILES_IGNORE_WHITESPACE );
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreCase()
{
   setFileDiffOptions( XxResources::CMDOPT_FILES_IGNORE_CASE );
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreBlankLines()
{
   setFileDiffOptions( XxResources::CMDOPT_FILES_IGNORE_BLANK_LINES );
}

//------------------------------------------------------------------------------
//
void XxApp::setFileDiffOptions( XxResources::Resource option )
{
   XxResources::Resource cmdResId = _nbFiles == 2 ? 
      XxResources::COMMAND_DIFF_FILES_2 : 
      XxResources::COMMAND_DIFF_FILES_3;
   _resources->setCommandOption( cmdResId, option, true );
   
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
   _resources->toggleBoolOpt( XxResources::HIDE_CR );
   synchronizeUI();
   onRedoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::dirDiffRecursive()
{
   _resources->toggleBoolOpt( XxResources::DIRDIFF_RECURSIVE );
   synchronizeUI();
   onRedoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::ignoreFileChanges()
{
   _resources->toggleBoolOpt( XxResources::DIRDIFF_IGNORE_FILE_CHANGES );
   synchronizeUI();
   onRedoDiff();
}

//------------------------------------------------------------------------------
//
void XxApp::qualityNormal()
{
   setQuality( XxResources::QUALITY_NORMAL );
}

//------------------------------------------------------------------------------
//
void XxApp::qualityFastest()
{
   setQuality( XxResources::QUALITY_FASTEST );
}

//------------------------------------------------------------------------------
//
void XxApp::qualityHighest()
{
   setQuality( XxResources::QUALITY_HIGHEST );
}

//------------------------------------------------------------------------------
//
void XxApp::setQuality( XxResources::Quality quality )
{
   XxResources::Resource cmdResId = _nbFiles == 2 ? 
      XxResources::COMMAND_DIFF_FILES_2 : 
      XxResources::COMMAND_DIFF_FILES_3;
   std::string cmd = _resources->getCommand( cmdResId );
   _resources->setQuality( cmd, quality );
   _resources->setCommand( cmdResId, cmd.c_str() );

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
      _mergedWindow = new XxMergedWindow( this, _mainWindow );
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
   _resources->toggleBoolOpt( XxResources::SHOW_TOOLBAR );
   synchronizeUI();

   if ( _resources->getBoolOpt( XxResources::SHOW_TOOLBAR ) ) {
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
   _resources->toggleBoolOpt( XxResources::SHOW_LINE_NUMBERS );
   
   if ( _resources->getBoolOpt( XxResources::SHOW_LINE_NUMBERS ) ) {
      // Compute the maximum line numbers width.  This has to be the same for
      // all the texts, in order to have a consistent horizontal scrollbar.
      uint lnw = 0;
      for ( int ii = 0; ii < _nbFiles; ++ii ) {
         lnw = std::max(
            lnw,
            _files[ii]->computeLineNumbersWidth( _font ) +
            ( _lineNumbers[ii]->width() - 
              _lineNumbers[ii]->contentsRect().width() + 2 ) );
         
      }
      for ( int ii = 0; ii < _nbFiles; ++ii ) {
         _lineNumbers[ii]->setFixedWidth( lnw );
         _lineNumbers[ii]->show();
      }
   }
   else {
      for ( int ii = 0; ii < _nbTextWidgets; ++ii ) {
         _lineNumbers[ii]->hide();
      }
   }

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
void XxApp::toggleShowMarkers()
{
   _resources->toggleBoolOpt( XxResources::SHOW_MARKERS );
}

//------------------------------------------------------------------------------
//
void XxApp::toggleOverview()
{
   _resources->toggleBoolOpt( XxResources::SHOW_OVERVIEW );

   if ( _resources->getBoolOpt( XxResources::SHOW_OVERVIEW ) ) {
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
   _resources->toggleBoolOpt( XxResources::SHOW_FILENAMES );

   for ( int ii = 0; ii < _nbTextWidgets; ++ii ) {
      if ( _resources->getBoolOpt( XxResources::SHOW_FILENAMES ) ) {
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
   _resources->toggleBoolOpt( XxResources::HORIZONTAL_DIFFS );

   if ( _diffs.get() != 0 ) {
      _diffs->initializeHorizontalDiffs( getFiles() );
      repaintTexts();
   }
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleIgnoreHorizontalWs()
{
   _resources->toggleBoolOpt( XxResources::IGNORE_HORIZONTAL_WS );

   if ( _diffs.get() != 0 ) {
      _diffs->initializeHorizontalDiffs( getFiles(), true );
      repaintTexts();
   }
   synchronizeUI();
}

//------------------------------------------------------------------------------
//
void XxApp::toggleCutAndPasteAnnotations()
{
   _resources->toggleBoolOpt( XxResources::CUT_AND_PASTE_ANNOTATIONS );
   synchronizeUI();
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
void XxApp::helpColorLegend()
{
   QDialog* legend = XxHelp::getColorLegend( _mainWindow );
   legend->show();
}

//------------------------------------------------------------------------------
//
void XxApp::helpGenInitFile()
{
   QString f;
   f = QFileDialog::getSaveFileName( 
      QString( "sample.xxdiffrc" ), QString::null, _mainWindow
   );
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
      std::ofstream outfile( f.ascii() );
      if ( outfile.fail() == true ) {
         throw new XxIoError( "Error opening output file." );
      }
      
      // Save to the file.
      _resources->genInitFile( this, outfile );
      
      outfile.close();
      if ( outfile.fail() == true ) {
         throw new XxIoError( "Error opening output file." );
      }
   }
   catch ( XxIoError* ioerr ) {
      QMessageBox::critical( 
         _mainWindow, "xxdiff", ioerr->what(), 1,0,0
      );
      delete ioerr;
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
      XxResources::Resource cmdResId = _nbFiles == 2 ? 
         XxResources::COMMAND_DIFF_FILES_2 : 
         XxResources::COMMAND_DIFF_FILES_3;
      
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreTrailing ],
         _resources->isCommandOption( 
            cmdResId, 
            XxResources::CMDOPT_FILES_IGNORE_TRAILING
         )
      );
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreWhitespace ],
         _resources->isCommandOption( 
            cmdResId, 
            XxResources::CMDOPT_FILES_IGNORE_WHITESPACE
         )
      );
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreCase ],
         _resources->isCommandOption( 
            cmdResId, 
            XxResources::CMDOPT_FILES_IGNORE_CASE
         )
      );
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreBlankLines ],
         _resources->isCommandOption( 
            cmdResId, 
            XxResources::CMDOPT_FILES_IGNORE_BLANK_LINES
         )
      );

      std::string cmd = _resources->getCommand( cmdResId );
      XxResources::Quality quality = _resources->getQuality( cmd );

      _optionsMenu->setItemChecked(
         _menuids[ ID_ToggleQualityNormal ],
         quality == XxResources::QUALITY_NORMAL
      );
      _optionsMenu->setItemChecked(
         _menuids[ ID_ToggleQualityFastest ],
         quality == XxResources::QUALITY_FASTEST
      );
      _optionsMenu->setItemChecked(
         _menuids[ ID_ToggleQualityHighest ],
         quality == XxResources::QUALITY_HIGHEST
      );
   }
   else {
      _optionsMenu->setItemChecked( 
         _menuids[ ID_ToggleDirDiffsRecursive ],
         _resources->getBoolOpt( XxResources::DIRDIFF_RECURSIVE )
      );
   }

   //---------------------------------------------------------------------------

   if ( _filesAreDirectories == false ) {
      _displayMenu->setItemChecked( 
         _menuids[ ID_ToggleHorizontalDiffs ],
         _resources->getBoolOpt( XxResources::HORIZONTAL_DIFFS )
      );
      _displayMenu->setItemChecked( 
         _menuids[ ID_ToggleIgnoreHorizontalWs ],
         _resources->getBoolOpt( XxResources::IGNORE_HORIZONTAL_WS )
      );
      _displayMenu->setItemChecked( 
         _menuids[ ID_ToggleHideCarriageReturns ],
         _resources->getBoolOpt( XxResources::HIDE_CR )
      );
   }
   else {
      _displayMenu->setItemChecked( 
         _menuids[ ID_ToggleDirDiffsIgnoreFileChanges ],
         _resources->getBoolOpt( XxResources::DIRDIFF_IGNORE_FILE_CHANGES )
      );
   }

   _displayMenu->setItemChecked( 
      _menuids[ ID_ToggleCutAndPasteAnnotations ],
      _resources->getBoolOpt( XxResources::CUT_AND_PASTE_ANNOTATIONS )
   );

   if ( _filesAreDirectories == false ) {

      uint tabWidth = _resources->getTabWidth();
      _displayMenu->setItemChecked( _menuids[ ID_TabsAtFour ], tabWidth == 4 );
      _displayMenu->setItemChecked( _menuids[ ID_TabsAtEight ], tabWidth == 8 );
   }

   //---------------------------------------------------------------------------

   _windowsMenu->setItemChecked( 
      _menuids[ ID_ToggleToolbar ], 
      _resources->getBoolOpt( XxResources::SHOW_TOOLBAR )
   );
   _windowsMenu->setItemChecked( 
      _menuids[ ID_ToggleLineNumbers ], 
      _resources->getBoolOpt( XxResources::SHOW_LINE_NUMBERS )
   );
   _windowsMenu->setItemChecked( 
      _menuids[ ID_ToggleOverview ],
      _resources->getBoolOpt( XxResources::SHOW_OVERVIEW )
   );
   _windowsMenu->setItemChecked( 
      _menuids[ ID_ToggleShowFilenames ],
      _resources->getBoolOpt( XxResources::SHOW_FILENAMES )
   );
}

//------------------------------------------------------------------------------
//
void XxApp::handlerSIGCHLD( int )
{
   XX_ASSERT( _socketNotifier != 0 );

   // Wake up the socket notifier.
   // This should get caught by a select() call from the main event loop.
   char buf = 'g';//arbage
   write( _sockfd, &buf, sizeof(char) );
}

XX_NAMESPACE_END

/******************************************************************************\
 * $Id: app.h 162 2001-05-28 18:32:02Z blais $
 * $Date: 2001-05-28 14:32:02 -0400 (Mon, 28 May 2001) $
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

#ifndef INCL_XXDIFF_APP
#define INCL_XXDIFF_APP

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_LINE
#include <line.h>
#endif

#ifndef INCL_XXDIFF_RESOURCES
#include <resources.h>
#endif

#ifndef INCL_QT_QAPPLICATION
#include <qapplication.h>
#define INCL_QT_QAPPLICATION
#endif

#ifndef INCL_STD_MEMORY
#include <memory>
#define INCL_STD_MEMORY
#endif

#ifndef INCL_STD_STRING
#include <string>
#define INCL_STD_STRING
#endif

#ifndef INCL_STD_LIST
#include <list>
#define INCL_STD_LIST
#endif

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QMainWindow;
class QScrollBar;
class QLabel;
class QDialog;
class QSocketNotifier;
class QToolBar;
class QMessageBox;

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxOverview;
class XxLineNumbers;
class XxText;
class XxMergedWindow;
class XxDiffs;
class XxBuffer;
class XxResources;
class XxOptionsDialog;
class XxStringResParser;
class XxSearchDialog;

/*==============================================================================
 * CLASS XxApp
 *============================================================================*/

// <summary> The main application </summary>

class XxApp : public QApplication {

   Q_OBJECT

public:

   /*----- types and enumerations -----*/

   enum { MAX_MENUIDS = 256 };

   /*----- member functions -----*/

   // Constructor.
   XxApp( int argc, char** argv, bool forceStyle = false );

   // Destructor.
   virtual ~XxApp();

   // See base clas.
   int exec();

   // Returns the number of files.
   uint getNbFiles() const;

   // Returns the file for requested buffer.
   XxBuffer* getFile( const XxFno no ) const;

   // Returns an array of the file pointers.
   const std::auto_ptr<XxBuffer>* getFiles() const;

   // Returns the maximum number of digits for line numbers of all files.
   uint getMaxDigits() const;

   // Returns the diffs.
   XxDiffs* getDiffs() const;

   // Returns the number of lines in the diffs.
   XxDln getNbLines() const;

   // Returns the font.
   const QFont& getFont() const;

   // Adjust scrollbars after a resize.
   void adjustScrollbars( bool force = false );

   // Returns the horizontal position.
   uint getHorizontalPos() const;

   // Requests for a certain top line to be displayed.  The actual top line that
   // will be displayed might not be the one that is requested, due to the fact
   // that we want to avoid letting the user scroll past the end of the file, by
   // resizing or otherwise).  The actual top line that was computed is
   // returned.
   // Out-of-bounds values are permitted.
   XxDln setTopLine( const XxDln lineNo );

   // Like vertical pos, but tries to center the line instead.  The actual top
   // line that was computed is returned.
   XxDln setCenterLine( const XxDln lineNo );

   // Like vertical pos, but tries to set the bottom line instead.  The actual
   // bottom line that was computed is returned.
   XxDln setBottomLine( const XxDln lineNo );

   // Sets the cursor line.  Returns the old cursor line.  Value will remain
   // unchanged if an out-of-bounds value is asked for.  Note: may scroll to
   // insure that the current line is visible.
   XxDln setCursorLine( const XxDln lineNo );

   // Returns the top/center/bottom/current display line.
   // <group>
   XxDln getTopLine() const;
   XxDln getCenterLine() const;
   XxDln getBottomLine() const;
   XxDln getCursorLine() const;
   // </group>

   // Returns the current number of display lines.
   XxDln getNbDisplayLines() const;

   // Process a main window resize event.
   void resizeEvent();

   // Returns the main window's size.
   QRect getMainWindowGeometry() const;

   // Returns a reference on the view popup menu.
   QPopupMenu* getViewPopup( const XxLine& line ) const;

   // Returns the return value to send back to caller.
   int getReturnValue() const;

   // Synchronizes the UI menus (toggles) with the resources.
   void synchronizeUI();

   // Changes the tab width.
   void setTabWidth( const int newTabWidth );

   // Get the latest computed value for the longest line.  See
   // computeTextWidth().
   uint getTextWidth() const;


public slots:

   // Redraws the text widgets.
   void repaintTexts();

   // On a number of line change in diffs.
   void onNbLinesChanged();

   // Updates the line number labels.
   void updateLineNumberLabels( int );

   // Scrollbars.
   // <group>
   void horizontalScroll( int );
   void scrollRight();
   void scrollLeft();
   void verticalScroll( int );
   void verticalScroll2( int );
   // </group>

   // This gets called in the main thread when the edit is done.
   void editDone();

   // Most menu items.
   // <group>
   void openLeft();
   void openMiddle();
   void openRight();
   void saveAsLeft();
   void saveAsMiddle();
   void saveAsRight();
   void saveAs();
   void saveSelectedOnly();
   void editLeft();
   void editMiddle();
   void editRight();
   void quit();
   void search();
   void searchForward();
   void searchBackward();
   void scrollDown();
   void scrollUp();
   void cursorDown();
   void cursorUp();
   void cursorTop();
   void cursorBottom();
   void redoDiff();
   void editDiffOptions();
   void editDisplayOptions();
   void diffFilesAtCursor();
   void copyFileLeftToRight();
   void copyFileRightToLeft();
   void nextDifference();
   void previousDifference();
   void nextUnselected();
   void previousUnselected();
   void selectGlobalLeft();
   void selectGlobalMiddle();
   void selectGlobalRight();
   void selectGlobalNeither();
   void selectGlobalUnselect();
   void selectGlobalUnselectedLeft();
   void selectGlobalUnselectedMiddle();
   void selectGlobalUnselectedRight();
   void selectGlobalUnselectedNeither();
   void selectGlobalMerge();
   void selectRegionLeft();
   void selectRegionMiddle();
   void selectRegionRight();
   void selectRegionNeither();
   void selectRegionUnselect();
   void selectRegionLeftAndNext();
   void selectRegionMiddleAndNext();
   void selectRegionRightAndNext();
   void selectRegionNeitherAndNext();
   void regionSplitSwapJoin();
   void selectLineLeft();
   void selectLineMiddle();
   void selectLineRight();
   void selectLineNeither();
   void selectLineUnselect();
   void tabsAt3();
   void tabsAt4();
   void tabsAt8();
   void ignoreTrailing();
   void ignoreWhitespace();
   void ignoreCase();
   void ignoreBlankLines();
   void hideCarriageReturns();
   void dirDiffRecursive();
   void ignoreFileChanges();
   void qualityNormal();
   void qualityFastest();
   void qualityHighest();
   void mergedView();
   void toggleToolbar();
   void toggleLineNumbers();
   void adjustLineNumbers();
   void toggleShowMarkers();
   void toggleVerticalLine();
   void toggleOverview();
   void toggleShowFilenames();
   void toggleHorizontalDiffs();
   void toggleIgnoreHorizontalWs();
   void toggleCutAndPasteAnnotations();
   void helpManPage();
   void helpColorLegend();
   void helpGenInitFile();
   void helpAbout();
   // </group>
   
signals:

   void postCreationAction();
   void cursorChanged( int cursorLine );
   void scrolled( int topLine );

private:

   /*----- friends -----*/

   friend class XxDiffArgumentsDialog;
   friend class XxOptionsDialog;

   /*----- member functions -----*/

   // Reads the file arguments, returns the number of files to be read, creates
   // temporary files if necessary (from stdin).
   uint readFileNames( 
      int 		argc, 
      char** 		argv,
      std::string 	filenames[3],
      std::string 	displayFilenames[3],
      bool		created[3],
      bool&		directories
   ) const;

   // Parse Xrm and/or rc file and removes the recognized ones.
   void buildResources();

   // Parse command line options and removes the recognized ones.
   void parseCommandLine( int& argc, char**& argv );

   // Parse resources specified on command line.
   void parseCommandLineResources();

   // Reads in a file.
   void readFile( 
      const XxFno no, 
      const char* filename,
      const char* displayFilename,
      bool        isTemporary = false
   );

   // Re-reads a file.
   void reReadFile( const XxFno no );

   // Processes the diff command, be really careful with this.  Returns false if
   // there was an error.
   bool processDiff();

   // Re-does the diff, changing the UI as little as possible.
   void onRedoDiff();

   // Creates the interface.
   // <group>
   void createUI( uint nbFiles );
   void createOnContextHelp();
   void createMenus();
   QToolBar* createToolbar();
   // </group>

   // Output diff errors.
   void outputDiffErrors( const char* errors );

   // Compute the number of pixels of the longest line.
   uint computeTextWidth() const;
   
   // Moves the cursor line so that it is visible after a scroll or resize.
   // Return true if the cursor has been moved.
   bool adjustCursor();

   // Tries to save the selected contents to the specified filename.  If ask is
   // true, ask the user for the filename (and use filename as default value).
   void saveToFile( const char* filename, const bool ask );

   // Edits the said file, and add signal handler to catch children exit, and
   // notify.
   void editFile( const char* filename );

   // Reopen file at no and rebuild diffs and update and all.
   void openFile( const XxFno no );

   // Copy file from directory diffs.
   void copyFile( XxFno nnno ) const;

   // Private non-const version of resource access (for dialogs).
   XxResources* getResourcesNC();

   // Implements apply-and-move-to-next algorithm.
   void selectAndNext( XxLine::Selection selection );

   // Check if the selections are all of the specified no.  If so, ask the user
   // to confirm, returns true if we should save.
   bool validateNeedToSave( uint no ) const;

   // Implementation of the ignoreXXX() methods.
   void setFileDiffOptions( XxResources::Resource );

   // Implementation of the qualityXXX() methods.
   void setQuality( XxResources::Quality );

   /*----- static member functions -----*/

   // SIGCHLD handler for editor.
   static void handlerSIGCHLD( int );


   /*----- data members -----*/

   // UI widgets.
   bool                    _isUICreated;
   QStyle*                 _style;
   QMainWindow*            _mainWindow;
   QMessageBox*		   _diffErrorsMsgBox;
   QWidget*                _centralWidget;
   QPopupMenu*             _viewPopup;
   QPopupMenu*             _optionsMenu;
   QPopupMenu*             _displayMenu;
   QPopupMenu*             _windowsMenu;
   int 			   _menuids[ MAX_MENUIDS ];
   XxOverview*             _overview;
   XxSearchDialog*         _searchDialog;
   XxOptionsDialog*        _optionsDialog;
   QLabel*                 _filenameLabel[3];
   QLabel*                 _lineNumberLabel[3];
   XxLineNumbers*          _lineNumbers[3];
   XxText*                 _text[3];
   XxMergedWindow*         _mergedWindow;
   QToolBar*               _toolbar;
   QScrollBar*             _vscroll[2];
   QScrollBar*             _hscroll;
   QFont                   _font;
   uint                    _textWidth;
   uint                    _displayWidth;  // Last value cached. 
   uint                    _displayHeight; // Last value cached.
   XxDln                   _displayLines;
   XxDln                   _cursorLine;
                                	
   int                     _nbFiles;
   int                     _nbTextWidgets;
   std::auto_ptr<XxBuffer> _files[3];
   std::auto_ptr<XxDiffs>  _diffs;
   bool                    _filesAreDirectories;
                                	
   // Resources.                	
   XxResources*            _resources;

   // Cmdline-related variables.
   int                     _returnValue;
   std::string             _userFilenames[3];
   std::string             _stdinFilename;
   XxStringResParser*      _stringResParser;
   bool                    _useXrm;
   bool                    _useRcfile;
   std::string             _extraDiffArgs;
   bool                    _sepDiff;

   /*----- static data members -----*/

   static int              _sockfd;
   static QSocketNotifier* _socketNotifier;

};


XX_NAMESPACE_END

#include <app.inline.h>

#endif

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

#ifndef INCL_XXDIFF_RESOURCES
#define INCL_XXDIFF_RESOURCES

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_QT_QOBJECT
#include <qobject.h>
#define INCL_QT_QOBJECT
#endif

#ifndef INCL_QT_QCOLOR
#include <qcolor.h>
#define INCL_QT_QCOLOR
#endif

#ifndef INCL_QT_QFONT
#include <qfont.h>
#define INCL_QT_QFONT
#endif

#ifndef INCL_QT_QRECT
#include <qrect.h>
#define INCL_QT_QFONT
#endif

#ifndef INCL_STD_MAP
#include <map>
#define INCL_STD_MAP
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxResParser;
class XxApp;

/*==============================================================================
 * PUBLIC DECLARATIONS
 *============================================================================*/

// Boolean options.
// IMPORTANT! The order must be the same as for the tokens declaration.
enum XxBoolOpt {
   BOOL_EXIT_ON_SAME,
   BOOL_EXIT_IF_NO_CONFLICTS,
   BOOL_EXIT_WITH_MERGE_STATUS,
   BOOL_SELECT_MERGE,
   BOOL_IGNORE_HORIZONTAL_WS,
   BOOL_IGNORE_PERHUNK_WS,
   BOOL_FORMAT_CLIPBOARD_TEXT,
   BOOL_IGNORE_ERRORS,
   BOOL_WARN_ABOUT_UNSAVED,
   BOOL_DISABLE_CURSOR_DISPLAY,
   BOOL_DRAW_PATTERN_IN_FILLER_LINES,
   BOOL_HIDE_CR,
   BOOL_DIRDIFF_IGNORE_FILE_CHANGES,
   BOOL_DIRDIFF_BUILD_FROM_OUTPUT,
   BOOL_DIRDIFF_RECURSIVE,
   BOOL_NULL_HORIZONTAL_MARKERS,
   BOOL_USE_INTERNAL_DIFF,
   BOOL_LAST // Not a real resource.
};

// Windows visible.
enum XxShowOpt {
   SHOW_TOOLBAR,
   SHOW_LINE_NUMBERS,
   SHOW_VERTICAL_LINE,
   SHOW_OVERVIEW,
   SHOW_FILENAMES,
   SHOW_PANE_MERGED_VIEW,
   SHOW_POPUP_MERGED_VIEW,
   SHOW_LAST // Not a real resource
};

// Accelerators.
enum XxAccel {
   ACCEL_EXIT,
   ACCEL_EXIT_ACCEPT,
   ACCEL_EXIT_MERGED,
   ACCEL_EXIT_REJECT,
   ACCEL_OPEN_LEFT,
   ACCEL_OPEN_MIDDLE,
   ACCEL_OPEN_RIGHT,
   ACCEL_SAVE_AS_LEFT,
   ACCEL_SAVE_AS_MIDDLE,
   ACCEL_SAVE_AS_RIGHT,
   ACCEL_SAVE_AS_MERGED,
   ACCEL_SAVE_AS,
   ACCEL_SAVE_SELECTED_ONLY,
   ACCEL_PATCH_FROM_LEFT,
   ACCEL_PATCH_FROM_MIDDLE,
   ACCEL_PATCH_FROM_RIGHT,
   ACCEL_EDIT_LEFT,
   ACCEL_EDIT_MIDDLE,
   ACCEL_EDIT_RIGHT,
   ACCEL_SAVE_OPTIONS,
   ACCEL_SEARCH,
   ACCEL_SEARCH_FORWARD,
   ACCEL_SEARCH_BACKWARD,
   ACCEL_PAGE_DOWN,
   ACCEL_PAGE_UP,
   ACCEL_CURSOR_DOWN,
   ACCEL_CURSOR_UP,
   ACCEL_CURSOR_TOP,
   ACCEL_CURSOR_BOTTOM,
   ACCEL_REDO_DIFF,
   ACCEL_EDIT_DIFF_OPTIONS,
   ACCEL_EDIT_DISPLAY_OPTIONS,
   ACCEL_DIFF_FILES_AT_CURSOR,
   ACCEL_NEXT_AND_DIFF_FILES,
   ACCEL_COPY_RIGHT_TO_LEFT,
   ACCEL_COPY_LEFT_TO_RIGHT,
   ACCEL_REMOVE_LEFT,
   ACCEL_REMOVE_RIGHT,
   ACCEL_NEXT_DIFFERENCE,
   ACCEL_PREVIOUS_DIFFERENCE,
   ACCEL_NEXT_UNSELECTED,
   ACCEL_PREVIOUS_UNSELECTED,
   ACCEL_SELECT_GLOBAL_LEFT,
   ACCEL_SELECT_GLOBAL_MIDDLE,
   ACCEL_SELECT_GLOBAL_RIGHT,
   ACCEL_SELECT_GLOBAL_NEITHER,
   ACCEL_SELECT_GLOBAL_UNSELECT,
   ACCEL_SELECT_GLOBAL_UNSELECTED_LEFT,
   ACCEL_SELECT_GLOBAL_UNSELECTED_MIDDLE,
   ACCEL_SELECT_GLOBAL_UNSELECTED_RIGHT,
   ACCEL_SELECT_GLOBAL_UNSELECTED_NEITHER,
   ACCEL_SELECT_GLOBAL_MERGE,
   ACCEL_SELECT_REGION_LEFT,
   ACCEL_SELECT_REGION_MIDDLE,
   ACCEL_SELECT_REGION_RIGHT,
   ACCEL_SELECT_REGION_NEITHER,
   ACCEL_SELECT_REGION_UNSELECT,
   ACCEL_SELECT_REGION_LEFT_AND_NEXT,
   ACCEL_SELECT_REGION_MIDDLE_AND_NEXT,
   ACCEL_SELECT_REGION_RIGHT_AND_NEXT,
   ACCEL_SELECT_REGION_NEITHER_AND_NEXT,
   ACCEL_SELECT_REGION_SPLIT_SWAP_JOIN,
   ACCEL_SELECT_LINE_LEFT,
   ACCEL_SELECT_LINE_MIDDLE,
   ACCEL_SELECT_LINE_RIGHT,
   ACCEL_SELECT_LINE_NEITHER,
   ACCEL_SELECT_LINE_UNSELECT,
   ACCEL_TABS_AT_3,
   ACCEL_TABS_AT_4,
   ACCEL_TABS_AT_8,
   ACCEL_IGNORE_TRAILING,
   ACCEL_IGNORE_WHITESPACE,
   ACCEL_IGNORE_CASE,
   ACCEL_IGNORE_BLANK_LINES,
   ACCEL_HIDE_CR,
   ACCEL_DIRDIFF_IGNORE_FILE_CHANGES,
   ACCEL_DIRDIFF_RECURSIVE,
   ACCEL_USE_INTERNAL_DIFF,
   ACCEL_QUALITY_NORMAL,
   ACCEL_QUALITY_FASTEST,
   ACCEL_QUALITY_HIGHEST,
   ACCEL_TOGGLE_PANE_MERGED_VIEW,
   ACCEL_TOGGLE_POPUP_MERGED_VIEW,
   ACCEL_TOGGLE_TOOLBAR,
   ACCEL_TOGGLE_LINE_NUMBERS,
   ACCEL_TOGGLE_MARKERS,
   ACCEL_TOGGLE_VERTICAL_LINE,
   ACCEL_TOGGLE_NULL_HORIZONTAL_MARKERS,
   ACCEL_TOGGLE_OVERVIEW,
   ACCEL_TOGGLE_SHOW_FILENAMES,
   ACCEL_HORDIFF_NONE,
   ACCEL_HORDIFF_SINGLE,
   ACCEL_HORDIFF_MULTIPLE,
   ACCEL_TOGGLE_IGNORE_HORIZONTAL_WS,
   ACCEL_TOGGLE_IGNORE_PERHUNK_WS,
   ACCEL_TOGGLE_FORMAT_CLIPBOARD_TEXT,
   ACCEL_IGNORE_FILE_NONE,
   ACCEL_IGNORE_FILE_LEFT,
   ACCEL_IGNORE_FILE_MIDDLE,
   ACCEL_IGNORE_FILE_RIGHT,
   ACCEL_HELP_MAN_PAGE,
   ACCEL_HELP_ON_CONTEXT,
   ACCEL_HELP_ABOUT,
   ACCEL_MERGED_CLOSE,
   ACCEL_FONT_RESIZE_BIGGER,
   ACCEL_FONT_RESIZE_SMALLER,
   ACCEL_LAST // Not a real resource.
};

// Colors.
enum XxColor {
   COLOR_SAME,
   COLOR_SAME_BLANK,           // (when blank side, from ignore-blank-lines)

   COLOR_INSERT,               // A in A--, -A-, --A blocks.
   COLOR_INSERT_BLANK,         // - in A--, -A-, --A blocks.

   COLOR_DIFF_ALL,             // A,B or C in ABC blocks.
   COLOR_DIFF_ALL_SUP,         // (shadowed)
   COLOR_DIFF_ALL_ONLY,        // (when other is blank)
   COLOR_DIFF_ALL_NONLY,       // (when blank)

   COLOR_DIFF_ONE,             // B in BAA, ABA, AAB blocks.
   COLOR_DIFF_ONE_SUP,         // (shadowed)
   COLOR_DIFF_ONE_ONLY,        // (when other is blank)
   COLOR_DIFF_ONE_NONLY,       // (when blank)
   COLOR_DIFF_TWO,             // A in BAA, ABA, AAB blocks.
   COLOR_DIFF_TWO_SUP,         // (shadowed)
   COLOR_DIFF_TWO_ONLY,        // (when other is blank)
   COLOR_DIFF_TWO_NONLY,       // (when blank)

   COLOR_DELETE,               // A in -AA, A-A, AA- blocks.
   COLOR_DELETE_BLANK,         // - in -AA, A-A, AA- blocks.

   COLOR_DIFFDEL,              // A and B in -AB, A-B, AB- blocks.
   COLOR_DIFFDEL_SUP,          // (shadowed)
   COLOR_DIFFDEL_ONLY,         // (when other is blank)
   COLOR_DIFFDEL_NONLY,        // (when blank)
   COLOR_DIFFDEL_BLANK,        // - in -AB, A-B, AB- blocks.

   COLOR_SELECTED,             // selected text.
   COLOR_SELECTED_SUP,         // (shadowed)

   COLOR_IGNORE_DISPLAY,       // ignored for display purposes.
   COLOR_IGNORE_DISPLAY_SUP,   // (shadowed)
   COLOR_IGNORE_DISPLAY_ONLY,  // (when other is blank)
   COLOR_IGNORE_DISPLAY_NONLY, // (when blank)

   COLOR_DELETED,              // deleted text.
   COLOR_DELETED_SUP,          // (shadowed)

   COLOR_IGNORED,              // ignored file.

   COLOR_DIRECTORIES,          // directories in directory diffs.

   COLOR_MERGED_UNDECIDED,     // undecided text (merged view).
   COLOR_MERGED_DECIDED_1,     // decided text, file 1 (merged view).
   COLOR_MERGED_DECIDED_1_SUP, // decided text, file 1, shadowed (merged view).
   COLOR_MERGED_DECIDED_2,     // decided text, file 2 (merged view).
   COLOR_MERGED_DECIDED_2_SUP, // decided text, file 2, shadowed (merged view).
   COLOR_MERGED_DECIDED_3,     // decided text, file 3 (merged view).
   COLOR_MERGED_DECIDED_3_SUP, // decided text, file 3, shadowed (merged view).
   COLOR_MERGED_DECIDED_NEITHER, // decided text, neither files.

   // Other colors.
   COLOR_BACKGROUND,
   COLOR_CURSOR,
   COLOR_VERTICAL_LINE,
   COLOR_TEXT_SELECTION,

   COLOR_LAST // Not a real resource.
};

// Command lines.
enum XxCommand {
   CMD_DIFF_FILES_2 = 0,
   CMD_DIFF_FILES_3 = 1,
   CMD_DIFF_DIRECTORIES = 2,
   CMD_DIFF_DIRECTORIES_REC = 3,
   CMD_EDIT,
   CMD_LAST // Not a real resource.
};

// Command option switches.
enum XxCommandSwitch {
   CMDSW_FILES_IGNORE_TRAILING,
   CMDSW_FILES_IGNORE_WHITESPACE,
   CMDSW_FILES_IGNORE_CASE,
   CMDSW_FILES_IGNORE_BLANK_LINES,
   CMDSW_FILES_QUALITY_NORMAL,
   CMDSW_FILES_QUALITY_FASTEST,
   CMDSW_FILES_QUALITY_HIGHEST,
   CMDSW_LAST // Not a real resource.
};

// Blips of text to tag regions in output files.
enum XxTag {
   TAG_CONFLICT_SEPARATOR,
   TAG_CONFLICT_END,
   TAG_CONDITIONAL_IF,
   TAG_CONDITIONAL_ELSEIF,
   TAG_CONDITIONAL_ELSE,
   TAG_CONDITIONAL_ENDIF,
   TAG_UNMERGE_START,
   TAG_UNMERGE_SEP,
   TAG_UNMERGE_SEP_EXTRA,
   TAG_UNMERGE_END,
   TAG_LAST // Not a real resource.
};

//------------------------------------------------------------------------------

// Type of file ignore.
enum XxIgnoreFile {
   IGNORE_NONE = 0,
   IGNORE_LEFT = 1,
   IGNORE_MIDDLE = 2,
   IGNORE_RIGHT = 3
};

// Quality types.
enum XxQuality {
   QUALITY_NORMAL,
   QUALITY_FASTEST,
   QUALITY_HIGHEST
};

// Types of horizontal diffs.
enum XxHordiff {
   HD_NONE,
   HD_SINGLE,
   HD_MULTIPLE
};

/*==============================================================================
 * CLASS XxResources
 *============================================================================*/

// <summary> container for non-UI application data </summary>

// This class encapsulates all data that is not directly UI-related (it can
// contains UI state, but not widgets and such things).  Most if not all state
// can be initialized through the resource parser.

class XxResources : public QObject {

   Q_OBJECT

public:

   /*----- member functions -----*/

   // Constructor, makes a resources object with default values.
   XxResources( bool original );

   // Destructor.
   virtual ~XxResources();

   //
   // Provide resources.
   //

   // Preferred geometry (on startup).
   // <group>
   // Returns false on failure to parse string.
   static const QRect& getDefaultGeometry();
   const QRect& getPreferredGeometry() const;
   void setPreferredGeometry( const QRect& );

   bool getMaximize() const;
   void setMaximize( bool fs );
   // </group>

   // Style key (identifying string).
   // <group>
   const QString& getStyleKey() const;
   void setStyleKey( const QString& );
   // </group>

   // Accelerators.
   // <group>
   // Returns false on failure to parse string.
   int getAccelerator( XxAccel ) const;
   bool setAccelerator( XxAccel accel, const QString& val );
   // </group>

   // Provide resource: fonts.
   // Application font.
   // <group>
   const QFont& getFontApp() const;
   bool setFontApp( const QString& );
   bool setFontApp( const QFont& );
   // </group>

   // Font for text display.
   // <group>
   const QFont& getFontText() const;
   bool setFontText( const QString& );
   bool setFontText( const QFont& );
   // </group>

   // Get/set ignore file setting.
   //
   // Note: the values are important, as they map the ignore conversion table.
   // (see line.h)
   // <group>
   XxIgnoreFile getIgnoreFile() const;
   void setIgnoreFile( XxIgnoreFile ignoreFile );
   // </group>

   // Get/set a specific color.
   // <group>
   const QColor& getColor( const XxColor, const bool fore = false ) const;
   void setColor( const XxColor, const bool fore, const QString& );
   void setColor( const XxColor, const bool fore, const QColor& );
   // </group>

   // Returns a region's fore and back colors.
   void getRegionColor(
      const XxColor color,
      QColor&       background,
      QColor&       foreground
   ) const;

   // Provide resource: boolean options.
   // <group>
   bool getBoolOpt( const XxBoolOpt ) const;
   void setBoolOpt( const XxBoolOpt, const bool );
   // Returns newly set value.
   bool toggleBoolOpt( const XxBoolOpt );
   // </group>

   // Provide resource: show window options.
   // <group>
   bool getShowOpt( const XxShowOpt ) const;
   void setShowOpt( const XxShowOpt, const bool );
   // Returns newly set value.
   bool toggleShowOpt( const XxShowOpt );
   // </group>

   // Tab expansion width.
   // <group>
   uint getTabWidth() const;
   void setTabWidth( uint );
   // </group>


   // Get/set a command.
   // <group>
   const QString& getCommand( XxCommand cmdId ) const;
   void setCommand( XxCommand cmdId, const QString& );
   // </group>

   // Get/set a command option.
   // <group>
   const QString& getCommandSwitch( XxCommandSwitch cmdId ) const;
   void setCommandSwitch( XxCommandSwitch cmdId, const QString& );
   // </group>

   // Get/set a command option's initial state.
   // <group>
   int getInitSwitch( XxCommandSwitch cmdId ) const;
   void setInitSwitch( XxCommandSwitch cmdId, const int );
   // </group>

   // Convenience methods for getting/setting an option into a specific command.
   // <group>
   bool isCommandSwitch( XxCommand cmdId, XxCommandSwitch cmdOptionId ) const;
   void setCommandSwitch(
      XxCommand       cmdId,
      XxCommandSwitch cmdOptionId,
      bool            setit
   );
   void toggleCommandSwitch( XxCommand cmdId, XxCommandSwitch cmdOptionId );
   // </group>

   // Convenience methods for setting the quality options in the commands. Note
   // that these methods don't change the commands in the resources, they modify
   // the one that is given.
   // <group>
   XxQuality getQuality( const QString& command ) const;
   void setQuality( QString& command, XxQuality quality ) const;
   // </group>


   // Width in pixels of file representation in overview.
   // <group>
   uint getOverviewFileWidth() const;
   void setOverviewFileWidth( uint );
   // </group>

   // Width in pixels of separation between files in overview.
   // <group>
   uint getOverviewSepWidth() const;
   void setOverviewSepWidth( uint );
   // </group>

   // Position in characters of vertical line.
   // <group>
   uint getVerticalLinePos() const;
   void setVerticalLinePos( uint );
   // </group>

   // Get text tags for conflictual regions.
   // <group>
   const QString& getTag( XxTag ) const;
   void setTag( XxTag, const QString& );
   // </group>

   // Get/set clipboard selection or line format string.
   // <group>
   const QString& getClipboardHeadFormat() const;
   void setClipboardHeadFormat( const QString& format );
   const QString& getClipboardLineFormat() const;
   void setClipboardLineFormat( const QString& format );
   // </group>

   // Get/set type of horizontal diffs.
   // <group>
   XxHordiff getHordiffType() const;
   void setHordiffType( XxHordiff hordiffType );
   // </group>

   // Get/set the horizontal diffs maximum table size.
   // <group>
   uint getHordiffMax() const;
   void setHordiffMax( uint );
   // </group>

   // Get/set the horizontal diffs minimum common region.
   // <group>
   uint getHordiffContext() const;
   void setHordiffContext( uint );
   // </group>

   // Percentage (between 0 and 100) of the merged pane upon startup.
   // <group>
   uint getShowPaneMergedViewPercent() const;
   void setShowPaneMergedViewPercent( uint );
   // </group>

   // Get/set merged filename.
   // <group>
   const QString& getMergedFilename() const;
   void setMergedFilename( const QString& fn );
   // </group>


   // Return a table for the dynamic programming algorithm, if the maximum size
   // of the table allows it.  If not, then return 0.
   int* getDynProgTable( const uint htx, const uint hty ) const;

   // Returns a buffer that will always be big enough for storing a list of
   // horizontal diffs, whatever the size may be.  This should never return 0.
   // Returns the size of the buffers.
   uint getHordiffBuffers( int*&  hbuffer0, int*&  hbuffer1 ) const;

   // Returns a character line buffer that will always be big enough for the
   // maximum line size for computing multiple horizontal diffs.
   uint getHordiffLineBuffers( char*& hdlinebuf0, char*& hdlinebuf1 ) const;

   // Apply the init switch resources to the current commands.
   // This should be carried out after parsing.
   void applyInitSwitch();

   /*----- static member functions -----*/

   // Compare fonts.
   static bool compareFonts( const QFont& f1, const QFont& f2 );

signals:

   /*----- member functions -----*/

   void changed();

private:

   /*----- member functions -----*/

   // Convenience method for initialization.
   inline void setFbColors(
      XxColor     color,
      const char* backstr,
      const char* forestr
   );

   // Initialization with default values.
   void initialize();

   // Initialization with default values like the original xdiff.
   void initializeOriginalXdiff();

   // Update horizontal diffs buffer size.
   void updateHordiffBuffers();

   /*----- data members -----*/

   QRect        _preferredGeometry;
   QString      _styleKey;
   bool         _maximize;
   int          _accelerators[ ACCEL_LAST ];
   QFont        _fontApp;
   QFont        _fontText;
   QColor       _backColors[ COLOR_LAST ];
   QColor       _foreColors[ COLOR_LAST ];
   bool         _boolOpts[ BOOL_LAST ];
   bool         _showOpts[ SHOW_LAST ];
   uint         _tabWidth;
   QString      _commands[ CMD_LAST ];
   QString      _commandSwitch[ CMDSW_LAST ];
   int          _initSwitch[ CMDSW_LAST ];
   uint         _overviewFileWidth;
   uint         _overviewSepWidth;
   uint         _verticalLinePos;
   QString      _tags[ TAG_LAST ];
   QString      _clipboardHeadFormat;
   QString      _clipboardLineFormat;
   XxIgnoreFile _ignoreFile;
   XxHordiff    _hordiffType;
   uint         _hordiffMax;
   uint         _hordiffContext;
   uint         _showPaneMergedViewPercent;
   QString      _mergedFilename;

   // Dynamic programming table used for horizontal diffs computation.
   // <group>
   mutable uint _hdTableSize;
   mutable int* _hdTable;
   uint         _hdBufferSize;
   int*         _hdBuffer0;
   int*         _hdBuffer1;
   uint         _hdLineBufSize;
   char*        _hdLineBuf0;
   char*        _hdLineBuf1;
   // </group>

   /*----- static data members -----*/

   static QRect _defaultGeometry;

};

XX_NAMESPACE_END

#include <resources.inline.h>

#endif


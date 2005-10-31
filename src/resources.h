/******************************************************************************\
 * $Id: resources.h 32 2000-09-21 20:39:55Z  $
 * $Date: 2000-09-21 16:39:55 -0400 (Thu, 21 Sep 2000) $
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

#ifndef INCL_XXDIFF_RESOURCES
#define INCL_XXDIFF_RESOURCES

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_LINE
#include <line.h>
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

#ifndef INCL_STD_STRING
#include <string>
#define INCL_STD_STRING
#endif

#ifndef INCL_STD_MAP
#include <map>
#define INCL_STD_MAP
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxResourcesParser;
class XxApp;


/*==============================================================================
 * CLASS XxResources
 *============================================================================*/

// <summary> container for the resources of the application </summary>

// This container asks the parser to parse the resources and stores them in a
// way that is efficient to retrieve (at least for those resources that require
// it).

class XxResources {

public:

   /*----- constants -----*/

   // Each resource is identified by a unique name accessed thru this enum.
   enum Resource {
      RESOURCE_FIRST,

      // Preferred geometry.
      PREFERRED_GEOMETRY,

      // Accelerators.
      ACCEL_FIRST,
      ACCEL_EXIT = ACCEL_FIRST,
      ACCEL_OPEN_LEFT,
      ACCEL_OPEN_MIDDLE, 
      ACCEL_OPEN_RIGHT, 
      ACCEL_SAVE_AS_LEFT,
      ACCEL_SAVE_AS_MIDDLE,
      ACCEL_SAVE_AS_RIGHT,
      ACCEL_SAVE_AS,
      ACCEL_SAVE_SELECTED_ONLY,
      ACCEL_EDIT_LEFT,
      ACCEL_EDIT_MIDDLE, 
      ACCEL_EDIT_RIGHT, 
      ACCEL_SEARCH,
      ACCEL_SEARCH_FORWARD,
      ACCEL_SEARCH_BACKWARD,
      ACCEL_SCROLL_DOWN,
      ACCEL_SCROLL_UP,
      ACCEL_CURSOR_DOWN,
      ACCEL_CURSOR_UP,
      ACCEL_CURSOR_TOP,
      ACCEL_CURSOR_BOTTOM,
      ACCEL_REDO_DIFF,
      ACCEL_EDIT_DIFF_OPTIONS,
      ACCEL_EDIT_DISPLAY_OPTIONS,
      ACCEL_DIFF_FILES_AT_CURSOR, 
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
      ACCEL_QUALITY_NORMAL,
      ACCEL_QUALITY_FASTEST,
      ACCEL_QUALITY_HIGHEST,
      ACCEL_MERGED_VIEW,
      ACCEL_TOGGLE_TOOLBAR,
      ACCEL_TOGGLE_LINE_NUMBERS,
      ACCEL_TOGGLE_MARKERS,
      ACCEL_TOGGLE_OVERVIEW,
      ACCEL_TOGGLE_SHOW_FILENAMES,
      ACCEL_TOGGLE_HORIZONTAL_DIFFS,
      ACCEL_TOGGLE_IGNORE_HORIZONTAL_WS,
      ACCEL_TOGGLE_CUT_AND_PASTE_ANNOTATIONS,
      ACCEL_HELP_MAN_PAGE,
      ACCEL_HELP_COLOR_LEGEND,
      ACCEL_HELP_ON_CONTEXT,
      ACCEL_HELP_GEN_INIT_FILE,
      ACCEL_HELP_ABOUT,
      ACCEL_MERGED_CLOSE,
      ACCEL_LAST, // Not a real resource.

      // Region colors.
      COLOR_FIRST,

      COLOR_BACK_SAME = COLOR_FIRST, // A in AAA block.
      COLOR_FORE_SAME,

      COLOR_BACK_DIFF_ONE,            // B in BAA, ABA, AAB blocks.
      COLOR_FORE_DIFF_ONE,            
      COLOR_BACK_DIFF_ONE_SUP,        // (shadowed)
      COLOR_FORE_DIFF_ONE_SUP,        
      COLOR_BACK_DIFF_ONE_ONLY,       // (when other is blank)
      COLOR_FORE_DIFF_ONE_ONLY,       
      COLOR_BACK_DIFF_ONE_NONLY,      // (when blank)
      COLOR_FORE_DIFF_ONE_NONLY,       
      COLOR_BACK_DIFF_TWO,            // A in BAA, ABA, AAB blocks.
      COLOR_FORE_DIFF_TWO,            
      COLOR_BACK_DIFF_TWO_SUP,        // (shadowed)
      COLOR_FORE_DIFF_TWO_SUP,        
      COLOR_BACK_DIFF_TWO_ONLY,       // (when other is blank)
      COLOR_FORE_DIFF_TWO_ONLY,       
      COLOR_BACK_DIFF_TWO_NONLY,      // (when blank)
      COLOR_FORE_DIFF_TWO_NONLY,       
                                       
      COLOR_BACK_DELETE,              // A in -AA, A-A, AA- blocks.
      COLOR_FORE_DELETE,              
      COLOR_BACK_DELETE_BLANK,        // - in -AA, A-A, AA- blocks.
      COLOR_FORE_DELETE_BLANK,        
                                       
      COLOR_BACK_INSERT,              // A in A--, -A-, --A blocks.
      COLOR_FORE_INSERT,              
      COLOR_BACK_INSERT_BLANK,        // - in A--, -A-, --A blocks.
      COLOR_FORE_INSERT_BLANK,        
                                       
      COLOR_BACK_DIFF_ALL,            // A,B or C in ABC blocks.
      COLOR_FORE_DIFF_ALL,            
      COLOR_BACK_DIFF_ALL_SUP,        // (shadowed)
      COLOR_FORE_DIFF_ALL_SUP,        
      COLOR_BACK_DIFF_ALL_ONLY,       // (when other is blank)
      COLOR_FORE_DIFF_ALL_ONLY,        
      COLOR_BACK_DIFF_ALL_NONLY,      // (when blank)
      COLOR_FORE_DIFF_ALL_NONLY,        

      COLOR_BACK_DIFFDEL,             // A and B in -AB, A-B, AB- blocks.
      COLOR_FORE_DIFFDEL,           
      COLOR_BACK_DIFFDEL_SUP,         // (shadowed)
      COLOR_FORE_DIFFDEL_SUP,       
      COLOR_BACK_DIFFDEL_ONLY,        // (when other is blank)
      COLOR_FORE_DIFFDEL_ONLY,           
      COLOR_BACK_DIFFDEL_NONLY,       // (when blank)
      COLOR_FORE_DIFFDEL_NONLY,           
      COLOR_BACK_DIFFDEL_BLANK,       // - in -AB, A-B, AB- blocks.
      COLOR_FORE_DIFFDEL_BLANK,

      COLOR_BACK_SELECTED,            // selected text.
      COLOR_FORE_SELECTED,
      COLOR_BACK_SELECTED_SUP,        // (shadowed)
      COLOR_FORE_SELECTED_SUP,

      COLOR_BACK_DELETED,             // deleted text.
      COLOR_FORE_DELETED,
      COLOR_BACK_DELETED_SUP,         // (shadowed)
      COLOR_FORE_DELETED_SUP,

      COLOR_BACK_DIRECTORIES,         // directories in directory diffs.
      COLOR_FORE_DIRECTORIES,      

      COLOR_BACK_MERGED_UNDECIDED,    // undecided text (merged view).
      COLOR_FORE_MERGED_UNDECIDED,
      COLOR_BACK_MERGED_DECIDED,      // decided text (merged view).
      COLOR_FORE_MERGED_DECIDED,      

      // Other colors.
      COLOR_BACKGROUND,
      COLOR_CURSOR,

      COLOR_LAST, // Not a real resource.

      // Fonts.
      FONT_FIRST,
      FONT_APP = FONT_FIRST,
      FONT_TEXT,
      FONT_LAST, // Not a real resource.

      // Boolean options.
      BOOL_FIRST,
      EXIT_ON_SAME = BOOL_FIRST,
      SHOW_TOOLBAR,
      SHOW_LINE_NUMBERS,
      SHOW_MARKERS,
      SHOW_OVERVIEW,
      SHOW_FILENAMES,
      HORIZONTAL_DIFFS,
      IGNORE_HORIZONTAL_WS,
      CUT_AND_PASTE_ANNOTATIONS,
      IGNORE_ERRORS,
      WARN_ABOUT_UNSAVED,
      DISABLE_CURSOR_DISPLAY,
      HIDE_CR,
      DIRDIFF_IGNORE_FILE_CHANGES,
      DIRDIFF_BUILD_FROM_OUTPUT,
      DIRDIFF_RECURSIVE,
      BOOL_LAST, // Not a real resource.

      // Command lines.
      COMMAND_FIRST,
      COMMAND_DIFF_FILES_2 = COMMAND_FIRST,
      COMMAND_DIFF_FILES_3,
      COMMAND_DIFF_DIRECTORIES,
      COMMAND_DIFF_DIRECTORIES_REC,
      COMMAND_MANUAL,
      COMMAND_EDIT,
      COMMAND_LAST, // Not a real resource.

      // Command line options for diff programs.
      CMDOPT_FIRST,
      CMDOPT_FILES_IGNORE_TRAILING = CMDOPT_FIRST,
      CMDOPT_FILES_IGNORE_WHITESPACE,
      CMDOPT_FILES_IGNORE_CASE,
      CMDOPT_FILES_IGNORE_BLANK_LINES,
      CMDOPT_FILES_QUALITY_NORMAL,
      CMDOPT_FILES_QUALITY_FASTEST,
      CMDOPT_FILES_QUALITY_HIGHEST,
      CMDOPT_LAST, // Not a real resource.

      // Blips of text to tag regions in output files.
      TAG_FIRST,
      TAG_CONFLICT_SEPARATOR = TAG_FIRST,
      TAG_CONFLICT_END,
      TAG_CONDITIONAL_IFDEF,
      TAG_CONDITIONAL_ELSEIF,
      TAG_CONDITIONAL_ELSE,
      TAG_CONDITIONAL_ENDIF,
      TAG_LAST, // Not a real resource.

      // Other options.
      TAB_WIDTH,
      OVERVIEW_FILE_WIDTH,
      OVERVIEW_SEP_WIDTH,

      RESOURCE_LAST
   };

   /*----- member functions -----*/

   // Creates or returns the sole instance of this class.
   static XxResources* getInstance();

   // Releases the sole instance of this class.
   static void releaseInstance();

   // Parses available resources in a resource builder.
   void parse( XxResourcesParser& );

   // Checks validity of the documentation of resources.
   bool checkResourcesDoc() const;
   // </group>

   //
   // Provide resources.
   //

   // Provide resource: geometry.
   const QRect& getPreferredGeometry() const;

   // Provide resource: accelerators.
   int getAccelerator( Resource ) const;

   // Provide resource: fonts.
   // <group>
   const QFont& getAppFont() const;
   const QFont& getTextFont() const;
   // </group>

   // Provide resource: display type colors.
   //
   // Get display color for line text.  Returns the resource for the background
   // color.
   Resource getLineColorType(
      const XxLine& line,
      const int     no,
      const bool    sup = false
   ) const;

   // Returns a region type's color.
   // Use background color enum to index.
   void getRegionColor(
      Resource type,
      QColor&  background,
      QColor&  foreground
   ) const;

   // Get/set a specific color.
   // <group>
   const QColor& getColor( const Resource ) const;
   void setColor( const Resource, const QColor& );
   // </group>

   // Provide resource: boolean options.
   // <group>
   bool getBoolOpt( const Resource ) const;
   void setBoolOpt( const Resource, const bool );
   void toggleBoolOpt( const Resource );
   // </group>

   uint getTabWidth() const;
   void setTabWidth( uint );

   // Get/set a command, or command option.
   //
   // cmdId must be between COMMAND_FIRST and COMMAND_LAST.
   // <group>
   const char* getCommand( Resource cmdId ) const;
   const char* getCommandOption( Resource cmdId ) const;
   void setCommand( Resource cmdId, const char* );
   // </group>

   uint getOverviewFileWidth() const;
   uint getOverviewSepWidth() const;
   // </group>

   // Get text tags for conflictual regions.
   const char* getTag( Resource ) const;

   // Convenience methods for setting the quality options in the commands. Note
   // that these methods don't change the commands in the resources, they modify
   // the one that is given.
   enum Quality {
      QUALITY_NORMAL, 
      QUALITY_FASTEST, 
      QUALITY_HIGHEST
   };
   // <group>
   Quality getQuality( const std::string& command ) const;
   void setQuality( std::string& command, Quality quality ) const;
   // </group>

   // Convenience methods for getting/setting an option into a specific command.
   // <group>
   bool isCommandOption( Resource cmdId, Resource cmdOptionId ) const;
   void setCommandOption( Resource cmdId, Resource cmdOptionId, bool setit );
   // </group>

   // Print out the modified resources and associated documentation as a
   // .xxdiffrc file.
   void genInitFile( const XxApp*, std::ostream& ) const;

   // Print out the resource list, default values and documentation.
   void listResources( std::ostream& ) const;


   /*----- static member functions -----*/

   //
   // Resource definition/documentation.
   //
   // <group>
   static const char* getResourceName( Resource );
   static const char* getResourceDoc( Resource );
   // </group>

   // Lookup resource by name. This is a slow, linear procedure.
   static XxResources::Resource getResourceId( const char* resname );

private:

   /*----- member functions -----*/

   // Constructs a resources object with default values.
   XxResources();

   // Destructor.
   ~XxResources();

   // Convenience method.
   bool query( 
      XxResourcesParser&    parser,   
      XxResources::Resource resource,
      std::string&          value
   ) const;

   /*----- data members -----*/

   static XxResources* _instance;

   QRect	_preferredGeometry;
   int		_accelerators[ ACCEL_LAST - ACCEL_FIRST ];
   QFont	_fontApp;
   QFont	_fontText;
   QColor	_colors[ COLOR_LAST - COLOR_FIRST ];
   bool		_boolOpts[ BOOL_LAST - BOOL_FIRST ];
   uint 	_tabWidth;
   std::string  _commands[ COMMAND_LAST - COMMAND_FIRST ];
   std::string  _commandOptions[ CMDOPT_LAST - CMDOPT_FIRST ];
   uint		_overviewFileWidth;
   uint		_overviewSepWidth;
   std::string  _tags[ TAG_LAST - TAG_FIRST ];

};



/*==============================================================================
 * CLASS XxResourcesParser
 *============================================================================*/

// <summary> abstract interface for resource parsers </summary>

class XxResourcesParser {

public:

   /*----- member functions -----*/

   // Destructor.
   virtual ~XxResourcesParser();

   // Query the database for the named resource.  Returns false is not present.
   // Otherwise return true and fills in the value string.
   virtual bool query( 
      XxResources::Resource resource,
      const char*           name,
      std::string&          value
   ) = 0;

protected:

   /*----- classes -----*/
   
   // Useful storage types for subclasses.
   struct ResValue {
      std::string _value;
      bool        _used;
   };

   struct Rltstr {
      bool operator()( const std::string& s1, const std::string& s2 ) const
      {
         return s1 < s2;
      }
   };

};


XX_NAMESPACE_END

#include <resources.inline.h>

#endif

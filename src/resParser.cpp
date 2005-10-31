/******************************************************************************\
 * $Id: resParser.cpp 302 2001-10-23 05:14:10Z blais $
 * $Date: 2001-10-23 01:14:10 -0400 (Tue, 23 Oct 2001) $
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

#include <defs.h>

#include <resParser.h>
#include <exceptions.h>

#ifndef INCL_RESPARSER_Y
#include <resParser.y.h>
#define INCL_RESPARSER_Y
#endif

#include <qaccel.h>
#include <qapplication.h>
#include <qfont.h>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

/*----- classes -----*/

struct StringToken {
   const char* _name;
   int         _token;
   const char* _desc;
};

/*----- variables -----*/

StringToken kwdList[] = {
   { "Geometry", PREFGEOMETRY, 0 },
   { "Accel", ACCEL, 0 },
   { "Color", COLOR, 0 },
   { "FontApp", FONT_APP, 0 },
   { "FontText", FONT_TEXT, 0 },
   { "Show", SHOW, 0 },
   { "Command", COMMAND, 0 },
   { "CommandSwitch", COMMANDSW, 0 },
   { "Tag", TAG, 0 },
   { "TabWidth", TAB_WIDTH, 0 },
   { "OverviewFileWidth", OVERVIEW_FILE_WIDTH, 0 },
   { "OverviewSepWidth", OVERVIEW_SEP_WIDTH, 0 },
   { "VerticalLinePosition", VERTICAL_LINE_POS, 0 },
   { "ClipboardFormat", CLIPBOARD_FORMAT, 0 }
};

StringToken boolkwdList[] = {
   { "ExitOnSame", EXIT_ON_SAME, 0 },
   { "HorizontalDiffs", HORIZONTAL_DIFFS, 0 },
   { "IgnoreHorizontalWhitespace", IGNORE_HORIZONTAL_WS, 0 },
   { "FormatClipboardText", FORMAT_CLIPBOARD_TEXT, 0 },
   { "IgnoreErrors", IGNORE_ERRORS, 0 },
   { "WarnAboutUnsaved", WARN_ABOUT_UNSAVED, 0 },
   { "DisableCursorDisplay", DISABLE_CURSOR_DISPLAY, 0 },
   { "HideCarriageReturns", HIDE_CR, 0 },
   { "DirDiffIgnoreFileChanges", DIRDIFF_IGNORE_FILE_CHANGES, 0 },
   { "DirDiffBuildSolelyFromOutput", DIRDIFF_BUILD_FROM_OUTPUT, 0 },
   { "DirDiffRecursive", DIRDIFF_RECURSIVE, 0 },
   { "UseInternalDiff", USE_INTERNAL_DIFF, 0 }
};

/* Be careful: order must be the same as for token declaration. */
XxBoolOpt boolMap[] = {
   BOOL_EXIT_ON_SAME,
   BOOL_HORIZONTAL_DIFFS,
   BOOL_IGNORE_HORIZONTAL_WS,
   BOOL_FORMAT_CLIPBOARD_TEXT,
   BOOL_IGNORE_ERRORS,
   BOOL_WARN_ABOUT_UNSAVED,
   BOOL_DISABLE_CURSOR_DISPLAY,
   BOOL_HIDE_CR,
   BOOL_DIRDIFF_IGNORE_FILE_CHANGES,
   BOOL_DIRDIFF_BUILD_FROM_OUTPUT,
   BOOL_DIRDIFF_RECURSIVE,
   BOOL_USE_INTERNAL_DIFF
};

StringToken accelList[] = {
   { "Exit", ACCEL_EXIT, 0 },
   { "OpenLeft", ACCEL_OPEN_LEFT, 0 },
   { "OpenMiddle", ACCEL_OPEN_MIDDLE, 0 },
   { "OpenRight", ACCEL_OPEN_RIGHT, 0 },
   { "SaveAsLeft", ACCEL_SAVE_AS_LEFT, 0 },
   { "SaveAsMiddle", ACCEL_SAVE_AS_MIDDLE, 0 },
   { "SaveAsRight", ACCEL_SAVE_AS_RIGHT, 0 },
   { "SaveAs", ACCEL_SAVE_AS, 0 },
   { "SaveSelectedOnly", ACCEL_SAVE_SELECTED_ONLY, 0 },
   { "EditLeft", ACCEL_EDIT_LEFT, 0 },
   { "EditMiddle", ACCEL_EDIT_MIDDLE, 0 },
   { "EditRight", ACCEL_EDIT_RIGHT, 0 },
   { "Search", ACCEL_SEARCH, 0 },
   { "SearchForward", ACCEL_SEARCH_FORWARD, 0 },
   { "SearchBackward", ACCEL_SEARCH_BACKWARD, 0 },
   { "ScrollDown", ACCEL_SCROLL_DOWN, 0 },
   { "ScrollUp", ACCEL_SCROLL_UP, 0 },
   { "CursorDown", ACCEL_CURSOR_DOWN, 0 },
   { "CursorUp", ACCEL_CURSOR_UP, 0 },
   { "CursorTop", ACCEL_CURSOR_TOP, 0 },
   { "CursorBottom", ACCEL_CURSOR_BOTTOM, 0 },
   { "RedoDiff", ACCEL_REDO_DIFF, 0 },
   { "EditDiffOptions", ACCEL_EDIT_DIFF_OPTIONS, 0 },
   { "EditDisplayOptions", ACCEL_EDIT_DISPLAY_OPTIONS, 0 },
   { "DiffFilesAtCursor", ACCEL_DIFF_FILES_AT_CURSOR, 0 },
   { "CopyFileRightToLeft", ACCEL_COPY_RIGHT_TO_LEFT, 0 },
   { "CopyFileLeftToRight", ACCEL_COPY_LEFT_TO_RIGHT, 0 },
   { "RemoveFileLeft", ACCEL_REMOVE_LEFT, 0 },
   { "RemoveFileRight", ACCEL_REMOVE_RIGHT, 0 },
   { "NextDifference", ACCEL_NEXT_DIFFERENCE, 0 },
   { "PreviousDifference", ACCEL_PREVIOUS_DIFFERENCE, 0 },
   { "NextUnselected", ACCEL_NEXT_UNSELECTED, 0 },
   { "PreviousUnselected", ACCEL_PREVIOUS_UNSELECTED, 0 },
   { "SelectGlobalLeft", ACCEL_SELECT_GLOBAL_LEFT, 0 },
   { "SelectGlobalMiddle", ACCEL_SELECT_GLOBAL_MIDDLE, 0 },
   { "SelectGlobalRight", ACCEL_SELECT_GLOBAL_RIGHT, 0 },
   { "SelectGlobalNeither", ACCEL_SELECT_GLOBAL_NEITHER, 0 },
   { "SelectGlobalUnselect", ACCEL_SELECT_GLOBAL_UNSELECT, 0 },
   { "SelectGlobalUnselectedLeft", ACCEL_SELECT_GLOBAL_UNSELECTED_LEFT, 0 },
   { "SelectGlobalUnselectedMiddle", ACCEL_SELECT_GLOBAL_UNSELECTED_MIDDLE, 0 },
   { "SelectGlobalUnselectedRight", ACCEL_SELECT_GLOBAL_UNSELECTED_RIGHT, 0 },
   { "SelectGlobalUnselectedNeither", 
                                    ACCEL_SELECT_GLOBAL_UNSELECTED_NEITHER, 0 },
   { "SelectGlobalMerge", ACCEL_SELECT_GLOBAL_MERGE, 0 },
   { "SelectRegionLeft", ACCEL_SELECT_REGION_LEFT, 0 },
   { "SelectRegionMiddle", ACCEL_SELECT_REGION_MIDDLE, 0 },
   { "SelectRegionRight", ACCEL_SELECT_REGION_RIGHT, 0 },
   { "SelectRegionNeither", ACCEL_SELECT_REGION_NEITHER, 0 },
   { "SelectRegionUnselect", ACCEL_SELECT_REGION_UNSELECT, 0 },
   { "SelectRegionLeftAndNext", ACCEL_SELECT_REGION_LEFT_AND_NEXT, 0 },
   { "SelectRegionMiddleAndNext", ACCEL_SELECT_REGION_MIDDLE_AND_NEXT, 0 },
   { "SelectRegionRightAndNext", ACCEL_SELECT_REGION_RIGHT_AND_NEXT, 0 },
   { "SelectRegionNeitherAndNext", ACCEL_SELECT_REGION_NEITHER_AND_NEXT, 0 },
   { "SelectRegionSplitSwapMerge", ACCEL_SELECT_REGION_SPLIT_SWAP_JOIN, 0 },
   { "SelectLineLeft", ACCEL_SELECT_LINE_LEFT, 0 },
   { "SelectLineMiddle", ACCEL_SELECT_LINE_MIDDLE, 0 },
   { "SelectLineRight", ACCEL_SELECT_LINE_RIGHT, 0 },
   { "SelectLineNeither", ACCEL_SELECT_LINE_NEITHER, 0 },
   { "SelectLineUnselect", ACCEL_SELECT_LINE_UNSELECT, 0 },
   { "TabsAt3", ACCEL_TABS_AT_3, 0 },
   { "TabsAt4", ACCEL_TABS_AT_4, 0 },
   { "TabsAt8", ACCEL_TABS_AT_8, 0 },
   { "IgnoreTrailing", ACCEL_IGNORE_TRAILING, 0 },
   { "IgnoreWhitespace", ACCEL_IGNORE_WHITESPACE, 0 },
   { "IgnoreCase", ACCEL_IGNORE_CASE, 0 },
   { "IgnoreBlankLines", ACCEL_IGNORE_BLANK_LINES, 0 },
   { "HideCarriageReturns", ACCEL_HIDE_CR, 0 },
   { "DirDiffIgnoreFileChanges", ACCEL_DIRDIFF_IGNORE_FILE_CHANGES, 0 },
   { "DirDiffRecursive", ACCEL_DIRDIFF_RECURSIVE, 0 },
   { "UseInternalDiff", ACCEL_USE_INTERNAL_DIFF, 0 },
   { "QualityNormal", ACCEL_QUALITY_NORMAL, 0 },
   { "QualityFastest", ACCEL_QUALITY_FASTEST, 0 },
   { "QualityHighest", ACCEL_QUALITY_HIGHEST, 0 },
   { "MergedView", ACCEL_MERGED_VIEW, 0 },
   { "ToggleToolbar", ACCEL_TOGGLE_TOOLBAR, 0 },
   { "ToggleLineNumbers", ACCEL_TOGGLE_LINE_NUMBERS, 0 },
   { "ToggleMarkers", ACCEL_TOGGLE_MARKERS, 0 },
   { "ToggleVerticalLine", ACCEL_TOGGLE_VERTICAL_LINE, 0 },
   { "ToggleOverview", ACCEL_TOGGLE_OVERVIEW, 0 },
   { "ToggleShowFilenames", ACCEL_TOGGLE_SHOW_FILENAMES, 0 },
   { "ToggleHorizontalDiffs", ACCEL_TOGGLE_HORIZONTAL_DIFFS, 0 },
   { "ToggleIgnoreHorizontalWhitespace", ACCEL_TOGGLE_IGNORE_HORIZONTAL_WS, 0 },
   { "ToggleFormatClipboardText", ACCEL_TOGGLE_FORMAT_CLIPBOARD_TEXT, 0 },
   { "IgnoreFileNone", ACCEL_IGNORE_FILE_NONE, 0 },
   { "IgnoreFileLeft", ACCEL_IGNORE_FILE_LEFT, 0 },
   { "IgnoreFileMiddle", ACCEL_IGNORE_FILE_MIDDLE, 0 },
   { "IgnoreFileRight", ACCEL_IGNORE_FILE_RIGHT, 0 },
   { "HelpManPage", ACCEL_HELP_MAN_PAGE, 0 },
   { "HelpOnContext", ACCEL_HELP_ON_CONTEXT, 0 },
   { "HelpGenInitFile", ACCEL_HELP_GEN_INIT_FILE, 0 },
   { "HelpAbout", ACCEL_HELP_ABOUT, 0 },
   { "MergedClose", ACCEL_MERGED_CLOSE, 0 }
};

StringToken colorList[] = {

   { "Same", COLOR_SAME,
     " Identical text " },
   { "DiffOne", COLOR_DIFF_ONE,
     " Different in one file " },
   { "DiffOneSup", COLOR_DIFF_ONE_SUP,
     " Different in one file (shadowed) " },
   { "DiffOneOnly", COLOR_DIFF_ONE_ONLY,
     " Different in one file (only text on lines) " },
   { "DiffOneNonly", COLOR_DIFF_ONE_NONLY,
     " Different in one file (blank side)" },
   { "DiffTwo", COLOR_DIFF_TWO,
     " Common text in two files only " },
   { "DiffTwoSup", COLOR_DIFF_TWO_SUP,
     " Common text in two files only (shadowed) " },
   { "DiffTwoOnly", COLOR_DIFF_TWO_ONLY,
     " Common text in two files only (only text on lines) " },
   { "DiffTwoNonly", COLOR_DIFF_TWO_NONLY,
     " Common text in two files only (blank side) " },

   { "Delete", COLOR_DELETE,
     " Delete text (side with text) " },
   { "DeleteBlank", COLOR_DELETE_BLANK,
     " Delete text (blank side) " },
                                      
   { "Insert", COLOR_INSERT,
     " Insert text (side with text) " },
   { "InsertBlank", COLOR_INSERT_BLANK,
     " Insert text (blank side) " },

   { "DiffAll", COLOR_DIFF_ALL,
     " Different in all files " },
   { "DiffAllSup", COLOR_DIFF_ALL_SUP,
     " Different in all files (shadowed) " },
   { "DiffAllOnly", COLOR_DIFF_ALL_ONLY,
     " Different in all files (only text on lines) " },
   { "DiffAllNonly", COLOR_DIFF_ALL_NONLY,
     " Different in all files (blank side) " },
                                      
   { "DiffDel", COLOR_DIFFDEL,
     " Different and delete text " },
   { "DiffDelSup", COLOR_DIFFDEL_SUP,
     " Different and delete text (shadowed) " },
   { "DiffDelOnly", COLOR_DIFFDEL_ONLY,
     " Different and delete text (only text on lines) " },
   { "DiffDelNonly", COLOR_DIFFDEL_NONLY,
     " Different and delete text (blank side) " },
   { "DiffDelBlank", COLOR_DIFFDEL_BLANK,
     " Different and delete text (empty side) " },
                                      
   { "Selected", COLOR_SELECTED,
     " Selected text " },
   { "SelectedSup", COLOR_SELECTED_SUP,
     " Selected text (shadowed) " },
                                      
   { "Deleted", COLOR_DELETED,
     " Deleted text " },
   { "DeletedSup", COLOR_DELETED_SUP,
     " Deleted text (shadowed) " },
   { "Ignored", COLOR_IGNORED,
     " Ignore text " },

   { "Directories", COLOR_DIRECTORIES,
     " Directories in directory diffs " },
                                      
   { "MergedUndecided", COLOR_MERGED_UNDECIDED,
     " Merged view undecided text " },
   { "MergedDecided1", COLOR_MERGED_DECIDED_1,
     " Merged view decided text, file 1 " },
   { "MergedDecided2", COLOR_MERGED_DECIDED_2,
     " Merged view decided text, file 2 " },
   { "MergedDecided3", COLOR_MERGED_DECIDED_3,
     " Merged view decided text, file 3 " },

   { "Background", COLOR_BACKGROUND,
     " Global background color " },
   { "Cursor", COLOR_CURSOR,
     " Line cursor color " },
   { "VerticalLine", COLOR_VERTICAL_LINE,
     " Vertical line color " }
};

StringToken commandList[] = {
   { "DiffFiles2", CMD_DIFF_FILES_2, 0 },
   { "DiffFiles3", CMD_DIFF_FILES_3, 0 },
   { "DiffDirectories", CMD_DIFF_DIRECTORIES, 0 },
   { "DiffDirectoriesRec", CMD_DIFF_DIRECTORIES_REC, 0 },
   { "Edit", CMD_EDIT, 0 }
};

StringToken commandSwitchList[] = {
   { "IgnoreTrailingBlanks", CMDSW_FILES_IGNORE_TRAILING, 0 },
   { "IgnoreWhitespace", CMDSW_FILES_IGNORE_WHITESPACE, 0 },
   { "IgnoreCase", CMDSW_FILES_IGNORE_CASE, 0 },
   { "IgnoreBlankLines", CMDSW_FILES_IGNORE_BLANK_LINES, 0 },
   { "QualityNormal", CMDSW_FILES_QUALITY_NORMAL, 0 },
   { "QualityFastest", CMDSW_FILES_QUALITY_FASTEST, 0 },
   { "QualityHighest", CMDSW_FILES_QUALITY_HIGHEST, 0 }
};

StringToken showList[] = {
   { "Toolbar", SHOW_TOOLBAR, 0 },
   { "LineNumbers", SHOW_LINE_NUMBERS, 0 },
   { "Markers", SHOW_MARKERS, 0 },
   { "VerticalLine", SHOW_VERTICAL_LINE, 0 },
   { "Overview", SHOW_OVERVIEW, 0 },
   { "Filenames", SHOW_FILENAMES, 0 } 
};

StringToken tagList[] = {
   { "Conflict.Separator", TAG_CONFLICT_SEPARATOR, 0 },
   { "Conflict.End", TAG_CONFLICT_END, 0 },
   { "Conditional.Ifdef", TAG_CONDITIONAL_IF, 0 },
   { "Conditional.Elseif", TAG_CONDITIONAL_ELSEIF, 0 },
   { "Conditional.Else", TAG_CONDITIONAL_ELSE, 0 },
   { "Conditional.Endif", TAG_CONDITIONAL_ENDIF, 0 }
};

//------------------------------------------------------------------------------
//
int compareTokensStrings( const void* v1, const void* v2 )
{
   return ::strcmp( 
      const_cast<StringToken*>( static_cast<const StringToken*>(v1) )->_name,
      const_cast<StringToken*>( static_cast<const StringToken*>(v2) )->_name
   );
}

//------------------------------------------------------------------------------
//
void sortTokens( StringToken* llist, const int nbmem ) 
{
   qsort( llist, nbmem, sizeof(StringToken), compareTokensStrings );
}

//------------------------------------------------------------------------------
//
int searchTokenString( 
   const StringToken* llist,
   const int          nbmem,
   const char*        name
)
{
   StringToken tok;
   tok._name = name;

   void* ptr = 
      bsearch( &tok, llist, nbmem, sizeof(StringToken), compareTokensStrings );
   if ( ptr != 0 ) {
      return static_cast<StringToken*>(ptr)->_token;
   }
   return -1;
}

//------------------------------------------------------------------------------
//
const char* searchToken( 
   const StringToken* llist,
   const int          nbmem,
   const int          token
)
{
   // Linear search to get a token string from a token value. Note: this does
   // not have to be efficient, since it is only done for outputting files and
   // resources.
   //
   // Return 0 if not found (this should not really happen).
   for ( int ii = 0; ii < nbmem; ++ii ) {
      if ( llist[ii]._token == token ) {
         return llist[ii]._name;
      }
   }
   return 0;
}

//------------------------------------------------------------------------------
//
bool readGeometry( const QString& val, QRect& geometry )
{
   QWidget* desktop = QApplication::desktop();
   XX_ASSERT( desktop != 0 );
   QSize dsize = desktop->size();

   // Reads in a value.  Returns true if successful, false if the resource was
   // not the specified type and left untouched.

   int l = -1;
   int t = -1;
   int w = -1;
   int h = -1;
   const char* vchar = val.latin1();
   if ( sscanf( vchar, "%dx%d+%d+%d", &w, &h, &l, &t ) == 4 ) {
      geometry = QRect( l, t, w, h );
   }
   else if ( sscanf( vchar, "%dx%d-%d+%d", &w, &h, &l, &t ) == 4 ) {
      geometry = QRect( dsize.width()-l-w, t, w, h );
   }
   else if ( sscanf( vchar, "%dx%d+%d-%d", &w, &h, &l, &t ) == 4 ) {
      geometry = QRect( l, dsize.height()-t-h, w, h );
   }
   else if ( sscanf( vchar, "%dx%d-%d-%d", &w, &h, &l, &t ) == 4 ) {
      geometry = QRect( dsize.width()-l-w, dsize.height()-t-h, w, h );
   }
   else if ( sscanf( vchar, "%dx%d", &w, &h ) == 2 ) {
      geometry = QRect( -1, -1, w, h );
   }
   else {
      return false;
   }
   if ( w <= 0 ) {
      geometry = QRect( 0, geometry.top(), dsize.width(), geometry.height() );
   }
   if ( h <= 0 ) {
      geometry = QRect( geometry.left(), 0, geometry.width(), dsize.height() );
   }
   return true;
}

//------------------------------------------------------------------------------
//
void writeGeometry( std::ostream& os, const QRect& geom ) 
{
   os << searchToken( 
      kwdList, sizeof(kwdList)/sizeof(StringToken), PREFGEOMETRY
   ) << ": " << geom.width() << "x" << geom.height() << std::endl;
   // Note: don't write window position.
}

}

//==============================================================================
// NAMESPACE XxResParserNS
//==============================================================================

namespace XxResParserNS {

// Suppress warnings under IRIX.
#ifdef COMPILER_MIPSPRO
#pragma set woff 1209
#pragma set woff 3322
#pragma set woff 1110
#pragma set woff 1174
#pragma set woff 1167
#pragma set woff 1506
#endif

#define YY_NEVER_INTERACTIVE 1
#define YY_SKIP_YYWRAP       1

#define YY_INPUT( buf, result, max_size )       \
if ( is->atEnd() ) {                            \
   result = YY_NULL;                            \
}                                               \
else {                                          \
   int ii = 0;					\
   for ( ; (ii < max_size) && (!is->atEnd()); ++ii ) { \
      is->readRawBytes( &buf[ii], 1 );		\
   }						\
   result = ii;					\
}

#define YY_NO_UNPUT

#define yylex    __yylex
#define yyback   __yyback
#define yyinput  __yyinput
#define yylook   __yylook
#define yyoutput __yyoutput
#define yyracc   __yyracc
#define yyreject __yyreject
#define yyunput __yyunput


/*----- function prototypes -----*/

void yyerror( const char* );
int yywrap();
int parseFromKeywordList(
   const StringToken* llist,
   const int          nbmem,
   int                retval,
   const char*        errmsg,
   const char*        name,
   int&               num,
   bool               noerror = 0
);


/*----- variables -----*/

QTextIStream* is = 0;

#define LEX_BUFFER_MAX	2048
char lexerBuffer[ LEX_BUFFER_MAX ];

#define YYPARSE_PARAM resources
#define YY_DECL       int yylex( YYSTYPE* yylval )

#include <resParser.lex.c>
#include <resParser.y.c>

// Reset warnings under IRIX.
#ifdef COMPILER_MIPSPRO
#pragma reset woff 1209
#pragma reset woff 3322
#pragma reset woff 1110
#pragma reset woff 1174
#pragma reset woff 1167
#pragma reset woff 1506
#endif

//------------------------------------------------------------------------------
//
void yyerror( const char* msg )
{
   // Send errors to stdout so we can filter out the debug info shmeglu while 
   // debugging parser.
#if YYDEBUG != 0
   std::cout
#else
   std::cerr
#endif
             << "Error parsing resource, (line " 
             << yylineno << "): " << msg << std::endl;
}

//------------------------------------------------------------------------------
//
int yywrap()
{
   return 1;
}

//------------------------------------------------------------------------------
//
int parseFromKeywordList(
   const StringToken* llist,
   const int          nbmem,
   int                retval,
   const char*        errmsg,
   const char*        name,
   int&               num,
   bool               noerror
)
{
   int token = searchTokenString( llist, nbmem, name );
   BEGIN(INITIAL);
   if ( token != -1 ) {
      num = token;
      return retval;
   }

   if ( !noerror ) {
      QString os;
      QTextOStream oss( &os );
      oss << "Unknown " << errmsg << ": " << name << flush;
      yyerror( os.latin1() ); 
   }
   num = ERROR_TOKEN;
   return ERROR_TOKEN;
}

}


XX_NAMESPACE_BEGIN
using namespace XxResParserNS;


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxResParser
 *============================================================================*/

//------------------------------------------------------------------------------
//              
void XxResParser::initialize()
{
   // Sort maps for efficiency in lookup.
   sortTokens( kwdList, sizeof(kwdList)/sizeof(StringToken) );
   sortTokens( boolkwdList, sizeof(boolkwdList)/sizeof(StringToken) );
   sortTokens( accelList, sizeof(accelList)/sizeof(StringToken) );
   sortTokens( colorList, sizeof(colorList)/sizeof(StringToken) );
   sortTokens( commandList, sizeof(commandList)/sizeof(StringToken) );
   sortTokens( commandSwitchList,
               sizeof(commandSwitchList)/sizeof(StringToken) );
   sortTokens( showList, sizeof(showList)/sizeof(StringToken) );
   sortTokens( tagList, sizeof(tagList)/sizeof(StringToken) );
}

//------------------------------------------------------------------------------
//
QString XxResParser::getRcFilename()
{
   // Decide on filename.
   const char* XXDIFFRC = getenv( "XXDIFFRC" );
   QString filename = QString::null;
   if ( XXDIFFRC ) {
      filename = XXDIFFRC;
   }
   else {
      const char* HOME = getenv( "HOME" );
      if ( HOME ) {
         filename = HOME;
      }
      else {
         filename = "~";
      }
      filename.append( "/.xxdiffrc" );
   }
   return filename;
}

//------------------------------------------------------------------------------
//
XxResParser::XxResParser()
{
}

//------------------------------------------------------------------------------
//
XxResParser::~XxResParser()
{
}

//------------------------------------------------------------------------------
//
void XxResParser::parse( const QString& filename, XxResources& resources )
{
   // Check for empty filename.
   if ( filename.isEmpty() ) {
      throw XxInternalError( XX_EXC_PARAMS );
   }

   // Stat the file.
   struct stat buf;
   if ( stat( filename.latin1(), &buf ) != 0 ) {
      return; // file does not exist.
   }
   if ( !S_ISREG( buf.st_mode )  ) {
      return; // file is not a regular file.
   }

   // Open the file.
   FILE* fp = fopen( filename.latin1(), "r" );
   if ( !fp ) {
      throw XxIoError( XX_EXC_PARAMS, "Couldn't open resource." );
   }

   QTextIStream file( fp );
   
   // Parse the file.
   parse( file, resources );
}

//------------------------------------------------------------------------------
//
void XxResParser::parse( QTextIStream& input, XxResources& resources )
{
#if YYDEBUG != 0
   yydebug = 1;
#endif

   is = & input;

   try {
      yylineno = 1; // Reset lineno.
   
      yyrestart( 0 );
      YY_FLUSH_BUFFER;
      BEGIN(INITIAL);
      yyparse( &resources );
   }
   catch ( ... ) {
      // Rethrow on error.
      throw;
   }
}

//------------------------------------------------------------------------------
//
void XxResParser::push()
{
   yy_push_state( yy_top_state() );
}

//------------------------------------------------------------------------------
//
void XxResParser::pop()
{
   yy_pop_state();
}

//------------------------------------------------------------------------------
//
QString XxResParser::getColorName( XxColor color )
{
   int icolor = int(color);
   for ( int ii = 0; ii < COLOR_LAST; ++ii ) {
      if ( colorList[ii]._token == icolor ) {
         return QString( colorList[ii]._name );
      }
   }
   return QString::null;
}

//------------------------------------------------------------------------------
//
QString XxResParser::getColorDescription( XxColor color )
{
   int icolor = int(color);
   for ( int ii = 0; ii < COLOR_LAST; ++ii ) {
      if ( colorList[ii]._token == icolor ) {
         return QString( colorList[ii]._desc );
      }
   }
   return QString::null;
}

//------------------------------------------------------------------------------
//
void XxResParser::genInitFile(
   const XxResources& res1,
   const XxResources& res2,
   std::ostream&      os
)
{
   // Compute differences between defaults and current resources.
   using namespace std;
   int ii;

   const int kwdsize = sizeof(kwdList)/sizeof(StringToken);

   const QRect& geom = res1.getPreferredGeometry();
   if ( geom != res2.getPreferredGeometry() ) {
      writeGeometry( os, geom );
   }
   // Perhaps we should change the default init geometry here to use the actual
   // application geometry.

   int nbaccel = sizeof(accelList)/sizeof(StringToken);
   const char* accelStr = searchToken( kwdList, kwdsize, ACCEL );
   for ( ii = 0; ii < nbaccel; ++ii ) {
      XxAccel accel = XxAccel( accelList[ii]._token );
      if ( res1.getAccelerator( accel ) != res2.getAccelerator( accel ) ) {
         int aval = res1.getAccelerator( accel );
         QString astr("");
         if ( aval != 0 ) {
            astr = QAccel::keyToString( aval );
         }
         os << accelStr << "." << accelList[ii]._name << ": \""
            << astr.latin1() << "\"" << endl;
      }
   }

   const QFont& fontApp = res1.getFontApp();
   if ( compareFonts( fontApp, res2.getFontApp() ) ) {
      os << searchToken( kwdList, kwdsize, FONT_APP )
         << ": \"" << fontApp.rawName().latin1() << "\"" << endl;
   }

   const QFont& fontText = res1.getFontText();
   if ( fontText != res2.getFontText() ) {
      os << searchToken( kwdList, kwdsize, FONT_TEXT ) 
         << ": \"" << fontText.rawName().latin1() << "\"" << endl;
   }

   int nbcolors = sizeof(colorList)/sizeof(StringToken);
   const char* colorStr = searchToken( kwdList, kwdsize, COLOR );
   for ( ii = 0; ii < nbcolors; ++ii ) {
      XxColor color = XxColor( colorList[ii]._token );

      if ( res1.getColor( color, true ) != 
           res2.getColor( color, true ) ) {
         os << colorStr << "." << colorList[ii]._name << ".Fore" << ": \""
            << res1.getColor( color, true ).name() << "\"" << endl;
      }

      if ( res1.getColor( color, false ) != 
           res2.getColor( color, false ) ) {
         os << colorStr << "." << colorList[ii]._name << ".Back" << ": \""
            << res1.getColor( color, false ).name() << "\"" << endl;
      }
   }

   int nbbool = sizeof(boolkwdList)/sizeof(StringToken);
   for ( ii = 0; ii < nbbool; ++ii ) {
      XxBoolOpt bo = XxBoolOpt(boolkwdList[ii]._token);
      bool b1 = res1.getBoolOpt( bo );
      if ( b1 != res2.getBoolOpt( bo ) ) {
         os << boolkwdList[ii]._name << ": " 
            << ( b1 ? "True" : "False" ) << endl;
      }
   }

   int nbshow = sizeof(showList)/sizeof(StringToken);
   const char* showStr = searchToken( kwdList, kwdsize, SHOW );
   for ( ii = 0; ii < nbshow; ++ii ) {
      XxShowOpt bo = XxShowOpt(showList[ii]._token);
      bool b1 = res1.getShowOpt( bo );
      if ( b1 != res2.getShowOpt( bo ) ) {
         os << showStr << "." << showList[ii]._name << ": "
            << ( b1 ? "True" : "False" ) << endl;
      }
   }

   if ( res1.getTabWidth() != res2.getTabWidth() ) {
      os << searchToken( kwdList, kwdsize, TAB_WIDTH ) << ": "
         << res1.getTabWidth() << endl;
   }
   
   int nbcommand = sizeof(commandList)/sizeof(StringToken);
   const char* commandStr = searchToken( kwdList, kwdsize, COMMAND );
   for ( ii = 0; ii < nbcommand; ++ii ) {
      XxCommand bo = XxCommand(commandList[ii]._token);
      const QString& b1 = res1.getCommand( bo );
      if ( b1 != res2.getCommand( bo ) ) {
         os << commandStr << "." << commandList[ii]._name << ": \""
            << b1.latin1() << "\"" << endl;
      }
   }

   int nbcommandSwitch = sizeof(commandSwitchList)/sizeof(StringToken);
   const char* commandSwitchStr = searchToken( kwdList, kwdsize, COMMANDSW );
   for ( ii = 0; ii < nbcommandSwitch; ++ii ) {
      XxCommandSwitch bo = XxCommandSwitch(commandSwitchList[ii]._token);
      const QString& b1 = res1.getCommandSwitch( bo );
      if ( b1 != res2.getCommandSwitch( bo ) ) {
         os << commandSwitchStr << "." << commandSwitchList[ii]._name << ": \""
            << b1.latin1() << "\"" << endl;
      }
   }

   if ( res1.getOverviewFileWidth() != res2.getOverviewFileWidth() ) {
      os << searchToken( kwdList, kwdsize, OVERVIEW_FILE_WIDTH ) << ": "
         << res1.getOverviewFileWidth() << endl;
   }

   if ( res1.getOverviewSepWidth() != res2.getOverviewSepWidth() ) {
      os << searchToken( kwdList, kwdsize, OVERVIEW_SEP_WIDTH ) << ": "
         << res1.getOverviewSepWidth() << endl;
   }

   if ( res1.getVerticalLinePos() != res2.getVerticalLinePos() ) {
      os << searchToken( kwdList, kwdsize, VERTICAL_LINE_POS ) << ": "
         << res1.getVerticalLinePos() << endl;
   }

   int nbtag = sizeof(tagList)/sizeof(StringToken);
   const char* tagStr = searchToken( kwdList, kwdsize, TAG );
   for ( ii = 0; ii < nbtag; ++ii ) {
      XxTag bo = XxTag(tagList[ii]._token);
      const QString& b1 = res1.getTag( bo );
      if ( b1 != res2.getTag( bo ) ) {
         os << tagStr << "." << tagList[ii]._name << ": \""
            << b1.latin1() << "\"" << endl;
      }
   }

   if ( res1.getClipboardFormat() != res2.getClipboardFormat() ) {
      os << searchToken( kwdList, kwdsize, CLIPBOARD_FORMAT ) << ": \""
         << res1.getClipboardFormat() << "\"" << endl;
   }

   // Ignore file not saved (cannot be read).

}

//------------------------------------------------------------------------------
//
void XxResParser::listResources( std::ostream& os )
{
   // Output all resource names and documentation.
   const XxResources res;
   using namespace std;

   int ii;
   const int kwdsize = sizeof(kwdList)/sizeof(StringToken);

   writeGeometry( os, res.getPreferredGeometry() );

   int nbaccel = sizeof(accelList)/sizeof(StringToken);
   const char* accelStr = searchToken( kwdList, kwdsize, ACCEL );
   for ( ii = 0; ii < nbaccel; ++ii ) {
      XxAccel accel = XxAccel( accelList[ii]._token );
      int aval = res.getAccelerator( accel );
      QString astr("");
      if ( aval != 0 ) {
         astr = QAccel::keyToString( aval );
      }
      os << accelStr << "." << accelList[ii]._name << ": \""
         << astr.latin1() << "\"" << endl;
   }

   const QFont& fontApp = res.getFontApp();
   os << searchToken( kwdList, kwdsize, FONT_APP )
      << ": \"" << fontApp.rawName().latin1() << "\"" << endl;

   const QFont& fontText = res.getFontText();
   os << searchToken( kwdList, kwdsize, FONT_TEXT ) 
      << ": \"" << fontText.rawName().latin1() << "\"" << endl;

   int nbcolors = sizeof(colorList)/sizeof(StringToken);
   const char* colorStr = searchToken( kwdList, kwdsize, COLOR );
   for ( ii = 0; ii < nbcolors; ++ii ) {
      XxColor color = XxColor( colorList[ii]._token );

      os << colorStr << "." << colorList[ii]._name << ".Fore" << ": \""
         << res.getColor( color, true ).name() << "\"" << endl;

      os << colorStr << "." << colorList[ii]._name << ".Back" << ": \""
         << res.getColor( color, false ).name() << "\"" << endl;
   }

   int nbbool = sizeof(boolkwdList)/sizeof(StringToken);
   for ( ii = 0; ii < nbbool; ++ii ) {
      XxBoolOpt bo = XxBoolOpt(boolkwdList[ii]._token);
      bool b1 = res.getBoolOpt( bo );
      os << boolkwdList[ii]._name << ": " << ( b1 ? "True" : "False" ) << endl;
   }

   int nbshow = sizeof(showList)/sizeof(StringToken);
   const char* showStr = searchToken( kwdList, kwdsize, SHOW );
   for ( ii = 0; ii < nbshow; ++ii ) {
      XxShowOpt bo = XxShowOpt(showList[ii]._token);
      bool b1 = res.getShowOpt( bo );
      os << showStr << "." << showList[ii]._name << ": " 
         << ( b1 ? "True" : "False" ) << endl;
   }

   os << searchToken( kwdList, kwdsize, TAB_WIDTH ) << ": "
      << res.getTabWidth() << endl;
   
   int nbcommand = sizeof(commandList)/sizeof(StringToken);
   const char* commandStr = searchToken( kwdList, kwdsize, COMMAND );
   for ( ii = 0; ii < nbcommand; ++ii ) {
      XxCommand bo = XxCommand(commandList[ii]._token);
      const QString& b1 = res.getCommand( bo );
      os << commandStr << "." << commandList[ii]._name << ": \""
         << b1.latin1() << "\"" << endl;
   }

   int nbcommandSwitch = sizeof(commandSwitchList)/sizeof(StringToken);
   const char* commandSwitchStr = searchToken( kwdList, kwdsize, COMMANDSW );
   for ( ii = 0; ii < nbcommandSwitch; ++ii ) {
      XxCommandSwitch bo = XxCommandSwitch(commandSwitchList[ii]._token);
      const QString& b1 = res.getCommandSwitch( bo );
      os << commandSwitchStr << "." << commandSwitchList[ii]._name << ": \""
         << b1.latin1() << "\"" << endl;
   }

   os << searchToken( kwdList, kwdsize, OVERVIEW_FILE_WIDTH ) << ": "
      << res.getOverviewFileWidth() << endl;

   os << searchToken( kwdList, kwdsize, OVERVIEW_SEP_WIDTH ) << ": "
      << res.getOverviewSepWidth() << endl;

   os << searchToken( kwdList, kwdsize, VERTICAL_LINE_POS ) << ": "
      << res.getVerticalLinePos() << endl;

   int nbtag = sizeof(tagList)/sizeof(StringToken);
   const char* tagStr = searchToken( kwdList, kwdsize, TAG );
   for ( ii = 0; ii < nbtag; ++ii ) {
      XxTag bo = XxTag(tagList[ii]._token);
      const QString& b1 = res.getTag( bo );
      os << tagStr << "." << tagList[ii]._name << ": \""
         << b1.latin1() << "\"" << endl;
   }

   os << searchToken( kwdList, kwdsize, CLIPBOARD_FORMAT ) << ": \""
      << res.getClipboardFormat() << "\"" << endl;

   // Ignore file not saved (cannot be read).
}   

//------------------------------------------------------------------------------
//
QString XxResParser::getBoolOptName( XxBoolOpt bo )
{
   const char* stoken = searchToken( 
      boolkwdList, sizeof(boolkwdList)/sizeof(StringToken), int(bo) + 1000 
   );
   return QString( stoken );
}

//------------------------------------------------------------------------------
//
bool XxResParser::compareFonts( const QFont& f1, const QFont& f2 )
{
   if ( f1.rawMode() || f2.rawMode() ) {
      return f1.rawName() == f2.rawName();
   }
   return f1 == f2;
}

XX_NAMESPACE_END


/******************************************************************************\
 * $Id: resParser.cpp 446 2001-12-06 06:06:31Z blais $
 * $Date: 2001-12-06 01:06:31 -0500 (Thu, 06 Dec 2001) $
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
 ******************************************************************************/

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <defs.h>

#include <resParser.h>
#include <exceptions.h>
#include <help.h>

#ifndef INCL_RESPARSER_Y
#include <resParser.y.h>
#define INCL_RESPARSER_Y
#endif

#include <qaccel.h>
#include <qapplication.h>
#include <qfont.h>
#include <qfile.h>
#include <qfileinfo.h>

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
   { "Geometry", PREFGEOMETRY,
     "Preferred geometry upon initialization.  Format is the same a X geometry \
specification, plus you can also use `Maximize' to maximize on startup" },

   { "Accel", ACCEL,
   "Accelerators for most functionality. The name of the accelerator should be \
explicit enough that you can find which function it is bound to." },

   { "Color", COLOR,
     "Color choice for diff hunks, and for certain other items in the text \
view." },

   { "FontApp", FONT_APP,
     "General application font, used for widgets and menus." },

   { "FontText", FONT_TEXT,
     "Font to use for diff text view." },

   { "Show", SHOW, 
     "Set of options to determine if some UI display element is visible or not \
upon startup." },

   { "Command", COMMAND, 
     "Commands to use to generate diffs.  xxdiff is only an interface to \
display diff results, it doesn't actually compute diffs itself, unless you use \
the internal commands options, where it uses the same arguments as specified \
here.  Most likely these are the GNU diff commands and options.  Note that you \
can specify command switches here and the user interface will be automatically \
updated accordingly (e.g. if you set a command with the -w switch, and \
UI understands that the ignore whitespace feature has been turned on)." },

   { "CommandSwitch", COMMANDSW,
     "Definitions of cmdline switches that should be used to toggle diff \
options. These resources allow you to specify what switch corresponds to what \
feature.  The defaults are the GNU diff switches (see diff(1)).  These are \
only valid for two-file diff for now.  See also the InitSwitch resources." },

   { "InitSwitch", INITSW,
     "Initialization of a particular switch after the resources have been read \
in. This is used to modify the command using the switch definitions. \
In other words, you could either set the switch within the command \
specification, or use a plain command and enable/disable the switch using this \
resource. This resource is only provided for completeness and convenience, as \
setting the appropriate switch in the command itself amounts to the same. \
See also the CommandSwitch resources.  For the quality options, you should \
just set a single one to 'true'." },

   { "Tag", TAG, 
     "Tags used for conditionals used for unselected regions, when that option \
is used to save files." },
   
   { "TabWidth", TAB_WIDTH, 
     "Tab character alignment width." },

   { "OverviewFileWidth", OVERVIEW_FILE_WIDTH, 
     "In overview area, width (in pixels) of each file." },

   { "OverviewSepWidth", OVERVIEW_SEP_WIDTH,
     "In overview area, width (in pixels) between each file." },

   { "VerticalLinePosition", VERTICAL_LINE_POS, 
     "Initial column to draw vertical alignment line." },

   { "ClipboardFormat", CLIPBOARD_FORMAT, 
     "Format of formatted clipboard text." },

   { "HorizontalDiffType", HORDIFF_TYPE,
     "Type of horizontal diffs display.  Can be one of \
None, Single, Multiple.  None: doesn't compute nor display horizontal diff; \
Single: just bracket the changes from line beginnings and ends; \
Multiple: compute multiple horizontal diff (if the sizes fit in the maximum \
size (see HorizontalDiffMax resource)." },

   { "HorizontalDiffMax", HORDIFF_MAX,
     "Maximum table size for dynamic-programming table used for computing \
the horizontal diff for change lines.  If the size of one horizontal hunk \
times the size of the other horizontal hunk is over that size, there are no \
multiple horizontal diffs computed for that line, and the algorithm behaves \
like the Single algorithm.  You most likely don't want to touch that value." },

   { "HorizontalDiffContext", HORDIFF_CONTEXT,
     "Minimum amount of characters or token that need to be common to both \
lines between multiple horizontal diff hunks.  If there are not that amount of \
common characters, skip the horizontal hunk and display it as a changed \
region.  This resource is very useful to remove the cases where few \
characters align, resulting in many small horizontal hunks, which can be \
quite confusing.  Usually a value of 5 gives enough horizontal context for \
the eye to figure out what happenened." },

   { "ShowPaneMergedViewPercent", SHOW_PANE_MERGED_VIEW_PERCENT, 
     "Initial vertical percentage size, between 0 and 100, of the pane merged \
view on startup." }

};

StringToken boolkwdList[] = {
   { "ExitOnSame", EXIT_ON_SAME,
     "If true, exit if both files have no differences." },

   { "IgnoreHorizontalWhitespace", IGNORE_HORIZONTAL_WS, 
     "Ignore horizontal whitespace in horizontal diffs." },

   { "FormatClipboardText", FORMAT_CLIPBOARD_TEXT, 
     "Enables clipboard (copy-paste) formatting.  If disable, text that goes \
in the clipboard is simply left unformatted." },

   { "IgnoreErrors", IGNORE_ERRORS,
     "Disables diff errors reporting." },

   { "WarnAboutUnsaved", WARN_ABOUT_UNSAVED, 
     "Enables warning upon exiting with unsaved selections." },

   { "DisableCursorDisplay", DISABLE_CURSOR_DISPLAY, 
     "Disables displaying the line cursor." },

   { "HideCarriageReturns", HIDE_CR, 
     "Hides CR characters found in files created under DOS/Windows." },

   { "DirDiffIgnoreFileChanges", DIRDIFF_IGNORE_FILE_CHANGES, 
     "In directory diffs, ignores file changes, show just additions and \
deletions." },

   { "DirDiffBuildSolelyFromOutput", DIRDIFF_BUILD_FROM_OUTPUT, 
     "In directory diffs, building diffs only from output, not checking \
against actual directory contents.  This is a self-verification feature \
only, and unless you're doing developemnt you should leave this to default \
(true)." },

   { "DirDiffRecursive", DIRDIFF_RECURSIVE,
     "In directory diffs, enable recursive diff'ing of subdirectories." },

   { "UseInternalDiff", USE_INTERNAL_DIFF,
     "(Not implemented) Use internal diff computation, does not spawn external \
diff program." }

};

/* Be careful: order must be the same as for token declaration. */
XxBoolOpt boolMap[] = {
   BOOL_EXIT_ON_SAME,
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
   { "ScrollDown", ACCEL_PAGE_DOWN, 0 },
   { "ScrollUp", ACCEL_PAGE_UP, 0 },
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
   { "SelectGlobalUnselectedNeither", ACCEL_SELECT_GLOBAL_UNSELECTED_NEITHER, 
     0 },
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
   { "TogglePaneMergedView", ACCEL_TOGGLE_PANE_MERGED_VIEW, 0 },
   { "TogglePopupMergedView", ACCEL_TOGGLE_POPUP_MERGED_VIEW, 0 },
   { "ToggleToolbar", ACCEL_TOGGLE_TOOLBAR, 0 },
   { "ToggleLineNumbers", ACCEL_TOGGLE_LINE_NUMBERS, 0 },
   { "ToggleMarkers", ACCEL_TOGGLE_MARKERS, 0 },
   { "ToggleVerticalLine", ACCEL_TOGGLE_VERTICAL_LINE, 0 },
   { "ToggleOverview", ACCEL_TOGGLE_OVERVIEW, 0 },
   { "ToggleShowFilenames", ACCEL_TOGGLE_SHOW_FILENAMES, 0 },
   { "HorizontalDiffNone", ACCEL_HORDIFF_NONE, 0 },
   { "HorizontalDiffSingle", ACCEL_HORDIFF_SINGLE, 0 },
   { "HorizontalDiffMultiple", ACCEL_HORDIFF_MULTIPLE, 0 },
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
                                      
   { "DiffOne", COLOR_DIFF_ONE,
     " (diff3 only) Different in one file " },
   { "DiffOneSup", COLOR_DIFF_ONE_SUP,
     " (diff3 only) Different in one file (shadowed) " },
   { "DiffOneOnly", COLOR_DIFF_ONE_ONLY,
     " (diff3 only) Different in one file (only text on lines) " },
   { "DiffOneNonly", COLOR_DIFF_ONE_NONLY,
     " (diff3 only) Different in one file (blank side)" },
   { "DiffTwo", COLOR_DIFF_TWO,
     " (diff3 only) Common text in two files only " },
   { "DiffTwoSup", COLOR_DIFF_TWO_SUP,
     " (diff3 only) Common text in two files only (shadowed) " },
   { "DiffTwoOnly", COLOR_DIFF_TWO_ONLY,
     " (diff3 only) Common text in two files only (only text on lines) " },
   { "DiffTwoNonly", COLOR_DIFF_TWO_NONLY,
     " (diff3 only) Common text in two files only (blank side) " },

   { "Delete", COLOR_DELETE,
     " (diff3 only) Delete text (side with text) " },
   { "DeleteBlank", COLOR_DELETE_BLANK,
     " (diff3 only) Delete text (blank side) " },
                                      
   { "DiffDel", COLOR_DIFFDEL,
     " (diff3 only) Different and delete text " },
   { "DiffDelSup", COLOR_DIFFDEL_SUP,
     " (diff3 only) Different and delete text (shadowed) " },
   { "DiffDelOnly", COLOR_DIFFDEL_ONLY,
     " (diff3 only) Different and delete text (only text on lines) " },
   { "DiffDelNonly", COLOR_DIFFDEL_NONLY,
     " (diff3 only) Different and delete text (blank side) " },
   { "DiffDelBlank", COLOR_DIFFDEL_BLANK,
     " (diff3 only) Different and delete text (empty side) " },
                                      
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
     " (dir.diffs only) Directories in directory diffs " },
                                      
   { "MergedUndecided", COLOR_MERGED_UNDECIDED,
     " Merged view undecided text " },
   { "MergedDecided1", COLOR_MERGED_DECIDED_1,
     " Merged view decided text, file 1 " },
   { "MergedDecided1Sup", COLOR_MERGED_DECIDED_1_SUP,
     " Merged view decided text, file 1 (shadowed) " },
   { "MergedDecided2", COLOR_MERGED_DECIDED_2,
     " Merged view decided text, file 2 " },
   { "MergedDecided2Sup", COLOR_MERGED_DECIDED_2_SUP,
     " Merged view decided text, file 2 (shadowed) " },
   { "MergedDecided3", COLOR_MERGED_DECIDED_3,
     " Merged view decided text, file 3 " },
   { "MergedDecided3Sup", COLOR_MERGED_DECIDED_3_SUP,
     " Merged view decided text, file 3 (shadowed) " },
   { "MergedDecidedNeither", COLOR_MERGED_DECIDED_NEITHER,
     " Merged view decided text, neither files " },

   { "Background", COLOR_BACKGROUND,
     " Global background color " },
   { "Cursor", COLOR_CURSOR,
     " Line cursor color " },
   { "VerticalLine", COLOR_VERTICAL_LINE,
     " Vertical line color " }
};

StringToken commandList[] = {
   { "DiffFiles2", CMD_DIFF_FILES_2, 
     "Command to use for comparing two files." },

   { "DiffFiles3", CMD_DIFF_FILES_3, 
     "Command to use for comparing three files." },
   
   { "DiffDirectories", CMD_DIFF_DIRECTORIES, 
     "Command to use for comparing two directories, non-recursively." },
   
   { "DiffDirectoriesRec", CMD_DIFF_DIRECTORIES_REC, 
     "Command to use for comparing two directories, recursively." },
   
   { "Edit", CMD_EDIT, 
     "Command to use to spawn an editor on a file." }
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
   { "Toolbar", SHOW_TOOLBAR, 
     "Show toolbar on startup." },

   { "LineNumbers", SHOW_LINE_NUMBERS, 
     "Show line numbers on startup." },

   { "VerticalLine", SHOW_VERTICAL_LINE, 
     "Show vertical line on startup." },

   { "Overview", SHOW_OVERVIEW, 
     "Show toolbar on startup." },

   { "Filenames", SHOW_FILENAMES, 
     "Show toolbar on startup." },

   { "PaneMergedView", SHOW_PANE_MERGED_VIEW,
     "Show pane merged view on startup." },

   { "PopupMergedView", SHOW_POPUP_MERGED_VIEW,
     "Show popup merged view on startup." }
};

StringToken tagList[] = {
   { "Conflict.Separator", TAG_CONFLICT_SEPARATOR, 
     "String used to separate chunks when saving with conflicts." },

   { "Conflict.End", TAG_CONFLICT_END, 
     "String used to end chunks when saving with conflicts." },

   { "Conditional.Ifdef", TAG_CONDITIONAL_IF, 
     "Ifdef String used when saving with conditionals." },

   { "Conditional.Elseif", TAG_CONDITIONAL_ELSEIF, 
     "Elseif String used when saving with conditionals." },

   { "Conditional.Else", TAG_CONDITIONAL_ELSE, 
     "Else String used when saving with conditionals." },

   { "Conditional.Endif", TAG_CONDITIONAL_ENDIF, 
     "Endif String used when saving with conditionals." }
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

#define STPARAM( ll )  ll, sizeof(ll)/sizeof(StringToken)

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
const StringToken* searchToken( 
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
         return &( llist[ii] );
      }
   }
   XX_ABORT();
   return 0;
}

//------------------------------------------------------------------------------
//
const char* searchTokenName(
   const StringToken* llist,
   const int          nbmem,
   const int          token
)
{
   return searchToken( llist, nbmem, token )->_name;
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
QString formatGeometry( const QRect& geom ) 
{
   return QString( "%1x%2" ).arg( geom.width() ).arg( geom.height() );
   // Note: don't write window position.
}

//------------------------------------------------------------------------------
//
void drbegin( QTextStream& os )
{
   os << "<PRE>" << endl;
}

//------------------------------------------------------------------------------
//
void drend( QTextStream& os )
{
   os << "</PRE>" << endl;
}

//------------------------------------------------------------------------------
//
void ddbegin( QTextStream& os )
{
   os << "<BLOCKQUOTE>" << endl;
}

//------------------------------------------------------------------------------
//
void ddend( QTextStream& os )
{
   os << "</BLOCKQUOTE>" << endl;
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

QTextStream* is = 0;

#define LEX_BUFFER_MAX	2048
char lexerBuffer[ LEX_BUFFER_MAX ];

#define YYPARSE_PARAM resources
#define YY_DECL       int yylex( YYSTYPE* yylval )

#include <resParser.l.c>
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
   sortTokens( STPARAM(kwdList) );
   sortTokens( STPARAM(boolkwdList) );
   sortTokens( STPARAM(accelList) );
   sortTokens( STPARAM(colorList) );
   sortTokens( STPARAM(commandList) );
   sortTokens( STPARAM(commandSwitchList) );
   sortTokens( STPARAM(showList) );
   sortTokens( STPARAM(tagList) );
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
   QFileInfo finfo( filename );
   if ( !finfo.exists() ) {
      return; // file does not exist.
   }
   if ( !finfo.isFile() ) {
      return; // file is not a regular file.
   }

   QFile file( filename );
   if ( file.open( IO_ReadOnly ) ) {
      QTextStream filestr( &file );
   
      // Parse the file.
      parse( filestr, resources );
      
      file.close();
   }
   else {
      throw XxIoError( XX_EXC_PARAMS, "Couldn't open resource file." );
   }
}

//------------------------------------------------------------------------------
//
void XxResParser::parse( QTextStream& input, XxResources& resources )
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

   // Apply the init switch resources after they have been parsed.
   resources.applyInitSwitch();
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
   QTextStream&       os
)
{
   // Compute differences between defaults and current resources.
   int ii;

   const QRect& geom = res1.getPreferredGeometry();
   if ( geom != res2.getPreferredGeometry() ) {
      os << searchTokenName( STPARAM(kwdList), PREFGEOMETRY ) << ": "
         << formatGeometry( geom ) << endl;
   }
   // Perhaps we should change the default init geometry here to use the actual
   // application geometry.

   int nbaccel = sizeof(accelList)/sizeof(StringToken);
   const char* accelStr = searchTokenName( STPARAM(kwdList), ACCEL );
   for ( ii = 0; ii < nbaccel; ++ii ) {
      XxAccel accel = XxAccel( accelList[ii]._token );
      if ( res1.getAccelerator( accel ) != res2.getAccelerator( accel ) ) {
         int aval = res1.getAccelerator( accel );
         QString astr("");
         if ( aval != 0 ) {
            astr = QAccel::keyToString( aval );
         }
         os << accelStr << "." << accelList[ii]._name << ": \""
            << astr << "\"" << endl;
      }
   }

   const QFont& fontApp = res1.getFontApp();
   if ( !XxResources::compareFonts( fontApp, res2.getFontApp() ) ) {
      os << searchTokenName( STPARAM(kwdList), FONT_APP )
         << ": \"" << fontApp.rawName() << "\"" << endl;
   }

   const QFont& fontText = res1.getFontText();
   if ( !XxResources::compareFonts( fontText, res2.getFontText() ) ) {
      os << searchTokenName( STPARAM(kwdList), FONT_TEXT ) 
         << ": \"" << fontText.rawName() << "\"" << endl;
   }

   int nbcolors = sizeof(colorList)/sizeof(StringToken);
   const char* colorStr = searchTokenName( STPARAM(kwdList), COLOR );
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
      XxBoolOpt bo = boolMap[ boolkwdList[ii]._token - BOOLKWD_BASE ];
      bool b1 = res1.getBoolOpt( bo );
      if ( b1 != res2.getBoolOpt( bo ) ) {
         os << boolkwdList[ii]._name << ": " 
            << ( b1 ? "True" : "False" ) << endl;
      }
   }

   int nbshow = sizeof(showList)/sizeof(StringToken);
   const char* showStr = searchTokenName( STPARAM(kwdList), SHOW );
   for ( ii = 0; ii < nbshow; ++ii ) {
      XxShowOpt bo = XxShowOpt(showList[ii]._token);
      bool b1 = res1.getShowOpt( bo );
      if ( b1 != res2.getShowOpt( bo ) ) {
         os << showStr << "." << showList[ii]._name << ": "
            << ( b1 ? "True" : "False" ) << endl;
      }
   }

   if ( res1.getTabWidth() != res2.getTabWidth() ) {
      os << searchTokenName( STPARAM(kwdList), TAB_WIDTH ) << ": "
         << res1.getTabWidth() << endl;
   }
   
   int nbcommand = sizeof(commandList)/sizeof(StringToken);
   const char* commandStr = searchTokenName( STPARAM(kwdList), COMMAND );
   for ( ii = 0; ii < nbcommand; ++ii ) {
      XxCommand bo = XxCommand(commandList[ii]._token);
      const QString& b1 = res1.getCommand( bo );
      if ( b1 != res2.getCommand( bo ) ) {
         os << commandStr << "." << commandList[ii]._name << ": \""
            << b1 << "\"" << endl;
      }
   }

   int nbcommandSwitch = sizeof(commandSwitchList)/sizeof(StringToken);
   const char* commandSwitchStr =
      searchTokenName( STPARAM(kwdList), COMMANDSW );
   for ( ii = 0; ii < nbcommandSwitch; ++ii ) {
      XxCommandSwitch bo = XxCommandSwitch(commandSwitchList[ii]._token);
      const QString& b1 = res1.getCommandSwitch( bo );
      if ( b1 != res2.getCommandSwitch( bo ) ) {
         os << commandSwitchStr << "." << commandSwitchList[ii]._name << ": \""
            << b1 << "\"" << endl;
      }
   }

   const char* initSwitchStr = searchTokenName( STPARAM(kwdList), INITSW );
   for ( ii = 0; ii < nbcommandSwitch; ++ii ) {
      XxCommandSwitch bo = XxCommandSwitch(commandSwitchList[ii]._token);
      int b1 = res1.getInitSwitch( bo );
      XX_CHECK( -1 <= b1 && b1 <= 1 );
      char* bmap[3] = { "Nop", "False", "True" };
      if ( b1 != res2.getInitSwitch( bo ) ) {
         os << initSwitchStr << "." << commandSwitchList[ii]._name << ": "
            << bmap[b1+1] << endl;
      }
   }

   if ( res1.getOverviewFileWidth() != res2.getOverviewFileWidth() ) {
      os << searchTokenName( STPARAM(kwdList), OVERVIEW_FILE_WIDTH ) << ": "
         << res1.getOverviewFileWidth() << endl;
   }

   if ( res1.getOverviewSepWidth() != res2.getOverviewSepWidth() ) {
      os << searchTokenName( STPARAM(kwdList), OVERVIEW_SEP_WIDTH ) << ": "
         << res1.getOverviewSepWidth() << endl;
   }

   if ( res1.getVerticalLinePos() != res2.getVerticalLinePos() ) {
      os << searchTokenName( STPARAM(kwdList), VERTICAL_LINE_POS ) << ": "
         << res1.getVerticalLinePos() << endl;
   }

   int nbtag = sizeof(tagList)/sizeof(StringToken);
   const char* tagStr = searchTokenName( STPARAM(kwdList), TAG );
   for ( ii = 0; ii < nbtag; ++ii ) {
      XxTag bo = XxTag(tagList[ii]._token);
      const QString& b1 = res1.getTag( bo );
      if ( b1 != res2.getTag( bo ) ) {
         os << tagStr << "." << tagList[ii]._name << ": \""
            << b1 << "\"" << endl;
      }
   }

   if ( res1.getClipboardFormat() != res2.getClipboardFormat() ) {
      os << searchTokenName( STPARAM(kwdList), CLIPBOARD_FORMAT ) << ": \""
         << res1.getClipboardFormat() << "\"" << endl;
   }

   if ( res1.getHordiffType() != res2.getHordiffType() ) {
      const char* hdtstr[3] = { "None", "Single", "Multiple" };
      os << searchTokenName( STPARAM(kwdList), HORDIFF_TYPE ) << ": "
         << hdtstr[ int(res1.getHordiffType()) ] << endl;
   }

   if ( res1.getHordiffMax() != res2.getHordiffMax() ) {
      os << searchTokenName( STPARAM(kwdList), HORDIFF_MAX ) << ": "
         << res1.getHordiffMax() << endl;
   }

   if ( res1.getHordiffContext() != res2.getHordiffContext() ) {
      os << searchTokenName( STPARAM(kwdList), HORDIFF_CONTEXT ) << ": "
         << res1.getHordiffContext() << endl;
   }

   if ( res1.getShowPaneMergedViewPercent() !=
        res2.getShowPaneMergedViewPercent() ) {
      os << searchTokenName( STPARAM(kwdList), SHOW_PANE_MERGED_VIEW_PERCENT )
         << ": " << res1.getShowPaneMergedViewPercent() << endl;
   }

   // Ignore file not saved (cannot be read).
}

//------------------------------------------------------------------------------
//
void XxResParser::listResources( QTextStream& os )
{
   // Output all resource names and documentation.
   const XxResources res( false );

   int ii;

   os << searchTokenName( STPARAM(kwdList), PREFGEOMETRY ) << ": "
      << formatGeometry( res.getPreferredGeometry() ) << endl;

   int nbaccel = sizeof(accelList)/sizeof(StringToken);
   const char* accelStr = searchTokenName( STPARAM(kwdList), ACCEL );
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
   os << searchTokenName( STPARAM(kwdList), FONT_APP )
      << ": \"" << fontApp.rawName().latin1() << "\"" << endl;

   const QFont& fontText = res.getFontText();
   os << searchTokenName( STPARAM(kwdList), FONT_TEXT ) 
      << ": \"" << fontText.rawName().latin1() << "\"" << endl;

   int nbcolors = sizeof(colorList)/sizeof(StringToken);
   const char* colorStr = searchTokenName( STPARAM(kwdList), COLOR );
   for ( ii = 0; ii < nbcolors; ++ii ) {
      XxColor color = XxColor( colorList[ii]._token );

      os << colorStr << "." << colorList[ii]._name << ".Fore" << ": \""
         << res.getColor( color, true ).name() << "\"" << endl;

      os << colorStr << "." << colorList[ii]._name << ".Back" << ": \""
         << res.getColor( color, false ).name() << "\"" << endl;
   }

   int nbbool = sizeof(boolkwdList)/sizeof(StringToken);
   for ( ii = 0; ii < nbbool; ++ii ) {
      XxBoolOpt bo = boolMap[ boolkwdList[ii]._token - BOOLKWD_BASE ];
      bool b1 = res.getBoolOpt( bo );
      os << boolkwdList[ii]._name << ": " << ( b1 ? "True" : "False" ) << endl;
   }

   int nbshow = sizeof(showList)/sizeof(StringToken);
   const char* showStr = searchTokenName( STPARAM(kwdList), SHOW );
   for ( ii = 0; ii < nbshow; ++ii ) {
      XxShowOpt bo = XxShowOpt(showList[ii]._token);
      bool b1 = res.getShowOpt( bo );
      os << showStr << "." << showList[ii]._name << ": " 
         << ( b1 ? "True" : "False" ) << endl;
   }

   os << searchTokenName( STPARAM(kwdList), TAB_WIDTH ) << ": "
      << res.getTabWidth() << endl;
   
   int nbcommand = sizeof(commandList)/sizeof(StringToken);
   const char* commandStr = searchTokenName( STPARAM(kwdList), COMMAND );
   for ( ii = 0; ii < nbcommand; ++ii ) {
      XxCommand bo = XxCommand(commandList[ii]._token);
      const QString& b1 = res.getCommand( bo );
      os << commandStr << "." << commandList[ii]._name << ": \""
         << b1.latin1() << "\"" << endl;
   }

   int nbcommandSwitch = sizeof(commandSwitchList)/sizeof(StringToken);
   const char* commandSwitchStr =
      searchTokenName( STPARAM(kwdList), COMMANDSW );
   for ( ii = 0; ii < nbcommandSwitch; ++ii ) {
      XxCommandSwitch bo = XxCommandSwitch(commandSwitchList[ii]._token);
      const QString& b1 = res.getCommandSwitch( bo );
      os << commandSwitchStr << "." << commandSwitchList[ii]._name << ": \""
         << b1.latin1() << "\"" << endl;
   }

   const char* initSwitchStr = searchTokenName( STPARAM(kwdList), INITSW );
   for ( ii = 0; ii < nbcommandSwitch; ++ii ) {
      XxCommandSwitch bo = XxCommandSwitch(commandSwitchList[ii]._token);
      int b1 = res.getInitSwitch( bo );
      XX_CHECK( -1 <= b1 && b1 <= 1 );
      char* bmap[3] = { "Nop", "False", "True" };
      os << initSwitchStr << "." << commandSwitchList[ii]._name << ": "
         << bmap[b1+1] << endl;
   }

   os << searchTokenName( STPARAM(kwdList), OVERVIEW_FILE_WIDTH ) << ": "
      << res.getOverviewFileWidth() << endl;

   os << searchTokenName( STPARAM(kwdList), OVERVIEW_SEP_WIDTH ) << ": "
      << res.getOverviewSepWidth() << endl;

   os << searchTokenName( STPARAM(kwdList), VERTICAL_LINE_POS ) << ": "
      << res.getVerticalLinePos() << endl;

   int nbtag = sizeof(tagList)/sizeof(StringToken);
   const char* tagStr = searchTokenName( STPARAM(kwdList), TAG );
   for ( ii = 0; ii < nbtag; ++ii ) {
      XxTag bo = XxTag(tagList[ii]._token);
      const QString& b1 = res.getTag( bo );
      os << tagStr << "." << tagList[ii]._name << ": \""
         << b1.latin1() << "\"" << endl;
   }

   os << searchTokenName( STPARAM(kwdList), CLIPBOARD_FORMAT ) << ": \""
      << res.getClipboardFormat() << "\"" << endl;

   const char* hdtstr[3] = { "None", "Single", "Multiple" };
   os << searchTokenName( STPARAM(kwdList), HORDIFF_TYPE ) << ": "
      << hdtstr[ int(res.getHordiffType()) ] << endl;
   
   os << searchTokenName( STPARAM(kwdList), HORDIFF_MAX ) << ": "
      << res.getHordiffMax() << endl;

   os << searchTokenName( STPARAM(kwdList), HORDIFF_CONTEXT ) << ": "
      << res.getHordiffContext() << endl;
   
   os << searchTokenName( STPARAM(kwdList), SHOW_PANE_MERGED_VIEW_PERCENT )
      << ": " << res.getShowPaneMergedViewPercent() << endl;

   // Ignore file not saved (cannot be read).
}   

//------------------------------------------------------------------------------
//
QString XxResParser::getResourceRef()
{
   // Important note: we cannot require a display connection here, so we cannot
   // print the correct font names and named colors.  This is fine, however,
   // since this output only goes towards printing the HTML help, and we don't
   // mind if the default values for fonts and colors are not included in the
   // documentation.  In fact, since they might vary between X servers, we'd
   // rather have a constant documentation output than a varying one.

   QString resref;
   QTextOStream os( &resref );

   // Output all resource names and documentation.
   const XxResources res( false );

   int ii;

   {
      drbegin( os );
      const StringToken* tok = searchToken( STPARAM(kwdList), PREFGEOMETRY );
      os << tok->_name << ": "
         << formatGeometry( res.getPreferredGeometry() ) << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      int nbaccel = sizeof(accelList)/sizeof(StringToken);
      const StringToken* tok = searchToken( STPARAM(kwdList), ACCEL );
      os << tok->_name << "." << "[NAME]." << ": \"[ACCELERATOR]\"" << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;

      drbegin( os );
      for ( ii = 0; ii < nbaccel; ++ii ) {
         XxAccel accel = XxAccel( accelList[ii]._token );
         int aval = res.getAccelerator( accel );
         QString astr("");
            if ( qApp != 0 ) {
               if ( aval != 0 ) {
                  astr = QAccel::keyToString( aval );
               }
            }
            else {
               astr = "&lt;key&gt;";
            }
         os << tok->_name << "." << accelList[ii]._name << ": \""
            << XxHelp::xmlize( astr ) << "\"" << endl;
      }
      drend( os );
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok = searchToken( STPARAM(kwdList), FONT_APP );
      const QFont& fontApp = res.getFontApp();
      os << tok->_name << ": \"";
      if ( qApp != 0 ) {
         os << XxHelp::xmlize( fontApp.rawName() );
      }
      else {
         os << "&lt;xfld-font-spec&gt;";
      }
      os << "\"" << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok = searchToken( STPARAM(kwdList), FONT_TEXT );
      const QFont& fontText = res.getFontText();
      os << tok->_name << ": \"";
      if ( qApp != 0 ) {
         os << XxHelp::xmlize( fontText.rawName() );
      }
      else {
         os << "&lt;xfld-font-spec&gt;";
      }
      os << "\"" << endl;

      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      int nbcolors = sizeof(colorList)/sizeof(StringToken);
      const StringToken* tok = searchToken( STPARAM(kwdList), COLOR );
      os << tok->_name << "." << "[NAME].[Fore|Back]"
         << ": \"[COLOR]\"" << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;

      for ( ii = 0; ii < nbcolors; ++ii ) {
         const StringToken* tokc = &( colorList[ii] );
         XxColor color = XxColor( tokc->_token );

         drbegin( os );

         os << tok->_name << "." << tokc->_name << ".Fore" << ": \"";
         if ( qApp != 0 ) {
            os << res.getColor( color, true ).name();
         }
         else { 
            os << "&lt;color&gt;";
         }
         os << "\"" << endl;

         os << tok->_name << "." << tokc->_name << ".Back" << ": \"";
         if ( qApp != 0 ) {
            os << res.getColor( color, false ).name();
         }
         else { 
            os << "&lt;color&gt;";
         }
         os << "\"" << endl;

         drend( os );
         ddbegin( os );
         os << tokc->_desc << endl;
         ddend( os );
      }
      ddend( os );
   }

   {
      int nbbool = sizeof(boolkwdList)/sizeof(StringToken);
      for ( ii = 0; ii < nbbool; ++ii ) {
         XxBoolOpt bo = boolMap[ boolkwdList[ii]._token - BOOLKWD_BASE ];
         bool b1 = res.getBoolOpt( bo );
         const StringToken* tok = &( boolkwdList[ii] );

         drbegin( os );
         os << tok->_name << ": " << ( b1 ? "True" : "False" ) << endl;
         drend( os );
         ddbegin( os );
         os << tok->_desc << endl;
         ddend( os );
      }
   }

   {
      drbegin( os );
      int nbshow = sizeof(showList)/sizeof(StringToken);
      const StringToken* tok = searchToken( STPARAM(kwdList), SHOW );
      os << tok->_name << "." << "[NAME]" << ": [True|False]" << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;

      for ( ii = 0; ii < nbshow; ++ii ) {
         const StringToken* tokc = &( showList[ii] );
         XxShowOpt bo = XxShowOpt(tokc->_token);
         bool b1 = res.getShowOpt( bo );

         drbegin( os );
         os << tok->_name << "."
            << tokc->_name << ": " << ( b1 ? "True" : "False" ) << endl;
         drend( os );
         ddbegin( os );
         os << tokc->_desc << endl;
         ddend( os );
      }
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok = searchToken( STPARAM(kwdList), TAB_WIDTH );
      os << tok->_name << ": " << res.getTabWidth() << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }


   {
      drbegin( os );
      int nbcommand = sizeof(commandList)/sizeof(StringToken);
      const StringToken* tok = searchToken( STPARAM(kwdList), COMMAND );
      os << tok->_name << "." << "[NAME]" << ": \"[COMMAND]\"" << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;

      for ( ii = 0; ii < nbcommand; ++ii ) {
         const StringToken* tokc = &( commandList[ii] );
         XxCommand bo = XxCommand(tokc->_token);
         QString b1 = XxHelp::xmlize( res.getCommand( bo ) );

         drbegin( os );
         os << tok->_name << "." << tokc->_name << ": \""
            << b1 << "\"" << endl;
         drend( os );
         ddbegin( os );
         os << tokc->_desc << endl;
         ddend( os );
      }
      ddend( os );
   }

   {
      drbegin( os );
      int nbcommandSwitch = sizeof(commandSwitchList)/sizeof(StringToken);
      const StringToken* tok = searchToken( STPARAM(kwdList), COMMANDSW );
      for ( ii = 0; ii < nbcommandSwitch; ++ii ) {
         XxCommandSwitch bo = XxCommandSwitch(commandSwitchList[ii]._token);
         QString b1 = XxHelp::xmlize( res.getCommandSwitch( bo ) );
         os << tok->_name << "." << commandSwitchList[ii]._name << ": \""
            << b1 << "\"" << endl;
      }
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      int nbcommandSwitch = sizeof(commandSwitchList)/sizeof(StringToken);
      const StringToken* tok = searchToken( STPARAM(kwdList), INITSW );
      for ( ii = 0; ii < nbcommandSwitch; ++ii ) {
         XxCommandSwitch bo = XxCommandSwitch(commandSwitchList[ii]._token);
         int b1 = res.getInitSwitch( bo );
         XX_CHECK( -1 <= b1 && b1 <= 1 );
         char* bmap[3] = { "Nop", "False", "True" };
         os << tok->_name << "." << commandSwitchList[ii]._name << ": "
            << bmap[b1+1] << "" << endl;
      }
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok =
         searchToken( STPARAM(kwdList), OVERVIEW_FILE_WIDTH );
      os << tok->_name << ": " << res.getOverviewFileWidth() << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok =
         searchToken( STPARAM(kwdList), OVERVIEW_SEP_WIDTH );
      os << tok->_name << ": " << res.getOverviewSepWidth() << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok =
         searchToken( STPARAM(kwdList), VERTICAL_LINE_POS );
      os << tok->_name << ": " << res.getVerticalLinePos() << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      int nbtag = sizeof(tagList)/sizeof(StringToken);
      const StringToken* tok = searchToken( STPARAM(kwdList), TAG );
      os << tok->_name << "." << "[NAME]" << ": [True|False]" << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;

      for ( ii = 0; ii < nbtag; ++ii ) {
         const StringToken* tokc = &( tagList[ii] );
         XxTag bo = XxTag(tokc->_token);
         QString b1 = XxHelp::xmlize( res.getTag( bo ) );

         drbegin( os );
         os << tok->_name << "."
            << tokc->_name << ": \"" << b1 << "\"" << endl;
         drend( os );
         ddbegin( os );
         os << tokc->_desc << endl;
         ddend( os );
      }
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok =
         searchToken( STPARAM(kwdList), CLIPBOARD_FORMAT );
      QString cf = XxHelp::xmlize( res.getClipboardFormat() );
      os << tok->_name << ": \"" << cf << "\"" << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok = searchToken( STPARAM(kwdList), HORDIFF_TYPE );
      const char* hdtstr[3] = { "None", "Single", "Multiple" };
      os << tok->_name << ": " << hdtstr[ int(res.getHordiffType()) ] << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok = searchToken( STPARAM(kwdList), HORDIFF_MAX );
      os << tok->_name << ": " << res.getHordiffMax() << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok = searchToken( STPARAM(kwdList),
                                            HORDIFF_CONTEXT );
      os << tok->_name << ": " << res.getHordiffContext() << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   {
      drbegin( os );
      const StringToken* tok =
         searchToken( STPARAM(kwdList), SHOW_PANE_MERGED_VIEW_PERCENT );
      os << tok->_name << ": " << res.getShowPaneMergedViewPercent() << endl;
      drend( os );
      ddbegin( os );
      os << tok->_desc << endl;
      ddend( os );
   }

   // Ignore file not saved (cannot be read).

   os << flush;
   return resref;
}   

//------------------------------------------------------------------------------
//
QString XxResParser::getKwdName( int kwd )
{
   const char* stoken = searchTokenName( STPARAM(kwdList), int(kwd) );
   XX_CHECK( stoken );
   return QString( stoken );
}

//------------------------------------------------------------------------------
//
QString XxResParser::getBoolOptName( XxBoolOpt bo )
{
   const char* stoken =
      searchTokenName( STPARAM(boolkwdList), int(bo) + BOOLKWD_BASE );
   return QString( stoken );
}

XX_NAMESPACE_END

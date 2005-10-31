/******************************************************************************\
 * $Id: resources.cpp 56 2000-12-25 20:15:47Z  $
 * $Date: 2000-12-25 15:15:47 -0500 (Mon, 25 Dec 2000) $
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

#include <app.h> // just for getting the size of the main window
#include <resources.h>
#include <optionsDialog.h>

#include <qcolor.h>
#include <qaccel.h>
#include <qobject.h>
#include <qnamespace.h>
#include <qapplication.h> // to get desktop

#include <iostream>
#include <string.h>
#include <stdio.h>


/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {
XX_NAMESPACE_USING

/*----- variables -----*/

struct KeyPair {
   std::string _name;
   int	       _code;
};

// Important note: those key definitions which are substring of other have to be
// declared in the following order: the enclosing key name must appear before
// the enclosed one.  That is simply due to our stupid search algorithm.
//
// For some reason the optimizer is wasting heaps of time on this.
KeyPair keycodes[] = {
   { "escape", Qt::Key_Escape },
   { "esc", Qt::Key_Escape },
   { "tab", Qt::Key_Tab },
   { "backtab", Qt::Key_BackTab },
   { "backspace", Qt::Key_BackSpace },
   { "return", Qt::Key_Return },
   { "enter", Qt::Key_Enter },
   { "insert", Qt::Key_Insert },
   { "delete", Qt::Key_Delete },
   { "pause", Qt::Key_Pause },
   { "print", Qt::Key_Print },
   { "sysreq", Qt::Key_SysReq },
   { "home", Qt::Key_Home },
   { "end", Qt::Key_End },
   { "left", Qt::Key_Left },
   { "up", Qt::Key_Up },
   { "right", Qt::Key_Right },
   { "down", Qt::Key_Down },
   { "prior", Qt::Key_Prior },
   { "pageup", Qt::Key_PageUp },
   { "next", Qt::Key_Next },
   { "pagedown", Qt::Key_PageDown },
   { "f10", Qt::Key_F10 },
   { "f11", Qt::Key_F11 },
   { "f12", Qt::Key_F12 },
   { "f13", Qt::Key_F13 },
   { "f14", Qt::Key_F14 },
   { "f15", Qt::Key_F15 },
   { "f16", Qt::Key_F16 },
   { "f17", Qt::Key_F17 },
   { "f18", Qt::Key_F18 },
   { "f19", Qt::Key_F19 },
   { "f20", Qt::Key_F20 },
   { "f21", Qt::Key_F21 },
   { "f22", Qt::Key_F22 },
   { "f23", Qt::Key_F23 },
   { "f24", Qt::Key_F24 },
   { "f25", Qt::Key_F25 },
   { "f26", Qt::Key_F26 },
   { "f27", Qt::Key_F27 },
   { "f28", Qt::Key_F28 },
   { "f29", Qt::Key_F29 },
   { "f30", Qt::Key_F30 },
   { "f31", Qt::Key_F31 },
   { "f32", Qt::Key_F32 },
   { "f33", Qt::Key_F33 },
   { "f34", Qt::Key_F34 },
   { "f35", Qt::Key_F35 },
   { "f1", Qt::Key_F1 },
   { "f2", Qt::Key_F2 },
   { "f3", Qt::Key_F3 },
   { "f4", Qt::Key_F4 },
   { "f5", Qt::Key_F5 },
   { "f6", Qt::Key_F6 },
   { "f7", Qt::Key_F7 },
   { "f8", Qt::Key_F8 },
   { "f9", Qt::Key_F9 },
   { "menu", Qt::Key_Menu },
   { "space", Qt::Key_Space },
   { "any", Qt::Key_Any },
   { "exclam", Qt::Key_Exclam },
   { "quotedbl", Qt::Key_QuoteDbl },
   { "numbersign", Qt::Key_NumberSign },
   { "dollar", Qt::Key_Dollar },
   { "percent", Qt::Key_Percent },
   { "ampersand", Qt::Key_Ampersand },
   { "apostrophe", Qt::Key_Apostrophe },
   { "parenleft", Qt::Key_ParenLeft },
   { "(", Qt::Key_ParenLeft },
   { "parenright", Qt::Key_ParenRight },
   { ")", Qt::Key_ParenRight },
   { "asterisk", Qt::Key_Asterisk },
   { "*", Qt::Key_Asterisk },
   { "plus", Qt::Key_Plus },
   { "comma", Qt::Key_Comma },
   { ",", Qt::Key_Comma },
   { "minus", Qt::Key_Minus },
   { "period", Qt::Key_Period },
   { ".", Qt::Key_Period },
   { "slash", Qt::Key_Slash },
   { "/", Qt::Key_Slash },
   { "0", Qt::Key_0 },
   { "1", Qt::Key_1 },
   { "2", Qt::Key_2 },
   { "3", Qt::Key_3 },
   { "4", Qt::Key_4 },
   { "5", Qt::Key_5 },
   { "6", Qt::Key_6 },
   { "7", Qt::Key_7 },
   { "8", Qt::Key_8 },
   { "9", Qt::Key_9 },
   { "colon", Qt::Key_Colon },
   { ":", Qt::Key_Colon },
   { "semicolon", Qt::Key_Semicolon },
   { ";", Qt::Key_Semicolon },
   { "less", Qt::Key_Less },
   { "<", Qt::Key_Less },
   { "equal", Qt::Key_Equal },
   { "=", Qt::Key_Equal },
   { "greater", Qt::Key_Greater },
   { ">", Qt::Key_Greater },
   { "question", Qt::Key_Question },
   { "?", Qt::Key_Question },
   { "at", Qt::Key_At },
   { "@", Qt::Key_At },
   { "bracketleft", Qt::Key_BracketLeft },
   { "[", Qt::Key_BracketLeft },
   { "backslash", Qt::Key_Backslash },
   { "\\", Qt::Key_Backslash },
   { "bracketright", Qt::Key_BracketRight },
   { "]", Qt::Key_BracketRight },
   { "asciicircum", Qt::Key_AsciiCircum },
   { "underscore", Qt::Key_Underscore },
   { "_", Qt::Key_Underscore },
   { "quoteleft", Qt::Key_QuoteLeft },
   { "`", Qt::Key_QuoteLeft },
   { "braceleft", Qt::Key_BraceLeft },
   { "{", Qt::Key_BraceLeft },
   { "bar", Qt::Key_Bar },
   { "|", Qt::Key_Bar },
   { "braceright", Qt::Key_BraceRight },
   { "]", Qt::Key_BraceRight },
   { "asciitilde", Qt::Key_AsciiTilde },
   { "~", Qt::Key_AsciiTilde },
   { "a", Qt::Key_A },
   { "b", Qt::Key_B },
   { "c", Qt::Key_C },
   { "d", Qt::Key_D },
   { "e", Qt::Key_E },
   { "f", Qt::Key_F },
   { "g", Qt::Key_G },
   { "h", Qt::Key_H },
   { "i", Qt::Key_I },
   { "j", Qt::Key_J },
   { "k", Qt::Key_K },
   { "l", Qt::Key_L },
   { "m", Qt::Key_M },
   { "n", Qt::Key_N },
   { "o", Qt::Key_O },
   { "p", Qt::Key_P },
   { "q", Qt::Key_Q },
   { "r", Qt::Key_R },
   { "s", Qt::Key_S },
   { "t", Qt::Key_T },
   { "u", Qt::Key_U },
   { "v", Qt::Key_V },
   { "w", Qt::Key_W },
   { "x", Qt::Key_X },
   { "y", Qt::Key_Y },
   { "z", Qt::Key_Z },
};

struct nameDoc {
   XxResources::Resource _res;
   const char*           _name;
   const char*           _doc;
};

const nameDoc mapStrings[ 
   XxResources::RESOURCE_LAST - XxResources::RESOURCE_FIRST
] = {
   { XxResources::RESOURCE_FIRST, "", "" },

   { XxResources::PREFERRED_GEOMETRY, 
     "geometry",
     "Preferred geometry on startup.  Same format as Xt."
   },

   { XxResources::ACCEL_EXIT,
     "accel.exit",
     "Accelerator for ``exit'' command."
   },
   { XxResources::ACCEL_OPEN_LEFT,
     "accel.openLeft",
     "Accelerator for ``open left'' command."
   }, 
   { XxResources::ACCEL_OPEN_MIDDLE,
    "accel.openMiddle",
     "Accelerator for ``open middle'' command."
   },
   { XxResources::ACCEL_OPEN_RIGHT,
     "accel.openRight",
     "Accelerator for ``open right'' command."
   },
   { XxResources::ACCEL_SAVE_AS_LEFT,
     "accel.saveAsLeft",
     "Accelerator for ``save as left'' command."
   },
   { XxResources::ACCEL_SAVE_AS_MIDDLE,
     "accel.saveAsMiddle",
     "Accelerator for ``save as middle'' command."
   },
   { XxResources::ACCEL_SAVE_AS_RIGHT,
     "accel.saveAsRight",
     "Accelerator for ``save as right'' command."
   }, 
   { XxResources::ACCEL_SAVE_AS,
     "accel.saveAs",
     "Accelerator for ``save as'' command."
   },
   { XxResources::ACCEL_SAVE_SELECTED_ONLY,
     "accel.saveSelectedOnly",
     "Accelerator for ``save selected only'' command."
   },
   { XxResources::ACCEL_EDIT_LEFT,
     "accel.editLeft",
     "Accelerator for ``edit left'' command."
   }, 
   { XxResources::ACCEL_EDIT_MIDDLE,
     "accel.editMiddle",
     "Accelerator for ``edit middle'' command."
   },
   { XxResources::ACCEL_EDIT_RIGHT,
     "accel.editRight",
     "Accelerator for ``edit right'' command."
   },
   { XxResources::ACCEL_SEARCH,
     "accel.search",
     "Accelerator for ``search'' command."
   },
   { XxResources::ACCEL_SEARCH_FORWARD,
     "accel.searchForward",
     "Accelerator for ``search forward'' command."
   },
   { XxResources::ACCEL_SEARCH_BACKWARD,
     "accel.searchBackward",
     "Accelerator for ``search backward'' command."
   },
   { XxResources::ACCEL_SCROLL_DOWN,
     "accel.scrollDown",
     "Accelerator for ``scroll down'' command."
   },
   { XxResources::ACCEL_SCROLL_UP,
     "accel.scrollUp",
     "Accelerator for ``scroll up'' command."
   },
   { XxResources::ACCEL_CURSOR_DOWN,
     "accel.cursorDown",
     "Accelerator for ``cursor down'' command."
   },
   { XxResources::ACCEL_CURSOR_UP,
     "accel.cursorUp",
     "Accelerator for ``cursor up'' command."
   },
   { XxResources::ACCEL_CURSOR_TOP,
     "accel.cursorTop",
     "Accelerator for ``cursor at top'' command."
   },
   { XxResources::ACCEL_CURSOR_BOTTOM,
     "accel.cursorBottom",
     "Accelerator for ``cursor at bottom'' command."
   },
   { XxResources::ACCEL_REDO_DIFF,
     "accel.redoDiff",
     "Accelerator for ``redo diff'' command."
   },
   { XxResources::ACCEL_EDIT_DIFF_OPTIONS,
     "accel.editDiffOptions",
     "Accelerator for ``edit diff options'' command."
   }, 
   { XxResources::ACCEL_EDIT_DISPLAY_OPTIONS,
     "accel.editDisplayOptions",
     "Accelerator for ``edit display options'' command."
   },
   { XxResources::ACCEL_DIFF_FILES_AT_CURSOR,
     "accel.diffFilesAtCursor",
     "Accelerator for ``diff files at cursor'' command."
   }, 
   { XxResources::ACCEL_NEXT_DIFFERENCE,
     "accel.nextDifference",
     "Accelerator for ``next difference'' command."
   }, 
   { XxResources::ACCEL_PREVIOUS_DIFFERENCE,
     "accel.previousDifference",
     "Accelerator for ``previous difference'' command."
   }, 
   { XxResources::ACCEL_NEXT_UNSELECTED,
     "accel.nextUnselected",
     "Accelerator for ``next unselected'' command."
   }, 
   { XxResources::ACCEL_PREVIOUS_UNSELECTED,
     "accel.previousUnselected",
     "Accelerator for ``previous unselected'' command."
   }, 
   { XxResources::ACCEL_SELECT_GLOBAL_LEFT,
     "accel.selectGlobalLeft",
     "Accelerator for ``select global left'' command."
   }, 
   { XxResources::ACCEL_SELECT_GLOBAL_MIDDLE,
     "accel.selectGlobalMiddle",
     "Accelerator for ``select global middle'' command."
   }, 
   { XxResources::ACCEL_SELECT_GLOBAL_RIGHT,
     "accel.selectGlobalRight",
     "Accelerator for ``select global right'' command."
   }, 
   { XxResources::ACCEL_SELECT_GLOBAL_NEITHER,
     "accel.selectGlobalNeither",
     "Accelerator for ``select global neither'' command."
   },
   { XxResources::ACCEL_SELECT_GLOBAL_UNSELECT,
     "accel.selectGlobalUnselect",
     "Accelerator for ``unselect global'' command."
   },
   { XxResources::ACCEL_SELECT_GLOBAL_UNSELECTED_LEFT,
     "accel.selectGlobalUnselectedLeft",
     "Accelerator for ``select global unselected left'' command."
   }, 
   { XxResources::ACCEL_SELECT_GLOBAL_UNSELECTED_MIDDLE,
     "accel.selectGlobalUnselectedMiddle",
     "Accelerator for ``select global unselected middle'' command."
   }, 
   { XxResources::ACCEL_SELECT_GLOBAL_UNSELECTED_RIGHT,
     "accel.selectGlobalUnselectedRight",
     "Accelerator for ``select global unselected right'' command."
   }, 
   { XxResources::ACCEL_SELECT_GLOBAL_UNSELECTED_NEITHER,
     "accel.selectGlobalUnselectedNeither",
     "Accelerator for ``select global unselected neither'' command."
   },
   { XxResources::ACCEL_SELECT_GLOBAL_MERGE,
     "accel.selectGlobalMerge",
     "Accelerator for ``merge'' command."
   },
   { XxResources::ACCEL_SELECT_REGION_LEFT,
     "accel.selectRegionLeft",
     "Accelerator for ``select region left'' command."
   }, 
   { XxResources::ACCEL_SELECT_REGION_MIDDLE,
     "accel.selectRegionMiddle",
     "Accelerator for ``select region middle'' command."
   }, 
   { XxResources::ACCEL_SELECT_REGION_RIGHT,
     "accel.selectRegionRight",
     "Accelerator for ``select region right'' command."
   },   
   { XxResources::ACCEL_SELECT_REGION_NEITHER,
     "accel.selectRegionNeither",
     "Accelerator for ``select region neither'' command."
   },
   { XxResources::ACCEL_SELECT_REGION_UNSELECT,
     "accel.selectRegionUnselect",
     "Accelerator for ``unselect region'' command."
   },
   { XxResources::ACCEL_SELECT_REGION_LEFT_AND_NEXT,
     "accel.selectRegionLeftAndNext",
     "Accelerator for ``select region left and move to next unselected or "
     "save'' command."
   }, 
   { XxResources::ACCEL_SELECT_REGION_MIDDLE_AND_NEXT,
     "accel.selectRegionMiddleAndNext",
     "Accelerator for ``select region middle and move to next unselected or "
     "save'' command."
   }, 
   { XxResources::ACCEL_SELECT_REGION_RIGHT_AND_NEXT,
     "accel.selectRegionRightAndNext",
     "Accelerator for ``select region right and move to next unselected or "
     "save'' command."
   }, 
   { XxResources::ACCEL_SELECT_REGION_NEITHER_AND_NEXT,
     "accel.selectRegionNeitherAndNext",
     "Accelerator for ``select region neither and move to next unselected or "
     "save'' command."
   },
   { XxResources::ACCEL_SELECT_REGION_SPLIT_SWAP_JOIN,
     "accel.selectRegionSplitSwapMerge",
     "Accelerator for ``split, swap or join'' command."
   },
   { XxResources::ACCEL_SELECT_LINE_LEFT,
     "accel.selectLineLeft",
     "Accelerator for ``select line left'' command."
   }, 
   { XxResources::ACCEL_SELECT_LINE_MIDDLE,
     "accel.selectLineMiddle",
     "Accelerator for ``select line middle'' command."
   }, 
   { XxResources::ACCEL_SELECT_LINE_RIGHT,
     "accel.selectLineRight",
     "Accelerator for ``select line right'' command."
   }, 
   { XxResources::ACCEL_SELECT_LINE_NEITHER,
     "accel.selectLineNeither",
     "Accelerator for ``select line neither'' command."
   },
   { XxResources::ACCEL_SELECT_LINE_UNSELECT,
     "accel.selectLineUnselect",
     "Accelerator for ``unselect line'' command."
   },
   { XxResources::ACCEL_TABS_AT_3,
     "accel.tabsAt3",
     "Accelerator for ``set tabs at 3'' command."
   },
   { XxResources::ACCEL_TABS_AT_4,
     "accel.tabsAt4",
     "Accelerator for ``set tabs at 4'' command."
   },
   { XxResources::ACCEL_TABS_AT_8,
     "accel.tabsAt8",
     "Accelerator for ``set tabs at 8'' command."
   },
   { XxResources::ACCEL_IGNORE_TRAILING,
     "accel.ignoreTrailing",
     "Accelerator for ``toggle ignore trailing blanks'' command."
   },
   { XxResources::ACCEL_IGNORE_WHITESPACE,
     "accel.ignoreWhitespace",
     "Accelerator for ``toggle ignore whitespace'' command."
   },
   { XxResources::ACCEL_IGNORE_CASE,
     "accel.ignoreCase",
     "Accelerator for ``toggle ignore case'' command."
   },
   { XxResources::ACCEL_IGNORE_BLANK_LINES,
     "accel.ignoreBlankLines",
     "Accelerator for ``toggle ignore blank lines'' command."
   },
   { XxResources::ACCEL_HIDE_CR,
     "accel.hideCarriageReturns",
     "Accelerator for ``toggle hide carriage returns'' command."
   },
   { XxResources::ACCEL_DIRDIFF_IGNORE_FILE_CHANGES,
     "accel.dirDiffIgnoreFileChanges",
     "Accelerator for ``toggle ignore file changes'' command."
   },
   { XxResources::ACCEL_DIRDIFF_RECURSIVE,
     "accel.dirDiffRecursive",
     "Accelerator for ``toggle recursive directory diffs'' command."
   },
   { XxResources::ACCEL_QUALITY_NORMAL,
     "accel.qualityNormal",
     "Accelerator for ``set normal quality'' command."
   },
   { XxResources::ACCEL_QUALITY_FASTEST,
     "accel.qualityFastest",
     "Accelerator for ``set fastest quality'' command."
   },
   { XxResources::ACCEL_QUALITY_HIGHEST,
     "accel.qualityHighest",
     "Accelerator for ``set highest quality'' command."
   },
   { XxResources::ACCEL_MERGED_VIEW,
     "accel.mergedView",
     "Accelerator for ``toggle merged view'' command."
   },
   { XxResources::ACCEL_TOGGLE_TOOLBAR,
     "accel.toggleToolbar",
     "Accelerator for ``toggle toolbar'' command."
   },
   { XxResources::ACCEL_TOGGLE_LINE_NUMBERS,
     "accel.toggleLineNumbers",
     "Accelerator for ``toggle line numbers'' command."
   },
   { XxResources::ACCEL_TOGGLE_MARKERS,
     "accel.toggleMarkers",
     "Accelerator for ``toggle markers'' command."
   },
   { XxResources::ACCEL_TOGGLE_OVERVIEW,
     "accel.toggleOverview",
     "Accelerator for ``toggle overview'' command."
   },
   { XxResources::ACCEL_TOGGLE_SHOW_FILENAMES,
     "accel.toggleShowFilenames",
     "Accelerator for ``toggle show filenames'' command."
   },
   { XxResources::ACCEL_TOGGLE_HORIZONTAL_DIFFS,
     "accel.toggleHorizontalDiffs",
     "Accelerator for ``toggle horizontal diffs'' command."
   },
   { XxResources::ACCEL_TOGGLE_IGNORE_HORIZONTAL_WS,
     "accel.toggleIgnoreHorizontalWhitespace",
     "Accelerator for ``toggle ignore horizontal whitespace'' command."
   },
   { XxResources::ACCEL_TOGGLE_CUT_AND_PASTE_ANNOTATIONS,
     "accel.toggleCutAndPasteAnnotations",
     "Accelerator for ``toggle cut-and-paste annotations'' command."
   },
   { XxResources::ACCEL_HELP_MAN_PAGE,
     "accel.helpManPage",
     "Accelerator for ``show man page'' command."
   },
   { XxResources::ACCEL_HELP_COLOR_LEGEND,
     "accel.helpColorLegend",
     "Accelerator for ``show color legend'' command."
   },
   { XxResources::ACCEL_HELP_ON_CONTEXT,
     "accel.helpOnContext",
     "Accelerator for ``help on context'' command."
   },
   { XxResources::ACCEL_HELP_GEN_INIT_FILE,
     "accel.helpGenInitFile",
     "Accelerator for ``generate init file'' command."
   },
   { XxResources::ACCEL_HELP_ABOUT,
     "accel.helpAbout",
     "Accelerator for ``show about box'' command."
   },
   { XxResources::ACCEL_MERGED_CLOSE,
     "accel.mergedClose",
     "Accelerator for ``close merged view'' command."
   },
   { XxResources::ACCEL_LAST, "", "" },

   //---------------------------------------------------------------------------
   { XxResources::COLOR_BACK_SAME,
     "color.same.back",
     "Background color for A in an AA (2-way) or AAA (3-way) hunk."
   },
   { XxResources::COLOR_FORE_SAME,
     "color.same.fore",
     "Foreground color for A in AAA block."
   },
   { XxResources::COLOR_BACK_DIFF_ONE,
     "color.diffOne.back",
     "Background color for B in BAA, ABA, AAB blocks."
   },
   { XxResources::COLOR_FORE_DIFF_ONE,
     "color.diffOne.fore",
     "Foreground color for B in BAA, ABA, AAB blocks."
   },
   { XxResources::COLOR_BACK_DIFF_ONE_SUP,
     "color.diffOneSup.back",
     "Background color for B in BAA, ABA, AAB blocks (shadowed)."
   },
   { XxResources::COLOR_FORE_DIFF_ONE_SUP,
     "color.diffOneSup.fore",
     "Foreground color for B in BAA, ABA, AAB blocks (shadowed)."
   },
   { XxResources::COLOR_BACK_DIFF_ONE_ONLY,
     "color.diffOneOnly.back",
     "Background color for B in BAA, ABA, AAB blocks (when other is blank)."
   },
   { XxResources::COLOR_FORE_DIFF_ONE_ONLY,
     "color.diffOneOnly.fore",
     "Foreground color for B in BAA, ABA, AAB blocks (when other is blank)."
   },
   { XxResources::COLOR_BACK_DIFF_ONE_NONLY,
     "color.diffOneNonly.back",
     "Background color for B in BAA, ABA, AAB blocks (when blank)."
   },
   { XxResources::COLOR_FORE_DIFF_ONE_NONLY,
     "color.diffOneNonly.fore",
     "Foreground color for B in BAA, ABA, AAB blocks (when blank)."
   },
   { XxResources::COLOR_BACK_DIFF_TWO,
     "color.diffTwo.back",
     "Background color for A in BAA, ABA, AAB blocks."
   },
   { XxResources::COLOR_FORE_DIFF_TWO,
     "color.diffTwo.fore",
     "Foreground color for A in BAA, ABA, AAB blocks."
   },
   { XxResources::COLOR_BACK_DIFF_TWO_SUP,
     "color.diffTwoSup.back",
     "Background color for A in BAA, ABA, AAB blocks (shadowed)."
   },
   { XxResources::COLOR_FORE_DIFF_TWO_SUP,
     "color.diffTwoSup.fore",
     "Foreground color for A in BAA, ABA, AAB blocks (shadowed)."
   },
   { XxResources::COLOR_BACK_DIFF_TWO_ONLY,
     "color.diffTwoOnly.back",
     "Background color for A in BAA, ABA, AAB blocks (when other is blank)."
   },
   { XxResources::COLOR_FORE_DIFF_TWO_ONLY,
     "color.diffTwoOnly.fore",
     "Foreground color for A in BAA, ABA, AAB blocks (when other is blank)."
   },
   { XxResources::COLOR_BACK_DIFF_TWO_NONLY,
     "color.diffTwoNonly.back",
     "Background color for A in BAA, ABA, AAB blocks (when blank)."
   },
   { XxResources::COLOR_FORE_DIFF_TWO_NONLY,
     "color.diffTwoNonly.fore",
     "Foreground color for A in BAA, ABA, AAB blocks (when blank)."
   },
   
   { XxResources::COLOR_BACK_DELETE,
     "color.delete.back",
     "Background color for A in -AA, A-A, AA- blocks."
   },
   { XxResources::COLOR_FORE_DELETE,
     "color.delete.fore",
     "Foreground color for A in -AA, A-A, AA- blocks."
   },
   { XxResources::COLOR_BACK_DELETE_BLANK,
     "color.deleteBlank.back",
     "Background color for - in -AA, A-A, AA- blocks."
   },
   { XxResources::COLOR_FORE_DELETE_BLANK,
     "color.deleteBlank.fore",
     "Foreground color for - in -AA, A-A, AA- blocks."
   },
                                      
   { XxResources::COLOR_BACK_INSERT,
     "color.insert.back",
     "Background color for A in A-, -A, A--, -A-, --A blocks."
   },
   { XxResources::COLOR_FORE_INSERT,
     "color.insert.fore",
     "Foreground color for A in A-, -A, A--, -A-, --A blocks."
   },
   { XxResources::COLOR_BACK_INSERT_BLANK,
     "color.insertBlank.back",
     "Background color for - in A-, -A, A--, -A-, --A blocks."
   },
   { XxResources::COLOR_FORE_INSERT_BLANK,
     "color.insertBlank.fore",
     "Foreground color for - in A-, -A, A--, -A-, --A blocks."
   },
                                      
   { XxResources::COLOR_BACK_DIFF_ALL,
     "color.diffAll.back",
     "Background color for A,B or C in AB or ABC blocks."
   },
   { XxResources::COLOR_FORE_DIFF_ALL,
     "color.diffAll.fore",
     "Foreground color for A,B or C in AB or ABC blocks."
   },
   { XxResources::COLOR_BACK_DIFF_ALL_SUP,
     "color.diffAllSup.back",
     "Background color for A,B or C in AB or ABC blocks (shadowed)."
   },
   { XxResources::COLOR_FORE_DIFF_ALL_SUP,
     "color.diffAllSup.fore",
     "Foreground color for A,B or C in AB or ABC blocks (shadowed)."
   },
   { XxResources::COLOR_BACK_DIFF_ALL_ONLY,
     "color.diffAllOnly.back",
     "Background color for A,B or C in AB or ABC blocks (when other is blank)."
   },
   { XxResources::COLOR_FORE_DIFF_ALL_ONLY,
     "color.diffAllOnly.fore",
     "Foreground color for A,B or C in AB or ABC blocks (when other is blank)."
   },
   { XxResources::COLOR_BACK_DIFF_ALL_NONLY,
     "color.diffAllNonly.back",
     "Background color for A,B or C in AB or ABC blocks (when blank)."
   },
   { XxResources::COLOR_FORE_DIFF_ALL_NONLY,
     "color.diffAllNonly.fore",
     "Foreground color for A,B or C in AB or ABC blocks (when blank)."
   },
                                      
   { XxResources::COLOR_BACK_DIFFDEL,
     "color.diffDel.back",
     "Background color for A and B in -AB, A-B, AB- blocks."
   },
   { XxResources::COLOR_FORE_DIFFDEL,
     "color.diffDel.fore",
     "Foreground color for A and B in -AB, A-B, AB- blocks."
   },
   { XxResources::COLOR_BACK_DIFFDEL_SUP,
     "color.diffDelSup.back",
     "Background color for A and B in -AB, A-B, AB- blocks (shadowed)."
   },
   { XxResources::COLOR_FORE_DIFFDEL_SUP,
     "color.diffDelSup.fore",
     "Foreground color for A and B in -AB, A-B, AB- blocks (shadowed)."
   },
   { XxResources::COLOR_BACK_DIFFDEL_ONLY,
     "color.diffDelOnly.back",
     "Background color for A and B in -AB, A-B, AB- blocks "
     "(when other is blank)."
   },
   { XxResources::COLOR_FORE_DIFFDEL_ONLY,
     "color.diffDelOnly.fore",
     "Foreground color for A and B in -AB, A-B, AB- blocks "
     "(when other is blank)."
   },
   { XxResources::COLOR_BACK_DIFFDEL_NONLY,
     "color.diffDelNonly.back",
     "Background color for A and B in -AB, A-B, AB- blocks (when blank)."
   },
   { XxResources::COLOR_FORE_DIFFDEL_NONLY,
     "color.diffDelNonly.fore",
     "Foreground color for A and B in -AB, A-B, AB- blocks (when blank)."
   },
   { XxResources::COLOR_BACK_DIFFDEL_BLANK,
     "color.diffDelBlank.back",
     "Background color for - in -AB, A-B, AB- blocks."
   },
   { XxResources::COLOR_FORE_DIFFDEL_BLANK,
     "color.diffDelBlank.fore",
     "Foreground color for - in -AB, A-B, AB- blocks."
   },
                                      
   { XxResources::COLOR_BACK_SELECTED,
     "color.selected.back",
     "Background color for selected text."
   },
   { XxResources::COLOR_FORE_SELECTED,
     "color.selected.fore",
     "Foreground color for selected text."
   },
   { XxResources::COLOR_BACK_SELECTED_SUP,
     "color.selectedSup.back",
     "Background color for selected text (shadowed)."
   },
   { XxResources::COLOR_FORE_SELECTED_SUP,
     "color.selectedSup.fore",
     "Foreground color for selected text (shadowed)."
   },
                                      
   { XxResources::COLOR_BACK_DELETED,
     "color.deleted.back",
     "Background color for deleted text."
   },
   { XxResources::COLOR_FORE_DELETED,
     "color.deleted.fore",
     "Foreground color for deleted text."
   },
   { XxResources::COLOR_BACK_DELETED_SUP,
     "color.deletedSup.back",
     "Background color for deleted text (shadowed)."
   },
   { XxResources::COLOR_FORE_DELETED_SUP,
     "color.deletedSup.fore",
     "Foreground color for deleted text (shadowed)."
   },

   { XxResources::COLOR_BACK_DIRECTORIES,
     "color.directories.back",
     "Background color for directories in directory diffs."
   },
   { XxResources::COLOR_FORE_DIRECTORIES,
     "color.directories.fore",
     "Foreground color for directories in directory diffs."
   },
                                      
   { XxResources::COLOR_BACK_MERGED_UNDECIDED,
     "color.mergedUndecided.back",
     "Background color for undecided text (merged view)."
   },
   { XxResources::COLOR_FORE_MERGED_UNDECIDED,
     "color.mergedUndecided.fore",
     "Foreground color for undecided text (merged view)."
   },
   { XxResources::COLOR_BACK_MERGED_DECIDED_1,
     "color.mergedDecided1.back",
     "Background color for decided text in file 1 (merged view)."
   },
   { XxResources::COLOR_FORE_MERGED_DECIDED_1,
     "color.mergedDecided1.fore",
     "Foreground color for decided text in file 1 (merged view)."
   },
   { XxResources::COLOR_BACK_MERGED_DECIDED_2,
     "color.mergedDecided2.back",
     "Background color for decided text in file 2 (merged view)."
   },
   { XxResources::COLOR_FORE_MERGED_DECIDED_2,
     "color.mergedDecided2.fore",
     "Foreground color for decided text in file 2 (merged view)."
   },
   { XxResources::COLOR_BACK_MERGED_DECIDED_3,
     "color.mergedDecided3.back",
     "Background color for decided text in file 3 (merged view)."
   },
   { XxResources::COLOR_FORE_MERGED_DECIDED_3,
     "color.mergedDecided3.fore",
     "Foreground color for decided text in file 3 (merged view)."
   },

   { XxResources::COLOR_BACKGROUND,
     "color.background",
     "Background color for remaining backgrounds."
   },
   { XxResources::COLOR_CURSOR,
     "color.cursor",
     "Cursor color."
   },
   { XxResources::COLOR_LAST, "", "" },

   //---------------------------------------------------------------------------
   { XxResources::FONT_APP,
     "font.app",
     "Application font, used for UI elements only."
     "This resource is ignored for versions 1.7 and above."
   },
   { XxResources::FONT_TEXT,
     "font.text",
     "Text font, used for diffed files only."
   },
   { XxResources::FONT_LAST, "", "" },

   //---------------------------------------------------------------------------
   { XxResources::EXIT_ON_SAME,
     "exitOnSame",
     "If this resource is true, if there are no diffs the program simply exits."
   },
   { XxResources::SHOW_TOOLBAR,
     "windows.showToolbar",
     "If this resource is true, show the toolbar."
   },
   { XxResources::SHOW_LINE_NUMBERS,
     "windows.showLineNumbers",
     "If this resource is true, show the line numbers."
   },
   { XxResources::SHOW_MARKERS,
     "windows.showMarkers",
     "If this resource is true, show markers (currently not implemented)."
   },
   { XxResources::SHOW_OVERVIEW,
     "windows.showOverview",
     "If this resource is true, show the overview area on startup."
   },
   { XxResources::SHOW_FILENAMES,
     "windows.showFilenames",
     "If this resource is true, show the filenames on startup."
   },

   { XxResources::HORIZONTAL_DIFFS,
     "horizontalDiffs",
     "If this resource is true, show the horizontal diffs on startup."
   },
   { XxResources::IGNORE_HORIZONTAL_WS,
     "ignoreHorizontalWhitespace",
     "If this resource is true, the horizontal diffs ignore whitespace."
   },
   { XxResources::CUT_AND_PASTE_ANNOTATIONS,
     "cutAndPasteAnnotations",
     "If this resource is true, add annotations when pasting from xxdiff hunks."
   },
   { XxResources::IGNORE_ERRORS,
     "ignoreErrors",
     "If this resource is true, ignore errors from diff subprocess."
   },
   { XxResources::WARN_ABOUT_UNSAVED,
     "warnAboutUnsaved",
     "If this resource is true, warn about unsaved selections when quitting."
   },
   { XxResources::DISABLE_CURSOR_DISPLAY,
     "disableCursorDisplay",
     "Disables the cursor display"
   },
   { XxResources::HIDE_CR,
     "hideCarriageReturns",
     "Hides the ugly ^M characters in DOS files."
   },
   { XxResources::DIRDIFF_IGNORE_FILE_CHANGES,
     "dirDiff.ignoreFileChanges",
     "Ignore the changes between files when displaying directory diffs."
   },
   { XxResources::DIRDIFF_BUILD_FROM_OUTPUT,
     "dirDiff.buildSolelyFromOutput",
     "Determines whether directory diffs buffer contents are built solely"
     "from the output of the directory diff command. If false, buffer"
     "contents are created by reading the directory contents directory."
     "This is used mainly for debugging, and should be set to true if "
     "GNU diff output is what you're using."
     "Note that this option only affects shallow directory diffs. Recursive"
     "directory diffs always build solely from output."
   },
   { XxResources::DIRDIFF_RECURSIVE,
     "dirDiff.recursive",
     "Determines whether directory diffs are recursive or not."
   },
   { XxResources::BOOL_LAST, "", "" },

   //---------------------------------------------------------------------------

   { XxResources::COMMAND_DIFF_FILES_2,
     "command.diffFiles2",
     "Command for diffing two files."
   },
   { XxResources::COMMAND_DIFF_FILES_3,
     "command.diffFiles3",
     "Command for diffing three files."
   },
   { XxResources::COMMAND_DIFF_DIRECTORIES,
     "command.diffDirectories",
     "Command for diffing two directories."
   },
   { XxResources::COMMAND_DIFF_DIRECTORIES_REC,
     "command.diffDirectoriesRec",
     "Command for diffing two directories, recursively."
   },
   { XxResources::COMMAND_MANUAL,
     "command.manual",
     "Set command for dynamic manual (this resource is unused if compile with "
     "inline help."
   },
   { XxResources::COMMAND_EDIT,
     "command.edit",
     "Command for editing a diff buffer."
   },
   { XxResources::COMMAND_LAST, "", "" },

   //---------------------------------------------------------------------------

   { XxResources::CMDOPT_FILES_IGNORE_TRAILING,
     "commandOption.ignoreTrailingBlanks",
     "File diff command option for ignoring trailing blanks."
   },
   { XxResources::CMDOPT_FILES_IGNORE_WHITESPACE,
     "commandOption.ignoreWhitespace",
     "File diff command option for ignoring whitespace."
   },
   { XxResources::CMDOPT_FILES_IGNORE_CASE,
     "commandOption.ignoreCase",
     "File diff command option for ignoring case when diffing."
   },
   { XxResources::CMDOPT_FILES_IGNORE_BLANK_LINES,
     "commandOption.ignoreBlankLines",
     "File diff command option for ignoring blank lines when diffing."
   },
   { XxResources::CMDOPT_FILES_QUALITY_NORMAL,
     "commandOption.qualityNormal",
     "File diff command option for normal quality."
   },
   { XxResources::CMDOPT_FILES_QUALITY_FASTEST,
     "commandOption.qualityFastest",
     "File diff command option for fastest quality."
   },
   { XxResources::CMDOPT_FILES_QUALITY_HIGHEST,
     "commandOption.qualityHighest",
     "File diff command option for highest quality."
   },
   { XxResources::CMDOPT_LAST, "", "" },

   //---------------------------------------------------------------------------

   { XxResources::TAG_CONFLICT_SEPARATOR,
     "tag.conflict.separator",
     "Text stub to prepend to conflict hunks when saving unselected regions."
   },
   { XxResources::TAG_CONFLICT_END,
     "tag.conflict.end",
     "Text stub to append to conflict hunks when saving unselected regions."
   },
   { XxResources::TAG_CONDITIONAL_IFDEF,
     "tag.conditional.ifdef",
     "Text stub for ifdef conditional marker when saving unselected regions."
   },
   { XxResources::TAG_CONDITIONAL_ELSEIF,
     "tag.conditional.elseif",
     "Text stub for elseif conditional marker when saving unselected regions."
     "This will be used for 3-way diffs only."
   },
   { XxResources::TAG_CONDITIONAL_ELSE,
     "tag.conditional.else",
     "Text stub for else conditional marker when saving unselected regions."
   },
   { XxResources::TAG_CONDITIONAL_ENDIF,
     "tag.conditional.endif",
     "Text stub for endif conditional marker when saving unselected regions."
   },
   { XxResources::TAG_LAST, "", "" },

   //---------------------------------------------------------------------------

   { XxResources::TAB_WIDTH,
     "tabWidth",
     "Width (in characters) of tabs."
   },
   { XxResources::OVERVIEW_FILE_WIDTH,
     "overviewFileWidth",
     "Width (in pixels) of a file in the overview area."
   },
   { XxResources::OVERVIEW_SEP_WIDTH,
     "overviewSepWidth",
     "Width (in pixels) of separation between files in the overview area."
   }

};


/*==============================================================================
 * LOCAL CLASS XxDefaultsParser
 *============================================================================*/

// <summary> a parser for default values </summary>

class XxDefaultsParser : public XxResourcesParser {

public:

   /*----- member functions -----*/

   // Constructor.
   XxDefaultsParser();

   // Destructor.
   virtual ~XxDefaultsParser();

   // Query the database for the named resource.  Returns false is not present.
   // Otherwise return true and fills in the value string.
   virtual bool query( 
      XxResources::Resource res,
      const char*           name,
      std::string&          value
   );

private:

   /*----- data members -----*/

   const char* _map[ XxResources::RESOURCE_LAST - XxResources::RESOURCE_FIRST ];

};

//------------------------------------------------------------------------------
//
XxDefaultsParser::XxDefaultsParser()
{
   int nbres = XxResources::RESOURCE_LAST - XxResources::RESOURCE_FIRST;
   for ( int ii = 0; ii < nbres; ++ii ) {
      _map[ ii ] = 0;
   }
   
   _map[ XxResources::PREFERRED_GEOMETRY ] = "1200x600";

   _map[ XxResources::ACCEL_EXIT ] = "Ctrl+Q";

   _map[ XxResources::ACCEL_SEARCH ] = "Ctrl+S";
   _map[ XxResources::ACCEL_SEARCH_FORWARD ] = "Ctrl+F";
   _map[ XxResources::ACCEL_SEARCH_BACKWARD ] = "Ctrl+B"; 
   _map[ XxResources::ACCEL_SCROLL_DOWN ] = "Ctrl+V";
   _map[ XxResources::ACCEL_SCROLL_UP ] = "Alt+V";
   _map[ XxResources::ACCEL_CURSOR_DOWN ] = "Ctrl+N";
   _map[ XxResources::ACCEL_CURSOR_UP ] = "Ctrl+P";
   _map[ XxResources::ACCEL_CURSOR_TOP ] = "Home"; // FIXME doesn't work
   _map[ XxResources::ACCEL_CURSOR_BOTTOM ] = "End"; // FIXME doesn't work
   _map[ XxResources::ACCEL_REDO_DIFF ] = "Ctrl+R";
   _map[ XxResources::ACCEL_NEXT_DIFFERENCE ] = "N";
   _map[ XxResources::ACCEL_PREVIOUS_DIFFERENCE ] = "P";
   _map[ XxResources::ACCEL_NEXT_UNSELECTED ] = "B";
   _map[ XxResources::ACCEL_PREVIOUS_UNSELECTED ] = "O";
   _map[ XxResources::ACCEL_SELECT_GLOBAL_LEFT ] = "Ctrl+Alt+H"; 
   _map[ XxResources::ACCEL_SELECT_GLOBAL_MIDDLE ] = "Ctrl+Alt+J"; 
   _map[ XxResources::ACCEL_SELECT_GLOBAL_RIGHT ] = "Ctrl+Alt+K"; 
   _map[ XxResources::ACCEL_SELECT_GLOBAL_NEITHER ] = "Ctrl+Alt+Y";
   _map[ XxResources::ACCEL_SELECT_GLOBAL_UNSELECT ] = "Ctrl+Alt+U";
   _map[ XxResources::ACCEL_SELECT_GLOBAL_MERGE ] = "Ctrl+Alt+M";
   _map[ XxResources::ACCEL_SELECT_REGION_LEFT ] = "H"; 
   _map[ XxResources::ACCEL_SELECT_REGION_MIDDLE ] = "J"; 
   _map[ XxResources::ACCEL_SELECT_REGION_RIGHT ] = "K";
   _map[ XxResources::ACCEL_SELECT_REGION_NEITHER ] = "Y";
   _map[ XxResources::ACCEL_SELECT_REGION_UNSELECT ] = "U";
   _map[ XxResources::ACCEL_SELECT_REGION_LEFT_AND_NEXT ] = "Ctrl+H";
   _map[ XxResources::ACCEL_SELECT_REGION_MIDDLE_AND_NEXT ] = "Ctrl+J";
   _map[ XxResources::ACCEL_SELECT_REGION_RIGHT_AND_NEXT ] = "Ctrl+K";
   _map[ XxResources::ACCEL_SELECT_REGION_NEITHER_AND_NEXT ] = "Ctrl+Y";
   _map[ XxResources::ACCEL_SELECT_REGION_SPLIT_SWAP_JOIN ] = "S";
   _map[ XxResources::ACCEL_SELECT_LINE_LEFT ] = "Shift+H"; 
   _map[ XxResources::ACCEL_SELECT_LINE_MIDDLE ] = "Shift+J"; 
   _map[ XxResources::ACCEL_SELECT_LINE_RIGHT ] = "Shift+K"; 
   _map[ XxResources::ACCEL_SELECT_LINE_NEITHER ] = "Shift+Y";
   _map[ XxResources::ACCEL_SELECT_LINE_UNSELECT ] = "Shift+U";
   _map[ XxResources::ACCEL_TABS_AT_3 ] = "3";
   _map[ XxResources::ACCEL_TABS_AT_4 ] = "4";
   _map[ XxResources::ACCEL_TABS_AT_8 ] = "8";
   _map[ XxResources::ACCEL_MERGED_VIEW ] = "Alt+Y";
   _map[ XxResources::ACCEL_TOGGLE_LINE_NUMBERS ] = "Alt+L";
   _map[ XxResources::ACCEL_TOGGLE_MARKERS ] = "Alt+M";
   _map[ XxResources::ACCEL_TOGGLE_OVERVIEW ] = "Alt+O";
   _map[ XxResources::ACCEL_TOGGLE_SHOW_FILENAMES ] = "Alt+S";
   _map[ XxResources::ACCEL_TOGGLE_HORIZONTAL_DIFFS ] = "Alt+W";
   _map[ XxResources::ACCEL_TOGGLE_CUT_AND_PASTE_ANNOTATIONS ] = "Alt+A";
   _map[ XxResources::ACCEL_HELP_ON_CONTEXT ] = "Shift+F1";

   _map[ XxResources::ACCEL_MERGED_CLOSE ] = "Alt+W";


   // FIXME there is currently a bug with the Qt raw fonts.  It breaks the
   // QTextView.  Put this back when done.
   //     _map[ XxResources::FONT_APP ] = 
   //        "-*-helvetica-bold-o-narrow-*-17-*-*-*-*-*-iso8859-1-";

   _map[ XxResources::FONT_TEXT ] = 
      "*-clean-medium-r-normal-*-14-*";
   // FIXME need to remove the stupid warning!
   //     _font.setRawName( 
   //        "-*-clean-medium-r-normal-140-14-*-*-*-*-*-iso8859-1-"
   //     );

   //---------------------------------------------------------------------------

   _map[ XxResources::COLOR_BACK_SAME             ] = "grey";
   _map[ XxResources::COLOR_FORE_SAME             ] = "black";

   _map[ XxResources::COLOR_BACK_DIFF_ONE         ] = "palegoldenrod";
   _map[ XxResources::COLOR_FORE_DIFF_ONE         ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_ONE_SUP     ] = "lemonchiffon3";
   _map[ XxResources::COLOR_FORE_DIFF_ONE_SUP     ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_ONE_ONLY    ] = "palegoldenrod";
   _map[ XxResources::COLOR_FORE_DIFF_ONE_ONLY    ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_ONE_NONLY   ] = "lemonchiffon3";
   _map[ XxResources::COLOR_FORE_DIFF_ONE_NONLY   ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_TWO         ] = "lightblue2";
   _map[ XxResources::COLOR_FORE_DIFF_TWO         ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_TWO_SUP     ] = "lightblue3";
   _map[ XxResources::COLOR_FORE_DIFF_TWO_SUP     ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_TWO_ONLY    ] = "lightblue2";
   _map[ XxResources::COLOR_FORE_DIFF_TWO_ONLY    ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_TWO_NONLY   ] = "lightblue3";
   _map[ XxResources::COLOR_FORE_DIFF_TWO_NONLY   ] = "black";

   _map[ XxResources::COLOR_BACK_DELETE           ] = "lightblue2";
   _map[ XxResources::COLOR_FORE_DELETE           ] = "black";
   _map[ XxResources::COLOR_BACK_DELETE_BLANK     ] = "grey64";
   _map[ XxResources::COLOR_FORE_DELETE_BLANK     ] = "black";

   _map[ XxResources::COLOR_BACK_INSERT           ] = "darkseagreen2";
   _map[ XxResources::COLOR_FORE_INSERT           ] = "black";
   _map[ XxResources::COLOR_BACK_INSERT_BLANK     ] = "grey64";
   _map[ XxResources::COLOR_FORE_INSERT_BLANK     ] = "black";

   _map[ XxResources::COLOR_BACK_DIFF_ALL         ] = "palegoldenrod";
   _map[ XxResources::COLOR_FORE_DIFF_ALL         ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_ALL_SUP     ] = "lemonchiffon3";
   _map[ XxResources::COLOR_FORE_DIFF_ALL_SUP     ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_ALL_ONLY    ] = "palegoldenrod";
   _map[ XxResources::COLOR_FORE_DIFF_ALL_ONLY    ] = "black";
   _map[ XxResources::COLOR_BACK_DIFF_ALL_NONLY   ] = "lemonchiffon3";
   _map[ XxResources::COLOR_FORE_DIFF_ALL_NONLY   ] = "black";

   _map[ XxResources::COLOR_BACK_DIFFDEL          ] = "palegoldenrod";
   _map[ XxResources::COLOR_FORE_DIFFDEL          ] = "black";
   _map[ XxResources::COLOR_BACK_DIFFDEL_SUP      ] = "lemonchiffon3";
   _map[ XxResources::COLOR_FORE_DIFFDEL_SUP      ] = "black";
   _map[ XxResources::COLOR_BACK_DIFFDEL_ONLY     ] = "palegoldenrod";
   _map[ XxResources::COLOR_FORE_DIFFDEL_ONLY     ] = "black";
   _map[ XxResources::COLOR_BACK_DIFFDEL_NONLY    ] = "lemonchiffon3";
   _map[ XxResources::COLOR_FORE_DIFFDEL_NONLY    ] = "black";
   _map[ XxResources::COLOR_BACK_DIFFDEL_BLANK    ] = "grey64";
   _map[ XxResources::COLOR_FORE_DIFFDEL_BLANK    ] = "black";

   _map[ XxResources::COLOR_BACK_SELECTED         ] = "plum";
   _map[ XxResources::COLOR_FORE_SELECTED         ] = "black";
   _map[ XxResources::COLOR_BACK_SELECTED_SUP     ] = "thistle";
   _map[ XxResources::COLOR_FORE_SELECTED_SUP     ] = "black";

   _map[ XxResources::COLOR_BACK_DELETED          ] = "lightslategrey";
   _map[ XxResources::COLOR_FORE_DELETED          ] = "black";
   _map[ XxResources::COLOR_BACK_DELETED_SUP      ] = "slategrey";
   _map[ XxResources::COLOR_FORE_DELETED_SUP      ] = "black";

   _map[ XxResources::COLOR_BACK_DIRECTORIES      ] = "mediumturquoise";
   _map[ XxResources::COLOR_FORE_DIRECTORIES      ] = "black";
                                                                      
   _map[ XxResources::COLOR_BACK_MERGED_UNDECIDED ] = "lemonchiffon3";
   _map[ XxResources::COLOR_FORE_MERGED_UNDECIDED ] = "black";
   _map[ XxResources::COLOR_BACK_MERGED_DECIDED_1 ] = "grey70";
   _map[ XxResources::COLOR_FORE_MERGED_DECIDED_1 ] = "black";
   _map[ XxResources::COLOR_BACK_MERGED_DECIDED_2 ] = "grey70";
   _map[ XxResources::COLOR_FORE_MERGED_DECIDED_2 ] = "black";
   _map[ XxResources::COLOR_BACK_MERGED_DECIDED_3 ] = "grey70";
   _map[ XxResources::COLOR_FORE_MERGED_DECIDED_3 ] = "black";

   _map[ XxResources::COLOR_BACKGROUND ] = "#40616a"; // FIXME find the name!
   _map[ XxResources::COLOR_CURSOR ] = "white";

   //---------------------------------------------------------------------------

   _map[ XxResources::EXIT_ON_SAME ] = "false";
   _map[ XxResources::SHOW_TOOLBAR ] = "false";
   _map[ XxResources::SHOW_LINE_NUMBERS ] = "false";
   _map[ XxResources::SHOW_OVERVIEW ] = "true";
   _map[ XxResources::SHOW_FILENAMES ] = "false";
   _map[ XxResources::HORIZONTAL_DIFFS ] = "false";
   _map[ XxResources::IGNORE_HORIZONTAL_WS ] = "true";
   _map[ XxResources::CUT_AND_PASTE_ANNOTATIONS ] = "false";
   _map[ XxResources::IGNORE_ERRORS ] = "false";
   _map[ XxResources::WARN_ABOUT_UNSAVED ] = "false";
   _map[ XxResources::DISABLE_CURSOR_DISPLAY ] = "false";
   _map[ XxResources::HIDE_CR ] = "false";
   _map[ XxResources::DIRDIFF_IGNORE_FILE_CHANGES ] = "false";
   _map[ XxResources::DIRDIFF_BUILD_FROM_OUTPUT ] = "true";
   _map[ XxResources::DIRDIFF_RECURSIVE ] = "false";
   _map[ XxResources::TAB_WIDTH ] = "8";

   _map[ XxResources::COMMAND_DIFF_FILES_2 ] = "diff";
   _map[ XxResources::COMMAND_DIFF_FILES_3 ] = "diff3";
   _map[ XxResources::COMMAND_DIFF_DIRECTORIES ] = "diff -q -s";
   _map[ XxResources::COMMAND_DIFF_DIRECTORIES_REC ] = "diff -q -s -r";
   _map[ XxResources::COMMAND_MANUAL ] = "(man xxdiff | col -b) 2>&1";
   // "cmp -s" barfs on directories.
   const char* editor = getenv( "EDITOR" );
   if ( editor != 0 ) {
      _map[ XxResources::COMMAND_EDIT ] = editor;
   }
   else {
      _map[ XxResources::COMMAND_EDIT ] = "vi";
   }

   _map[ XxResources::CMDOPT_FILES_IGNORE_TRAILING ] = "-b";
   _map[ XxResources::CMDOPT_FILES_IGNORE_WHITESPACE ] = "-w";
   _map[ XxResources::CMDOPT_FILES_IGNORE_CASE ] = "-i";
   _map[ XxResources::CMDOPT_FILES_IGNORE_BLANK_LINES ] = "-B";
   _map[ XxResources::CMDOPT_FILES_QUALITY_NORMAL ] = "";
   _map[ XxResources::CMDOPT_FILES_QUALITY_FASTEST ] = "-d";
   _map[ XxResources::CMDOPT_FILES_QUALITY_HIGHEST ] = "-H";

   _map[ XxResources::TAG_CONFLICT_SEPARATOR ] = ">>>>>>>>>>>>>>>>>>>> File %d";
   _map[ XxResources::TAG_CONFLICT_END ] = "<<<<<<<<<<<<<<<<<<<<";
   _map[ XxResources::TAG_CONDITIONAL_IFDEF ] = "#ifdef %s";
   _map[ XxResources::TAG_CONDITIONAL_ELSEIF ] = "#elif defined( %s )";
   _map[ XxResources::TAG_CONDITIONAL_ELSE ] = "#else /* %s */";
   _map[ XxResources::TAG_CONDITIONAL_ENDIF ] = "#endif /* %s */";

   _map[ XxResources::OVERVIEW_FILE_WIDTH ] = "20";
   _map[ XxResources::OVERVIEW_SEP_WIDTH ] = "14";
}

//------------------------------------------------------------------------------
//
XxDefaultsParser::~XxDefaultsParser()
{}

//------------------------------------------------------------------------------
//
bool XxDefaultsParser::query( 
   XxResources::Resource res,
   const char*           /*name*/,
   std::string&          value
)
{
   const char* cres = _map[ int(res) ];
   if ( cres == 0 ) {
      return false;
   }
   value = cres;
   return true;
}


/*==============================================================================
 * LOCAL FUNCTIONS
 *============================================================================*/

//------------------------------------------------------------------------------
//
bool readGeometry( const std::string& val, QRect& geometry )
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
   if ( sscanf( val.c_str(), "%dx%d+%d+%d", &w, &h, &l, &t ) == 4 ) {
      geometry = QRect( l, t, w, h );
      return true;
   }
   if ( sscanf( val.c_str(), "%dx%d-%d+%d", &w, &h, &l, &t ) == 4 ) {
      geometry = QRect( dsize.width()-l-w, t, w, h );
      return true;
   }
   if ( sscanf( val.c_str(), "%dx%d+%d-%d", &w, &h, &l, &t ) == 4 ) {
      geometry = QRect( l, dsize.height()-t-h, w, h );
      return true;
   }
   if ( sscanf( val.c_str(), "%dx%d-%d-%d", &w, &h, &l, &t ) == 4 ) {
      geometry = QRect( dsize.width()-l-w, dsize.height()-t-h, w, h );
      return true;
   }
   else if ( sscanf( val.c_str(), "%dx%d", &w, &h ) == 2 ) {
      geometry = QRect( -1, -1, w, h );
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
//
void writeGeometry( std::ostream& os, const QRect& geometry )
{
   os << geometry.width() << "x" << geometry.height()
      << "+" << geometry.top() << "+" << geometry.left();
}

//------------------------------------------------------------------------------
//
bool readBoolean( const std::string& val, bool& mybool )
{
   // Reads in a value.  Returns true if successful, false if the resource was
   // not the specified type and left untouched.

   if ( val == "True" || val == "true" ) {
      mybool = true;
      return true;
   }
   else if ( val == "False" || val == "false" ) {
      mybool = false;
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
//
void writeBoolean( std::ostream& os, const bool mybool )
{
   os << ( mybool ? "True" : "False" );
}

//------------------------------------------------------------------------------
//
bool readAccelerator( const std::string& val, int& accel )
{
   // Reads in a value.  Returns true if successful, false if the resource was
   // not the specified type and left untouched.

   using namespace std;

   // I wish I could use QAccel::stringToKey, but it's broken.  The Qt docs say:
   //
   // "Note that this function currently only supports character accelerators
   // (unlike keyToString() which can produce Ctrl+Backspace, etc. from the
   // appropriate key codes)."
   //
   // So we do this by hand.  This should really just be provided by Qt.
   
   if ( val.empty() ) {
      return 0;
   }

   // Remove whitespace.
   string::size_type bpos = val.find_first_not_of( " \t" );
   if ( bpos == string::npos ) {
      bpos = 0;
   }
   string::size_type epos = val.find_last_not_of( " \t" );
   if ( epos == string::npos ) {
      epos = val.length()-1;
   }

   string cval( val, bpos, epos - bpos + 1 );

   // Turn string into lowercase.
   string::size_type len = cval.length();
   char* c_str = const_cast<char*>( cval.c_str() );
   const int offset = 'a' - 'A';
   for ( string::size_type ii = 0; ii < len; ++ii ) {
      if ( c_str[ii] >= 'A' && c_str[ii] <= 'Z' ) {
         c_str[ii] += offset;
      }
   }

   // Read modifier, if present.
   int modifier = 0;
   if ( cval.find( "alt" ) != string::npos || 
        cval.find( "meta" ) != string::npos ) {
      modifier |= Qt::ALT;
   }
   if ( cval.find( "ctrl" ) != string::npos || 
        cval.find( "control" ) != string::npos ) {
      modifier |= Qt::CTRL;
   }
   if ( cval.find( "shift" ) != string::npos ) {
      modifier |= Qt::SHIFT;
   }

   // Read non-modifier.
   string::size_type xpos = cval.find_last_of( "-+" );
   if ( xpos == string::npos ) {
      xpos = 0;
   }

   int key = 0;
   for ( uint ii = 0; ii < sizeof(keycodes); ++ii ) {
      if ( cval.find( keycodes[ii]._name, xpos ) != string::npos ) {
         key = keycodes[ii]._code;
         break;
      }
   }

   accel = modifier | key;
   return true;
}

//------------------------------------------------------------------------------
//
void writeAccelerator( std::ostream& os, int accel )
{
   QString str = QAccel::keyToString( accel );
   os << str;
}

//------------------------------------------------------------------------------
//
bool readColor( const std::string& val, QColor& color )
{
   // Reads in a value.  Returns true if successful, false if the resource was
   // not the specified type and left untouched.

   using namespace std;

   // Remove whitespace before and after.
   string::size_type bpos = val.find_first_not_of( " \t" );
   string::size_type epos = val.find_last_not_of( " \t" );
   string cleanVal( val, bpos, epos - bpos + 1 );
   color.setNamedColor( cleanVal.c_str() );
   return true;
}

//------------------------------------------------------------------------------
//
void writeColor( std::ostream& os, const QColor& color )
{
   os << "#" 
      << std::hex << color.red() 
      << std::hex << color.green()
      << std::hex << color.blue();
}

//------------------------------------------------------------------------------
//
void writeDocAttrib(
   const XxResources* resources,
   std::ostream& os,
   XxResources::Resource res
)
{
   const char* name = XxResources::getResourceName( res );
   const char* doc = XxResources::getResourceDoc( res );
   os << std::endl;
   os << "# " << doc << std::endl;
   os << name << " : ";
}

}

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxResources
 *============================================================================*/

XxResources* XxResources::_instance = 0;

//------------------------------------------------------------------------------
//
XxResources* XxResources::getInstance()
{
   if ( _instance == 0 ) {
      _instance = new XxResources;
   }
   return _instance;
}

//------------------------------------------------------------------------------
//
void XxResources::releaseInstance()
{
   delete _instance;
}

//------------------------------------------------------------------------------
//
XxResources::XxResources()
{
   // Initialize because this is more handy than specifying empty defaults for
   // all accelerators.
   for ( int ii = 0; ii < ACCEL_LAST - ACCEL_FIRST; ++ii ) {
      _accelerators[ ii ] = 0;
   }

   // Use a default font that looks like the one from the default SGI scheme.
   _fontApp.setFamily( "Helvetica" );
   _fontApp.setItalic( true ); 
   _fontApp.setBold( true );

   // Initialize to defaults.
   XxDefaultsParser defaultsParser;
   parse( defaultsParser );
}

//------------------------------------------------------------------------------
//
const char* XxResources::getResourceName( Resource res )
{
   XX_CHECK( int( RESOURCE_FIRST ) <= res && res <= int( RESOURCE_LAST ) );
   return mapStrings[ int( res ) ]._name;
}

//------------------------------------------------------------------------------
//
const char* XxResources::getResourceDoc( Resource res )
{
   XX_CHECK( int( RESOURCE_FIRST ) <= res && res <= int( RESOURCE_LAST ) );
   return mapStrings[ int( res ) ]._doc;
}

//------------------------------------------------------------------------------
//
XxResources::Resource XxResources::getResourceId( const char* resname )
{
   int nbres = int( XxResources::RESOURCE_LAST - XxResources::RESOURCE_FIRST );
   for ( int ii = 0; ii < nbres; ++ii ) {
      if ( ::strcmp( mapStrings[ XxResources::Resource( ii ) ]._name, 
                     resname ) == 0 ) {
         return XxResources::Resource( XxResources::RESOURCE_FIRST + ii );
      }
   }
   return RESOURCE_LAST;
}

//------------------------------------------------------------------------------
//
bool XxResources::checkResourcesDoc() const
{
   std::ostream& os = std::cerr;
   bool err = false;
   int nbres = int( XxResources::RESOURCE_LAST - XxResources::RESOURCE_FIRST );
   for ( int ii = 0; ii < nbres; ++ii ) {
      if ( mapStrings[ XxResources::Resource( ii ) ]._res != ii ) {
         os << "Error in resource " << ii << " : " 
            << mapStrings[ XxResources::Resource( ii ) ]._name << std::endl;
      }
   }
   return err;
}

//------------------------------------------------------------------------------
//
XxResources::~XxResources()
{}

//------------------------------------------------------------------------------
//
bool XxResources::query( 
   XxResourcesParser&    parser,   
   XxResources::Resource resource,
   std::string&          value
) const
{
   return parser.query( resource, mapStrings[ resource ]._name, value );
}

//------------------------------------------------------------------------------
//
void XxResources::parse( XxResourcesParser& parser )
{
   std::string val;

   if ( query( parser, PREFERRED_GEOMETRY, val ) ) {
      readGeometry( val, _preferredGeometry );
   }

   for ( int ii = 0; ii < ACCEL_LAST - ACCEL_FIRST; ++ii ) {
      if ( query( parser, Resource(ii + ACCEL_FIRST), val ) ) {
         readAccelerator( val, _accelerators[ ii ] );
      }
   }

   for ( int ii = 0; ii < COLOR_LAST - COLOR_FIRST; ++ii ) {
      if ( query( parser, Resource(ii + COLOR_FIRST), val ) ) {
         readColor( val, _colors[ ii ] );
      }
   }

   if ( query( parser, FONT_APP, val ) ) {
      _fontApp.setRawName( val.c_str() );
   }
   if ( query( parser, FONT_TEXT, val ) ) {
      _fontText.setRawName( val.c_str() );
   }

   for ( int ii = 0; ii < BOOL_LAST - BOOL_FIRST; ++ii ) {
      if ( query( parser, Resource(ii + BOOL_FIRST), val ) ) {
         readBoolean( val, _boolOpts[ ii ] );
      }
   }

   if ( query( parser, TAB_WIDTH, val ) ) {
      _tabWidth = atoi( val.c_str() );
   }

   for ( int ii = 0; ii < COMMAND_LAST - COMMAND_FIRST; ++ii ) {
      if ( query( parser, Resource(ii + COMMAND_FIRST), val ) ) {
         _commands[ ii ] = val;
      }
   }

   for ( int ii = 0; ii < CMDOPT_LAST - CMDOPT_FIRST; ++ii ) {
      if ( query( parser, Resource(ii + CMDOPT_FIRST), val ) ) {
         _commandOptions[ ii ] = val;
      }
   }

   for ( int ii = 0; ii < TAG_LAST - TAG_FIRST; ++ii ) {
      if ( query( parser, Resource(ii + TAG_FIRST), val ) ) {
         _tags[ ii ] = val;
      }
   }

   if ( query( parser, OVERVIEW_FILE_WIDTH, val ) ) {
      _overviewFileWidth = atoi( val.c_str() );
   }
   if ( query( parser, OVERVIEW_SEP_WIDTH, val ) ) {
      _overviewSepWidth = atoi( val.c_str() );
   }
   
}   

//------------------------------------------------------------------------------
//
const char* XxResources::getCommand( Resource cmdId ) const
{
   int id = int( cmdId ) - int( COMMAND_FIRST );
   XX_CHECK( id < int(COMMAND_LAST) - int(COMMAND_FIRST) );
   return _commands[ id ].c_str();
}

//------------------------------------------------------------------------------
//
void XxResources::setCommand( Resource cmdId, const char* t ) 
{
   int id = int( cmdId ) - int( COMMAND_FIRST );
   XX_CHECK( id < int(COMMAND_LAST) - int(COMMAND_FIRST) );
   _commands[ id ].assign( t );
}

//------------------------------------------------------------------------------
//
const char* XxResources::getCommandOption( Resource cmdId ) const
{
   int id = int( cmdId ) - int( CMDOPT_FIRST );
   XX_CHECK( id < int(CMDOPT_LAST) - int(CMDOPT_FIRST) );
   return _commandOptions[ id ].c_str();
}

//------------------------------------------------------------------------------
//
XxResources::Quality XxResources::getQuality( const std::string& command ) const
{
   switch ( XxOptionsDialog::isInCommand( 
      command,
      getCommandOption( CMDOPT_FILES_QUALITY_NORMAL ),
      getCommandOption( CMDOPT_FILES_QUALITY_FASTEST ),
      getCommandOption( CMDOPT_FILES_QUALITY_HIGHEST )
   ) ) {
      case 1: return QUALITY_NORMAL;
      case 2: return QUALITY_FASTEST;
      case 3: return QUALITY_HIGHEST;
   }

   // In other cases, just return normal, whatever.  The user can definitely
   // push it around, but there's no reason to barf for this little fluff.
   return QUALITY_NORMAL;
}

//------------------------------------------------------------------------------
//
void XxResources::setQuality( std::string& command, Quality quality ) const
{
   const char* opt1 = getCommandOption( CMDOPT_FILES_QUALITY_NORMAL );
   const char* opt2 = getCommandOption( CMDOPT_FILES_QUALITY_FASTEST );
   const char* opt3 = getCommandOption( CMDOPT_FILES_QUALITY_HIGHEST );

   switch ( quality ) {
      case QUALITY_NORMAL: {
         XxOptionsDialog::setOneOfInCommand( command, opt1, opt2, opt3 );
      } break;
      case QUALITY_FASTEST: {
         XxOptionsDialog::setOneOfInCommand( command, opt2, opt1, opt3 );
      } break;
      case QUALITY_HIGHEST: {
         XxOptionsDialog::setOneOfInCommand( command, opt3, opt1, opt2 );
      } break;
   }
}

//------------------------------------------------------------------------------
//
bool XxResources::isCommandOption( Resource cmdId, Resource cmdOptionId ) const
{
   const char* cmd = getCommand( cmdId );
   const char* opt = getCommandOption( cmdOptionId );

   return XxOptionsDialog::isInCommand( cmd, opt );
}

//------------------------------------------------------------------------------
//
void XxResources::setCommandOption( 
   Resource cmdId,
   Resource cmdOptionId,
   bool     setit
)
{
   std::string cmd = getCommand( cmdId );
   const char* opt = getCommandOption( cmdOptionId );

   if ( setit == true ) {
      XxOptionsDialog::addToCommand( cmd, opt );
   }
   else {
      XxOptionsDialog::removeFromCommand( cmd, opt );
   }

   setCommand( cmdId, cmd.c_str() );
}

//------------------------------------------------------------------------------
//
void XxResources::toggleCommandOption( 
   Resource cmdId,
   Resource cmdOptionId
)
{
   std::string cmd = getCommand( cmdId );
   const char* opt = getCommandOption( cmdOptionId );

   if ( ! XxOptionsDialog::isInCommand( cmd, opt ) ) {
      XxOptionsDialog::addToCommand( cmd, opt );
   }
   else {
      XxOptionsDialog::removeFromCommand( cmd, opt );
   }

   setCommand( cmdId, cmd.c_str() );
}

//------------------------------------------------------------------------------
//
const char* XxResources::getTag( Resource cmdId ) const
{
   int id = int( cmdId ) - int( TAG_FIRST );
   XX_CHECK( id < int(TAG_LAST) - int(TAG_FIRST) );
   return _tags[ id ].c_str();
}

//------------------------------------------------------------------------------
//
void XxResources::setColor(
   const Resource resource,
   const QColor&  color
)
{
   int i = int(resource) - int(COLOR_FIRST);
   XX_CHECK( i < int(COLOR_LAST) - int(COLOR_FIRST) );
   _colors[ i ] = color;
}


//------------------------------------------------------------------------------
//
XxResources::Resource XxResources::getLineColorType(
   const XxLine& line,
   const int     no,
   const bool    sup
) const
{
   XxLine::Selection sel = line.getSelection();
   if ( sel == XxLine::SEL1 ||
        sel == XxLine::SEL2 ||
        sel == XxLine::SEL3 ) {

      if ( int(sel) == no ) {
         return ( sup == false ) ?
            COLOR_BACK_SELECTED : COLOR_BACK_SELECTED_SUP;
      }
      else if ( 
         ( ( line.getType() == XxLine::DELETE_1 || 
             line.getType() == XxLine::DIFF_1 ||
             line.getType() == XxLine::INSERT_1 ) && 
           no != 0 && int(sel) != XxLine::SEL1 ) ||

         ( ( line.getType() == XxLine::DELETE_2 || 
             line.getType() == XxLine::DIFF_2 || 
             line.getType() == XxLine::INSERT_2 ) &&
           no != 1 && int(sel) != XxLine::SEL2 ) ||

         ( ( line.getType() == XxLine::DELETE_3 || 
             line.getType() == XxLine::DIFF_3 ||
             line.getType() == XxLine::INSERT_3 ) && 
           no != 2 && int(sel) != XxLine::SEL3 ) 
      ) {
         // For regions that are not selected but whose text is the same as the
         // ones that are selected, color as selected as well.
         return ( sup == false ) ?
            COLOR_BACK_SELECTED : COLOR_BACK_SELECTED_SUP;
      }
      else {
         return ( sup == false ) ?
            COLOR_BACK_DELETED : COLOR_BACK_DELETED_SUP;
      }
   }
   else if ( sel == XxLine::NEITHER ) {
      return ( sup == false ) ?
         COLOR_BACK_DELETED : COLOR_BACK_DELETED_SUP;
   }
   // else

   int lno = -1;
   switch ( line.getType() ) {
      case XxLine::SAME: lno = -1; break;
      case XxLine::DIFF_1: lno = 0; break;
      case XxLine::DIFF_2: lno = 1; break;
      case XxLine::DIFF_3: lno = 2; break;
      case XxLine::DELETE_1: lno = 0; break;
      case XxLine::DELETE_2: lno = 1; break;
      case XxLine::DELETE_3: lno = 2; break;
      case XxLine::INSERT_1: lno = 0; break;
      case XxLine::INSERT_2: lno = 1; break;
      case XxLine::INSERT_3: lno = 2; break;
      case XxLine::DIFF_ALL: lno = -1; break;
      case XxLine::DIFFDEL_1: lno = 0; break;
      case XxLine::DIFFDEL_2: lno = 1; break;
      case XxLine::DIFFDEL_3: lno = 2; break;
      case XxLine::DIRECTORIES: lno = -1; break;
   }

   switch ( line.getType() ) {

      case XxLine::SAME: {
         return COLOR_BACK_SAME;
      }

      case XxLine::DIFF_1: 
      case XxLine::DIFF_2: 
      case XxLine::DIFF_3: {
         if ( no == lno ) {
            if ( line.getLineNo(no) == -1 ) {
               return COLOR_BACK_DIFF_ONE_NONLY;
            }
            else if ( line.getLineNo((no+1)%3) == -1 ) {
               return COLOR_BACK_DIFF_ONE_ONLY;
            }
            else {
               return ( sup == false ) ?
                  COLOR_BACK_DIFF_ONE : COLOR_BACK_DIFF_ONE_SUP;
            }
         }
         else {
            if ( line.getLineNo(no) == -1 ) {
               return COLOR_BACK_DIFF_TWO_NONLY;
            }
            else if ( line.getLineNo((no+1)%3) == -1 ) {
               return COLOR_BACK_DIFF_TWO_ONLY;
            }
            else {
               return ( sup == false ) ?
                  COLOR_BACK_DIFF_TWO : COLOR_BACK_DIFF_TWO_SUP;
            }
         }
      }

      case XxLine::DELETE_1: 
      case XxLine::DELETE_2:
      case XxLine::DELETE_3: {
         if ( no == lno ) {
            return COLOR_BACK_DELETE_BLANK;
         }
         else {
            return COLOR_BACK_DELETE;
         }
      }

      case XxLine::INSERT_1:
      case XxLine::INSERT_2:
      case XxLine::INSERT_3: {
         if ( no == lno ) {
            return COLOR_BACK_INSERT;
         }
         else {
            return COLOR_BACK_INSERT_BLANK;
         }
      }

      case XxLine::DIFF_ALL: {
         if ( line.getLineNo(no) == -1 ) {
            return COLOR_BACK_DIFF_ALL_NONLY;
         }
         else if ( line.getLineNo((no+1)%3) == -1 &&
                   line.getLineNo((no+2)%3) == -1 ) {
            return COLOR_BACK_DIFF_ALL_ONLY;
         }
         else {
            return ( sup == false ) ?
               COLOR_BACK_DIFF_ALL : COLOR_BACK_DIFF_ALL_SUP;
         }
      }

      case XxLine::DIFFDEL_1:
      case XxLine::DIFFDEL_2:
      case XxLine::DIFFDEL_3: {
         if ( no == lno ) {
            return COLOR_BACK_DIFFDEL_BLANK;
         }
         // else
         if ( line.getLineNo(no) == -1 ) {
            return COLOR_BACK_DIFFDEL_NONLY;
         }
         else if ( line.getLineNo((no+1)%3) == -1 &&
                   line.getLineNo((no+2)%3) == -1 ) {
            return COLOR_BACK_DIFFDEL_ONLY;
         }
         else {
            return ( sup == false ) ?
               COLOR_BACK_DIFFDEL : COLOR_BACK_DIFFDEL_SUP;
         }
      }

      case XxLine::DIRECTORIES: {
         return COLOR_BACK_DIRECTORIES;
      }

   }

   return COLOR_BACK_SAME; // unreached.
}

//------------------------------------------------------------------------------
//
void XxResources::genInitFile( const XxApp* app, std::ostream& os ) const
{
   using namespace std;

   std::string val;
   XxDefaultsParser defaults;

   QRect curGeometry = app->getMainWindowGeometry();
   QRect defGeometry;
   if ( query( defaults, PREFERRED_GEOMETRY, val ) ) {
      readGeometry( val, defGeometry );
   }
   if ( curGeometry != defGeometry ) {
      writeDocAttrib( this, os, PREFERRED_GEOMETRY );
      writeGeometry( os, curGeometry );
      os << endl << endl;
   }

   for ( int ii = 0; ii < ACCEL_LAST - ACCEL_FIRST; ++ii ) {
      int accel = 0;
      if ( query( defaults, Resource(ii + ACCEL_FIRST), val ) ) {
         readAccelerator( val, accel );
      }
      if ( accel != _accelerators[ ii ] ) { 
         writeDocAttrib( this, os, Resource(ii + ACCEL_FIRST) );
         writeAccelerator( os, _accelerators[ ii ] );
         os << endl << endl;
      }
   }

   for ( int ii = 0; ii < COLOR_LAST - COLOR_FIRST; ++ii ) {
      QColor color;
      if ( query( defaults, Resource(ii + COLOR_FIRST), val ) ) {
         readColor( val, color );
      }
      if ( color != _colors[ ii ] ) {
         writeDocAttrib( this, os, Resource(ii + COLOR_FIRST) );
         writeColor( os, _colors[ ii ] );
         os << endl << endl;
      }
   }

   {
      QFont dfont;
      if ( query( defaults, FONT_APP, val ) ) {
         dfont.setRawName( val.c_str() );
      }
      if ( dfont != _fontApp ) {
         writeDocAttrib( this, os, FONT_APP );
         os << _fontApp.rawName();
      }
   }

   {
      QFont dfont;
      if ( query( defaults, FONT_TEXT, val ) ) {
         dfont.setRawName( val.c_str() );
      }
      if ( dfont != _fontText ) {
         writeDocAttrib( this, os, FONT_TEXT );
         os << _fontText.rawName();
         os << endl << endl;
      }
   }

   for ( int ii = 0; ii < BOOL_LAST - BOOL_FIRST; ++ii ) {
      bool dbool;
      if ( query( defaults, Resource(ii + BOOL_FIRST), val ) ) {
         readBoolean( val, dbool );
      }
      if ( dbool != _boolOpts[ ii ] ) {
         writeDocAttrib( this, os, Resource(ii + BOOL_FIRST) );
         writeBoolean( os, _boolOpts[ ii ] );
         os << endl << endl;
      }
   }

   uint dtw = 8;
   if ( query( defaults, TAB_WIDTH, val ) ) {
      dtw = atoi( val.c_str() );
   }

   if ( dtw != _tabWidth ) {
      writeDocAttrib( this, os, TAB_WIDTH );
      os << _tabWidth;
      os << endl << endl;
   }

   for ( int ii = 0; ii < COMMAND_LAST - COMMAND_FIRST; ++ii ) {
      query( defaults, Resource(ii + COMMAND_FIRST), val );
      if ( val != _commands[ ii ] ) {
         writeDocAttrib( this, os, Resource(ii + COMMAND_FIRST) );
         os << _commands[ ii ];
         os << endl << endl;
      }
   }

   for ( int ii = 0; ii < CMDOPT_LAST - CMDOPT_FIRST; ++ii ) {
      query( defaults, Resource(ii + CMDOPT_FIRST), val );
      if ( val != _commandOptions[ ii ] ) {
         writeDocAttrib( this, os, Resource(ii + CMDOPT_FIRST) );
         os << _commandOptions[ ii ];
         os << endl << endl;
      }
   }

   for ( int ii = 0; ii < TAG_LAST - TAG_FIRST; ++ii ) {
      query( defaults, Resource(ii + TAG_FIRST), val );
      if ( val != _tags[ ii ] ) {
         writeDocAttrib( this, os, Resource(ii + TAG_FIRST) );
         os << _tags[ ii ];
         os << endl << endl;
      }
   }

   {
      uint ofw = 0;
      if ( query( defaults, OVERVIEW_FILE_WIDTH, val ) ) {
         ofw = atoi( val.c_str() );
      }
      if ( ofw != _overviewFileWidth ) {
         writeDocAttrib( this, os, OVERVIEW_FILE_WIDTH );
         os << _overviewFileWidth;
         os << endl << endl;
      }
   }

   {
      uint ofw = 0;
      if ( query( defaults, OVERVIEW_SEP_WIDTH, val ) ) {
         ofw = atoi( val.c_str() );
      }
      if ( ofw != _overviewSepWidth ) {
         writeDocAttrib( this, os, OVERVIEW_SEP_WIDTH );
         os << _overviewSepWidth;
         os << endl << endl;
      }
   }

}   

//------------------------------------------------------------------------------
//
void XxResources::listResources( std::ostream& os ) const
{
   using namespace std;

   XxDefaultsParser defaults;
   
   std::string val;
   for ( int ii = RESOURCE_FIRST; ii < RESOURCE_LAST; ++ii ) {
      if ( query( defaults, Resource(ii), val ) ) {
         writeDocAttrib( this, os, Resource(ii) );
         os << val << endl << endl;
      }
   }
}   

/*==============================================================================
 * CLASS XxResourcesParser
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxResourcesParser::~XxResourcesParser()
{
}


XX_NAMESPACE_END

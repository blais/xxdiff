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

#include <app.h> // just for getting the size of the main window
#include <accelUtil.h>
#include <resources.h>
#include <optionsDialog.h>
#include <exceptions.h>
#include <util.h>

#include <QColor>
#include <QObject>
#include <QApplication> // to get desktop
#include <QRegularExpression>
#include <QStyleFactory>

#include <iostream>
#include <string.h> // ::strcmp
#include <stdio.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxResources
 *============================================================================*/

QRect XxResources::_defaultGeometry( -1, -1, 1200, 600 );

//------------------------------------------------------------------------------
//
XxResources::XxResources( bool original ) :
   _hdTableSize( 0 ),
   _hdTable( 0 ),
   _hdBufferSize( 0 ),
   _hdBuffer0( 0 ),
   _hdBuffer1( 0 ),
   _hdLineBufSize( 0 ),
   _hdLineBuf0( 0 ),
   _hdLineBuf1( 0 )
{
   if ( !original ) {
      initialize();
   }
   else {
      initializeOriginalXdiff();
   }
}

//------------------------------------------------------------------------------
//
void XxResources::initialize()
{
   initializeOriginalXdiff();

   _boolOpts[ BOOL_IGNORE_HORIZONTAL_WS ] = false;
   _boolOpts[ BOOL_IGNORE_PERHUNK_WS ] = false;
   _boolOpts[ BOOL_FORMAT_CLIPBOARD_TEXT ] = true;
   _boolOpts[ BOOL_DRAW_PATTERN_IN_FILLER_LINES ] = true;

   _showOpts[ SHOW_OVERVIEW ] = true;
   _showOpts[ SHOW_FILENAMES ] = true;

   _hordiffType = HD_MULTIPLE;
}

//------------------------------------------------------------------------------
//
void setCombined(int* array, XxAccel index, Qt::Key key) {
   array[index] = key;
}

void setCombined(int* array, XxAccel index, unsigned int key) {
   array[index] = key;
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void setCombined(int* array, XxAccel index, const QKeyCombination& kc) {
   array[index] = kc.toCombined();
}
#endif

void XxResources::initializeOriginalXdiff()
{
   // Just like xdiff.
   _preferredGeometry = _defaultGeometry;
   _styleKey =  // Default style.
#ifdef Q_OS_MAC
      "macOS";
#elif defined(Q_OS_WIN)
      "Windows";
#else
      "";
#endif
   _maximize = false;

   //---------------------------------------------------------------------------

   // Initialize because this is more handy than specifying empty defaults for
   // all accelerators.
   for ( int ii = 0; ii < ACCEL_LAST; ++ii ) {
      _accelerators[ ii ] = 0;
   }

   setCombined(_accelerators, ACCEL_EXIT, Qt::CTRL | Qt::Key_Q);
   setCombined(_accelerators, ACCEL_EXIT_ACCEPT, Qt::Key_A);
   setCombined(_accelerators, ACCEL_EXIT_MERGED, Qt::Key_M);
   setCombined(_accelerators, ACCEL_EXIT_REJECT, Qt::Key_R);

   setCombined(_accelerators, ACCEL_SAVE_AS_LEFT, Qt::CTRL | Qt::SHIFT | Qt::Key_L);
   setCombined(_accelerators, ACCEL_SAVE_AS_RIGHT, Qt::CTRL | Qt::SHIFT | Qt::Key_R);
   setCombined(_accelerators, ACCEL_SAVE_AS_MERGED, Qt::CTRL | Qt::Key_M);

   setCombined(_accelerators, ACCEL_SEARCH, Qt::CTRL | Qt::Key_S);
   setCombined(_accelerators, ACCEL_SEARCH_FORWARD, Qt::CTRL | Qt::Key_F);
   setCombined(_accelerators, ACCEL_SEARCH_BACKWARD, Qt::CTRL | Qt::Key_B);
   setCombined(_accelerators, ACCEL_PAGE_DOWN, Qt::CTRL | Qt::Key_V);
   setCombined(_accelerators, ACCEL_PAGE_UP, Qt::ALT | Qt::Key_V);
   setCombined(_accelerators, ACCEL_CURSOR_DOWN, Qt::CTRL | Qt::Key_N);
   setCombined(_accelerators, ACCEL_CURSOR_UP, Qt::CTRL | Qt::Key_P);
   setCombined(_accelerators, ACCEL_CURSOR_TOP, Qt::Key_Home);
   setCombined(_accelerators, ACCEL_CURSOR_BOTTOM, Qt::Key_End);
   setCombined(_accelerators, ACCEL_REDO_DIFF, Qt::CTRL | Qt::Key_R);
   setCombined(_accelerators, ACCEL_DIFF_FILES_AT_CURSOR, Qt::Key_Return);
   setCombined(_accelerators, ACCEL_NEXT_AND_DIFF_FILES, Qt::Key_Space);
   setCombined(_accelerators, ACCEL_NEXT_DIFFERENCE, Qt::Key_N);
   setCombined(_accelerators, ACCEL_PREVIOUS_DIFFERENCE, Qt::Key_P);
   setCombined(_accelerators, ACCEL_NEXT_UNSELECTED, Qt::Key_B);
   setCombined(_accelerators, ACCEL_PREVIOUS_UNSELECTED, Qt::Key_O);
   setCombined(_accelerators, ACCEL_SELECT_GLOBAL_LEFT, Qt::CTRL|Qt::ALT | Qt::Key_H);
   setCombined(_accelerators, ACCEL_SELECT_GLOBAL_MIDDLE, Qt::CTRL|Qt::ALT | Qt::Key_J);
   setCombined(_accelerators, ACCEL_SELECT_GLOBAL_RIGHT, Qt::CTRL|Qt::ALT | Qt::Key_K);
   setCombined(_accelerators, ACCEL_SELECT_GLOBAL_NEITHER, Qt::CTRL|Qt::ALT | Qt::Key_Y);
   setCombined(_accelerators, ACCEL_SELECT_GLOBAL_UNSELECT, Qt::CTRL|Qt::ALT | Qt::Key_U);
   setCombined(_accelerators, ACCEL_SELECT_GLOBAL_MERGE, Qt::CTRL|Qt::ALT | Qt::Key_M);
   setCombined(_accelerators, ACCEL_SELECT_REGION_LEFT, Qt::Key_H);
   setCombined(_accelerators, ACCEL_SELECT_REGION_MIDDLE, Qt::Key_J);
   setCombined(_accelerators, ACCEL_SELECT_REGION_RIGHT, Qt::Key_K);
   setCombined(_accelerators, ACCEL_SELECT_REGION_NEITHER, Qt::Key_Y);
   setCombined(_accelerators, ACCEL_SELECT_REGION_UNSELECT, Qt::Key_U);
   setCombined(_accelerators, ACCEL_SELECT_REGION_LEFT_AND_NEXT, Qt::CTRL | Qt::Key_H);
   setCombined(_accelerators, ACCEL_SELECT_REGION_MIDDLE_AND_NEXT, Qt::CTRL | Qt::Key_J);
   setCombined(_accelerators, ACCEL_SELECT_REGION_RIGHT_AND_NEXT, Qt::CTRL | Qt::Key_K);
   setCombined(_accelerators, ACCEL_SELECT_REGION_NEITHER_AND_NEXT, Qt::CTRL | Qt::Key_Y);
   setCombined(_accelerators, ACCEL_SELECT_REGION_SPLIT_SWAP_JOIN, Qt::Key_S);
   setCombined(_accelerators, ACCEL_SELECT_LINE_LEFT, Qt::SHIFT | Qt::Key_H);
   setCombined(_accelerators, ACCEL_SELECT_LINE_MIDDLE, Qt::SHIFT | Qt::Key_J);
   setCombined(_accelerators, ACCEL_SELECT_LINE_RIGHT, Qt::SHIFT | Qt::Key_K);
   setCombined(_accelerators, ACCEL_SELECT_LINE_NEITHER, Qt::SHIFT | Qt::Key_Y);
   setCombined(_accelerators, ACCEL_SELECT_LINE_UNSELECT, Qt::SHIFT | Qt::Key_U);
   setCombined(_accelerators, ACCEL_TABS_AT_3, Qt::Key_3);
   setCombined(_accelerators, ACCEL_TABS_AT_4, Qt::Key_4);
   setCombined(_accelerators, ACCEL_TABS_AT_8, Qt::Key_8);
   setCombined(_accelerators, ACCEL_TOGGLE_PANE_MERGED_VIEW, Qt::ALT | Qt::Key_Y);
   setCombined(_accelerators, ACCEL_TOGGLE_LINE_NUMBERS, Qt::ALT | Qt::Key_L);
   setCombined(_accelerators, ACCEL_TOGGLE_MARKERS, Qt::ALT | Qt::Key_M);
   setCombined(_accelerators, ACCEL_TOGGLE_OVERVIEW, Qt::ALT | Qt::Key_O);
   setCombined(_accelerators, ACCEL_TOGGLE_SHOW_FILENAMES, Qt::ALT | Qt::Key_S);
   setCombined(_accelerators, ACCEL_HORDIFF_NONE, Qt::ALT | Qt::Key_W);
   setCombined(_accelerators, ACCEL_HORDIFF_SINGLE, Qt::ALT | Qt::Key_E);
   setCombined(_accelerators, ACCEL_HORDIFF_MULTIPLE, Qt::ALT | Qt::Key_R);
   setCombined(_accelerators, ACCEL_TOGGLE_FORMAT_CLIPBOARD_TEXT, Qt::ALT | Qt::Key_A);
   setCombined(_accelerators, ACCEL_HELP_ON_CONTEXT, Qt::SHIFT | Qt::Key_F1);

   setCombined(_accelerators, ACCEL_MERGED_CLOSE, Qt::ALT | Qt::Key_W);

   setCombined(_accelerators, ACCEL_FONT_RESIZE_BIGGER, Qt::CTRL | Qt::Key_Plus);
   setCombined(_accelerators, ACCEL_FONT_RESIZE_SMALLER, Qt::CTRL | Qt::Key_Minus);

   //---------------------------------------------------------------------------

#ifdef XX_KDE
   if ( qApp != 0 )
      _fontText = KGlobalSettings::fixedFont();
   else {
#else
      // // Use a default font that looks like the one from the default SGI scheme.
      // _fontApp.setFamily( "Helvetica" );
      // _fontApp.setItalic( true );
      // _fontApp.setBold( true );

      // Note: using "-*-screen-bold-i-normal--*-100-*-*-*-*-iso8859-1"
      // seems to yield the corresponding font to the SGI font under Linux.

      // Try to set the default font to be as close as possible as that under the
      // original xdiff under SGI.
      //_fontText.fromString( "*-clean-medium-r-normal-*-14-*" ); // XLFD warning
      // _fontText.fromString( "-*-clean-medium-r-normal-*-*-140-75-75-*-*-*-*" );
      //_fontText.setItalic( false );
      //_fontText.setFamily( "Lucida" );
      _fontText.setStyleHint( QFont::TypeWriter, QFont::PreferMatch );
      _fontText.setFamily( "Lucidatypewriter" );
      _fontText.setPointSize( 9 );
      _fontText.setFixedPitch( true ); // FIXME this does not work, qt bug.
#endif
#ifdef XX_KDE
   }
#endif

   //---------------------------------------------------------------------------

   if ( qApp != 0 ) { // protect setNamedColor() in case we have no display.
      setFbColors( COLOR_SAME                   , "#bebebe", "#000000" );
      setFbColors( COLOR_SAME_BLANK             , "#b3b3b3", "#000000" );
      setFbColors( COLOR_DIFF_ONE               , "#eee8aa", "#000000" );
      setFbColors( COLOR_DIFF_ONE_SUP           , "#cdc9a5", "#000000" );
      setFbColors( COLOR_DIFF_ONE_ONLY          , "#eee8aa", "#000000" );
      setFbColors( COLOR_DIFF_ONE_NONLY         , "#cdc9a5", "#000000" );
      setFbColors( COLOR_DIFF_TWO               , "#b2dfee", "#000000" );
      setFbColors( COLOR_DIFF_TWO_SUP           , "#9ac0cd", "#000000" );
      setFbColors( COLOR_DIFF_TWO_ONLY          , "#b2dfee",  "#000000" );
      setFbColors( COLOR_DIFF_TWO_NONLY         , "#9ac0cd", "#000000" );

      setFbColors( COLOR_DELETE                 , "#b2dfee", "#000000" );
      setFbColors( COLOR_DELETE_BLANK           , "#a3a3a3", "#000000" );

      setFbColors( COLOR_INSERT                 , "#b4eeb4", "#000000" );
      setFbColors( COLOR_INSERT_BLANK           , "#a3a3a3", "#000000" );

      setFbColors( COLOR_DIFF_ALL               , "#eee8aa", "#000000" );
      setFbColors( COLOR_DIFF_ALL_SUP           , "#cdc9a5", "#000000" );
      setFbColors( COLOR_DIFF_ALL_ONLY          , "#eee8aa", "#000000" );
      setFbColors( COLOR_DIFF_ALL_NONLY         , "#cdc9a5", "#000000" );

      setFbColors( COLOR_DIFFDEL                , "#eee8aa", "#000000" );
      setFbColors( COLOR_DIFFDEL_SUP            , "#cdc9a5", "#000000" );
      setFbColors( COLOR_DIFFDEL_ONLY           , "#eee8aa", "#000000" );
      setFbColors( COLOR_DIFFDEL_NONLY          , "#cdc9a5", "#000000" );
      setFbColors( COLOR_DIFFDEL_BLANK          , "#a3a3a3", "#000000" );

      setFbColors( COLOR_SELECTED               , "#dda0dd", "#000000" );
      setFbColors( COLOR_SELECTED_SUP           , "#d8bfd8", "#000000" );

      setFbColors( COLOR_IGNORE_DISPLAY         , "#d9d9d9", "#000000" );
      setFbColors( COLOR_IGNORE_DISPLAY_SUP     , "#cccccc", "#000000" );
      setFbColors( COLOR_IGNORE_DISPLAY_ONLY    , "#cccccc", "#000000" );
      setFbColors( COLOR_IGNORE_DISPLAY_NONLY   , "#cccccc", "#000000" );

      setFbColors( COLOR_DELETED                , "#778899", "#000000" );
      setFbColors( COLOR_DELETED_SUP            , "#708090", "#000000" );

      setFbColors( COLOR_IGNORED                , "#b3b3b3", "#4d4d4d" );

      setFbColors( COLOR_DIRECTORIES            , "#48d1cc", "#000000" );

      setFbColors( COLOR_MERGED_UNDECIDED       , "#cdc9a5", "#000000" );
      setFbColors( COLOR_MERGED_DECIDED_1       , "#999999", "#000000" );
      setFbColors( COLOR_MERGED_DECIDED_1_SUP   , "#b3b3b3", "#000000" );
      setFbColors( COLOR_MERGED_DECIDED_2       , "#999999", "#000000" );
      setFbColors( COLOR_MERGED_DECIDED_2_SUP   , "#b3b3b3", "#000000" );
      setFbColors( COLOR_MERGED_DECIDED_3       , "#999999", "#000000" );
      setFbColors( COLOR_MERGED_DECIDED_3_SUP   , "#b3b3b3", "#000000" );
      setFbColors( COLOR_MERGED_DECIDED_NEITHER , "#b3b3b3", "#000000" );
   }

   _backColors[ COLOR_BACKGROUND ] =      QColor( 0x40, 0x61, 0x6a );
   _backColors[ COLOR_CURSOR ] =          QColor( 0xff, 0xff, 0xff ); // white
   _backColors[ COLOR_VERTICAL_LINE ] =   QColor( 0xff, 0x00, 0x00 ); // red
   _backColors[ COLOR_TEXT_SELECTION ] =  QColor( 0xff, 0x00, 0x00 ); // red
   // Note: we don't use the fore colors for these guys.

   //---------------------------------------------------------------------------

   _boolOpts[ BOOL_EXIT_ON_SAME ] = false;
   _boolOpts[ BOOL_EXIT_IF_NO_CONFLICTS ] = false;
   _boolOpts[ BOOL_EXIT_WITH_MERGE_STATUS ] = false;
   _boolOpts[ BOOL_SELECT_MERGE ] = false;
   _boolOpts[ BOOL_IGNORE_HORIZONTAL_WS ] = true; // FIXME is this right?
   _boolOpts[ BOOL_IGNORE_PERHUNK_WS ] = false;
   _boolOpts[ BOOL_FORMAT_CLIPBOARD_TEXT ] = false;
   _boolOpts[ BOOL_IGNORE_ERRORS ] = false;
   _boolOpts[ BOOL_WARN_ABOUT_UNSAVED ] = false;
   _boolOpts[ BOOL_DISABLE_CURSOR_DISPLAY ] = false;
   _boolOpts[ BOOL_DRAW_PATTERN_IN_FILLER_LINES ] = false;
   _boolOpts[ BOOL_HIDE_CR ] = false;
   _boolOpts[ BOOL_DIRDIFF_IGNORE_FILE_CHANGES ] = false;
   _boolOpts[ BOOL_DIRDIFF_BUILD_FROM_OUTPUT ] = true;
   _boolOpts[ BOOL_DIRDIFF_RECURSIVE ] = false;
   _boolOpts[ BOOL_USE_INTERNAL_DIFF ] = true;
   _boolOpts[ BOOL_JUMP_TO_FIRST_DIFF ] = false;
   _boolOpts[ BOOL_NULL_HORIZONTAL_MARKERS ] = true;

   //---------------------------------------------------------------------------

   // Defaults just like xdiff, pretty plain.
   _showOpts[ SHOW_PANE_MERGED_VIEW ] = false;
   _showOpts[ SHOW_POPUP_MERGED_VIEW ] = false;
   _showOpts[ SHOW_TOOLBAR ] = false;
   _showOpts[ SHOW_LINE_NUMBERS ] = false;
   _showOpts[ SHOW_VERTICAL_LINE ] = false;
   _showOpts[ SHOW_OVERVIEW ] = true;
   _showOpts[ SHOW_FILENAMES ] = false;

   //---------------------------------------------------------------------------

   _tabWidth = 8;

   //---------------------------------------------------------------------------

#ifndef WINDOWS
   _commands[ CMD_DIFF_FILES_2 ] = "diff";
#else
   _commands[ CMD_DIFF_FILES_2 ] = "diff.exe";
#endif
   _commands[ CMD_DIFF_FILES_3 ] = "diff3";
   _commands[ CMD_DIFF_DIRECTORIES ] = "diff -q -s";
   _commands[ CMD_DIFF_DIRECTORIES_REC ] = "diff -q -s -r";

   // "cmp -s" barfs on directories.
   const char* editor = getenv( "EDITOR" );
   if ( editor != 0 ) {
      _commands[ CMD_EDIT ] = QString::fromLocal8Bit( editor );
   }
   else {
      _commands[ CMD_EDIT ] = "xterm -e vi";
   }

   //---------------------------------------------------------------------------

   _commandSwitch[ CMDSW_FILES_IGNORE_TRAILING ] = "-b";
   _commandSwitch[ CMDSW_FILES_IGNORE_WHITESPACE ] = "-w";
   _commandSwitch[ CMDSW_FILES_IGNORE_CASE ] = "-i";
   _commandSwitch[ CMDSW_FILES_IGNORE_BLANK_LINES ] = "-B";
   _commandSwitch[ CMDSW_FILES_QUALITY_NORMAL ] = "";
   _commandSwitch[ CMDSW_FILES_QUALITY_FASTEST ] = "-H";
   _commandSwitch[ CMDSW_FILES_QUALITY_HIGHEST ] = "-d";

   //---------------------------------------------------------------------------

   _initSwitch[ CMDSW_FILES_IGNORE_TRAILING ] = -1;
   _initSwitch[ CMDSW_FILES_IGNORE_WHITESPACE ] = -1;
   _initSwitch[ CMDSW_FILES_IGNORE_CASE ] = -1;
   _initSwitch[ CMDSW_FILES_IGNORE_BLANK_LINES ] = -1;
   _initSwitch[ CMDSW_FILES_QUALITY_NORMAL ] = -1;
   _initSwitch[ CMDSW_FILES_QUALITY_FASTEST ] = -1;
   _initSwitch[ CMDSW_FILES_QUALITY_HIGHEST ] = -1;

   //---------------------------------------------------------------------------

   _overviewFileWidth = 20;
   _overviewSepWidth = 14;
   _verticalLinePos = 80;

   //---------------------------------------------------------------------------

   _tags[ TAG_CONFLICT_START ] = "<<<<<<< %L";
   _tags[ TAG_CONFLICT_SEP ] = "=======";
   _tags[ TAG_CONFLICT_SEP_EXTRA ] = "||||||| %M";
   _tags[ TAG_CONFLICT_END ] = ">>>>>>> %R";
   _tags[ TAG_CONDITIONAL_IF ] = "#if defined( %s )";
   _tags[ TAG_CONDITIONAL_ELSEIF ] = "#elif defined( %s )";
   _tags[ TAG_CONDITIONAL_ENDIF ] = "#endif";

   // Defaults match CVS format.
   _tags[ TAG_UNMERGE_START ] = "^<<<<<<< (.*)$";
   _tags[ TAG_UNMERGE_SEP ] = "^=======\\s*$";
   _tags[ TAG_UNMERGE_SEP_EXTRA ] = "^\\|\\|\\|\\|\\|\\|\\| (.*)$";
   _tags[ TAG_UNMERGE_END ] = "^>>>>>>> (.*)$";

   //---------------------------------------------------------------------------

   _clipboardHeadFormat =
      QString("\nIn file \"%F\":\n------------------------------\n");
   _clipboardLineFormat = QString("%L: %s");

   _ignoreFile = IGNORE_NONE;

   //---------------------------------------------------------------------------

   _hordiffType = HD_NONE;
   _hordiffMax = 10000;
   _hordiffContext = 5;
   updateHordiffBuffers();

   //---------------------------------------------------------------------------

   _showPaneMergedViewPercent = 40;

   _mergedFilename = QString("%L.merge");
}

//------------------------------------------------------------------------------
//
XxResources::~XxResources()
{
   if ( _hdTable != 0 ) {
      free( _hdTable );
   }
   delete[] _hdBuffer0;
   delete[] _hdBuffer1;
   delete[] _hdLineBuf0;
   delete[] _hdLineBuf1;
}

//------------------------------------------------------------------------------
//
void XxResources::setPreferredGeometry( const QRect& geometry )
{
   _preferredGeometry = geometry;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setStyleKey( const QString& styleKey )
{
   QStringList styles = QStyleFactory::keys();
   XX_ASSERT( styles.indexOf( styleKey ) != -1 );
   _styleKey = styleKey;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setMaximize( bool fs )
{
   _maximize = fs;
   emit changed();
}

//------------------------------------------------------------------------------
//
bool XxResources::setAccelerator( XxAccel accel, const QString& val )
{
   int accelval;
   if ( XxAccelUtil::read( val, accelval ) == true ) {
      int iaccel( accel );
      XX_CHECK( iaccel < int(ACCEL_LAST) );
      _accelerators[ iaccel ] = accelval;
      emit changed();
      return true;
   }
   return false; // Parse error for accelerator string.
}

//------------------------------------------------------------------------------
//
bool XxResources::setFontApp( const QString& val )
{
   _fontApp.fromString( val );
   emit changed();
   return true; // never generates error.
}

//------------------------------------------------------------------------------
//
bool XxResources::setFontApp( const QFont& font )
{
   _fontApp = font;
   emit changed();
   return true; // never generates error.
}

//------------------------------------------------------------------------------
//
bool XxResources::setFontText( const QString& val )
{
   _fontText.fromString( val );
   emit changed();
   return true; // never generates error.
}

//------------------------------------------------------------------------------
//
bool XxResources::setFontText( const QFont& font )
{
   _fontText = font;
   emit changed();
   return true; // never generates error.
}

//------------------------------------------------------------------------------
//
void XxResources::setIgnoreFile( XxIgnoreFile ignoreFile )
{
   _ignoreFile = ignoreFile;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setCommand( XxCommand cmdId, const QString& t )
{
   _commands[ int(cmdId) ] = t;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setCommandSwitch(
   XxCommandSwitch cmdId,
   const QString&  val
)
{
   _commandSwitch[ int(cmdId) ] = val;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setInitSwitch(
   XxCommandSwitch cmdId,
   const int       val
)
{
   _initSwitch[ int(cmdId) ] = val;
   emit changed();
}

//------------------------------------------------------------------------------
//
bool XxResources::isCommandSwitch(
   XxCommand       cmdId,
   XxCommandSwitch optId
) const
{
   QString cmd = getCommand( cmdId );
   QString opt = getCommandSwitch( optId );
   return XxOptionsDialog::isInCommand( cmd, opt );
}

//------------------------------------------------------------------------------
//
void XxResources::setCommandSwitch(
   XxCommand       cmdId,
   XxCommandSwitch optId,
   bool            setit
)
{
   QString cmd = getCommand( cmdId );
   QString opt = getCommandSwitch( optId );

   if ( setit == true ) {
      XxOptionsDialog::addToCommand( cmd, opt );
   }
   else {
      XxOptionsDialog::removeFromCommand( cmd, opt );
   }

   setCommand( cmdId, cmd );
}

//------------------------------------------------------------------------------
//
void XxResources::toggleCommandSwitch(
   XxCommand       cmdId,
   XxCommandSwitch optId
)
{
   QString cmd = getCommand( cmdId );
   QString opt = getCommandSwitch( optId );

   if ( ! XxOptionsDialog::isInCommand( cmd, opt ) ) {
      XxOptionsDialog::addToCommand( cmd, opt );
   }
   else {
      XxOptionsDialog::removeFromCommand( cmd, opt );
   }

   setCommand( cmdId, cmd );
}

//------------------------------------------------------------------------------
//
XxQuality XxResources::getQuality( const QString& command ) const
{
   switch ( XxOptionsDialog::isInCommand(
      command,
      getCommandSwitch( CMDSW_FILES_QUALITY_NORMAL ),
      getCommandSwitch( CMDSW_FILES_QUALITY_FASTEST ),
      getCommandSwitch( CMDSW_FILES_QUALITY_HIGHEST )
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
void XxResources::setQuality( QString& command, XxQuality quality ) const
{
   QString opt1 = getCommandSwitch( CMDSW_FILES_QUALITY_NORMAL );
   QString opt2 = getCommandSwitch( CMDSW_FILES_QUALITY_FASTEST );
   QString opt3 = getCommandSwitch( CMDSW_FILES_QUALITY_HIGHEST );

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
void XxResources::setTag( XxTag tagId, const QString& tag )
{
   _tags[ int(tagId) ] = tag;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setColor(
   const XxColor  colorType,
   const bool     fore,
   const QString& colorstr
)
{
   if ( qApp != 0 ) { // protect setNamedColor() in case we have no display.
      if ( fore ) {
         _foreColors[ int(colorType) ].setNamedColor( colorstr );
      }
      else {
         _backColors[ int(colorType) ].setNamedColor( colorstr );
      }
   }
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setColor(
   const XxColor colorType,
   const bool    fore,
   const QColor& color
)
{
   if ( fore ) {
      _foreColors[ int(colorType) ] = color;
   }
   else {
      _backColors[ int(colorType) ] = color;
   }
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setBoolOpt( const XxBoolOpt opt, const bool value )
{
   XX_ASSERT( 0 <= opt && opt < BOOL_LAST );
   _boolOpts[ int(opt) ] = value;

   emit changed();
}

//------------------------------------------------------------------------------
//
bool XxResources::toggleBoolOpt( const XxBoolOpt opt )
{
   int ibool = int(opt);
   _boolOpts[ ibool ] = ! _boolOpts[ ibool ];
   emit changed();
   return _boolOpts[ ibool ];
}

//------------------------------------------------------------------------------
//
void XxResources::setShowOpt( const XxShowOpt opt, const bool value )
{
   _showOpts[ int(opt) ] = value;
   emit changed();
}

//------------------------------------------------------------------------------
//
bool XxResources::toggleShowOpt( const XxShowOpt opt )
{
   int ibool = int(opt);
   _showOpts[ ibool ] = ! _showOpts[ ibool ];
   emit changed();
   return _showOpts[ ibool ];
}

//------------------------------------------------------------------------------
//
void XxResources::setTabWidth( uint tabWidth )
{
   _tabWidth = tabWidth;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setOverviewFileWidth( uint w )
{
   _overviewFileWidth = w;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setOverviewSepWidth( uint w )
{
   _overviewSepWidth = w;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setVerticalLinePos( uint vlinePos )
{
   _verticalLinePos = vlinePos;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setClipboardHeadFormat( const QString& format )
{
   _clipboardHeadFormat = XxUtil::unescapeChars( format );

   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setClipboardLineFormat( const QString& format )
{
   _clipboardLineFormat = XxUtil::unescapeChars( format );
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setHordiffType( XxHordiff hordiffType )
{
   _hordiffType = hordiffType;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setHordiffMax( uint hordiffMax )
{
   _hordiffMax = hordiffMax;
   updateHordiffBuffers();
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setHordiffContext( uint hordiffContext )
{
   _hordiffContext = hordiffContext;
   updateHordiffBuffers();
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setShowPaneMergedViewPercent( uint p )
{
   _showPaneMergedViewPercent = p;
   if ( p > 100 ) {
      issueWarning( "Maximum pane merged view percent is 100" );
      p = 100;
   }
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxResources::setMergedFilename( const QString& fn )
{
   _mergedFilename = fn;
   emit changed();
}

//------------------------------------------------------------------------------
//
bool XxResources::compareFonts( const QFont& f1, const QFont& f2 )
{
/* Remove this code since rawMode() is deprecated
   if ( f1.rawMode() || f2.rawMode() ) {
      return f1.toString() == f2.toString();
   } */
   return f1 == f2;
}

//------------------------------------------------------------------------------
//
void XxResources::applyInitSwitch()
{
   // Note that these are only valid for 2-way diff command.
   const XxCommand cmdId = CMD_DIFF_FILES_2;

   int ii;
   for ( ii = int(CMDSW_FILES_IGNORE_TRAILING);
         ii <= int(CMDSW_FILES_IGNORE_BLANK_LINES);
         ++ii ) {
      QString swstr = getCommandSwitch( XxCommandSwitch(ii) );
      if ( _initSwitch[ ii ] != -1 && !swstr.isEmpty() ) {
         setCommandSwitch( cmdId,
                           XxCommandSwitch(ii),
                           bool( _initSwitch[ ii ] ) );
      }
   }

   // If the user set many of those resource enabled, just set it all, it's his
   // problem, really, and this won't really hurt anyway.
   // Perhaps we should remove the switch if there is none though.
   QString cmd = getCommand( cmdId );
   for ( ii = int(CMDSW_FILES_QUALITY_NORMAL);
         ii <= int(CMDSW_FILES_QUALITY_HIGHEST);
         ++ii ) {

      if ( _initSwitch[ ii ] == 1 ) {
         switch ( ii ) {
            case CMDSW_FILES_QUALITY_NORMAL:
               setQuality( cmd, QUALITY_NORMAL );
               break;
            case CMDSW_FILES_QUALITY_FASTEST:
               setQuality( cmd, QUALITY_FASTEST );
               break;
            case CMDSW_FILES_QUALITY_HIGHEST:
               setQuality( cmd, QUALITY_HIGHEST );
               break;
         }
      }
   }
   setCommand( cmdId, cmd );
}

//------------------------------------------------------------------------------
//
void XxResources::updateHordiffBuffers()
{
   if ( _hdBuffer0 != 0 ) {
      delete[] _hdBuffer0;
      delete[] _hdBuffer1;
      delete[] _hdLineBuf0;
      delete[] _hdLineBuf1;
      _hdBuffer0 = 0;
      _hdBuffer1 = 0;
      _hdLineBuf0 = 0;
      _hdLineBuf1 = 0;
   }

   // An upper bound in the number of horizontal diff hunks is determined by he
   // size of the context.  Each horizontal common region is at least the
   // size of the context (that's the whole point of it).

   int maxLineLen = _hordiffMax / _hordiffContext;
   int maxNbHunks = ( maxLineLen / _hordiffContext + 1 );
   _hdBufferSize = maxNbHunks * 2 + 4/*beg, end hunks*/ + 1 /*-1*/;
   _hdBuffer0 = new int[ _hdBufferSize ];
   _hdBuffer1 = new int[ _hdBufferSize ];

   _hdLineBufSize = maxLineLen + 2; // leave 1 more for the extra table size and
                                    // 1 more if we ever decide to 0-terminate
                                    // the string.
   _hdLineBuf0 = new char[ _hdLineBufSize ];
   _hdLineBuf1 = new char[ _hdLineBufSize ];
}

XX_NAMESPACE_END

/******************************************************************************\
 * $Id: resParser.y 481 2002-02-07 07:42:21Z blais $
 * $Date: 2002-02-07 02:42:21 -0500 (Thu, 07 Feb 2002) $
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

%{
#include <qstring.h>

#define RESOURCES  ( static_cast<XxResources*>(resources) )

#define BOOLKWD_BASE                 1000


/* define out second declaration because we're including the files. */
#if 0
%}
%union
{
    int   num;
    char* str;
}
%{
#endif
%}

/* generic */
%token <num> COLON DOT ERROR_TOKEN
%token <num> BOOLEAN NUMBER INITSWSTATE
%token <str> STRING


/* top- and sub- level */
%token       PREFGEOMETRY
%token <str>   GEOMSPEC
%token         GEOMFULL

%token       ACCEL
%token <num>   ACCELNAME

%token       COLOR
%token <num>   COLORNAME BACK FORE

/* see BOOLKWD_BASE above */
%token <num> EXIT_ON_SAME                 1000
%token <num> EXIT_IF_NO_CONFLICTS         1001
%token <num> SELECT_MERGE                 1002
%token <num> IGNORE_HORIZONTAL_WS         1003
%token <num> FORMAT_CLIPBOARD_TEXT        1004
%token <num> IGNORE_ERRORS                1005
%token <num> WARN_ABOUT_UNSAVED           1006
%token <num> DISABLE_CURSOR_DISPLAY       1007
%token <num> HIDE_CR                      1008
%token <num> DIRDIFF_IGNORE_FILE_CHANGES  1009
%token <num> DIRDIFF_BUILD_FROM_OUTPUT    1010
%token <num> DIRDIFF_RECURSIVE            1011
%token <num> USE_INTERNAL_DIFF            1012
/* XX_ENABLE_SAVE_MERGED_FILE */
/* %token <num> FORCE_SAVE_MERGED_FILE       1013 */

/* Note: check that we do not exceed maximum in resParser.cpp */

%token       FONT_APP
%token       FONT_TEXT

%token       COMMAND
%token <num>   COMMANDNAME

%token       COMMANDSW
%token <num>   COMMANDSWNAME

%token       INITSW
%token <num>   INITSWNAME

%token       TAG
%token <num>   TAGNAME

%token       SHOW
%token <num>   SHOWNAME

%token <num> TAB_WIDTH

%token <num> OVERVIEW_FILE_WIDTH

%token <num> OVERVIEW_SEP_WIDTH

%token <num> VERTICAL_LINE_POS

%token <num> CLIPBOARD_FORMAT

%token <num> HORDIFF_TYPE
%token <num>   HORDIFF
%token <num> HORDIFF_MAX
%token <num> HORDIFF_CONTEXT

%token <num> SHOW_PANE_MERGED_VIEW_PERCENT

%token <num> MERGED_FILENAME

/* typed rules */
%type <num> colorbf
%type <num> boolkwd

%start xxdiffrc
%pure_parser

%%
xxdiffrc	: stmts
		;

stmts		: /* Empty */
		| stmts stmt
		;

stmt		: error '\n'
		| prefgeometry
		| accel
		| color
		| boolopt
		| command
		| commandsw
		| initsw
		| font
		| tag
		| show
		| tab_width
		| overview_file_width
		| overview_sep_width
		| vertical_line_pos
		| clipboard_format
		| hordiff_type
		| hordiff_max
		| hordiff_context
		| show_pane_merged_view_percent
		| merged_filename
		;

prefgeometry	: PREFGEOMETRY COLON GEOMSPEC
		{ 
                   /*printf( "==> geometry: %s\n", $3 );*/
                   QString val( $3 );
                   QRect geometry;
                   if ( readGeometry( val, geometry ) ) {
                      RESOURCES->setPreferredGeometry( geometry );
                   }
                   else {
                      yyerror( "Bad geometry specification." );
                      // Should never happen, the lexer regexp should be tough
                      // enough.
                   }
                }
		| PREFGEOMETRY COLON GEOMFULL
		{ 
                   /*printf( "==> geometry: %s\n", $3 );*/
                   RESOURCES->setMaximize( true );
                }
		;

accel		: ACCEL DOT ACCELNAME COLON STRING
		{
                   /*printf( "==> accel %d: %s\n", $3, $5 );*/
                   if ( !RESOURCES->setAccelerator( XxAccel($3), $5 ) ) {
                      char buf[2048];
                      ::snprintf( buf, 2048,
                                  "Unrecognized accelerator: %s\n", $5 );
                      yyerror( buf );
                   }
                }
		;

color		: COLOR DOT COLORNAME COLON STRING
		{
                   /*printf( "==> color %d back: %s\n", $3, $5 );*/
                   RESOURCES->setColor(
                      XxColor($3), false, QString($5)
                   );
                }
		| COLOR DOT COLORNAME DOT colorbf COLON STRING
		{
                   /*printf( "==> color %d %s: %s\n", 
                           $3, ($5 == BACK) ? "back" : "fore", $7 );*/
                   RESOURCES->setColor( 
                      XxColor($3), ($5 == FORE), QString($7)
                   );
                }
		;

colorbf         : BACK 
		| FORE 
		;

boolopt		: boolkwd COLON BOOLEAN
		{
                   /*printf( "==> boolopt %d: %d\n", $1, $3 );*/
                   RESOURCES->setBoolOpt( boolMap[ $1 - BOOLKWD_BASE ], $3 );
                }
		;

boolkwd		: EXIT_ON_SAME
		| EXIT_IF_NO_CONFLICTS
		| SELECT_MERGE
		| IGNORE_HORIZONTAL_WS
		| FORMAT_CLIPBOARD_TEXT
		| IGNORE_ERRORS
		| WARN_ABOUT_UNSAVED
		| DISABLE_CURSOR_DISPLAY
		| HIDE_CR
		| DIRDIFF_IGNORE_FILE_CHANGES
		| DIRDIFF_BUILD_FROM_OUTPUT
		| DIRDIFF_RECURSIVE
		| USE_INTERNAL_DIFF
		;
		/*| FORCE_SAVE_MERGED_FILE*/

command		: COMMAND DOT COMMANDNAME COLON STRING
		{
                   /*printf( "==> command %d: %s\n", $3, $5 );*/
                   RESOURCES->setCommand( XxCommand($3), $5 );
                }
		;

commandsw	: COMMANDSW DOT COMMANDSWNAME COLON STRING
		{
                   /*printf( "==> commandsw %d: %s\n", $3, $5 );*/
                   RESOURCES->setCommandSwitch( XxCommandSwitch($3), $5 );
                }
		;

initsw		: INITSW DOT INITSWNAME COLON INITSWSTATE
		{
                   /*printf( "==> initsw opt %d: %s\n", $3, $5 );*/
                   RESOURCES->setInitSwitch( XxCommandSwitch($3), $5 );
                }
		;

font		: FONT_APP COLON STRING
		{
                   /*printf( "==> font app: %s\n", $3 );*/
                   RESOURCES->setFontApp( $3 );
                }
		| FONT_TEXT COLON STRING
		{
                   /*printf( "==> font text: %s\n", $3 );*/
                   RESOURCES->setFontText( $3 );
                }
		;

tag		: TAG DOT TAGNAME COLON STRING
		{
                   /*printf( "==> tag %d: %s\n", $3, $5 );*/
                   RESOURCES->setTag( XxTag($3), $5 );
                }
		;

show		: SHOW DOT SHOWNAME COLON BOOLEAN
		{
                   /*printf( "==> show %d: %d\n", $3, $5 );*/
                   RESOURCES->setShowOpt( XxShowOpt($3), $5 );
                }
		;

tab_width	: TAB_WIDTH COLON NUMBER
		{
                   /*printf( "==> tab width: %d\n", $3);*/
                   RESOURCES->setTabWidth( $3 );
                }
		;

overview_file_width : OVERVIEW_FILE_WIDTH COLON NUMBER
		{
                   /*printf( "==> overview file width: %d\n", $3 );*/
                   RESOURCES->setOverviewFileWidth( $3 );
                }
		;

overview_sep_width : OVERVIEW_SEP_WIDTH COLON NUMBER
		{
                   /*printf( "==> overview sep width: %d\n", $3 );*/
                   RESOURCES->setOverviewSepWidth( $3 );
                }
		;

vertical_line_pos : VERTICAL_LINE_POS COLON NUMBER
		{
                   /*printf( "==> vertical line pos: %d\n", $3 );*/
                   RESOURCES->setVerticalLinePos( $3 );
                }
		;

clipboard_format : CLIPBOARD_FORMAT COLON STRING
		{
                   /*printf( "==> clipboard format: %s\n", $3 );*/
                   RESOURCES->setClipboardFormat( $3 );
                }
		;


hordiff_type	: HORDIFF_TYPE COLON HORDIFF
		{
                   /*printf( "==> hordiff type: %s\n", $3 );*/
                   RESOURCES->setHordiffType( XxHordiff($3) );
                }
		;

hordiff_max	: HORDIFF_MAX COLON NUMBER
		{
                   /*printf( "==> hordiff max: %d\n", $3 );*/
                   RESOURCES->setHordiffMax( $3 );
                }
		;

hordiff_context : HORDIFF_CONTEXT COLON NUMBER
		{
                   /*printf( "==> hordiff context: %d\n", $3 );*/
                   RESOURCES->setHordiffContext( $3 );
                }
		;

show_pane_merged_view_percent : SHOW_PANE_MERGED_VIEW_PERCENT COLON NUMBER
		{
                   /*printf( "==> show pane merged view percent: %d\n", $3 );*/
                   RESOURCES->setShowPaneMergedViewPercent( $3 );
                }
		;

merged_filename : MERGED_FILENAME COLON STRING
		{
                   /*printf( "==> merged filename: %s\n", $3 );*/
                   RESOURCES->setMergedFilename( $3 );
                }
		;

%%


/*  		{ */
/*                     Skip until newline */
/*  		   char c; */
/*  		   while ( (c = yyinput() ) != '\n' ); */
/*  		   yyclearin; */
/*  		} */


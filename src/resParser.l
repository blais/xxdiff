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

%{
#include <resources.h>
#include <resParser_yacc.h>

#include <iostream>


namespace XxResParserNS {

struct StringToken;

int parseFromKeywordList(
   const StringToken* llist,
   const int          nbmem,
   int                retval,
   const char*        errmsg,
   const char*        name,
   int&               num,
   bool               noerror = 0
);

extern StringToken kwdList[];
extern StringToken boolkwdList[];
extern StringToken accelList[];
extern StringToken colorList[];
extern StringToken commandList[];
extern StringToken commandSwitchList[];
extern StringToken showList[];
extern StringToken tagList[];

extern int kwdList_size;
extern int boolkwdList_size;
extern int accelList_size;
extern int colorList_size;
extern int commandList_size;
extern int commandSwitchList_size;
extern int showList_size;
extern int tagList_size;

}
using namespace XxResParserNS;


// Declare internal buffer for lexer.
#define LEX_BUFFER_MAX	2048
char lexerBuffer[ LEX_BUFFER_MAX ];


// Override YY_INPUT to get the input from a Qt stream--setup as
// the input file--rather than from a FILE*.
#define YY_INPUT( buf, result, max_size )                              \
QTextStream* input_stream_ptr = reinterpret_cast<QTextStream*>(resParserin); \
if ( input_stream_ptr->atEnd() ) {                                     \
   result = YY_NULL;                                                   \
}                                                                      \
else {                                                                 \
   int ii = 0;                                                         \
   for ( ; (ii < max_size) && (!input_stream_ptr->atEnd()); ++ii ) {   \
	input_stream_ptr->readRawBytes( &buf[ii], 1 );                 \
   }                                                                   \
   result = ii;                                                        \
}


%}

geomspec	([0-9]+x[0-9]+)?([+-][0-9]+)?([+-][0-9]+)?
string		\"([^"]|\\.)*\"
number		[0-9]+
id              [a-zA-Z][a-zA-Z0-9]*
iddot           [a-zA-Z][a-zA-Z0-9\.]*

%option stack
%option yylineno
%option nounput
%option never-interactive
%option bison-bridge
%option header-file="resParser_lex.h"

%s GEOM_SC
%s ACCEL_SC
%s COLOR_SC COLORBF_SC
%s COMMAND_SC
%s COMMANDSW_SC
%s INITSW_SC
%s INITSWSTATE_SC
%s TAG_SC
%s SHOW_SC
%s BOOL_SC
%s HORDIFF_SC

%%
":"		{ return COLON; }

"\."		{ return DOT; }

<GEOM_SC>{

{geomspec}	{
   int len = ::strlen( yytext );
   if ( len >= LEX_BUFFER_MAX ) {
      ::strncpy( lexerBuffer, yytext, LEX_BUFFER_MAX );
      lexerBuffer[ LEX_BUFFER_MAX-1 ] = 0;
   }
   else {
      ::strncpy( lexerBuffer, yytext, len );
      lexerBuffer[ len ] = 0;
   }
   yylval->str = lexerBuffer;
   BEGIN(INITIAL);
   return GEOMSPEC;
}

[M|m]aximize	{
   BEGIN(INITIAL);
   return GEOMFULL;
}

}

<ACCEL_SC>{id}	{
   BEGIN(INITIAL);
   return parseFromKeywordList(
      accelList, accelList_size,
      ACCELNAME, "accel",
      yytext, yylval->num
   );
}

<COLOR_SC>{id}	{
   BEGIN(INITIAL);
   int token = parseFromKeywordList(
      colorList, colorList_size,
      COLORNAME, "color",
      yytext, yylval->num
   );
   if ( token != ERROR_TOKEN ) {
      BEGIN(COLORBF_SC);
   }
   return token;
}

<COLORBF_SC>([bB]ack|[fF]ore)	{
   BEGIN(INITIAL);
   yylval->num = ( (yytext[0] == 'b') || (yytext[0] == 'B') ) ? BACK : FORE;
   return yylval->num;
}

<COMMAND_SC>{id}	{
   BEGIN(INITIAL);
   return parseFromKeywordList(
      commandList, commandList_size,
      COMMANDNAME, "command",
      yytext, yylval->num
   );
}

<COMMANDSW_SC>{id}	{
   BEGIN(INITIAL);
   return parseFromKeywordList(
      commandSwitchList, commandSwitchList_size,
      COMMANDSWNAME, "command option switch",
      yytext, yylval->num
   );
}

<INITSW_SC>{id}	{
   BEGIN(INITIAL);
   int ret = parseFromKeywordList(
      commandSwitchList, commandSwitchList_size,
      INITSWNAME, "init command switch",
      yytext, yylval->num
   );
   BEGIN(INITSWSTATE_SC);
   return ret;
}

<TAG_SC>{iddot}		{
   BEGIN(INITIAL);
   return parseFromKeywordList(
      tagList, tagList_size,
      TAGNAME, "tag",
      yytext, yylval->num
   );
}

<SHOW_SC>{id}		{
   BEGIN(INITIAL);
   int ret = parseFromKeywordList(
      showList, showList_size,
      SHOWNAME, "show window",
      yytext, yylval->num
   );
   BEGIN(BOOL_SC);
   return ret;
}

<INITSWSTATE_SC>{
([Tt](rue|RUE))|1    { yylval->num = 1; BEGIN(INITIAL); return INITSWSTATE; }
([Ff](alse|ALSE))|0  { yylval->num = 0; BEGIN(INITIAL); return INITSWSTATE; }
([Nn](oo?p))|-1      { yylval->num = -1; BEGIN(INITIAL); return INITSWSTATE; }
}

<BOOL_SC>{
([Tt](rue|RUE))|1    { yylval->num = true; BEGIN(INITIAL); return BOOLEAN; }
([Ff](alse|ALSE))|0  { yylval->num = false; BEGIN(INITIAL); return BOOLEAN; }
}

<HORDIFF_SC>{
([Nn]one)|0       { yylval->num = HD_NONE; BEGIN(INITIAL); return HORDIFF; }
([Ss]ingle)|1     { yylval->num = HD_SINGLE; BEGIN(INITIAL); return HORDIFF; }
([Mm]ultiple)|2   { yylval->num = HD_MULTIPLE; BEGIN(INITIAL); return HORDIFF; }
}

{id}	{
   BEGIN(INITIAL);
   parseFromKeywordList(
      boolkwdList, boolkwdList_size,
      1, "keyword",
      yytext, yylval->num,
      true
   );
   if ( yylval->num != ERROR_TOKEN ) {
      BEGIN(BOOL_SC);
      return yylval->num;
   }
   /* else */

   BEGIN(INITIAL);
   parseFromKeywordList(
      kwdList, kwdList_size,
      1, "keyword",
      yytext, yylval->num
   );
   switch ( yylval->num ) {
      case PREFGEOMETRY: BEGIN(GEOM_SC); break;
      case ACCEL: BEGIN(ACCEL_SC); break;
      case COLOR: BEGIN(COLOR_SC); break;
      case COMMAND: BEGIN(COMMAND_SC); break;
      case COMMANDSW: BEGIN(COMMANDSW_SC); break;
      case INITSW: BEGIN(INITSW_SC); break;
      case TAG: BEGIN(TAG_SC); break;
      case SHOW: BEGIN(SHOW_SC); break;
      case HORDIFF_TYPE: BEGIN(HORDIFF_SC); break;
   }
   return yylval->num;
}

{string}	{
   /* strip the quotes */
   int len = ::strlen( yytext );
   if ( len >= LEX_BUFFER_MAX ) {
      ::strncpy( lexerBuffer, &(yytext[1]), LEX_BUFFER_MAX );
      lexerBuffer[ LEX_BUFFER_MAX-1 ] = 0;
   }
   else {
      ::strncpy( lexerBuffer, &(yytext[1]), len-2 );
      lexerBuffer[ len-2 ] = 0;
   }
   yylval->str = lexerBuffer;
   return STRING;
}

{number}	{
   (void)sscanf( (char *)yytext, "%d", &yylval->num );
   return NUMBER;
}

\#[^\n]*\n	{;}

[\n\t ]		{;}

.		{
                  {
                     /*
                     QString os;
                     QTextOStream oss( &os );
                     oss << "ignoring char: " << yytext << flush;
                     yyerror( os.latin1() );
                     */
                  }
                }

%%

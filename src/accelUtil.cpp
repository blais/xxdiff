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

#include <accelUtil.h>
#include <resources.h>

#include <qaccel.h>

#include <iostream>
#include <string.h>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {
XX_NAMESPACE_USING

/*----- variables -----*/

struct KeyPair {
   const char* _name;
   int         _code;
};

// The following list must be kept sorted in the source code.
const KeyPair keycodes[] = {
   { "(", Qt::Key_ParenLeft },
   { ")", Qt::Key_ParenRight },
   { "*", Qt::Key_Asterisk },
   { ",", Qt::Key_Comma },
   { ".", Qt::Key_Period },
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
   { ":", Qt::Key_Colon },
   { ";", Qt::Key_Semicolon },
   { "<", Qt::Key_Less },
   { "=", Qt::Key_Equal },
   { ">", Qt::Key_Greater },
   { "?", Qt::Key_Question },
   { "@", Qt::Key_At },
   { "[", Qt::Key_BracketLeft },
   { "\\", Qt::Key_Backslash },
   { "]", Qt::Key_BraceRight },
   { "]", Qt::Key_BracketRight },
   { "_", Qt::Key_Underscore },
   { "`", Qt::Key_QuoteLeft },
   { "a", Qt::Key_A },
   { "ampersand", Qt::Key_Ampersand },
   { "any", Qt::Key_Any },
   { "apostrophe", Qt::Key_Apostrophe },
   { "asciicircum", Qt::Key_AsciiCircum },
   { "asciitilde", Qt::Key_AsciiTilde },
   { "asterisk", Qt::Key_Asterisk },
   { "at", Qt::Key_At },
   { "b", Qt::Key_B },
   { "backslash", Qt::Key_Backslash },
   { "backspace", Qt::Key_BackSpace },
   { "backtab", Qt::Key_BackTab },
   { "bar", Qt::Key_Bar },
   { "braceleft", Qt::Key_BraceLeft },
   { "braceright", Qt::Key_BraceRight },
   { "bracketleft", Qt::Key_BracketLeft },
   { "bracketright", Qt::Key_BracketRight },
   { "c", Qt::Key_C },
   { "colon", Qt::Key_Colon },
   { "comma", Qt::Key_Comma },
   { "d", Qt::Key_D },
   { "delete", Qt::Key_Delete },
   { "dollar", Qt::Key_Dollar },
   { "down", Qt::Key_Down },
   { "e", Qt::Key_E },
   { "end", Qt::Key_End },
   { "enter", Qt::Key_Enter },
   { "equal", Qt::Key_Equal },
   { "esc", Qt::Key_Escape },
   { "escape", Qt::Key_Escape },
   { "exclam", Qt::Key_Exclam },
   { "f", Qt::Key_F },
   { "f1", Qt::Key_F1 },
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
   { "f2", Qt::Key_F2 },
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
   { "f3", Qt::Key_F3 },
   { "f30", Qt::Key_F30 },
   { "f31", Qt::Key_F31 },
   { "f32", Qt::Key_F32 },
   { "f33", Qt::Key_F33 },
   { "f34", Qt::Key_F34 },
   { "f35", Qt::Key_F35 },
   { "f4", Qt::Key_F4 },
   { "f5", Qt::Key_F5 },
   { "f6", Qt::Key_F6 },
   { "f7", Qt::Key_F7 },
   { "f8", Qt::Key_F8 },
   { "f9", Qt::Key_F9 },
   { "g", Qt::Key_G },
   { "greater", Qt::Key_Greater },
   { "h", Qt::Key_H },
   { "home", Qt::Key_Home },
   { "i", Qt::Key_I },
   { "insert", Qt::Key_Insert },
   { "j", Qt::Key_J },
   { "k", Qt::Key_K },
   { "l", Qt::Key_L },
   { "left", Qt::Key_Left },
   { "less", Qt::Key_Less },
   { "m", Qt::Key_M },
   { "menu", Qt::Key_Menu },
   { "minus", Qt::Key_Minus },
   { "n", Qt::Key_N },
   { "next", Qt::Key_Next },
   { "numbersign", Qt::Key_NumberSign },
   { "o", Qt::Key_O },
   { "p", Qt::Key_P },
   { "pagedown", Qt::Key_PageDown },
   { "pageup", Qt::Key_PageUp },
   { "parenleft", Qt::Key_ParenLeft },
   { "parenright", Qt::Key_ParenRight },
   { "pause", Qt::Key_Pause },
   { "percent", Qt::Key_Percent },
   { "period", Qt::Key_Period },
   { "plus", Qt::Key_Plus },
   { "print", Qt::Key_Print },
   { "prior", Qt::Key_Prior },
   { "q", Qt::Key_Q },
   { "question", Qt::Key_Question },
   { "quotedbl", Qt::Key_QuoteDbl },
   { "quoteleft", Qt::Key_QuoteLeft },
   { "r", Qt::Key_R },
   { "return", Qt::Key_Return },
   { "right", Qt::Key_Right },
   { "s", Qt::Key_S },
   { "semicolon", Qt::Key_Semicolon },
   { "slash", Qt::Key_Slash },
   { "space", Qt::Key_Space },
   { "sysreq", Qt::Key_SysReq },
   { "t", Qt::Key_T },
   { "tab", Qt::Key_Tab },
   { "u", Qt::Key_U },
   { "underscore", Qt::Key_Underscore },
   { "up", Qt::Key_Up },
   { "v", Qt::Key_V },
   { "w", Qt::Key_W },
   { "x", Qt::Key_X },
   { "y", Qt::Key_Y },
   { "z", Qt::Key_Z },
   { "{", Qt::Key_BraceLeft },
   { "|", Qt::Key_Bar },
   { "~", Qt::Key_AsciiTilde },
};

//------------------------------------------------------------------------------
//
int compareTokens( const void* v1, const void* v2 )
{
   return ::strcmp( 
      const_cast<KeyPair*>( static_cast<const KeyPair*>(v1) )->_name,
      const_cast<KeyPair*>( static_cast<const KeyPair*>(v2) )->_name
   );
}

//------------------------------------------------------------------------------
//
int searchToken( 
   const KeyPair* llist,
   const int      nbmem,
   const char*    name
)
{
   KeyPair tok;
   tok._name = name;

   void* ptr = 
      bsearch( &tok, llist, nbmem, sizeof(KeyPair), compareTokens );
   if ( ptr != 0 ) {
      return static_cast<KeyPair*>(ptr)->_code;
   }
   return -1;
}

}

/*==============================================================================
 * CLASS XxAccelUtil
 *============================================================================*/

//------------------------------------------------------------------------------
//
bool XxAccelUtil::read( const QString& val, int& accel )
{
   if ( val.isEmpty() ) {
      accel = 0;
      return true;
   }

   // Remove whitespace.
   QString cval = val.stripWhiteSpace().lower();

   // Read modifier, if present.
   const int notfound = -1;
   int modifier = 0;
   if ( cval.find( "alt" ) != notfound || 
        cval.find( "meta" ) != notfound ) {
      modifier |= Qt::ALT;
   }
   if ( cval.find( "ctrl" ) != notfound || 
        cval.find( "control" ) != notfound ) {
      modifier |= Qt::CTRL;
   }
   if ( cval.find( "shift" ) != notfound ) {
      modifier |= Qt::SHIFT;
   }

   // Read non-modifier.
   int fomin = cval.findRev( '-' );
   int foplus = cval.findRev( '+' );
   int xpos = (fomin > foplus) ? fomin : foplus;
   if ( xpos != notfound ) {
      cval = cval.mid( ++xpos );
   }

   // Search in list.
   int key = searchToken( 
      keycodes, sizeof(keycodes)/sizeof(KeyPair), cval.latin1()
   );
   if ( key == -1 ) {
      return false;
   }

   accel = modifier | key;
   return true;
}

//------------------------------------------------------------------------------
//
void XxAccelUtil::write( std::ostream& os, int accel )
{
   QString str = QAccel::keyToString( accel );
   os << str.ascii();
}

XX_NAMESPACE_END


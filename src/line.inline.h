/******************************************************************************\
 * $Id: line.inline.h 138 2001-05-20 18:08:45Z blais $
 * $Date: 2001-05-20 14:08:45 -0400 (Sun, 20 May 2001) $
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


XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxLine
 *============================================================================*/

//------------------------------------------------------------------------------
//
inline void XxLine::init()
{
   _lefthd[0] = _lefthd[1] = _lefthd[2] = -1;
   _righthd[0] = _righthd[1] = _righthd[2] = -1;
}

//------------------------------------------------------------------------------
//
inline XxLine::Type XxLine::getType() const
{
   return _type;
}

//------------------------------------------------------------------------------
//
inline XxLine::Selection XxLine::getSelection() const
{
   return _selection;
}

//------------------------------------------------------------------------------
//
inline void XxLine::setSelection( Selection selection )
{
   if ( _type != SAME ) {
      _selection = selection;
   }
}

//------------------------------------------------------------------------------
//
inline bool XxLine::getSelectedText( XxFno& no, XxFln& fline ) const
{
   if ( _type == SAME ) {
      fline = _lineNo[ 0 ];
      XX_CHECK( fline != -1 ); // sanity check
      no = 0;
      return true;
   }
   if ( _selection == SEL1 || _selection == SEL2 || _selection == SEL3 ) {
      fline = _lineNo[ XxFno(_selection) ];
      no = XxFno(_selection);
      return true;
   }
   if ( _selection == NEITHER ) {
      fline = -1;
      no = -1;
      return true;
   }
   // Unselected.
   fline = -1;
   no = -1;
   return false;
}

//------------------------------------------------------------------------------
//
inline XxFln XxLine::getLineNo( const XxFno no ) const
{
   XX_ASSERT( no == 0 || no == 1 || no == 2 );
   return _lineNo[no];
}

//------------------------------------------------------------------------------
//
inline int XxLine::getLeftHdiffPos( const XxFno no ) const
{
   XX_CHECK( no == 0 || no == 1 || no == 2 );
   return _lefthd[ no ];
}

//------------------------------------------------------------------------------
//
inline int XxLine::getRightHdiffPos( const XxFno no ) const
{
   XX_CHECK( no == 0 || no == 1 || no == 2 );
   return _righthd[ no ];
}

//------------------------------------------------------------------------------
//
inline bool XxLine::hasHorizontalDiffs( const XxFno no ) const
{
   XX_CHECK( no == 0 || no == 1 || no == 2 );
   return !( _lefthd[no] == -1 && _righthd[no] == -1 );
}

//------------------------------------------------------------------------------
// 
inline XxHunk XxLine::getHunkId() const
{
   return _hunkId;
}

//------------------------------------------------------------------------------
//
inline void XxLine::setHunkId( XxHunk hunkId )
{
   _hunkId = hunkId;
}

XX_NAMESPACE_END

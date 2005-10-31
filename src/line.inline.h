/******************************************************************************\
 * $Id: line.inline.h 2 2000-09-15 02:19:22Z blais $
 * $Date: 2000-09-14 22:19:22 -0400 (Thu, 14 Sep 2000) $
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
inline bool XxLine::getSelectedText( int& no, int& fline ) const
{
   if ( _type == SAME ) {
      fline = _lineNo[ 0 ];
      XX_CHECK( fline != -1 ); // sanity check
      no = 0;
      return true;
   }
   if ( _selection == SEL1 || _selection == SEL2 || _selection == SEL3 ) {
      fline = _lineNo[ int(_selection) ];
      no = int(_selection);
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
inline int XxLine::getLineNo( const int no ) const
{
   XX_ASSERT( no == 0 || no == 1 || no == 2 );
   return _lineNo[no];
}

//------------------------------------------------------------------------------
//
inline int XxLine::getLeftHdiffPos( const int no ) const
{
   XX_CHECK( no == 0 || no == 1 || no == 2 );
   return _lefthd[ no ];
}

//------------------------------------------------------------------------------
//
inline int XxLine::getRightHdiffPos( const int no ) const
{
   XX_CHECK( no == 0 || no == 1 || no == 2 );
   return _righthd[ no ];
}

//------------------------------------------------------------------------------
//
inline bool XxLine::hasHorizontalDiffs( const int no ) const
{
   XX_CHECK( no == 0 || no == 1 || no == 2 );
   return !( _lefthd[no] == -1 && _righthd[no] == -1 );
}

//------------------------------------------------------------------------------
// 
inline int XxLine::getHunkId() const
{
   return _hunkId;
}

//------------------------------------------------------------------------------
//
inline void XxLine::setHunkId( int hunkId )
{
   _hunkId = hunkId;
}

XX_NAMESPACE_END

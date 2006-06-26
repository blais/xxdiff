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


XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxLine
 *============================================================================*/

//------------------------------------------------------------------------------
//
inline XxLine::XxLine() :
   _type( SAME ),
   _selection( NEITHER ),
   _hunkId( -1 ),
   _perHunkWs( false )
{
   // This is just initialization to remove UMRs in Purify.
   for ( int ii = 0; ii < 3; ++ii ) {
      _lineNo[ii] = -1;
      _hordiffs[ii] = 0;
   }
}

//------------------------------------------------------------------------------
//
inline XxLine::XxLine( const XxLine& copy )
{
   _type = copy._type;
   _selection = copy._selection;
   _hunkId = copy._hunkId;
   _perHunkWs = copy._perHunkWs;

   for ( int ii = 0; ii < 3; ++ii ) {
      _lineNo[ii] = copy._lineNo[ii];
      _hordiffs[ii] = 0; // Don't copy pointers.
   }
}

//------------------------------------------------------------------------------
//
inline XxLine::~XxLine()
{
   for ( int ii = 0; ii < 3; ++ii ) {
#ifndef WINDOWS
      delete[] _hordiffs[ii];
#else
      delete[] const_cast<int*>( _hordiffs[ii] );
#endif
   }
}

//------------------------------------------------------------------------------
//
inline void XxLine::init()
{
   _hordiffs[0] = _hordiffs[1] = _hordiffs[2] = 0;
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
inline bool XxLine::isSelected( XxFno fno ) const
{
   // If not selected, simply return false.
   if ( _selection > 2 ) {
      return false;
   }

   // Get seletion mask and check the mask against the requested file.
   int mask = _selectMasks[ _type ][ _selection ];
   return ( mask & ( 1 << fno ) ) != 0;
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
inline bool XxLine::getIgnoreDisplay() const
{
   return _perHunkWs;
}

//------------------------------------------------------------------------------
//
inline void XxLine::setIgnoreDisplay( const bool f )
{
   _perHunkWs = f;
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
inline void XxLine::setLineNo( const XxFno no, const XxFln fline )
{
   XX_ASSERT( no == 0 || no == 1 || no == 2 );
   XX_ASSERT( _lineNo[no] != -1 ); // Important check! See header file.
   _lineNo[no] = fline;
}

//------------------------------------------------------------------------------
//
inline const int* XxLine::getHorDiffs( const XxFno no ) const
{
   XX_CHECK( no == 0 || no == 1 || no == 2 );
   return _hordiffs[ no ];
}

//------------------------------------------------------------------------------
//
inline bool XxLine::hasHorizontalDiffs( const XxFno no ) const
{
   XX_CHECK( no == 0 || no == 1 || no == 2 );
   return _hordiffs[no] != 0;
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

//------------------------------------------------------------------------------
//
inline XxFno XxLine::mapTypeToFileNo( Type type )
{
   XxFno lno = -1;
   switch ( type ) {
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
      case XxLine::NB_TYPES: XX_ABORT();
   }
   return lno;
}

XX_NAMESPACE_END

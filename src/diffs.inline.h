/******************************************************************************\
 * $Id: diffs.inline.h 138 2001-05-20 18:08:45Z blais $
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
 * CLASS XxDiffs
 *============================================================================*/

//------------------------------------------------------------------------------
//
inline uint XxDiffs::getNbLines() const
{
   return _lines.size();
}

//------------------------------------------------------------------------------
//
inline const XxLine& XxDiffs::getLine( const XxDln lineno ) const
{
   XX_ASSERT( lineno > 0 && lineno <= XxDln(_lines.size()) );
   return _lines[ lineno - 1 ];
}

//------------------------------------------------------------------------------
//
inline XxLine& XxDiffs::getLineNC( const XxDln lineno )
{
   XX_ASSERT( lineno > 0 && lineno <= XxDln(_lines.size()) );
   return _lines[ lineno - 1 ];
}

//------------------------------------------------------------------------------
//
inline void XxDiffs::clearDirty()
{
   _dirty = false;
}

//------------------------------------------------------------------------------
//
inline bool XxDiffs::isDirty() const
{
   return _dirty;
}

//------------------------------------------------------------------------------
//
inline bool XxDiffs::isDirectoryDiff() const
{
   return _isDirectoryDiff;
}

XX_NAMESPACE_END

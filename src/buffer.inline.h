/******************************************************************************\
 * $Id: buffer.inline.h 138 2001-05-20 18:08:45Z blais $
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
 * CLASS XxBuffer
 *============================================================================*/

//------------------------------------------------------------------------------
//
inline const char* XxBuffer::getName() const
{
   return _name.c_str();
}

//------------------------------------------------------------------------------
//
inline const char* XxBuffer::getDisplayName() const
{
   return _displayName.c_str();
}

//------------------------------------------------------------------------------
//
inline bool XxBuffer::isTemporary() const
{
   return _temporary;
}

//------------------------------------------------------------------------------
//
inline uint XxBuffer::getNbLines() const
{
   return _index.size() - 1;
}

//------------------------------------------------------------------------------
//
inline const char* XxBuffer::getTextLine( 
   const XxFln lineno, 
   uint&       length 
) const
{
   XX_ASSERT( lineno > 0 );
   XX_ASSERT( lineno < XxFln(_index.size()) );
   length = _index[ lineno ] - _index[ lineno-1 ] - 1;
   return & _buffer[ _index[ lineno-1 ] ];
}

XX_NAMESPACE_END

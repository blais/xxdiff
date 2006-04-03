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
 * CLASS XxBuffer
 *============================================================================*/

//------------------------------------------------------------------------------
//
inline bool XxBuffer::isTemporary() const
{
   return _temporary;
}

//------------------------------------------------------------------------------
//
inline QString XxBuffer::getName() const
{
   if ( isTemporary() ) {
      return QString("-");
   }
   return _name;
}

//------------------------------------------------------------------------------
//
inline const QString& XxBuffer::getDisplayName() const
{
   return _displayName;
}

//------------------------------------------------------------------------------
//
inline uint XxBuffer::getNbLines() const
{
   if ( _index.size() == 0 ) {
      return 0;
   }
   XX_ASSERT( _index.size() >= 2 );
   return _index.size() - 2; // See header file for description.
}

//------------------------------------------------------------------------------
//
inline const char* XxBuffer::getTextLine( 
   const XxFln lineno, 
   uint&       length 
) const
{
   XX_ASSERT( 0 < lineno && lineno <= XxFln(_index.size()) );

#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
   length = _lengths[ lineno ];
   /*XX_TRACE( length << " " << _index[ lineno ] - _index[ lineno-1 ] - 1 );*/
   /*XX_CHECK( length == _index[ lineno ] - _index[ lineno-1 ] - 1 );*/
#else
   // Note: in the case where the data buffer lines are contiguous, this would
   // also work (this is what it used to be until the unmerged feature was
   // introduced):
   length = _index[ lineno + 1 ] - _index[ lineno ] - 1;
#endif

   return & _buffer[ _index[ lineno ] ];
}

//------------------------------------------------------------------------------
//
inline XxFln XxBuffer::getDisplayLineNo( const XxFln fline ) const
{
   if ( _dpyLineNos.empty() ) {
      return fline;
   }
   XX_ASSERT( 0 < fline && fline <= XxFln(_index.size()) );
   return _dpyLineNos[ fline ];
}

//------------------------------------------------------------------------------
//
inline const char* XxBuffer::getBuffer( uint& size ) const
{
   size = _bufferSize;
   return _buffer;
}

XX_NAMESPACE_END

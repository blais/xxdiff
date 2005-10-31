/******************************************************************************\
 * $Id: builder.inline.h 519 2002-02-23 17:43:56Z blais $
 * $Date: 2002-02-23 12:43:56 -0500 (Sat, 23 Feb 2002) $
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

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxBuilder
 *============================================================================*/

//------------------------------------------------------------------------------
//
inline bool XxBuilder::hasErrors() const
{
   return !_errors.isEmpty();
}

//------------------------------------------------------------------------------
//
inline const QString& XxBuilder::getErrors() const
{
   return _errors;
}

//------------------------------------------------------------------------------
//
inline int XxBuilder::getStatus() const
{
   return _status;
}

//------------------------------------------------------------------------------
//
inline void XxBuilder::addLine( const XxLine& line )
{
#ifdef XX_DEBUG
   if ( _debugPrint ) {
      XX_TRACE( "AddLine: " << line );
   }
#endif
   _lines.push_back( line );
}

XX_NAMESPACE_END

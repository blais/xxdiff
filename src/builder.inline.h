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

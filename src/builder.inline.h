/******************************************************************************\
 * $Id: builder.inline.h 432 2001-11-30 07:21:57Z blais $
 * $Date: 2001-11-30 02:21:57 -0500 (Fri, 30 Nov 2001) $
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

XX_NAMESPACE_END

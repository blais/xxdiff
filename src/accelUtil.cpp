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

#include <QKeySequence>

#include <iostream>
#include <string.h>

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
   QString cval = val.trimmed().toLower();

   QKeySequence keyseq = QKeySequence::fromString( cval );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
   accel = keyseq[0].toCombined();
#else
   accel = keyseq[0];
#endif

   // Check that converting back gets the original value
   return ( cval == keyseq.toString().toLower() );
}

//------------------------------------------------------------------------------
//
void XxAccelUtil::write( std::ostream& os, int accel )
{
   os << QKeySequence( accel ).toString().toLocal8Bit().constData();
}

XX_NAMESPACE_END

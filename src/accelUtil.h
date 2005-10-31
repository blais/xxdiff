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

#ifndef INCL_XXDIFF_ACCELUTIL
#define INCL_XXDIFF_ACCELUTIL

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_QT_QSTRING
#include <qstring.h>
#define INCL_QT_QSTRING
#endif

#ifndef INCL_STD_IOSTREAM
#include <iostream>
#define INCL_STD_IOSTREAM
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxAccelUtil
 *============================================================================*/

// <summary> utiilty for translating accelerators </summary>

// Note: we hope that this shouldn't be a necessity when we move to Qt-3.
//
// This is implemented because Qt-2.x's QAccel::stringToKey() dumps core and
// doesn't do an appropriate job.

class XxAccelUtil {

public:

   /*----- static member functions -----*/

   // Reads in a value.  Returns true if successful, false if the resource was
   // not the specified type and left untouched.

   // I wish I could use QAccel::stringToKey, but it's broken.  The Qt docs say:
   //
   // "Note that this function currently only supports character accelerators
   // (unlike keyToString() which can produce Ctrl+Backspace, etc. from the
   // appropriate key codes)."
   //
   // So we do this by hand.  This should really just be provided by Qt.
   static bool read( const QString& val, int& accel );

   // Utility to write to a stream.
   static void write( std::ostream& os, int accel );

};

XX_NAMESPACE_END

#endif

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

#ifndef INCL_XXDIFF_HORDIFFIMP
#define INCL_XXDIFF_HORDIFFIMP

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxResources;

/*==============================================================================
 * CLASS XxHordiffImp
 *============================================================================*/

// <summary> horizontal diffs computation code </summary>

// This code is the implementation of the horizontal diffs algorithm used by the
// line class.

class XxHordiffImp {

public:

   /*----- static member functions -----*/

   // Compute horizontal diffs bounds for 2 files.
   static void boundsHordiff2(
      const XxResources& resources,
      const char*        text[2],
      const uint         len[2],
      int                lefthd[2],
      int                righthd[2]
   );

   // Compute horizontal diffs bounds for 3 files.
   static void boundsHordiff3(
      const XxResources& resources,
      const char*        text[3],
      const uint         len[3],
      int                lefthd[3],
      int                righthd[3]
   );

   // Compute multiple horizontal diffs for 2 files.
   // hordiff0 and hordiff1 are out values.
   static void multipleHordiffs2( 
      const XxResources& resources,
      const int*&        hordiff0,
      const char*        text0,
      const int          lefthd0, 
      const int          righthd0,
      const int*&        hordiff1, 
      const char*        text1,
      const int          lefthd1, 
      const int          righthd1
   );

};

XX_NAMESPACE_END

#endif


/******************************************************************************\
 * $Id: types.h 432 2001-11-30 07:21:57Z blais $
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

#ifndef INCL_XXDIFF_TYPES
#define INCL_XXDIFF_TYPES

XX_NAMESPACE_BEGIN

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

// These are used throughout the code, but this file is a good location for
// those two defines because it is most likely to have been included when using
// those defines.

// File line number, that is, a line number within a file.
typedef int XxFln;

// Display line number, that is, a line number for the reconciled files display.
typedef int XxDln;

// File no (0, 1 or 2)
typedef int XxFno;

// Hunk id.
typedef int XxHunk;

XX_NAMESPACE_END

#endif


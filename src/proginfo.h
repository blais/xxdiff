/******************************************************************************\
 * $Id: proginfo.h 479 2002-02-07 06:38:35Z  $
 * $Date: 2002-02-07 01:38:35 -0500 (Thu, 07 Feb 2002) $
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

#ifndef INCL_XXDIFF_VERSION
#define INCL_XXDIFF_VERSION

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

//
// Version number.
//
#ifdef XX_DEBUG
#define XX_VERSION 	"(debug version $Date: 2002-02-07 01:38:35 -0500 (Thu, 07 Feb 2002) $)"
#else
#define XX_VERSION 	"2.0.4"
#endif

#endif


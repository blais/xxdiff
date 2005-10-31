/******************************************************************************\
 * $Id: diffutils_hack.h 432 2001-11-30 07:21:57Z blais $
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

#ifndef INCL_XXDIFF_DIFFUTILS_HACK
#define INCL_XXDIFF_DIFFUTILS_HACK

/* Redirect output to xxdiff buffers.*/

#include <stdio.h>
#ifndef WINDOWS
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int xxdiff_diff_printf(const char *, ...);
extern int xxdiff_diff_fprintf(FILE *, const char *, ...);
extern int xxdiff_diff_vfprintf(FILE *, const char *, /* va_list */ char *);
extern size_t xxdiff_diff_fwrite( 
   const void *ptr, size_t size, size_t nmemb, FILE *stream
);
extern ssize_t xxdiff_diff_write( int fd, const void *buf, size_t count );

#define printf  xxdiff_diff_printf
#define fprintf  xxdiff_diff_fprintf
#define vfprintf  xxdiff_diff_vfprintf
#define fwrite  xxdiff_diff_fwrite
#define write  xxdiff_diff_write

#define exit(x)  xxdiff_diff_exit(x)
#define _exit(x)  xxdiff_diff_exit(x)

#define main  error_rename_main_to_diffutils_prog_where_prog_is_prog

#ifdef __cplusplus
}
#endif

#endif /* INCL_XXDIFF_DIFFUTILS_HACK */

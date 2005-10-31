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

#include <getopt.h>
#include <diffutils.h>

#include <iostream>

#include <stdio.h>
#include <setjmp.h>

// I haven't tested on any platform that doesn't have vsnprintf yet.
// I just enable this for now.
// Please log a bug if this causes problems for your compilation.
#define HAVE_VPRINTF

#ifdef HAVE_VPRINTF

#if __STDC__
#include <stdarg.h>
#define VA_START(args, lastarg) va_start(args, lastarg)
#else /* !__STDC__ */
#include <varargs.h>
#define VA_START(args, lastarg) va_start(args)
#endif /* !__STDC__ */

#else /* !HAVE_VPRINTF */

#ifdef HAVE_DOPRNT
#define va_alist args
#define va_dcl int args;
#else /* !HAVE_DOPRNT */
#define va_alist a1, a2, a3, a4, a5, a6, a7, a8
#define va_dcl char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
#endif /* !HAVE_DOPRNT */

#endif /* !HAVE_VPRINTF */

extern "C" {

extern int diffutils_diff_main( int, char** );
extern int diffutils_diff3_main( int, char** );
extern FILE* outfile;

}

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

QTextStream* streamStdout = 0;
QTextStream* streamStderr = 0;

// For non-local exit.
jmp_buf exit_env;

char* buffer = 0;
int buflen = 0;
const int BUFINC = 1024;

}

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

//#define DBGSS

extern "C" {

/*------------------------------------------------------------------------------
 */
int buffer_vsnprintf( const char *format, va_list arg )
{
   int nb;
   while ( 1 ) {
      nb = vsnprintf( buffer, buflen, format, arg );
      if ( nb >= 0 && nb < buflen ) {
         break;
      }
      buflen = buflen + BUFINC;
      buffer = (char*) realloc( buffer, sizeof(char) * buflen );
   }
   return nb;
}

/*------------------------------------------------------------------------------
 */
void xxdiff_diff_fprintf( FILE* fp, char *message, ... )
{
   va_list args;
   VA_START( args, message );
   buffer_vsnprintf( message, args );
   va_end (args);
   
   if ( fp == outfile ) {
      XX_ASSERT( streamStdout );
      *streamStdout << buffer;
   }
   else {
      XX_ASSERT( streamStderr );
      *streamStderr << buffer;
   }
}

/*------------------------------------------------------------------------------
 */
void xxdiff_diff_printf( char* message, ... )
{
   va_list args;
   VA_START( args, message );
   buffer_vsnprintf( message, args );
   va_end (args);
   
   XX_ASSERT( streamStdout );
   *streamStdout << buffer;
}

/*------------------------------------------------------------------------------
 */
size_t xxdiff_diff_fwrite(
   const void* ptr,
   size_t      size,
   size_t      nmemb,
   FILE*       stream
)
{
   if ( stream == outfile ) {
      XX_ASSERT( streamStdout );
      streamStdout->writeRawBytes( 
         static_cast<const char*>(ptr), size * nmemb
      );
   }
   else {
      XX_ASSERT( streamStderr );
      streamStderr->writeRawBytes(
         static_cast<const char*>(ptr), size * nmemb
      );
   }
   return size * nmemb;
}

/*------------------------------------------------------------------------------
 */
ssize_t xxdiff_diff_write( 
   int         fd,
   const void* buf,
   size_t      count
)
{
   if ( fd == 1 ) {
      XX_ASSERT( streamStdout );
      streamStdout->writeRawBytes( static_cast<const char*>(buf), count );
   }
   else {
      XX_ASSERT( streamStderr );
      streamStderr->writeRawBytes( static_cast<const char*>(buf), count );
   }
   return count;
}

/*------------------------------------------------------------------------------
 */
void xxdiff_diff_exit( int x )
{
   longjmp( exit_env, 1000 + x );
   // bye bye, internal diff!
}

} // extern "C"

XX_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
//
XxDiffutils::XxDiffutils()
{}

//------------------------------------------------------------------------------
//
XxDiffutils::~XxDiffutils()
{
   if ( _istream ) {
      delete _istream;
   }
}

//------------------------------------------------------------------------------
//
void XxDiffutils::diff( int argc, char** out_args )
{
   diff_fun( argc, out_args, diffutils_diff_main );
}

//------------------------------------------------------------------------------
//
void XxDiffutils::diff3( int argc, char** out_args )
{
   diff_fun( argc, out_args, diffutils_diff3_main );
}

//------------------------------------------------------------------------------
//
void XxDiffutils::diff_fun( 
   int argc,
   char** out_args, 
   int(*main_fun)(int, char**) 
)
{
   QByteArray qs_cout;
   QTextStream oss_cout( qs_cout, IO_WriteOnly );
   QByteArray qs_cerr;
   QTextStream oss_cerr( qs_cerr, IO_WriteOnly );

   streamStdout = & oss_cout;
   streamStderr = & oss_cerr;

   int i = setjmp( exit_env );
   if ( i == 0 ) {
      optind = 0;
      main_fun( argc, out_args );
   }

   oss_cout << flush;
   oss_cerr << flush;
   _outputB = qs_cout /*+ qs_cerr*/;
   _istream = new QTextStream( _outputB, IO_ReadOnly );
}

//------------------------------------------------------------------------------
//
QString XxDiffutils::readLine()
{
   if ( !_istream ) {
      return QString::null;
   }

   return _istream->readLine();
}

XX_NAMESPACE_END


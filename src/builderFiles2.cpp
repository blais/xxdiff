/******************************************************************************\
 * $Id: builderFiles2.cpp 525 2002-02-25 00:17:30Z blais $
 * $Date: 2002-02-24 19:17:30 -0500 (Sun, 24 Feb 2002) $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <builderFiles2.h>
#include <exceptions.h>
#include <diffs.h>
#include <util.h>
#include <diffutils.h>
#include <buffer.h>

#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>

#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#ifndef WINDOWS
#include <sys/wait.h>
#else
#endif

#include <iostream>
#include <sys/types.h>

//#define LOCAL_TRACE
#ifdef LOCAL_TRACE
#define XX_LOCAL_TRACE(x) XX_TRACE(x)
#else
#define XX_LOCAL_TRACE(x)
#endif

#include <memory>

namespace {
XX_NAMESPACE_USING

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

//------------------------------------------------------------------------------
//
bool parseDiffLine( 
   XxLine::Type&  type,
   const QString& line,
   XxFln&         f1n1, 
   XxFln&         f1n2, 
   XxFln&         f2n1, 
   XxFln&         f2n2 
)
{
   /* 
    * this code taken from "ediff.c" by David MacKenzie, a published,
    * uncopyrighted program to translate diff output into plain English 
    */ 
   const char* buf = line.latin1();

   bool error = true;
   if ( ( buf[0] == '<' ) || ( buf[0] == '>' ) || ( buf[0] == '-' ) ) {
      type = XxLine::SAME;
      error = false;
   }
   else if ( sscanf( buf, "%d,%dc%d,%d", &f1n1, &f1n2, &f2n1, &f2n2 ) == 4 ) {
      type = XxLine::DIFF_ALL;
      error = false;
   }
   else if ( sscanf( buf, "%d,%dc%d", &f1n1, &f1n2, &f2n1) == 3 ) {
      f2n2 = f2n1;
      type = XxLine::DIFF_ALL;
      error = false;
   }
   else if ( sscanf( buf, "%dc%d,%d", &f1n1, &f2n1, &f2n2 ) == 3 ) {
      f1n2 = f1n1;
      type = XxLine::DIFF_ALL;
      error = false;
   }
   else if ( sscanf( buf, "%dc%d", &f1n1, &f2n1 ) == 2 ) {
      f2n2 = f2n1;
      f1n2 = f1n1;
      type = XxLine::DIFF_ALL;
      error = false;
   }
   else if ( sscanf( buf, "%d,%dd%d", &f1n1, &f1n2, &f2n1 ) == 3 ) {
      f2n2 = f2n1;
      type = XxLine::INSERT_1;
      error = false;
   }
   else if ( sscanf( buf, "%dd%d", &f1n1, &f2n1 ) == 2 ) {
      f2n2 = f2n1;
      f1n2 = f1n1;
      type = XxLine::INSERT_1;
      error = false;
   }
   else if ( sscanf( buf, "%da%d,%d", &f1n1, &f2n1, &f2n2 ) == 3 ) {
      f1n2 = f1n1;
      type = XxLine::INSERT_2;
      error = false;
   }
   else if ( sscanf( buf, "%da%d", &f1n1, &f2n1 ) == 2 ) {
      f1n2 = f1n1;
      f2n2 = f2n1;
      type = XxLine::INSERT_2;
      error = false;
   }
   else if ( strncmp( buf, "\\ No newline at end of file", 27 ) == 0 ) {
      type = XxLine::DIRECTORIES; /* Ignore this line. */
      error = false;
   }

   return error;
}

#ifndef NO_PARSE_DIFF_ERROR // Not needed for now.

/*==============================================================================
 * CLASS XxParseDiffError
 *============================================================================*/

class XxParseDiffError : public XxError,
                         public std::runtime_error {

public:

   /*----- member functions -----*/

   // Constructor with state.
   XxParseDiffError(
      XX_EXC_PARAMS_DECL(file,line),
      const XxFln fline1, 
      const XxFln fline2, 
      const XxFln f1n1, 
      const XxFln f1n2, 
      const XxFln f2n1, 
      const XxFln f2n2 
   );

};

//------------------------------------------------------------------------------
//
XxParseDiffError::XxParseDiffError(
   XX_EXC_PARAMS_DECL(file,line),
   const XxFln fline1, 
   const XxFln fline2, 
   const XxFln f1n1, 
   const XxFln f1n2, 
   const XxFln f2n1, 
   const XxFln f2n2 
) :
   XxError( file, line ),
   std::runtime_error( "Parse diff output error." )
{
   QTextStream oss( &_msg, IO_WriteOnly | IO_Append );
   oss << "Error parsing diff output: file1:" 
       << fline1 << " (" << f1n1 << "," << f1n2 << ")  file2: " 
       << fline2 << " (" << f2n1 << "," << f2n2 << ")" << endl;
}
#endif

}

XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxBuilderFiles2
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxBuilderFiles2::XxBuilderFiles2( bool useInternalDiff ) :
   XxBuilder(),
   _useInternalDiff( useInternalDiff )
{}

//------------------------------------------------------------------------------
//
XxBuilderFiles2::~XxBuilderFiles2()
{}

//------------------------------------------------------------------------------
//
std::auto_ptr<XxDiffs> XxBuilderFiles2::process( 
   const QString&  command,
   const XxBuffer& buffer1,
   const XxBuffer& buffer2
)
{
   initLines();

//#define XX_INTERNAL_DIFFS

   // Note: only one buffer can be a temporary.
   const char* cstdin = 0;
   uint len;
   if ( buffer1.isTemporary() ) {
      cstdin = buffer1.getTextLine( 1, len );
   }
   else if ( buffer2.isTemporary() ) {
      cstdin = buffer2.getTextLine( 1, len );
   }

   QStringList filenames;
   filenames.append( buffer1.getName() );
   filenames.append( buffer2.getName() );
   const char** out_args;
#ifdef XX_INTERNAL_DIFFS
   int argc = 
#endif
   XxUtil::splitArgs( command, filenames, out_args );

#ifndef XX_INTERNAL_DIFFS
   FILE* fout;
   FILE* ferr;
   XxUtil::spawnCommand( out_args, &fout, &ferr, 0, cstdin );
   if ( fout == 0 || ferr == 0 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }
#else
   std::auto_ptr<XxDiffutils> diffutils( new XxDiffutils );
   diffutils->diff( argc, const_cast<char**>(out_args) );
#endif
   XxUtil::freeArgs( out_args );

   _curHunk = 0;
   XxFln fline1 = 1;
   XxFln fline2 = 1;

   QFile qfout;
   qfout.open( IO_ReadOnly, fout );
   QTextStream outputs( &qfout );

   QTextOStream errors( &_errors );
   XxFln f1n1, f1n2, f2n1, f2n2;

   while ( true ) {
#ifndef XX_INTERNAL_DIFFS
      QString line = outputs.readLine();
      if ( line.isNull() ) {
         break;
      }
#else
      QString line = diffutils->readLine();
      if ( line.isNull() ) {
         break;
      }
#endif

      XxLine::Type type;
      if ( parseDiffLine( type, line, f1n1, f1n2, f2n1, f2n2 ) == true ) {
         XX_LOCAL_TRACE( "ERROR" );
         errors << "Diff error:" << endl;
         errors << line << endl;
         continue;
      }

      switch ( type ) {
         case XxLine::INSERT_1: {
            XX_LOCAL_TRACE( XxLine::mapToString( type ).latin1() );
            XX_LOCAL_TRACE( "Output: f1n1=" << f1n1 << "  f1n2=" << f1n2 <<
                            "  f2n1=" << f2n1 << "  f2n2=" << f2n2 );

	    if ( f1n1 != fline1 ) {
               XxFln fsize = f1n1 - fline1;

               createIgnoreBlock( 
                  fline1, fline2, fsize 
               );
               fline1 += fsize;
               fline2 += fsize;
            }

            int fsize = f1n2 - f1n1 + 1;
            createInsertLeftBlock( fline1, fsize );
            fline1 += fsize;

         } break;

         case XxLine::INSERT_2: {
            XX_LOCAL_TRACE( XxLine::mapToString( type ).latin1() );
            XX_LOCAL_TRACE( "Output: f1n1=" << f1n1 << "  f1n2=" << f1n2 <<
                            "  f2n1=" << f2n1 << "  f2n2=" << f2n2 );

	    if ( f2n1 != fline2 ) {
               int fsize = f2n1 - fline2;

               createIgnoreBlock( 
                  fline1, fline2, fsize 
               );
               fline1 += fsize;
               fline2 += fsize;
            }

            int fsize = f2n2 - f2n1 + 1;
            createInsertRightBlock( fline2, fsize );
            fline2 += fsize;

         } break;

         case XxLine::DIFF_ALL: {
            XX_LOCAL_TRACE( XxLine::mapToString( type ).latin1() );
            XX_LOCAL_TRACE( "Output: f1n1=" << f1n1 << "  f1n2=" << f1n2 <<
                            "  f2n1=" << f2n1 << "  f2n2=" << f2n2 );

            // Sanity check: the number of unchanged lines should be the same.
            // ... *NOT* if we're using the GNU diff option that ignores blank
            // line changes (-B)!  Disabled.
            //
	    // if ( (f1n1 - fline1) != (f2n1 - fline2) ) {
            //    throw XxParseDiffError( 
            //       fline1, fline2, f1n1, f1n2, f2n1, f2n2 
            //    );
            // }

	    if ( f1n1 != fline1 ) {
               int fsize = f1n1 - fline1;

               createIgnoreBlock( 
                  fline1, fline2, fsize 
               );
               fline1 += fsize;
               fline2 += fsize;
            }

            int fsize1 = f1n2 - f1n1 + 1;
            int fsize2 = f2n2 - f2n1 + 1;
            createChangeBlock( fline1, fline2, fsize1, fsize2 );
            fline1 += fsize1;
            fline2 += fsize2;

         } break;

         case XxLine::SAME: {
            XX_LOCAL_TRACE( XxLine::mapToString( type ).latin1() );
         } break;

         /* Used to ignore a line */
         case XxLine::DIRECTORIES: {
         } break;

         case XxLine::DIFF_1:
         case XxLine::DIFF_2:
         case XxLine::DIFF_3:
         case XxLine::DELETE_1:
         case XxLine::DELETE_2:
         case XxLine::DELETE_3:
         case XxLine::INSERT_3:
         case XxLine::DIFFDEL_1:
         case XxLine::DIFFDEL_2:
         case XxLine::DIFFDEL_3: {
         }

         case XxLine::NB_TYPES: {
            XX_ABORT();
         }
      }
   }
   qfout.close();

   // Collect stderr.
   QFile qferr;
   qferr.open( IO_ReadOnly, ferr );
   {
      QTextStream errorss( &qferr );
      QString errstr = errorss.read();
      if ( !errstr.isNull() ) {
         errors << errstr << endl;
      }
   }
   qferr.close();

   // Saved error text.
   errors << flush;
   XX_LOCAL_TRACE( "Errors: " << _errors );

   // If we've read no lines and there are diff errors then blow off
   if ( ( fline1 == 1 ) && ( fline2 == 1 ) && hasErrors() ) {
#if !defined(XX_INTERNAL_DIFFS) && !defined(WINDOWS)
      int stat_loc;
      if ( wait( &stat_loc ) == -1 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }
      _status = (WIFEXITED(stat_loc)) ? (WEXITSTATUS(stat_loc)) : 2;
#else
      _status = 2;
#endif
      throw XxError( XX_EXC_PARAMS, _errors );
   }

   // Add final ignore region if present.
   uint nbRemainingLines = buffer1.getNbLines() + 1 - fline1;
   if ( nbRemainingLines != buffer2.getNbLines() + 1 - fline2 ) {
      throw XxError( XX_EXC_PARAMS, _errors );
   }
   if ( nbRemainingLines > 0 ) { 
      createIgnoreBlock( fline1, fline2, nbRemainingLines );
   }

#if !defined(XX_INTERNAL_DIFFS) && !defined(WINDOWS)
   int stat_loc;
   if ( wait( &stat_loc ) == -1 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }
   _status = (WIFEXITED(stat_loc)) ? (WEXITSTATUS(stat_loc)) : 2;
#else
   _status = 2;
#endif

#ifndef XX_INTERNAL_DIFFS
   ::fclose( fout );
   ::fclose( ferr );
#else
   std::auto_ptr<XxDiffutils> null( 0 );
   diffutils = null;
#endif

   std::auto_ptr<XxDiffs> ap( new XxDiffs( _lines ) );
   return ap;
}

//------------------------------------------------------------------------------
//
void XxBuilderFiles2::createIgnoreBlock( 
   XxFln fline1,
   XxFln fline2,
   uint  fsize
)
{
   for ( uint ii = 0; ii < fsize; ++ii ) {
      XxLine line( XxLine::SAME, fline1 + ii, fline2 + ii );
      line.setHunkId( _curHunk );
      addLine( line );
   }
   _curHunk++;
}

//------------------------------------------------------------------------------
//
void XxBuilderFiles2::createChangeBlock( 
   XxFln fline1,
   XxFln fline2,
   uint  fsize1,
   uint  fsize2
)
{
   // Add common lines.
   uint commonLines = std::min( fsize1, fsize2 ); 
   for ( uint ii = 0; ii < commonLines; ++ii ) {
      XxLine line( XxLine::DIFF_ALL, fline1 + ii, fline2 + ii );
      line.setHunkId( _curHunk );
      addLine( line );
   }

   // Add remaining lines.
   if ( fsize1 > fsize2 ) { 
      uint diffLines = fsize1 - commonLines;
      for ( uint ii = 0; ii < diffLines; ++ii ) {
         XxLine line( XxLine::DIFF_ALL, fline1 + commonLines + ii, -1 );
         line.setHunkId( _curHunk );
         addLine( line );
      }
   }
   else {
      uint diffLines = fsize2 - commonLines;
      for ( uint ii = 0; ii < diffLines; ++ii ) {
         XxLine line( XxLine::DIFF_ALL, -1, fline2 + commonLines + ii );
         line.setHunkId( _curHunk );
         addLine( line );
      }
   }
   _curHunk++;
}

//------------------------------------------------------------------------------
//
void XxBuilderFiles2::createInsertLeftBlock( 
   XxFln fline1,
   uint  fsize
)
{
   for ( uint ii = 0; ii < fsize; ++ii ) {
      XxLine line( XxLine::INSERT_1, fline1 + ii, -1 );
      line.setHunkId( _curHunk );
      addLine( line );
   }
   _curHunk++;
}

//------------------------------------------------------------------------------
//
void XxBuilderFiles2::createInsertRightBlock( 
   XxFln fline2,
   uint  fsize
)
{
   for ( uint ii = 0; ii < fsize; ++ii ) {
      XxLine line( XxLine::INSERT_2, -1, fline2 + ii );
      line.setHunkId( _curHunk );
      addLine( line );
   }
   _curHunk++;
}

XX_NAMESPACE_END

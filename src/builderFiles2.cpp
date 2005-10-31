/******************************************************************************\
 * $Id: builderFiles2.cpp 140 2001-05-22 07:30:19Z blais $
 * $Date: 2001-05-22 03:30:19 -0400 (Tue, 22 May 2001) $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <builderFiles2.h>
#include <exceptions.h>
#include <diffs.h>
#include <util.h>

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>

//#define LOCAL_TRACE
#ifdef LOCAL_TRACE
#define XX_LOCAL_TRACE(x) XX_TRACE(x)
#else
#define XX_LOCAL_TRACE(x)
#endif

namespace {
XX_NAMESPACE_USING

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

//------------------------------------------------------------------------------
//
bool parseDiffLine( 
   XxLine::Type& type,
   const char*   buf, 
   XxFln&        f1n1, 
   XxFln&        f1n2, 
   XxFln&        f2n1, 
   XxFln&        f2n2 
)
{
   /* 
    * this code taken from "ediff.c" by David MacKenzie, a published,
    * uncopyrighted program to translate diff output into plain English 
    */ 

   bool error = true;
   if ( ( buf[0] == '<' ) || ( buf[0] == '>' ) || ( buf[0] == '-' ) ) {
      type = XxLine::SAME;
      error = false;
   }
   else if ( sscanf( buf, "%d,%dc%d,%d\n", &f1n1, &f1n2, &f2n1, &f2n2 ) == 4 ) {
      type = XxLine::DIFF_ALL;
      error = false;
   }
   else if ( sscanf( buf, "%d,%dc%d\n", &f1n1, &f1n2, &f2n1) == 3 ) {
      f2n2 = f2n1;
      type = XxLine::DIFF_ALL;
      error = false;
   }
   else if ( sscanf( buf, "%dc%d,%d\n", &f1n1, &f2n1, &f2n2 ) == 3 ) {
      f1n2 = f1n1;
      type = XxLine::DIFF_ALL;
      error = false;
   }
   else if ( sscanf( buf, "%dc%d\n", &f1n1, &f2n1 ) == 2 ) {
      f2n2 = f2n1;
      f1n2 = f1n1;
      type = XxLine::DIFF_ALL;
      error = false;
   }
   else if ( sscanf( buf, "%d,%dd%d\n", &f1n1, &f1n2, &f2n1 ) == 3 ) {
      f2n2 = f2n1;
      type = XxLine::INSERT_1;
      error = false;
   }
   else if ( sscanf( buf, "%dd%d\n", &f1n1, &f2n1 ) == 2 ) {
      f2n2 = f2n1;
      f1n2 = f1n1;
      type = XxLine::INSERT_1;
      error = false;
   }
   else if ( sscanf( buf, "%da%d,%d\n", &f1n1, &f2n1, &f2n2 ) == 3 ) {
      f1n2 = f1n1;
      type = XxLine::INSERT_2;
      error = false;
   }
   else if ( sscanf( buf, "%da%d\n", &f1n1, &f2n1 ) == 2 ) {
      f1n2 = f1n1;
      f2n2 = f2n1;
      type = XxLine::INSERT_2;
      error = false;
   }
   // else
   return error;
}

/*==============================================================================
 * CLASS XxParseDiffError
 *============================================================================*/

class XxParseDiffError : public std::exception {

public:

   /*----- member functions -----*/

   // Default constructor.
   XxParseDiffError();

   // Constructor with state.
   XxParseDiffError(
      const XxFln fline1, 
      const XxFln fline2, 
      const XxFln f1n1, 
      const XxFln f1n2, 
      const XxFln f2n1, 
      const XxFln f2n2 
   );

   // Destructor.
   virtual ~XxParseDiffError() XX_THROW_NOTHING;

   // See base class.
   virtual const char* what() const XX_THROW_NOTHING;

private:

   /*----- data members -----*/

   std::string _msg;

};

/*==============================================================================
 * CLASS XxParseDiffError
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxParseDiffError::XxParseDiffError()
{
   std::ostringstream oss;
   oss << "Error parsing diff output." << std::ends;
   _msg = oss.str();
}

//------------------------------------------------------------------------------
//
XxParseDiffError::XxParseDiffError(
   const XxFln fline1, 
   const XxFln fline2, 
   const XxFln f1n1, 
   const XxFln f1n2, 
   const XxFln f2n1, 
   const XxFln f2n2 
)
{
   std::ostringstream oss;
   oss << "Error parsing diff output: file1:" 
       << fline1 << " (" << f1n1 << "," << f1n2 << ")  file2: " 
       << fline2 << " (" << f2n1 << "," << f2n2 << ")" << std::endl;
   _msg = oss.str();
}

//------------------------------------------------------------------------------
//
XxParseDiffError::~XxParseDiffError() XX_THROW_NOTHING
{}

//------------------------------------------------------------------------------
//
const char* XxParseDiffError::what() const XX_THROW_NOTHING
{
   return _msg.c_str();
}


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
XxBuilderFiles2::XxBuilderFiles2() :
   XxBuilder()
{}

//------------------------------------------------------------------------------
//
XxBuilderFiles2::~XxBuilderFiles2()
{}


//------------------------------------------------------------------------------
//
std::auto_ptr<XxDiffs> XxBuilderFiles2::process( 
   const char* command,
   const char* path1,
   const uint  nbLines1,
   const char* path2,
   const uint  nbLines2
)
{
   const char* args[3];
   args[0] = path1;
   args[1] = path2;
   args[2] = 0;
   FILE* fp = XxUtil::spawnCommandWithOutput( command, args );
   if ( fp == 0 ) {
      throw new XxIoError;
   }

   _curHunk = 0;
   XxFln fline1 = 1;
   XxFln fline2 = 1;

   std::ostringstream errors;
   char buffer[BUFSIZ+1];
   XxFln f1n1, f1n2, f2n1, f2n2;
   while ( fgets( buffer, BUFSIZ, fp ) != 0 ) {
      XxLine::Type type;
      if ( parseDiffLine( type, buffer, f1n1, f1n2, f2n1, f2n2 ) == true ) {
         XX_LOCAL_TRACE( "ERROR" );
         errors << "Diff error:" << std::endl;
         errors << buffer << std::endl;
         continue;
      }

      switch ( type ) {
         case XxLine::INSERT_1: {
            XX_LOCAL_TRACE( XxLine::mapToString( type ) );
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
            XX_LOCAL_TRACE( XxLine::mapToString( type ) );
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
            XX_LOCAL_TRACE( XxLine::mapToString( type ) );
            XX_LOCAL_TRACE( "Output: f1n1=" << f1n1 << "  f1n2=" << f1n2 <<
                            "  f2n1=" << f2n1 << "  f2n2=" << f2n2 );

            // Sanity check: the number of unchanged lines should be the same.
            // ... *NOT* if we're using the GNU diff option that ignores blank
            // line changes (-B)!  Disabled.
            //
	    // if ( (f1n1 - fline1) != (f2n1 - fline2) ) {
            //    throw new XxParseDiffError( 
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
            XX_LOCAL_TRACE( XxLine::mapToString( type ) );
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
         case XxLine::DIFFDEL_3:
         case XxLine::DIRECTORIES: {
         }

      }
   }

   // Save error text.
   _errors = errors.str();
   XX_LOCAL_TRACE( "Errors" << _errors );

   // If we've read no lines and there are diff errors then blow off
   if ( ( fline1 == 1 ) && ( fline2 == 1 ) && hasErrors() ) {
      int stat_loc;
      if ( wait( &stat_loc ) == -1 ) {
         throw new XxIoError;
      }
      _status = (WIFEXITED(stat_loc)) ? (WEXITSTATUS(stat_loc)) : 2;
      throw new XxIoError;
   }

   // Add final ignore region if present.
   uint nbRemainingLines = nbLines1 + 1 - fline1;
   if ( nbRemainingLines != nbLines2 + 1 - fline2 ) {
      throw new XxIoError;
   }
   if ( nbRemainingLines > 0 ) { 
      createIgnoreBlock( fline1, fline2, nbRemainingLines );
   }

   int stat_loc;
   if ( wait( &stat_loc ) == -1 ) {
      throw new XxIoError;
   }
   _status = (WIFEXITED(stat_loc)) ? (WEXITSTATUS(stat_loc)) : 2;

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


//------------------------------------------------------------------------------
//
void XxBuilderFiles2::addLine( const XxLine& line )
{
   XX_LOCAL_TRACE( "AddLine: " << line );
   _lines.push_back( line );
}


XX_NAMESPACE_END

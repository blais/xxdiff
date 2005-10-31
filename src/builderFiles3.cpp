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

#include <builderFiles3.h>
#include <exceptions.h>
#include <diffs.h>
#include <util.h>
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

namespace {
XX_NAMESPACE_USING

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
      const XxFln f1n1, 
      const XxFln f1n2, 
      const XxFln f2n1, 
      const XxFln f2n2,
      const XxFln f3n1, 
      const XxFln f3n2 
   );

};

//------------------------------------------------------------------------------
//
XxParseDiffError::XxParseDiffError(
   XX_EXC_PARAMS_DECL(file,line),
   XxFln f1n1, 
   XxFln f1n2, 
   XxFln f2n1, 
   XxFln f2n2,
   XxFln f3n1, 
   XxFln f3n2
) :
   XxError( file, line ),
   std::runtime_error( "Parse diff output error." )
{
   QTextStream oss( &_msg, IO_WriteOnly | IO_Append );
   oss << "Error parsing diff3 output:"
       << " (" << f1n1 << "," << f1n2 << ")  file2: " 
       << " (" << f2n1 << "," << f2n2 << ")  file3: " 
       << " (" << f3n1 << "," << f3n2 << ")" << endl;
}

/*==============================================================================
 * LOCAL FUNCTIONS
 *============================================================================*/

//------------------------------------------------------------------------------
//
bool parseDiffLine( 
   XxLine::Type&  type,
   const QString& line, 
   int&           sno, 
   XxFln&         f1n1, 
   XxFln&         f1n2, 
   XxFln&         f2n1, 
   XxFln&         f2n2, 
   XxFln&         f3n1, 
   XxFln&         f3n2 
)
{
   //
   // This hunk parser is based on the following description of the diff3
   // output format:
   //
   //  Detailed Description of `diff3' Normal Format
   //
   //
   //     Each hunk begins with a line marked `===='.  Three-way hunks have
   //  plain `====' lines, and two-way hunks have `1', `2', or `3' appended to
   //  specify which of the three input files differ in that hunk.  The hunks
   //  contain copies of two or three sets of input lines each preceded by one
   //  or two commands identifying where the lines came from.
   //
   //     Normally, two spaces precede each copy of an input line to
   //  distinguish it from the commands.  But with the `-T' or `--initial-tab'
   //  option, `diff3' uses a tab instead of two spaces; this lines up tabs
   //  correctly.  *Note Tabs::, for more information.
   //
   //     Commands take the following forms:
   //
   //  `FILE:La'
   //       This hunk appears after line L of file FILE, and contains no lines
   //       in that file.  To edit this file to yield the other files, one
   //       must append hunk lines taken from the other files.  For example,
   //       `1:11a' means that the hunk follows line 11 in the first file and
   //       contains no lines from that file.
   //
   //  `FILE:Rc'
   //       This hunk contains the lines in the range R of file FILE.  The
   //       range R is a comma-separated pair of line numbers, or just one
   //       number if the range is a singleton.  To edit this file to yield the
   //       other files, one must change the specified lines to be the lines
   //       taken from the other files.  For example, `2:11,13c' means that
   //       the hunk contains lines 11 through 13 from the second file.
   //
   //     If the last line in a set of input lines is incomplete (*note
   //  Incomplete Lines::.), it is distinguished on output from a full line by
   //  a following line that starts with `\'.
   //
   //===========================================================================
   // This means that the ONLY valid combinations are:
   //
   // [diff3 output]                     [text blocks]    [what we call them]
   // --------------------------------------------------------------------------
   //                                     |         |
   // (nothing)                           |  (AAA)  |  SAME
   //                                     |         |
   // ====1    1:...c   2:...c   3:...c   |  (BAA)  |  DIFF_1
   // ====2    1:...c   2:...c   3:...c   |  (ABA)  |  DIFF_2
   // ====3    1:...c   2:...c   3:...c   |  (AAB)  |  DIFF_3
   //                                     |         |
   // ====1    1:...a   2:...c   3:...c   |  (-AA)  |  DELETE_1
   // ====2    1:...c   2:...a   3:...c   |  (A-A)  |  DELETE_2
   // ====3    1:...c   2:...c   3:...a   |  (AA-)  |  DELETE_3
   //                                     |         |
   // ====1    1:...c   2:...a   3:...a   |  (A--)  |  INSERT_1
   // ====2    1:...a   2:...c   3:...a   |  (-A-)  |  INSERT_2
   // ====3    1:...a   2:...a   3:...c   |  (--A)  |  INSERT_3
   //                                     |         |
   // ====     1:...c   2:...c   3:...c   |  (ABC)  |  DIFF_ALL
   //                                     |         |
   // ====     1:...a   2:...c   3:...c   |  (-AB)  |  DIFFDEL_1
   // ====     1:...c   2:...a   3:...c   |  (A-B)  |  DIFFDEL_2
   // ====     1:...c   2:...c   3:...a   |  (AB-)  |  DIFFDEL_3
   //
   // We use the same terminology/type for simpler two-way diffs, so the names
   // we chose have to make sense there too:
   //
   // [diff  output]                     [text blocks]    [what we call them]
   // --------------------------------------------------------------------------
   //                                     |         |
   // (nothing)                           |  (AA )  |  SAME
   //                                     |         |
   // X,YcZ,W or X,YcZ or XcZ,W or XcZ    |  (AB )  |  DIFF_ALL
   //                                     |         |
   // X,YdZ or XdZ                        |  (A- )  |  INSERT_1
   //                                     |         |
   // XaZ,W or XaZ                        |  (-A )  |  INSERT_2
   //
 
#define PARSE_CHECK(xxxx) \
  if ( !(xxxx) ) { \
      throw XxParseDiffError( XX_EXC_PARAMS, \
         f1n1, f1n2, f2n1, f2n2, f3n1, f3n2 \
      ); \
  }

   const char* buf = line.latin1();

   XX_LOCAL_TRACE( "" );
   XX_LOCAL_TRACE( "" );
   XX_LOCAL_TRACE( "" );
   XX_LOCAL_TRACE( "Line: " << buf );

   int r, n1, n2;
   char cbuf[80];
   bool checkForCompletion = false;
   bool error = true;
   type = XxLine::SAME; // Ignore.
   if ( buf[0] == ' ' && buf[1] == ' ' ) {
      XX_LOCAL_TRACE( "recognized text line" );
      error = false;
   }
   else if ( sscanf( buf, "====%d", &sno ) == 1 ) {
      XX_LOCAL_TRACE( "recognized numbered block start" );
      f1n1 = f1n2 = f2n1 = f2n2 = f3n1 = f3n2 = -1;
      error = false;
   }
   else if ( strncmp( buf, "====", 4 ) == 0 ) {
      XX_LOCAL_TRACE( "recognized un-unumbered block start" );
      sno = -1;
      f1n1 = f1n2 = f2n1 = f2n2 = f3n1 = f3n2 = -1;
      error = false;
   }
   else if ( sscanf( buf, "%d:%d,%d%[c]", &r, &n1, &n2, cbuf ) == 4 ) {
      XX_LOCAL_TRACE( "recognized range change" );
      switch ( r ) {
         case 1: {
            f1n1 = n1;
            f1n2 = n2;
         } break;
         case 2: {
            f2n1 = n1;
            f2n2 = n2;
         } break;
         case 3: {
            f3n1 = n1;
            f3n2 = n2;
         } break;
      }
      checkForCompletion = true;
      error = false;
   }
   else if ( sscanf( buf, "%d:%d%[c]", &r, &n1, cbuf ) == 3 ) {
      XX_LOCAL_TRACE( "recognized one-line change" );
      switch ( r ) {
         case 1: {
            f1n1 = n1;
            f1n2 = n1;
         } break;
         case 2: {
            f2n1 = n1;
            f2n2 = n1;
         } break;
         case 3: {
            f3n1 = n1;
            f3n2 = n1;
         } break;
      }
      checkForCompletion = true;
      error = false;
   }
   else if ( sscanf( buf, "%d:%d%[a]", &r, &n1, cbuf ) == 3 ) {
      XX_LOCAL_TRACE( "recognized append" );
      switch ( r ) {
         case 1: {
            f1n1 = n1;
            f1n2 = -1;
         } break;
         case 2: {
            f2n1 = n1;
            f2n2 = -1;
         } break;
         case 3: {
            f3n1 = n1;
            f3n2 = -1;
         } break;
      }
      checkForCompletion = true;
      error = false;
   }
   else if ( strncmp( buf, "\\ No newline at end of file", 27 ) == 0 ) {
      XX_LOCAL_TRACE( "recognized incomplete line marker" );
      error = false;
   }
   else {
      PARSE_CHECK( false );
   }

   if ( checkForCompletion == true ) {
      if ( f1n1 != -1 && f2n1 != -1 && f3n1 != -1 ) {
         // Completed parsing hunk!
         int fsize1 = f1n2 - f1n1;
         int fsize2 = f2n2 - f2n1;
         int fsize3 = f3n2 - f3n1;
         if ( sno == 1 ) {
            if ( fsize1 >= 0 ) { 
               if ( fsize2 >= 0 ) {
                  PARSE_CHECK( fsize3 >= 0 )
                  type = XxLine::DIFF_1;
               }
               else {
                  PARSE_CHECK( fsize3 < 0 )
                  type = XxLine::INSERT_1;
               }
            }
            else {
               PARSE_CHECK( fsize2 >= 0 && fsize3 >= 0 )
               type = XxLine::DELETE_1;
            }
         }
         else if ( sno == 2 ) {
            if ( fsize1 >= 0 ) { 
               if ( fsize2 >= 0 ) {
                  PARSE_CHECK( fsize3 >= 0 )
                  type = XxLine::DIFF_2;
               }
               else {
                  PARSE_CHECK( fsize3 >= 0 )
                  type = XxLine::DELETE_2;
               }
            }
            else {
               PARSE_CHECK( fsize2 >= 0 && fsize3 < 0 )
               type = XxLine::INSERT_2;
            }
         }
         else if ( sno == 3 ) {
            if ( fsize1 >= 0 ) { 
               if ( fsize3 >= 0 ) {
                  PARSE_CHECK( fsize2 >= 0 )
                  type = XxLine::DIFF_3;
               }
               else {
                  PARSE_CHECK( fsize2 >= 0 )
                  type = XxLine::DELETE_3;
               }
            }
            else {
               PARSE_CHECK( fsize2 < 0 && fsize3 >= 0 )
               type = XxLine::INSERT_3;
            }
         }
         else if ( sno == -1 ) {
            if ( fsize1 < 0 ) { 
               PARSE_CHECK( fsize2 >= 0 && fsize3 >= 0 )
               type = XxLine::DIFFDEL_1;
            }
            else if ( fsize2 < 0 ) { 
               PARSE_CHECK( fsize1 >= 0 && fsize3 >= 0 )
               type = XxLine::DIFFDEL_2;
            }
            else if ( fsize3 < 0 ) { 
               PARSE_CHECK( fsize1 >= 0 && fsize2 >= 0 )
               type = XxLine::DIFFDEL_3;
            }
            else {
               type = XxLine::DIFF_ALL;
            }
         }
         else {
            PARSE_CHECK( false );
         }
      }
   }

   return error;
}

}

XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxBuilderFiles3
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxBuilderFiles3::XxBuilderFiles3() :
   XxBuilder()
{}

//------------------------------------------------------------------------------
//
XxBuilderFiles3::~XxBuilderFiles3()
{}


//------------------------------------------------------------------------------
//
std::auto_ptr<XxDiffs> XxBuilderFiles3::process( 
   const QString&  command,
   const XxBuffer& buffer1,
   const XxBuffer& buffer2,
   const XxBuffer& buffer3
)
{
   initLines();

   // Note: only one buffer can be a temporary.
   const char* cstdin = 0;
   uint len;
   if ( buffer1.isTemporary() ) {
      cstdin = buffer1.getTextLine( 1, len );
   }
   else if ( buffer2.isTemporary() ) {
      cstdin = buffer2.getTextLine( 1, len );
   }
   else if ( buffer3.isTemporary() ) {
      cstdin = buffer3.getTextLine( 1, len );
   }

   QStringList filenames;
   filenames.append( buffer1.getName() );
   filenames.append( buffer2.getName() );
   filenames.append( buffer3.getName() );
   const char** out_args;
   XxUtil::splitArgs( command, filenames, out_args );

   FILE* fout;
   FILE* ferr;
   XxUtil::spawnCommand( out_args, &fout, &ferr, 0, cstdin );
   if ( fout == 0 || ferr == 0 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }
   XxUtil::freeArgs( out_args );

   _curHunk = 0;
   XxFln fline1 = 1;
   XxFln fline2 = 1;
   XxFln fline3 = 1;

   bool foundDifferences = false;
   QTextOStream errors( &_errors );
   int sno;
   XxFln f1n1, f1n2, f2n1, f2n2, f3n1, f3n2;

   QFile qfout;
   qfout.open( IO_ReadOnly, fout );
   QTextStream outputs( &qfout );

   while ( true ) {
      QString line = outputs.readLine();
      if ( line.isNull() ) {
         break;
      }

      XxLine::Type type;
      if ( parseDiffLine( type, line,
                          sno, f1n1, f1n2, f2n1, f2n2, f3n1, f3n2 ) == true ) {
         XX_LOCAL_TRACE( "ERROR" );
         errors << "Diff error:" << endl;
         errors << line << endl;
         continue;
      }

#ifdef XX_DEBUG
      XX_LOCAL_TRACE( "ParseDiffLine results: " );
      XX_LOCAL_TRACE( XxLine::mapToString( type ).latin1() );
      XX_LOCAL_TRACE( "  sno=" << sno );
      XX_LOCAL_TRACE( "  f1n1=" << f1n1 << "  f1n2=" << f1n2 );
      XX_LOCAL_TRACE( "  f2n1=" << f2n1 << "  f2n2=" << f2n2 );
      XX_LOCAL_TRACE( "  f3n1=" << f3n1 << "  f3n2=" << f3n2 );
#endif
      
      if ( type != XxLine::SAME ) {
         foundDifferences = true;
         int fsize1 = f1n1 - fline1;
         int fsize2 = f2n1 - fline2;
         int fsize3 = f3n1 - fline3;
         fsize1 += (f1n2 < 0) ? 1 : 0; // add one for append cases.
         fsize2 += (f2n2 < 0) ? 1 : 0;
         fsize3 += (f3n2 < 0) ? 1 : 0;
         XX_LOCAL_TRACE( "fline1  " << fline1 << "   fsize1  " << fsize1 );
         XX_LOCAL_TRACE( "fline2  " << fline2 << "   fsize2  " << fsize2 );
         XX_LOCAL_TRACE( "fline3  " << fline3 << "   fsize3  " << fsize3 );
         PARSE_CHECK( fsize1 == fsize2 && fsize2 == fsize3 );

         // Create possible SAME region.
         if ( fsize1 > 0 ) {
            createIgnoreBlock( fline1, fline2, fline3, fsize1 );
            fline1 += fsize1;
            fline2 += fsize1;
            fline3 += fsize1;
         }

         // fsize1 = std::max( 0, f1n2 - f1n1 );
         // fsize2 = std::max( 0, f2n2 - f2n1 );
         // fsize3 = std::max( 0, f3n2 - f3n1 );

         // We can use this instead here, since there will always be one of the
         // three sizes > 0.
         fsize1 = (f1n2 < 0) ? 0 : f1n2 - f1n1;
         fsize2 = (f2n2 < 0) ? 0 : f2n2 - f2n1;
         fsize3 = (f3n2 < 0) ? 0 : f3n2 - f3n1;

         int maxsize = std::max( fsize1, std::max( fsize2, fsize3 ) ) + 1;
         XX_LOCAL_TRACE( "maxsize=" << maxsize );
         XX_ASSERT( maxsize > 0 );

         int n1, n2, n3;
         for ( int ii = 0;  ii < maxsize; ++ii ) {
            n1 = n2 = n3 = -1;
            if ( fline1 <= f1n2 ) {
               n1 = fline1++;
            }
            if ( fline2 <= f2n2 ) {
               n2 = fline2++;
            }
            if ( fline3 <= f3n2 ) {
               n3 = fline3++;
            }

            XxLine line( type, n1, n2, n3 );
            line.setHunkId( _curHunk );
            addLine( line );
         }
         _curHunk++;
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
   if ( ( fline1 == 1 ) && ( fline2 == 1 ) && ( fline3 == 1 ) &&
        hasErrors() ) {
#ifndef WINDOWS
      int stat_loc;
      if ( wait( &stat_loc ) == -1 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }
      _status = (WIFEXITED(stat_loc)) ? (WEXITSTATUS(stat_loc)) : 2;
      throw XxError( XX_EXC_PARAMS, _errors );
#else
      _status = 2;
#endif
   }

   // Add final ignore region if present.
   uint nbRemainingLines = buffer1.getNbLines() + 1 - fline1;
   if ( nbRemainingLines != buffer2.getNbLines() + 1 - fline2 ||
        nbRemainingLines != buffer3.getNbLines() + 1 - fline3 ) {
      throw XxError( XX_EXC_PARAMS, _errors );
   }
   if ( nbRemainingLines > 0 ) { 
      createIgnoreBlock( fline1, fline2, fline3, int(nbRemainingLines) );
   }

#ifndef WINDOWS
   int stat_loc;
   if ( wait( &stat_loc ) == -1 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }
   _status = (WIFEXITED(stat_loc)) ? (WEXITSTATUS(stat_loc)) : 2;
#else
   _status = 2;
#endif
   // Fix for deficient non-GNU diff3.
   if ( _status == 0 && foundDifferences == true ) {
      _status = 1;
   }

   ::fclose( fout );
   ::fclose( ferr );

   std::auto_ptr<XxDiffs> ap( new XxDiffs( _lines ) );
   return ap;
}

//------------------------------------------------------------------------------
//
void XxBuilderFiles3::createIgnoreBlock( 
   XxFln fline1,
   XxFln fline2,
   XxFln fline3,
   uint  fsize
)
{
   for ( uint ii = 0; ii < fsize; ++ii ) {
      XxLine line( XxLine::SAME, fline1 + ii, fline2 + ii, fline3 + ii );
      line.setHunkId( _curHunk );
      addLine( line );
   }
   _curHunk++;
}

XX_NAMESPACE_END

/******************************************************************************\
 * $Id: builderUnmerge.cpp 519 2002-02-23 17:43:56Z blais $
 * $Date: 2002-02-23 12:43:56 -0500 (Sat, 23 Feb 2002) $
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

#include <builderUnmerge.h>
#include <diffs.h>
#include <buffer.h>
#include <resources.h>

#include <qcstring.h>
#include <qtextstream.h>

#include <stdio.h>
#include <iostream>

// #define LOCAL_TRACE
#ifdef LOCAL_TRACE
#define XX_LOCAL_TRACE(x) XX_TRACE(x)
#else
#define XX_LOCAL_TRACE(x)
#endif

#include <memory>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxBuilderUnmerge
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxBuilderUnmerge::XxBuilderUnmerge() :
   XxBuilder()
{}

//------------------------------------------------------------------------------
//
XxBuilderUnmerge::~XxBuilderUnmerge()
{}

//------------------------------------------------------------------------------
//
// FIXME grab information from the tags to set the display names.

std::auto_ptr<XxDiffs> XxBuilderUnmerge::process(
   const XxBuffer&    buffer,
   const XxResources& resources,
   QString&           outFileLeft,
   QString&           outFileRight
)
{
   initLines();

   // Note: starting from qt-3, QRegExp will have group matching, so we could
   // have the tags specified using reg.exps instead of strings.

   enum InConflict { OUTSIDE, IN1, IN2 };
   InConflict inConflict = OUTSIDE;

   QTextOStream errors( &_errors );

   XxFln f1n1 = -1;
   XxFln f1n2 = -1;
   XxFln f2n1 = -1;
   XxFln f2n2 = -1;
   f1n1 = 1;

   QString tagStart = resources.getTag( TAG_UNMERGE_START );
   QString tagSep = resources.getTag( TAG_UNMERGE_SEP );
   QString tagEnd = resources.getTag( TAG_UNMERGE_END );

//    // Prepare name buffers.
//    char* outStart = 0;
//    char* outSep = 0;
//    char* outEnd = 0;
//    int nbExpStart = 0;
//    int nbExpSep = 0;
//    int nbExpEnd = 0;
//    QRegExp leftRE( "%L" );
//    QRegExp rightRE( "%R" );

//    // Clean tags of garbage that scanf could interpret maliciously.
//    QRegExp otherRE( "%[^LR]" );
//    tagStart.replace( otherRE, "" );
//    tagSep.replace( otherRE, "" );
//    tagEnd.replace( otherRE, "" );

//    const QString sin = "%s";

//    // Setup output variables.
//    if ( tagStart.find( leftRE ) != -1 ) {
//       tagStart.replace( leftRE, sin );
//       outStart = bufleft;
//       ++nbExpStart;
//    }
//    else if ( tagStart.find( rightRE ) != -1 ) {
//       tagStart.replace( rightRE, sin );
//       outStart = bufright;
//       ++nbExpStart;
//    }

//    if ( tagSep.find( leftRE ) != -1 ) {
//       tagSep.replace( leftRE, sin );
//       outSep = bufleft;
//       ++nbExpSep;
//    }
//    else if ( tagSep.find( rightRE ) != -1 ) {
//       tagSep.replace( rightRE, sin );
//       outSep = bufright;
//       ++nbExpSep;
//    }

//    if ( tagEnd.find( leftRE ) != -1 ) {
//       tagEnd.replace( leftRE, sin );
//       outEnd = bufleft;
//       ++nbExpEnd;
//    }
//    else if ( tagEnd.find( rightRE ) != -1 ) {
//       tagEnd.replace( rightRE, sin );
//       outEnd = bufright;
//       ++nbExpEnd;
//    }

   XX_LOCAL_TRACE( "start tag: " << tagStart /*<< " " << nbExpStart*/ );
   XX_LOCAL_TRACE( "sep tag: " << tagSep /*<< " " << nbExpSep*/ );
   XX_LOCAL_TRACE( "end tag: " << tagEnd /*<< " " << nbExpEnd*/ );

   QCString fileLeft;
   QCString fileRight;

   int nbLines = buffer.getNbLines();
   XX_LOCAL_TRACE( "nbLines " << nbLines );
   for ( XxFln l = 1; l <= nbLines; ++l ) {
      uint len;
      const char* textline = buffer.getTextLine( l, len );

      if ( inConflict == OUTSIDE ) {
         XX_LOCAL_TRACE( sscanf( textline, tagStart ) );

         if ( qstrncmp( textline, tagStart, tagStart.length() ) == 0 ) {
            XX_LOCAL_TRACE( "f1n1, l - f1n1 " << f1n1 << " " << l - f1n1 );
            XX_CHECK( l - f1n1 >= 0 );
            createIgnoreBlock( f1n1, f1n1, l - f1n1 );
            f1n1 = l + 1;
            inConflict = IN1;

            // Grab string after opening tag, if present.
            // Note that we only take the first such tag to appear in the file.
            if ( len > tagStart.length() && fileLeft.isEmpty() ) {
               fileLeft = QCString( &textline[tagStart.length()],
                                    len - tagStart.length() + 1 );
               fileLeft = fileLeft.stripWhiteSpace();
            }
         }
         else {
            // Do nothing, accumulate.
         }
      }
      else if ( inConflict == IN1 ) {
         if ( qstrncmp( textline, tagSep, tagSep.length() ) == 0 ) {
            f1n2 = l;
            f2n1 = l + 1;
            inConflict = IN2;
         }
         else {
            // Do nothing, accumulate.
         }
      }
      else if ( inConflict == IN2 ) {
         if ( qstrncmp( textline, tagEnd, tagEnd.length() ) == 0 ) {
            f2n2 = l;
            int fsize1 = f1n2 - f1n1;
            int fsize2 = f2n2 - f2n1;
            XX_CHECK( fsize1 >= 0 && fsize2 >= 0 );
            XX_LOCAL_TRACE( "f1n1, f2n1, fsize1, fsize2 " 
                            << f1n1 << " " << f2n1 << " "
                            << fsize1 << " " << fsize2 );
            if ( fsize1 == 0 && fsize2 == 0 ) {
               errors << "Warning: empty change at line " << (f1n1 - 1)
                      << " in file with conflicts." << endl;
            }
            if ( fsize1 == 0 ) {
               createInsertRightBlock( f2n1, fsize2 );
            }
            else if ( fsize2 == 0 ) {
               createInsertLeftBlock( f1n1, fsize1 );
            }
            else {
               createChangeBlock( f1n1, f2n1, fsize1, fsize2 );
            }

            f1n1 = l + 1;
            inConflict = OUTSIDE;

            // Grab string after opening tag, if present.
            // Note that we only take the first such tag to appear in the file.
            if ( len > tagEnd.length() && fileRight.isEmpty() ) {
               fileRight = QCString( &textline[tagEnd.length()],
                                     len - tagEnd.length() + 1 );
               fileRight = fileRight.stripWhiteSpace();
            }
         }
         else {
            // Do nothing, accumulate.
         }
      }

   }
   
   // Add final ignore region if present.
   if ( inConflict == OUTSIDE && f1n1 <= nbLines ) {
      createIgnoreBlock( f1n1, f1n1, nbLines + 1 - f1n1 );
   }
   else if ( inConflict == IN1 ) {
      /*--_curHunk; // Extend current hunk.*/ // (it works anyway)
      createInsertLeftBlock( f1n1, nbLines + 1 - f1n1 );
      errors << "Warning: file ends inside change at line " << (f1n1 - 1)
             << endl;
   }
   else if ( inConflict == IN2 ) {
      int fsize1 = f1n2 - f1n1;
      int fsize2 = nbLines + 1 - f2n1;
      if ( fsize1 == 0 && fsize2 == 0 ) {
         // Don't bother... we'll be warning anyway.
      }
      if ( fsize1 == 0 ) {
         createInsertRightBlock( f2n1, fsize2 );
      }
      else if ( fsize2 == 0 ) {
         createInsertLeftBlock( f1n1, fsize1 );
      }
      else {
         createChangeBlock( f1n1, f2n1, fsize1, fsize2 );
      }
      errors << "Warning: file ends inside change at line " << (f1n1 - 1)
             << endl;
   }

   XX_LOCAL_TRACE( "DONE" );

   // Saved error text.
   errors << flush;
   XX_LOCAL_TRACE( "Errors: " << _errors );

   XX_LOCAL_TRACE( "Left: " << fileLeft );
   XX_LOCAL_TRACE( "Right: " << fileRight );

   outFileLeft = fileLeft;
   outFileRight = fileRight;
   std::auto_ptr<XxDiffs> ap( new XxDiffs( _lines, false, false ) );
   return ap;
}

//------------------------------------------------------------------------------
//
void XxBuilderUnmerge::createIgnoreBlock( 
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
void XxBuilderUnmerge::createChangeBlock( 
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
void XxBuilderUnmerge::createInsertLeftBlock( 
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
void XxBuilderUnmerge::createInsertRightBlock( 
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

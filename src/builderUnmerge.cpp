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

#include <builderUnmerge.h>
#include <diffs.h>
#include <buffer.h>
#include <resources.h>

#include <qcstring.h>
#include <qtextstream.h>
#include <qregexp.h>

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

namespace {
XX_NAMESPACE_USING

/*==============================================================================
 * CLASS XxConflictFormatError
 *============================================================================*/

class XxConflictFormatError : public XxError, 
                              public std::runtime_error {

public:

   /*----- member functions -----*/

   // Constructor with state.
   XxConflictFormatError(
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
XxConflictFormatError::XxConflictFormatError(
   XX_EXC_PARAMS_DECL(file,line),
   XxFln f1n1, 
   XxFln f1n2,
   XxFln f2n1, 
   XxFln f2n2,
   XxFln f3n1, 
   XxFln f3n2
) :
   XxError( file, line ),
   std::runtime_error( "Conflicts format error." )
{
   QTextStream oss( &_msg, IO_WriteOnly | IO_Append );
   oss << "Error parsing conflicts file:"
       << " (" << f1n1 << "," << f1n2 << ")  file2: " 
       << " (" << f2n1 << "," << f2n2 << ")  file3: " 
       << " (" << f3n1 << "," << f3n2 << ")" << endl;
}

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

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

}

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

   enum InConflict { OUTSIDE = 0, IN1 = 1, IN2 = 2 };
   InConflict inConflict = OUTSIDE;

   QTextOStream errors( &_errors );

   XxFln f1n1 = -1;
   XxFln f1n2 = -1;
   XxFln f2n1 = -1;
   XxFln f2n2 = -1;
   f1n1 = 1;

   QRegExp reStart = QRegExp( resources.getTag( TAG_UNMERGE_START ) );
   QRegExp reSep = QRegExp( resources.getTag( TAG_UNMERGE_SEP ) );
   QRegExp reEnd = QRegExp( resources.getTag( TAG_UNMERGE_END ) );
   if ( !reStart.isValid() ) {
      QString str;
      QTextOStream oss( &str );
      oss << "TAG_UNMERGE_START is invalid: " << reStart.pattern() << endl;
      throw XxError( XX_EXC_PARAMS, str );
   }
   if ( !reSep.isValid() ) {
      QString str;
      QTextOStream oss( &str );
      oss << "TAG_UNMERGE_SEP is invalid: " << reSep.pattern() << endl;
      throw XxError( XX_EXC_PARAMS, str );
   }
   if ( !reEnd.isValid() ) {
      QString str;
      QTextOStream oss( &str );
      oss << "TAG_UNMERGE_END is invalid: " << reEnd.pattern() << endl;
      throw XxError( XX_EXC_PARAMS, str );
   }

   XX_LOCAL_TRACE( "start re: " << reStart.pattern() /*<< " " << nbExpStart*/ );
   XX_LOCAL_TRACE( "sep re: " << reSep.pattern() /*<< " " << nbExpSep*/ );
   XX_LOCAL_TRACE( "end re: " << reEnd.pattern() /*<< " " << nbExpEnd*/ );

   int nbLines = buffer.getNbLines();
   XX_LOCAL_TRACE( "nbLines " << nbLines );
   for ( XxFln l = 1; l <= nbLines; ++l ) {
      uint len;
      const char* textlineOrig = buffer.getTextLine( l, len );
      QCString textline( textlineOrig, len+1 ); // This copy sucks...

      if ( inConflict == OUTSIDE ) {
         XX_LOCAL_TRACE( "===> " << reStart.pattern() << " " << textline );
         if ( reStart.search( textline ) != -1 ) {
            XX_LOCAL_TRACE( "f1n1, l - f1n1 " << f1n1 << " " << l - f1n1 );
            XX_CHECK( l - f1n1 >= 0 );
            createIgnoreBlock( f1n1, f1n1, l - f1n1 );
            f1n1 = l + 1;
            inConflict = IN1;

            // Grab string after opening tag, if present.
            // Note that we only take the first such tag to appear in the file.
            QStringList texts = reStart.capturedTexts();
            if ( texts.size() > 0 ) {
               outFileLeft = texts.front().stripWhiteSpace();
            }
         }
         else {
            // Do nothing, accumulate.
         }
      }
      else if ( inConflict == IN1 ) {
         if ( reSep.search( textline ) != -1 ) {
            f1n2 = l;
            f2n1 = l + 1;
            inConflict = IN2;
         }
         else {
            // Do nothing, accumulate.
         }
      }
      else if ( inConflict == IN2 ) {
         if ( reEnd.search( textline ) != -1 ) {
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

            f1n2 = f2n1 = f2n2 = -1;
            f1n1 = l + 1;
            inConflict = OUTSIDE;

            // Grab string after opening tag, if present.
            // Note that we only take the first such tag to appear in the file.
            QStringList texts = reEnd.capturedTexts();
            if ( texts.size() > 0 ) {
               outFileRight = texts.front().stripWhiteSpace();
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

   XX_LOCAL_TRACE( "Left: " << outFileLeft );
   XX_LOCAL_TRACE( "Right: " << outFileRight );

   std::auto_ptr<XxDiffs> ap( new XxDiffs( _lines, false, false ) );
   
   return ap;
}

//------------------------------------------------------------------------------
//
std::auto_ptr<XxDiffs> XxBuilderUnmerge::process(
   const XxBuffer&    buffer,
   const XxResources& resources,
   QString&           outFileLeft,
   QString&           outFileMiddle,
   QString&           outFileRight
)
{
   initLines();

   // Note: starting from qt-3, QRegExp will have group matching, so we could
   // have the tags specified using reg.exps instead of strings.

   enum InConflict { OUTSIDE = 0, IN1 = 1, IN2 = 2, IN3 = 3 };
   InConflict inConflict = OUTSIDE;

   QTextOStream errors( &_errors );

   XxFln f1n1 = -1;
   XxFln f1n2 = -1;
   XxFln f2n1 = -1;
   XxFln f2n2 = -1;
   XxFln f3n1 = -1;
   XxFln f3n2 = -1;
   f1n1 = 1;

   QRegExp reStart = QRegExp( resources.getTag( TAG_UNMERGE_START ) );
   QRegExp reSep1 = QRegExp( resources.getTag( TAG_UNMERGE_SEP_EXTRA ) );
   QRegExp reSep2 = QRegExp( resources.getTag( TAG_UNMERGE_SEP ) );
   QRegExp reEnd = QRegExp( resources.getTag( TAG_UNMERGE_END ) );
   if ( !reStart.isValid() ) {
      QString str;
      QTextOStream oss( &str );
      oss << "TAG_UNMERGE_START is invalid: " << reStart.pattern() << endl;
      throw XxError( XX_EXC_PARAMS, str );
   }
   if ( !reSep1.isValid() ) {
      QString str;
      QTextOStream oss( &str );
      oss << "TAG_UNMERGE_SEP_EXTRA is invalid: " << reSep1.pattern() << endl;
      throw XxError( XX_EXC_PARAMS, str );
   }
   if ( !reSep2.isValid() ) {
      QString str;
      QTextOStream oss( &str );
      oss << "TAG_UNMERGE_SEP is invalid: " << reSep2.pattern() << endl;
      throw XxError( XX_EXC_PARAMS, str );
   }
   if ( !reEnd.isValid() ) {
      QString str;
      QTextOStream oss( &str );
      oss << "TAG_UNMERGE_END is invalid: " << reEnd.pattern() << endl;
      throw XxError( XX_EXC_PARAMS, str );
   }

   XX_LOCAL_TRACE( "start re: " << reStart.pattern() /*<< " " << nbExpStart*/ );
   XX_LOCAL_TRACE( "sep1 re: " << reSep1.pattern() /*<< " " << nbExpSep*/ );
   XX_LOCAL_TRACE( "sep2 re: " << reSep2.pattern() /*<< " " << nbExpSep*/ );
   XX_LOCAL_TRACE( "end re: " << reEnd.pattern() /*<< " " << nbExpEnd*/ );

   QString fileFirst;

   int nbLines = buffer.getNbLines();
   XX_LOCAL_TRACE( "nbLines " << nbLines );
   for ( XxFln l = 1; l <= nbLines; ++l ) {

      uint len;
      const char* textlineOrig = buffer.getTextLine( l, len );
      QCString textline( textlineOrig, len+1 ); // This copy sucks...
      XX_LOCAL_TRACE( "<" << inConflict << "> " << 
                      l << " textline = " << textline );

      if ( inConflict == OUTSIDE ) {
         if ( reStart.search( textline ) != -1 ) {
            XX_LOCAL_TRACE( "f1n1, l - f1n1 " << f1n1 << " " << l - f1n1 );
            XX_CHECK( l - f1n1 >= 0 );
            createIgnoreBlock( f1n1, f1n1, f1n1, l - f1n1 );
            f1n1 = l + 1;
            inConflict = IN1;

            // Grab string after opening tag, if present.
            // Note that we only take the first such tag to appear in the file.
            QStringList texts = reStart.capturedTexts();
            if ( texts.size() > 0 ) {
               fileFirst = texts.front().stripWhiteSpace();
            }
         }
         else {
            // Do nothing, accumulate.
         }
      }
      else if ( inConflict == IN1 ) {
         if ( reSep1.search( textline ) != -1 ) {
            f1n2 = l;
            f2n1 = l + 1;
            inConflict = IN2;

            // Grab string after tag, if present.
            // Note that we only take the first such tag to appear in the file.
            QStringList texts = reSep1.capturedTexts();
            if ( texts.size() > 0 ) {
               outFileMiddle = texts.front().stripWhiteSpace();
            }
         }
         else if ( reSep2.search( textline ) != -1 ) {
            f1n2 = l;
            f3n1 = l + 1;
            inConflict = IN3;
         }
         else {
            // Do nothing, accumulate.
         }
      }
      else if ( inConflict == IN2 ) {
         if ( reSep2.search( textline ) != -1 ) {
            f2n2 = l;
            f3n1 = l + 1;
            inConflict = IN3;
         }
         else {
            // Do nothing, accumulate.
         }
      }
      else if ( inConflict == IN3 ) {
         if ( reEnd.search( textline ) != -1 ) {
            f3n2 = l;
            int fsize1 = f1n2 - f1n1;
            int fsize2 = f2n2 - f2n1;
            int fsize3 = f3n2 - f3n1;
            XX_CHECK( fsize1 >= 0 && fsize2 >= 0 && fsize3 >= 0 );
            XX_LOCAL_TRACE( "f1n1, f1n2, f2n1, f2n2, f3n1, f3n2 " 
                            << f1n1 << " " << f1n2 << " "
                            << f2n1 << " " << f2n2 << " "
                            << f3n1 << " " << f3n2 );
            if ( fsize1 == 0 && fsize2 == 0 && fsize3 == 0 ) {
               errors << "Warning: empty change at line " << (f1n1 - 1)
                      << " in file with conflicts." << endl;
            }
            else if ( f2n1 == -1 ) {
               //
               // This was a two-region conflict.
               //
               if ( !fileFirst.isEmpty() && outFileMiddle.isEmpty() ) {
                  outFileMiddle = fileFirst;
               }

               if ( fsize1 == 0 ) {
                  for ( int ii = 0; ii < fsize3; ++ii ) {
                     XxLine line( XxLine::DELETE_2, f3n1 + ii, -1, f3n1 + ii );
                     line.setHunkId( _curHunk );
                     addLine( line );
                  }
               }
               else if ( fsize3 == 0 ) {
                  for ( int ii = 0; ii < fsize1; ++ii ) {
                     XxLine line( XxLine::INSERT_2, -1, f1n1 + ii, -1 );
                     line.setHunkId( _curHunk );
                     addLine( line );
                  }
               }
               else { // if ( fsize1 > 0 && fsize3 > 0 )
                  for ( int ii = 0; ii < fsize1; ++ii ) {
                     XxLine line( XxLine::DIFF_2,
                                  f3n1 + ii, f1n1 + ii, f3n1 + ii );
                     line.setHunkId( _curHunk );
                     addLine( line );
                  }
               }
               _curHunk++;
            }
            else {
               //
               // This was a three-region conflict.
               //
               if ( !fileFirst.isEmpty() && outFileLeft.isEmpty() ) {
                  outFileLeft = fileFirst;
               }

               if ( fsize1 > 0 && fsize2 > 0 && fsize3 > 0 ) {
                  create3Block(
                     XxLine::DIFF_ALL, f1n1, f1n2, f2n1, f2n2, f3n1, f3n2
                  );
               }
               else if ( fsize1 == 0 ) {
                  if ( fsize2 == 0 || fsize3 == 0 ) {
                     throw XxConflictFormatError(
                        XX_EXC_PARAMS, f1n1, f1n2, f2n1, f2n2, f3n1, f3n2
                     );
                  }
                  create3Block(
                     XxLine::DIFFDEL_1, f1n1, f1n2, f2n1, f2n2, f3n1, f3n2
                  );
               }
               else if ( fsize2 == 0 ) {
                  if ( fsize1 == 0 || fsize3 == 0 ) {
                     throw XxConflictFormatError(
                        XX_EXC_PARAMS, f1n1, f1n2, f2n1, f2n2, f3n1, f3n2
                     );
                  }
                  create3Block(
                     XxLine::DIFFDEL_2, f1n1, f1n2, f2n1, f2n2, f3n1, f3n2
                  );
               }
               else if ( fsize3 == 0 ) {
                  if ( fsize1 == 0 || fsize2 == 0 ) {
                     throw XxConflictFormatError(
                        XX_EXC_PARAMS, f1n1, f1n2, f2n1, f2n2, f3n1, f3n2
                     );
                  }
                  create3Block(
                     XxLine::DIFFDEL_3, f1n1, f1n2, f2n1, f2n2, f3n1, f3n2
                  );
               }
               else {
                  XX_ASSERT( false );
               }
               _curHunk++;
            }

            f1n2 = f2n1 = f2n2 = f3n1 = f3n2 = -1;
            f1n1 = l + 1;
            inConflict = OUTSIDE;

            // Grab string after opening tag, if present.
            // Note that we only take the first such tag to appear in the file.
            QStringList texts = reEnd.capturedTexts();
            if ( texts.size() > 0 ) {
               outFileRight = texts.front().stripWhiteSpace();
            }
         }
         else {
            // Do nothing, accumulate.
         }
      }

   }
   
   // Warn if we ended up in an unfinished region.
   if ( inConflict != OUTSIDE ) {
      createIgnoreBlock( f1n1 - 1, f1n1 - 1, f1n1 - 1, nbLines + 1 - (f1n1-1) );

      errors << "Warning: file ends inside change at line " << (f1n1 - 1)
             << endl;
   }
   else if ( f1n1 <= nbLines ) {
      // Add final ignore region if present.
      createIgnoreBlock( f1n1, f1n1, f1n1, nbLines + 1 - f1n1 );
   }

   XX_LOCAL_TRACE( "DONE" );

   // Saved error text.
   errors << flush;
   XX_LOCAL_TRACE( "Errors: " << _errors );

   XX_LOCAL_TRACE( "Left: " << outFileLeft );
   XX_LOCAL_TRACE( "Middle: " << outFileMiddle );
   XX_LOCAL_TRACE( "Right: " << outFileRight );

   outFileLeft = outFileLeft;
   outFileMiddle = outFileMiddle;
   outFileRight = outFileRight;
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
void XxBuilderUnmerge::createIgnoreBlock( 
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


//------------------------------------------------------------------------------
//
void XxBuilderUnmerge::create3Block( 
   XxLine::Type type,
   XxFln        f1n1,
   XxFln        f1n2,
   XxFln        f2n1,
   XxFln        f2n2,
   XxFln        f3n1,
   XxFln        f3n2
)
{
   int fsize1 = f1n2 - f1n1;
   int fsize2 = f2n2 - f2n1;
   int fsize3 = f3n2 - f3n1;

   int maxsize = std::max( fsize1, std::max( fsize2, fsize3 ) ); // FIXME + 1
   XX_LOCAL_TRACE( "maxsize=" << maxsize );
   XX_ASSERT( maxsize > 0 );

   int n1, n2, n3;
   for ( int ii = 0;  ii < maxsize; ++ii ) {
      n1 = n2 = n3 = -1;
      if ( f1n1 < f1n2 ) {
         n1 = f1n1++;
      }
      if ( f2n1 < f2n2 ) {
         n2 = f2n1++;
      }
      if ( f3n1 < f3n2 ) {
         n3 = f3n1++;
      }

      XxLine line( type, n1, n2, n3 );
      line.setHunkId( _curHunk );
      addLine( line );
   }
}

XX_NAMESPACE_END

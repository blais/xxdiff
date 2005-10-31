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

#include <builderDirs2.h>
#include <exceptions.h>
#include <diffs.h>
#include <util.h>
#include <buffer.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qfile.h>

#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

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
 * LOCAL DECLARATIONS
 *============================================================================*/

/*----- types and enumerations -----*/

enum DirDiffType {
   IDENTICAL,
   DIFFER,
   ONLY_IN,
   COMMON_SUBDIR,
   UNKNOWN
};

#ifdef LOCAL_TRACE
char* typeString[5] = { 
   "IDENTICAL", "DIFFER", "ONLY_IN", "UNKNOWN", "COMMON_SUBDIR"
};
#endif

/*----- classes -----*/

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
      const char* buf
   );

};

/*==============================================================================
 * CLASS XxParseDiffError
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxParseDiffError::XxParseDiffError(
   XX_EXC_PARAMS_DECL(file,line),
   const char* buf
) :
   XxError( file, line ),
   std::runtime_error( "Parse diff output error." )
{
   QTextStream oss( &_msg, IO_WriteOnly | IO_Append );
   oss << "Error parsing diff output: " << endl
       << buf << endl;
}

/*==============================================================================
 * LOCAL FUNCTIONS
 *============================================================================*/

//------------------------------------------------------------------------------
//
bool parseDiffLine( 
   const QString& line,
   const QString& dir1,
   int            len1,
   const QString& dir2,
   int            len2,
   DirDiffType&   type,
   QString&       filename,
   int&           onlyDir
)
{
   const char* buf = line.latin1();

   bool error;
   const char* bufPtr = buf;
   onlyDir = -1; /* don't take chances with chance */
   if ( ::strncmp( bufPtr, "Only in ", 8 ) == 0 ) {
      bufPtr += 8;
      const char* colonPtr = ::strstr( bufPtr, ": " );
      if ( colonPtr == 0 ) {
         throw XxParseDiffError( XX_EXC_PARAMS, buf );
      }
      
      //int len = colonPtr - bufPtr;
      int cmp1 = ::strncmp( bufPtr, dir1.latin1(), len1 );
      int cmp2 = ::strncmp( bufPtr, dir2.latin1(), len2 );
      // Note: you cannot compare the lengths because these might be directory
      // diffs.
      if ( cmp1 == 0 && cmp2 == 0 ) {
         // Take the longest match. This may create errors/ambiguities if one of
         // the directories is a subdirectory of the other, but the output is
         // ambiguous, and to really check we would have to stat. Besides GNU
         // diff leaves this ambibuity.
         onlyDir = (len1 > len2) ? 0 : 1;
      }
      else if ( cmp1 != 0 && cmp2 != 0 ) {
         throw XxParseDiffError( XX_EXC_PARAMS, buf );
      }
      else {
         onlyDir = (cmp1 == 0) ? 0 : 1;
      }

      int lens = ( onlyDir == 0 ? len1 : len2 );
      // Note: skip the "/" after the specified directory path
      filename = QString();

      const char* dnamePtr = bufPtr + lens;
      if ( *dnamePtr == '/' ) {
         ++dnamePtr;
      }
      if ( colonPtr - dnamePtr > 0 ) {
         QString aname; 
         aname.setLatin1( dnamePtr, colonPtr - dnamePtr );
         filename += aname;
         if ( filename.constref( filename.length() - 1 ) != '/' ) {
            filename.append( '/' );
         }
      }

      // Note: need to remove \n
      QString bname;
      bname.setLatin1( colonPtr + 2 );

      filename += bname;
      type = ONLY_IN;
      error = false;
   }
   else if ( ::strncmp( bufPtr, "Files ", 6 ) == 0 ) {
      bufPtr += 6;
      const char* andPtr = ::strstr( bufPtr, " and " );
      if ( andPtr == 0 ) {
         throw XxParseDiffError( XX_EXC_PARAMS, buf );
      }
      const char* filenamePtr = bufPtr + len1;
      if ( *filenamePtr == '/' ) {
         ++filenamePtr;
      }
      int mlen = andPtr - filenamePtr;
      if ( mlen > 0 ) {
         filename.setLatin1( filenamePtr, mlen );
      }
      else {
         filename = QString();
      }

      bufPtr = andPtr + 5;
      const char* endtagPtr = ::strstr( bufPtr, " differ" );
      if ( endtagPtr != 0 ) {
         type = DIFFER;
      }
      else if ( ( endtagPtr = ::strstr( bufPtr, " are identical" ) ) != 0 ) {
         type = IDENTICAL;
      }
      else {
         throw XxParseDiffError( XX_EXC_PARAMS, buf );
      }
      error = false;
   }
   else if ( ::strncmp( bufPtr, "Common subdirectories: ", 23 ) == 0 ) {
      bufPtr += 23;
      const char* andPtr = ::strstr( bufPtr, " and " );
      if ( andPtr == 0 ) {
         throw XxParseDiffError( XX_EXC_PARAMS, buf );
      }
      const char* filenamePtr = bufPtr + len1;
      if ( *filenamePtr == '/' ) {
         ++filenamePtr;
      }
      int mlen = andPtr - filenamePtr;
      if ( mlen > 0 ) {
         filename.setLatin1( filenamePtr, mlen );
      }
      else {
         filename = QString();
      }

      type = COMMON_SUBDIR;
      error = false;
   }
   else {
      error = true;
   }
   // else
   return error;
}

//------------------------------------------------------------------------------
//
void setType( 
   const QStringList&        entries, 
   std::vector<DirDiffType>& types, 
   const QString&            filename, 
   DirDiffType               type
)
{
   int index = entries.findIndex( filename );
   if ( index == -1 ) {
#ifdef LOCAL_TRACE
      for ( unsigned int ii = 0; ii < entries.count(); ++ii ) {
         XX_TRACE( entries[ii] );
      }
      XX_TRACE( "filename \"" << filename.latin1() << "\"" );
#endif
      throw XxInternalError( XX_EXC_PARAMS );
   }

   if ( index >= int( types.size() ) ) {
      throw XxInternalError( XX_EXC_PARAMS );
   }
   types[ index ] = type;
}

//------------------------------------------------------------------------------
//
void patchUpMissingTypes( 
   const QStringList&        entries1,
   const QStringList&        entries2,
   std::vector<DirDiffType>& types1,
   std::vector<DirDiffType>& types2
)
{
   for ( uint ii = 0; ii < types1.size(); ++ii ) {
      if ( types1[ii] == UNKNOWN ) {
         int index = entries2.findIndex( entries1[ii] );
         if ( index != -1 ) {
            types1[ ii ] = IDENTICAL;
            types2[ index ] = IDENTICAL;
         }
         else {
            types1[ ii ] = ONLY_IN;
         }
      }
   }
}

//------------------------------------------------------------------------------
//
void buildSolelyFromOutput(
   FILE*                     fout,
   QTextOStream&             errors,
   XxBuffer&                 buffer1,
   XxBuffer&                 buffer2,
   std::vector<DirDiffType>& types1,
   std::vector<DirDiffType>& types2
)
{
   QStringList entries1;
   QStringList entries2;

   types1.clear();
   types2.clear();

   QString path1 = buffer1.getName();
   QString path2 = buffer2.getName();
   const int len1 = path1.length();
   const int len2 = path2.length();

   QFile qfout;
   qfout.open( IO_ReadOnly, fout );
   QTextStream outputs( &qfout );

   while ( true ) {
      QString line = outputs.readLine();
      if ( line.isNull() ) {
         break;
      }

      DirDiffType type;
      QString filename;
      int onlyDir = -1;
      if ( parseDiffLine(
         line, path1, len1, path2, len2, type, filename, onlyDir
      ) == true ) {
         XX_LOCAL_TRACE( "ERROR" );
         errors << "Diff error:" << endl;
         errors << line << endl;
         continue;
      }

#ifdef LOCAL_TRACE
      XX_TRACE( line 
                << typeString[ type ] << "   " 
                << filename.latin1() << "   "
                << onlyDir );
      if ( type == UNKNOWN ) {
         throw XxInternalError( XX_EXC_PARAMS );
      }
#endif

      switch ( type ) {

         case IDENTICAL: 
         case DIFFER: 
         case COMMON_SUBDIR: {
            entries1.append( filename );
            entries2.append( filename );
            types1.push_back( type );
            types2.push_back( type );
         } break;

         case ONLY_IN: {
            if ( onlyDir == 0 ) {
               entries1.append( filename );
               types1.push_back( type );
            }
            else {
               entries2.append( filename );
               types2.push_back( type );
            }
         } break;
         default: {
            throw XxInternalError( XX_EXC_PARAMS );
         }
      }
   }
   qfout.close();

   // Build the buffers.
   buffer1.setDirectoryEntries( entries1 );
   buffer2.setDirectoryEntries( entries2 );
}

//------------------------------------------------------------------------------
//
void buildAgainstReadDirectory(
   FILE*                     fout,
   QTextOStream&             errors,
   const XxBuffer&           buffer1,
   const XxBuffer&           buffer2,
   std::vector<DirDiffType>& types1,
   std::vector<DirDiffType>& types2
)
{
   const QStringList& entries1 = buffer1.getDirectoryEntries();
   const QStringList& entries2 = buffer2.getDirectoryEntries();
#ifdef LOCAL_TRACE
   {
      for ( QStringList::ConstIterator iter = entries1.begin();
            iter != entries1.end(); ++iter ) {
         XX_TRACE( (*iter).latin1() );
      }
      for ( QStringList::ConstIterator iter = entries2.begin();
            iter != entries2.end(); ++iter ) {
         XX_TRACE( (*iter).latin1() );
      }
   }
#endif

   types1.clear();
   types2.clear();
   types1.insert( types1.begin(), entries1.count(), UNKNOWN );
   types2.insert( types2.begin(), entries2.count(), UNKNOWN );

   QString path1 = buffer1.getName();
   QString path2 = buffer2.getName();

   const int len1 = path1.length();
   const int len2 = path2.length();

   QFile qfout;
   qfout.open( IO_ReadOnly, fout );
   QTextStream outputs( &qfout );

   while ( true ) {
      QString line = outputs.readLine();
      if ( line.isNull() ) {
         break;
      }

      DirDiffType type;
      QString filename;
      int onlyDir = -1;
      if ( parseDiffLine(
         line, path1, len1, path2, len2, type, filename, onlyDir
      ) == true ) {
         XX_LOCAL_TRACE( "ERROR" );
         errors << "Diff error:" << endl;
         errors << line << endl;
         continue;
      }

#ifdef LOCAL_TRACE
      XX_TRACE( line
                << typeString[ type ] << "   " 
                << filename.latin1() << "   "
                << onlyDir );
      if ( type == UNKNOWN ) {
         throw XxInternalError( XX_EXC_PARAMS );
      }
#endif

      switch ( type ) {

         case IDENTICAL: 
         case DIFFER: 
         case COMMON_SUBDIR: {
            setType( entries1, types1, filename, type );
            setType( entries2, types2, filename, type );
         } break;

         case ONLY_IN: {
            if ( onlyDir == 0 ) {
               setType( entries1, types1, filename, ONLY_IN );
            }
            else {
               setType( entries2, types2, filename, ONLY_IN );
            }
         } break;
         default: {
            throw XxInternalError( XX_EXC_PARAMS );
         }
      }
   }
   qfout.close();

   {
      std::vector<DirDiffType>::const_iterator it1 = 
         std::find( types1.begin(), types1.end(), UNKNOWN );
      std::vector<DirDiffType>::const_iterator it2 = 
         std::find( types2.begin(), types2.end(), UNKNOWN );
      if ( it1 != types1.end() || it2 != types2.end() ) {
         errors << "Forgotten files in directory diffs." << endl
                << "Check your subordinate directory diff program." << endl;
         
         // Patch it up, fallback somehow: for each file that is UNKNOWN in the
         // first array, if the file is available and UNKNOWN in the second,
         // make them IDENTICAL, otherwise make them ONLY_IN.
         //
         // It would be nice if this worked nicely when not using the '-s'
         // option to dir diff.

         patchUpMissingTypes( entries1, entries2, types1, types2 );
         patchUpMissingTypes( entries2, entries1, types2, types1 );
      }
   }
#ifdef LOCAL_TRACE
   XX_TRACE( "Done patching." );
#endif
}

}

XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxBuilderDirs2
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxBuilderDirs2::XxBuilderDirs2(
   bool buildSolelyFromOutput,
   bool isDiffRecursive,
   bool ignoreFileChanges
) :
   XxBuilder(),
   _buildSolelyFromOutput( buildSolelyFromOutput ),
   _isDiffRecursive( isDiffRecursive ),
   _ignoreFileChanges( ignoreFileChanges )
{}

//------------------------------------------------------------------------------
//
XxBuilderDirs2::~XxBuilderDirs2()
{}

//------------------------------------------------------------------------------
//
std::auto_ptr<XxDiffs> XxBuilderDirs2::process( 
   const QString& command,
   XxBuffer&      buffer1,
   XxBuffer&      buffer2
)
{
   initLines();

   QString path1 = buffer1.getName();
   QString path2 = buffer2.getName();

   QStringList filenames;
   filenames.append( path1 );
   filenames.append( path2 );
   const char** out_args;
   XxUtil::splitArgs( command, filenames, out_args );

   FILE* fout;
   FILE* ferr;
   XxUtil::spawnCommand( out_args, &fout, &ferr );
   if ( fout == 0 || ferr == 0 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }
   XxUtil::freeArgs( out_args );

   std::vector<DirDiffType> types1;
   std::vector<DirDiffType> types2;

   QTextOStream errors( &_errors );

   // Note: for now we don't support recursive diffs built against a directory.
   if ( _buildSolelyFromOutput || _isDiffRecursive ) {
      buildSolelyFromOutput(
         fout, errors, buffer1, buffer2, types1, types2
      );
   }
   else {
      buildAgainstReadDirectory( 
         fout, errors, buffer1, buffer2, types1, types2
      );
   }
   ::fclose( fout );

#ifdef LOCAL_TRACE
   XX_TRACE( "------------------------------" );
   for ( unsigned int ii = 0; ii < types1.size(); ++ii ) {
      XX_TRACE( typeString[ types1[ii] ] );
   }
   XX_TRACE( "------------------------------" );
   XX_TRACE( "------------------------------" );
   for ( unsigned int ii = 0; ii < types2.size(); ++ii ) {
      XX_TRACE( typeString[ types2[ii] ] );
   }
   XX_TRACE( "------------------------------" );
#endif
      
   XxFln fline1 = 1;
   XxFln fline2 = 1;
   {
      // Create regions with it. Hopefully our searches resulted in something
      // coherent w.r.t. to the quantities and pair matching of entries. Barf if
      // it doesn't.
      std::vector<DirDiffType>::const_iterator it1 = types1.begin();
      std::vector<DirDiffType>::const_iterator it2 = types2.begin();
   
      while ( it1 != types1.end() || it2 != types2.end() ) {
         if ( it1 == types1.end() ) {
            if ( *it2 != ONLY_IN ) {
               throw XxInternalError( XX_EXC_PARAMS );
            }
            XxLine line( XxLine::INSERT_2, -1, fline2++ );
            addLine( line );
            ++it2;
            continue;
         }
         if ( it2 == types2.end() ) {
            if ( *it1 != ONLY_IN ) {
               throw XxInternalError( XX_EXC_PARAMS );
            }
            XxLine line( XxLine::INSERT_1, fline1++, -1 );
            addLine( line );
            ++it1;
            continue;
         }

         if ( *it1 == ONLY_IN ) {
            XxLine line( XxLine::INSERT_1, fline1++, -1 );
            addLine( line );
            ++it1;
         }
         else if ( *it2 == ONLY_IN ) {
            XxLine line( XxLine::INSERT_2, -1, fline2++ );
            addLine( line );
            ++it2;
         }
         else if ( *it1 == DIFFER ) {
            if ( *it2 != *it1 ) {
               throw XxInternalError( XX_EXC_PARAMS );
            }
            XxLine::Type dtype = 
               _ignoreFileChanges == true ? XxLine::SAME : XxLine::DIFF_ALL;
            XxLine line( dtype, fline1++, fline2++ );
            addLine( line );
            ++it1;
            ++it2;
         }
         else if ( *it1 == IDENTICAL ) {
            if ( *it2 != *it1 ) {
               throw XxInternalError( XX_EXC_PARAMS );
            }
            XxLine line( XxLine::SAME, fline1++, fline2++ );
            addLine( line );
            ++it1;
            ++it2;
         }
         else if ( *it1 == COMMON_SUBDIR ) {
            if ( *it2 != *it1 ) {
               throw XxInternalError( XX_EXC_PARAMS );
            }
            XxLine line( XxLine::DIRECTORIES, fline1++, fline2++ );
            addLine( line );
            ++it1;
            ++it2;
         }
         else {
            XX_ASSERT( false );
         }
      }
   }

   // Create hunks.
   if ( _lines.size() > 0 ) {
      XxHunk curHunk = 0;
      XxLine::Type prevType = _lines[0].getType();
      _lines[0].setHunkId( curHunk );
      for ( XxDln ii = 1; ii < XxDln(_lines.size()); ++ii ) {
         XxLine& cline = _lines[ii];
         if ( prevType != cline.getType() ) {
            ++curHunk;
            prevType = cline.getType();
         }
         cline.setHunkId( curHunk );
      }
   }

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
   ::fclose( ferr );

   // Saved error text.
   errors << flush;
   XX_LOCAL_TRACE( "Errors: " << _errors );

   // If we've read no lines and there are diff errors then blow off
   if ( ( fline1 == 1 ) && ( fline2 == 1 ) && hasErrors() ) {
#ifndef WINDOWS
      int stat_loc;
      if ( wait( &stat_loc ) == -1 ) {
         throw XxIoError( XX_EXC_PARAMS );
      }
      _status = (WIFEXITED(stat_loc)) ? (WEXITSTATUS(stat_loc)) : 2;
      throw XxIoError( XX_EXC_PARAMS );
#else
      _status = 2;
#endif
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

   std::auto_ptr<XxDiffs> ap( new XxDiffs( _lines, true ) );
   return ap;
}

XX_NAMESPACE_END

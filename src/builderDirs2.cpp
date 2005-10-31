/******************************************************************************\
 * $Id: builderDirs2.cpp 56 2000-12-25 20:15:47Z  $
 * $Date: 2000-12-25 15:15:47 -0500 (Mon, 25 Dec 2000) $
 *
 * Copyright (C) 1999, 2000  Martin Blais <blais@iro.umontreal.ca>
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

#include <builderDirs2.h>
#include <exceptions.h>
#include <diffs.h>
#include <util.h>
#include <buffer.h>

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
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

/*----- types and enumerations -----*/

enum DirDiffType {
   IDENTICAL,
   DIFFER,
   ONLY_IN,
   COMMON_SUBDIR,
   UNKNOWN
};

#ifdef LOCAL_TRACE
char* typeString[4] = { 
   "IDENTICAL", "DIFFER", "ONLY_IN", "UNKNOWN", "COMMON_SUBDIR"
};
#endif

/*----- classes -----*/

/*==============================================================================
 * STRUCT StringEqual
 *============================================================================*/

struct StringEqual {

   StringEqual( const char* s ) :
      _s( s )
   {}

   bool operator () ( const char* s ) {
      return ::strcmp( _s, s ) == 0;
   }

   const char* _s;

};

/*==============================================================================
 * CLASS XxParseDiffError
 *============================================================================*/

class XxParseDiffError : public std::exception {

public:

   /*----- member functions -----*/

   // Constructor with state.
   XxParseDiffError(
      const char* buf,
      const char* file, 
      int         lineno
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
XxParseDiffError::XxParseDiffError(
   const char* buf,
   const char* file, 
   int         lineno
)
{
   std::ostringstream oss;
   oss << "Error parsing diff output: " << std::endl
       << buf << std::endl
       << "File: " << file << "Line: " << lineno << std::endl
       << std::ends;
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

/*==============================================================================
 * LOCAL FUNCTIONS
 *============================================================================*/

//------------------------------------------------------------------------------
//
bool parseDiffLine( 
   const char*  buf,
   const char*  dir1,
   int          len1,
   const char*  dir2,
   int          len2,
   DirDiffType& type,
   std::string& filename,
   int&         onlyDir
)
{
   bool error;
   const char* bufPtr = buf;
   if ( ::strncmp( bufPtr, "Only in ", 8 ) == 0 ) {
      bufPtr += 8;
      const char* colonPtr = ::strstr( bufPtr, ": " );
      if ( colonPtr == 0 ) {
         throw new XxParseDiffError( buf, XX_INTERROR_PARAMS );
      }

      //int len = colonPtr - bufPtr;
      int cmp1 = ::strncmp( bufPtr, dir1, len1 );
      int cmp2 = ::strncmp( bufPtr, dir2, len2 );
      // Note: you cannot compare the lengths because these might be directory
      // diffs.
      if ( cmp1 == 0 && cmp2 == 0 ) {
         // Take the longest match. This may create errors/ambiguities if one of
         // the directories is a subdirectory of the other, but the output is
         // ambiguous, and to really check we would have to stat. Besides GNU
         // diff leaves this ambibuity.
         onlyDir = cmp1 > cmp2 ? 0 : 1;
      }
      else if ( cmp1 != 0 && cmp2 != 0 ) {
         throw new XxParseDiffError( buf, XX_INTERROR_PARAMS );
      }
      else {
         onlyDir = cmp1 == 0 ? 0 : 1;
      }

      int lens = ( onlyDir == 0 ? len1 : len2 );
      // Note: skip the "/" after the specified directory path
      filename = std::string(); // Note: clear() ins't implemented in libstdc++.

      const char* dnamePtr = bufPtr + lens;
      if ( *dnamePtr == '/' ) {
         ++dnamePtr;
      }
      if ( colonPtr - dnamePtr > 0 ) {
         filename += std::string( dnamePtr, colonPtr - dnamePtr );
         if ( filename[ filename.length() - 1 ] != '/' ) {
            filename += "/";
         }
      }

      // Note: need to remove \n
      filename += std::string( colonPtr + 2, ::strlen( colonPtr + 2 ) - 1 );
      type = ONLY_IN;
      error = false;
   }
   else if ( ::strncmp( bufPtr, "Files ", 6 ) == 0 ) {
      bufPtr += 6;
      const char* andPtr = ::strstr( bufPtr, " and " );
      if ( andPtr == 0 ) {
         throw new XxParseDiffError( buf, XX_INTERROR_PARAMS );
      }
      const char* filenamePtr = bufPtr + len1;
      if ( *filenamePtr == '/' ) {
         ++filenamePtr;
      }
      filename = std::string( filenamePtr, andPtr - filenamePtr );

      bufPtr = andPtr + 5;
      const char* endtagPtr = ::strstr( bufPtr, " differ" );
      if ( endtagPtr != 0 ) {
         type = DIFFER;
      }
      else if ( ( endtagPtr = ::strstr( bufPtr, " are identical" ) ) != 0 ) {
         type = IDENTICAL;
      }
      else {
         throw new XxParseDiffError( buf, XX_INTERROR_PARAMS );
      }
      error = false;
   }
   else if ( ::strncmp( bufPtr, "Common subdirectories: ", 23 ) == 0 ) {
      bufPtr += 23;
      const char* andPtr = ::strstr( bufPtr, " and " );
      if ( andPtr == 0 ) {
         throw new XxParseDiffError( buf, XX_INTERROR_PARAMS );
      }
      const char* filenamePtr = bufPtr + len1;
      if ( *filenamePtr == '/' ) {
         ++filenamePtr;
      }
      filename = std::string( filenamePtr, andPtr - filenamePtr );

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
int searchEntry( 
   const std::vector<const char*>& entries, 
   const char*                     filename
)
{
   std::vector<const char*>::const_iterator iter = 
      std::find_if( 
         entries.begin(), 
         entries.end(),
         StringEqual( filename )
      );

   if ( iter == entries.end() ) {
      return -1;
   }

   return iter - &entries[0];
}

//------------------------------------------------------------------------------
//
void setType( 
   const std::vector<const char*>& entries, 
   std::vector<DirDiffType>&       types, 
   const char*                     filename, 
   DirDiffType                     type
)
{
   int index = searchEntry( entries, filename );

   if ( index == -1 ) {
#ifdef LOCAL_TRACE
      for ( int ii = 0; ii < entries.size(); ++ii ) {
         std::cout << entries[ii] << std::endl;
      }
      std::cout << "filename \"" << filename << "\"" << std::endl;
#endif
      throw new XxInternalError( XX_INTERROR_PARAMS );
   }

   if ( index >= int( types.size() ) ) {
      throw new XxInternalError( XX_INTERROR_PARAMS );
   }
   types[ index ] = type;
}

//------------------------------------------------------------------------------
//
void patchUpMissingTypes( 
   const std::vector<const char*>& entries1,
   const std::vector<const char*>& entries2,
   std::vector<DirDiffType>&       types1,
   std::vector<DirDiffType>&       types2
)
{
   for ( uint ii = 0; ii < types1.size(); ++ii ) {
      if ( types1[ii] == UNKNOWN ) {
         int index = searchEntry( entries2, entries1[ii] );
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
   FILE*                     fp,
   std::ostream&             errors,
   const char*               path1,
   XxBuffer*                 buffer1,
   const char*               path2,
   XxBuffer*                 buffer2,
   std::vector<DirDiffType>& types1,
   std::vector<DirDiffType>& types2
)
{
   std::vector<const char*> entries1;
   std::vector<const char*> entries2;

   types1.clear();
   types2.clear();

   int len1 = ::strlen( path1 );
   int len2 = ::strlen( path2 );

   char buffer[BUFSIZ+1];
   while ( fgets( buffer, BUFSIZ, fp ) != 0 ) {

      DirDiffType type;
      std::string filename;
      int onlyDir;
      if ( parseDiffLine(
         buffer, path1, len1, path2, len2, type, filename, onlyDir
      ) == true ) {
         XX_LOCAL_TRACE( "ERROR" );
         errors << "Diff error:" << std::endl;
         errors << buffer << std::endl;
         continue;
      }

#ifdef LOCAL_TRACE
      std::cout << buffer << std::endl;
      std::cout << typeString[ type ] << "   " 
                << filename << "   "
                << onlyDir << std::endl;
      if ( type == UNKNOWN ) {
         throw new XxInternalError( XX_INTERROR_PARAMS );
      }
#endif

      const char* dupname1 = ::strdup( filename.c_str() );
      const char* dupname2 = ::strdup( filename.c_str() );
      switch ( type ) {

         case IDENTICAL: 
         case DIFFER: 
         case COMMON_SUBDIR: {
            entries1.push_back( dupname1 );
            entries2.push_back( dupname2 );
            types1.push_back( type );
            types2.push_back( type );
         } break;

         case ONLY_IN: {
            if ( onlyDir == 0 ) {
               entries1.push_back( dupname1 );
               types1.push_back( type );
            }
            else {
               entries2.push_back( dupname2 );
               types2.push_back( type );
            }
         } break;
         default: {
            throw new XxInternalError( XX_INTERROR_PARAMS );
         }
      }
   }

   // Build the buffers.
   buffer1->setDirectoryEntries( entries1 );
   buffer2->setDirectoryEntries( entries2 );
}

//------------------------------------------------------------------------------
//
void buildAgainstReadDirectory(
   FILE*                     fp,
   std::ostream&             errors,
   const char*               path1,
   const XxBuffer*           buffer1,
   const char*               path2,
   const XxBuffer*           buffer2,
   std::vector<DirDiffType>& types1,
   std::vector<DirDiffType>& types2
)
{
   const std::vector<const char*>& entries1 = buffer1->getDirectoryEntries();
   const std::vector<const char*>& entries2 = buffer2->getDirectoryEntries();

   types1.clear();
   types2.clear();
   types1.insert( types1.begin(), entries1.size(), UNKNOWN );
   types2.insert( types2.begin(), entries2.size(), UNKNOWN );

   int len1 = ::strlen( path1 ); 
   int len2 = ::strlen( path2 );

   char buffer[BUFSIZ+1];
   while ( fgets( buffer, BUFSIZ, fp ) != 0 ) {

      DirDiffType type;
      std::string filename;
      int onlyDir;
      if ( parseDiffLine(
         buffer, path1, len1, path2, len2, type, filename, onlyDir
      ) == true ) {
         XX_LOCAL_TRACE( "ERROR" );
         errors << "Diff error:" << std::endl;
         errors << buffer << std::endl;
         continue;
      }

#ifdef LOCAL_TRACE
      std::cout << buffer << std::endl;
      std::cout << typeString[ type ] << "   " 
                << filename << "   "
                << onlyDir << std::endl;
      if ( type == UNKNOWN ) {
         throw new XxInternalError( XX_INTERROR_PARAMS );
      }
#endif

      switch ( type ) {

         case IDENTICAL: 
         case DIFFER: 
         case COMMON_SUBDIR: {
            setType( entries1, types1, filename.c_str(), type );
            setType( entries2, types2, filename.c_str(), type );
         } break;

         case ONLY_IN: {
            if ( onlyDir == 0 ) {
               setType( entries1, types1, filename.c_str(), ONLY_IN );
            }
            else {
               setType( entries2, types2, filename.c_str(), ONLY_IN );
            }
         } break;
         default: {
            throw new XxInternalError( XX_INTERROR_PARAMS );
         }
      }
   }

   {
      std::vector<DirDiffType>::const_iterator it1 = 
         std::find( types1.begin(), types1.end(), UNKNOWN );
      std::vector<DirDiffType>::const_iterator it2 = 
         std::find( types2.begin(), types2.end(), UNKNOWN );
      if ( it1 != types1.end() || it2 != types2.end() ) {
         errors << "Forgotten files in directory diffs." << std::endl
                << "Check your subordinate directory diff program." << std::endl
                << std::ends;
         
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
   std::cout << "Done patching." << std::endl;
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
   const char* command,
   const char* path1,
   XxBuffer*   buffer1,
   const char* path2,
   XxBuffer*   buffer2
)
{
   // Spawn command.
   const char* args[3];
   args[0] = path1;
   args[1] = path2;
   args[2] = 0;
   FILE* fp = XxUtil::spawnCommandWithOutput( command, args );
   if ( fp == 0 ) {
      throw new XxIoError;
   }

   std::vector<DirDiffType> types1;
   std::vector<DirDiffType> types2;

   std::ostringstream errors;

   // Note: for now we don't support recursive diffs built against a directory.
   if ( _buildSolelyFromOutput || _isDiffRecursive ) {
      buildSolelyFromOutput(
         fp, errors, path1, buffer1, path2, buffer2, types1, types2
      );
   }
   else {
      buildAgainstReadDirectory( 
         fp, errors, path1, buffer1, path2, buffer2, types1, types2
      );
   }

#ifdef LOCAL_TRACE
   std::cout << "------------------------------" << std::endl;
   for ( int ii = 0; ii < types1.size(); ++ii ) {
      std::cout << typeString[ types1[ii] ] << std::endl;
   }
   std::cout << "------------------------------" << std::endl;
   std::cout << "------------------------------" << std::endl;
   for ( int ii = 0; ii < types2.size(); ++ii ) {
      std::cout << typeString[ types2[ii] ] << std::endl;
   }
   std::cout << "------------------------------" << std::endl;
#endif

      
   int fline1 = 1;
   int fline2 = 1;
   {
      // Create regions with it. Hopefully our searches resulted in something
      // coherent w.r.t. to the quantities and pair matching of entries. Barf if
      // it doesn't.
      std::vector<DirDiffType>::const_iterator it1 = types1.begin();
      std::vector<DirDiffType>::const_iterator it2 = types2.begin();
   
      while ( it1 != types1.end() || it2 != types2.end() ) {
         if ( it1 == types1.end() ) {
            if ( *it2 != ONLY_IN ) {
               throw new XxInternalError( XX_INTERROR_PARAMS );
            }
            XxLine line( XxLine::INSERT_2, -1, fline2++ );
            addLine( line );
            ++it2;
            continue;
         }
         if ( it2 == types2.end() ) {
            if ( *it1 != ONLY_IN ) {
               throw new XxInternalError( XX_INTERROR_PARAMS );
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
               throw new XxInternalError( XX_INTERROR_PARAMS );
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
               throw new XxInternalError( XX_INTERROR_PARAMS );
            }
            XxLine line( XxLine::SAME, fline1++, fline2++ );
            addLine( line );
            ++it1;
            ++it2;
         }
         else if ( *it1 == COMMON_SUBDIR ) {
            if ( *it2 != *it1 ) {
               throw new XxInternalError( XX_INTERROR_PARAMS );
            }
            XxLine line( XxLine::DIRECTORIES, fline1++, fline2++ );
            addLine( line );
            ++it1;
            ++it2;
         }
         // FIXME simplify the three cases above.
         else {
            XX_ASSERT( false );
         }
      }
   }

   // Create hunks.
   if ( _lines.size() > 0 ) {
      int curHunk = 0;
      XxLine::Type prevType = _lines[0].getType();
      _lines[0].setHunkId( curHunk );
      for ( uint ii = 1; ii < _lines.size(); ++ii ) {
         XxLine& cline = _lines[ii];
         if ( prevType != cline.getType() ) {
            ++curHunk;
            prevType = cline.getType();
         }
         cline.setHunkId( curHunk );
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

   int stat_loc;
   if ( wait( &stat_loc ) == -1 ) {
      throw new XxIoError;
   }
   _status = (WIFEXITED(stat_loc)) ? (WEXITSTATUS(stat_loc)) : 2;

   std::auto_ptr<XxDiffs> ap( new XxDiffs( _lines, true ) );
   return ap;
}

//------------------------------------------------------------------------------
//
void XxBuilderDirs2::addLine( const XxLine& line )
{
   XX_LOCAL_TRACE( "AddLine: " << line );
   _lines.push_back( line );
}

XX_NAMESPACE_END

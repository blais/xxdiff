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

#include <buffer.h>
#include <exceptions.h>
#include <util.h>

#include <qfontmetrics.h>
#include <qfont.h>
#include <qrect.h>
#include <qdir.h>

#include <iostream>
#include <string.h>
#include <algorithm>

#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#include <algorithm>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

//------------------------------------------------------------------------------
//
const char* strnchr( const char* s, int c, const char* end ) 
{
   const char* pc = s;
   while ( pc < end ) {
      if ( *pc == c ) {
         return pc;
      }
      pc++;
   }
   return 0;
}

//------------------------------------------------------------------------------
//
const char* strnstr( const char* haystack, const uint n, const char* needle ) 
{
   const char* p = haystack;
   const char* pe = p + n - ::strlen( needle );
   while ( p < pe ) {
      while ( *p != *needle && p < pe ) { ++p; }
      const char* pp = p;
      const char* nn = needle;
      while ( (*pp == *nn) && (*nn != 0) ) { ++pp; ++nn; }
      if ( *nn == 0 ) {
         return p;
      }
      ++p;
   }
   return 0;
}

/*----- constants -----*/

const int renderBufferIncrement = 128;

}

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxBuffer
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxBuffer::XxBuffer( 
   const bool     /*passiveDummy*/, // ignored
   const QString& filename,
   const QString& displayFilename,
   const char     newlineChar
) :
   _newlineChar( newlineChar ),
   _name( filename ),
   _displayName( displayFilename ),
   _hiddenCR( false ),
   _temporary( false ),
   _deleteFile( false ),
   _proxy( false ),
   _buffer( 0 )
{
   XX_ASSERT( filename.length() != 0 );

   init();
}

//------------------------------------------------------------------------------
//
XxBuffer::XxBuffer( 
   const QString&   filename,
   const QString&   displayFilename,
   const QFileInfo& fileInfo,
   const bool       temporary,
   const char       newlineChar
) :
   _newlineChar( newlineChar ),
   _name( filename ),
   _displayName( displayFilename ),
   _fileInfo( fileInfo ),
   _hiddenCR( false ), // used to be parameter here.
   _temporary( temporary ),
   _deleteFile( false ),
   _proxy( false ),
   _buffer( 0 )
{
   XX_ASSERT( !filename.isEmpty() );

   // Stat the file.
   if ( !_temporary ) {

      if ( !fileInfo.exists() ) {
         throw XxIoError( XX_EXC_PARAMS );
      }

      if ( fileInfo.isDir() ) {
         loadDirectory();
      }
      else {
         loadFile( fileInfo );
      }
   }
   else {
      loadStream( stdin );
   }

   init();
}

//------------------------------------------------------------------------------
//
XxBuffer::XxBuffer(
   const XxBuffer&  orig,
   const QString&   filename,
   const QString&   displayFilename,
   const QFileInfo& fileInfo
) :
   _newlineChar( orig._newlineChar ),
   _name( filename ),
   _displayName( displayFilename ),
   _fileInfo( fileInfo ),
   _hiddenCR( orig._hiddenCR ),
   _temporary( false ),
   _deleteFile( false ),
   _proxy( true ),
   _buffer( orig._buffer ), // Here is the "sharing the buffer" part.
   _bufferSize( orig._bufferSize ),
   _index( orig._index )
#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
   , _lengths( orig._lengths )
#endif
{
   XX_ASSERT( filename.length() != 0 );

   init();
}

//------------------------------------------------------------------------------
//
void XxBuffer::init()
{
   _renderBufferSize = 0; // FIXME
   _renderBuffer = 0;
}

//------------------------------------------------------------------------------
//
XxBuffer::~XxBuffer()
{
   if ( !_proxy && _buffer != 0 ) {
      free( _buffer );
   }
   _buffer = 0;

   if ( _renderBuffer != 0 ) {
      free( _renderBuffer );
   }

   if ( _deleteFile == true ) {
      XxUtil::removeFile( _name.latin1() );
   }
}

//------------------------------------------------------------------------------
//
void XxBuffer::setDisplayName( const QString& fn )
{
   _displayName = fn;
}

//------------------------------------------------------------------------------
//
const QFileInfo& XxBuffer::getFileInfo() const
{
   return _fileInfo;
}

//------------------------------------------------------------------------------
//
void XxBuffer::loadStream( FILE* fin )
{
   // Read stream into variable size buffer.
   _bufferSize = 0;
   XX_ASSERT( _buffer == 0 );
   const unsigned int chunkSize = BUFSIZ * 16;
   while ( !feof( fin ) ) {
      _buffer = static_cast<char*>(
         realloc( _buffer, _bufferSize * sizeof(char) + chunkSize )
      );
      size_t nitems = fread( & _buffer[ _bufferSize ], 1, chunkSize, fin );
      if ( nitems < chunkSize ) {
         if ( ferror( fin ) != 0 ) {
            throw XxIoError( XX_EXC_PARAMS, "Error reading input stream." );
         }
      }
      _bufferSize += nitems;
   }

   // Don't close the stream here, as it might be stdin, and we need that to
   // remain open to be able to map the filedesc with dup2() and
   // fileno(). Granted, we could just make it 0.

   // Add a final newline if there isn't one.  This will simplify indexing.
   if ( _bufferSize > 0 ) { // to support empty files.
      if ( _buffer[ _bufferSize - 1 ] != _newlineChar ) {

         // Potentially reallocate the buffer to accomodate the new char.
         _buffer = static_cast<char*>(
            realloc( _buffer, _bufferSize * sizeof(char) + 1 )
         );

         _buffer[ _bufferSize ] = _newlineChar;
         _bufferSize++;
      }
   }

   if ( _hiddenCR ) {
      processCarriageReturns();
   }

   indexFile();
}

//------------------------------------------------------------------------------
//
void XxBuffer::loadFile( const QFileInfo& finfo )
{
   // Read file into buffer.
   FILE* fp = fopen( _name.latin1(), "r" );
   if ( fp == 0 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }

   if ( finfo.size() == 0 ) {
      // This file may be one of those special files that stats to 0 and that
      // has contents, like in /proc (under Linux), so stream it.
      loadStream( fp );
   }
   else {
      // Find out the file size.
      _bufferSize = finfo.size();

      // Allocate buffer.
      // add one for potential added newline.
      _buffer = static_cast<char*>( malloc( (_bufferSize + 1) * sizeof(char) ) );

      // Use an inegality check here, because under windows the fileinfo size we
      // get counts the CR-LF for 2 bytes, but once read they are only in for one
      // byte.
      uint readBufSize = fread( _buffer, 1, _bufferSize, fp );
      if ( readBufSize > _bufferSize ) {
         throw XxIoError( XX_EXC_PARAMS );
      }

      // Add a final newline if there isn't one.  This will simplify indexing.
      if ( _bufferSize > 0 ) { // to support empty files.
         if ( _buffer[ _bufferSize - 1 ] != _newlineChar ) {
            _buffer[ _bufferSize ] = _newlineChar;
            _bufferSize++;
         }
      }

      if ( _hiddenCR ) {
         processCarriageReturns();
      }

      indexFile();
   }

   if ( fclose( fp ) != 0 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }
}

//------------------------------------------------------------------------------
//
void XxBuffer::loadDirectory()
{
   QDir dir( _name );
   dir.setFilter( QDir::Dirs | QDir::Files | QDir::Hidden | QDir::System );
// Dirs - List directories only 
// Files - List files only 
// Drives - List disk drives (does nothing under unix) 
// NoSymLinks - Do not list symbolic links (where they exist) 
// Readable - List files for which the application has read access. 
// Writable - List files for which the application has write access. 
// Executable - List files for which the application has execute access 
// Modified - Only list files that have been modified (does nothing under unix) 
// Hidden - List hidden files (on unix, files starting with a .) 
// System - List system files (does nothing under unix) 

   dir.setSorting( QDir::Unsorted );
   XX_CHECK( dir.exists() );
   QStringList directoryEntries = dir.entryList();
   directoryEntries.remove( "." );
   directoryEntries.remove( ".." );
   setDirectoryEntries( directoryEntries );
}

//------------------------------------------------------------------------------
//
void XxBuffer::setDirectoryEntries( 
   const QStringList& directoryEntries 
)
{
   _directoryEntries = directoryEntries;

   _bufferSize = 0;
   for ( QStringList::Iterator itr = _directoryEntries.begin(); 
         itr != _directoryEntries.end();
         ++itr ) {
      _bufferSize += (*itr).length() + 1;
   }

   // Allocate buffer.
   // add one for potential added newline.
   _buffer = static_cast<char*>( malloc( (_bufferSize + 1) * sizeof(char) ) );
   
   char* bufferPtr = _buffer;
   for ( QStringList::Iterator it = _directoryEntries.begin(); 
         it != _directoryEntries.end();
         ++it ) {
      int len = (*it).length();
      ::strncpy( bufferPtr, (*it).latin1(), len );
      bufferPtr[len] = _newlineChar;
      bufferPtr += len + 1;
   }

   indexFile();
}

//------------------------------------------------------------------------------
//
void XxBuffer::processCarriageReturns()
{
   // Don't reallocate the buffer, just remove the offending bytes by
   // reorganizing the text (we'll have some extra buffer space allocated, but I
   // prefer this than having to allocate two buffers while doing the removal).

   char* srcPtr = _buffer;
   char* endPtr = _buffer + _bufferSize;

   // First scan for first occurence of CR. For files without CRs this fast loop
   // will simply zoom through the file until the first CR character.
   while ( srcPtr < endPtr && *srcPtr != '\015' ) {
      ++srcPtr;
   }
   char* destPtr = srcPtr;

   while ( srcPtr < endPtr ) {
      if ( *srcPtr != '\015' ) {
         *destPtr = *srcPtr;
         ++destPtr;
      }
      ++srcPtr;
   }

   _bufferSize = destPtr - _buffer;
}

//------------------------------------------------------------------------------
//
void XxBuffer::indexFile()
{
   XX_ASSERT( _buffer );

   uint ii;
   _index.push_back( -1 );
   _index.push_back( 0 );
#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
   int prev = 0;
   _lengths.push_back( -1 );
#endif

   for ( ii = 0; ii < _bufferSize; ++ii ) {
      if ( _buffer[ii] == _newlineChar ) {
         _index.push_back( ii + 1 );
#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
         _lengths.push_back( ii - prev );
         prev = static_cast<short>( ii + 1 );
#endif
      }
   }
   _lengths.push_back( 0 ); // Just so that the two arrays are the same size.
#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
   XX_ASSERT( _index.size() == _lengths.size() );
#endif

   // Make sure the file was [possibly artificially] terminated with a newline.
   XX_ASSERT( _index.back() == int(_bufferSize) );
}

//------------------------------------------------------------------------------
//
void XxBuffer::reindex( const std::vector<XxFln>& reindexTbl )
{
   XX_ASSERT( _buffer );

   // Swap old indexes.
   std::vector<int> oldIndex;
   std::swap( _index, oldIndex );

#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
   std::vector<short> oldLengths;
   std::swap( _lengths, oldLengths );
#endif

   uint ii;
   _index.push_back( -1 );
#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
   _lengths.push_back( -1 );
#endif
   
   for ( ii = 1; ii < reindexTbl.size(); ++ii ) {
      int reidx = reindexTbl[ii];
      _index.push_back( oldIndex[reidx] );
#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
      _lengths.push_back( oldLengths[reidx] );
#endif
   }

   // Add last line
#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
   _index.push_back( oldLengths[ reindexTbl.back() ] );
#else
   _index.push_back( oldIndex[ reindexTbl.back() + 1 ] );
#endif

   _dpyLineNos = reindexTbl;
}

//------------------------------------------------------------------------------
//
std::ostream& XxBuffer::dump( std::ostream& os ) const
{
   os << "Buffer dump:" << std::endl;
   os << "------------------------------" << std::endl;
   int nbLines = getNbLines();
   for ( XxFln ii = 1; ii <= nbLines; ++ii ) {
      uint length;
      const char* line = getTextLine( ii, length );
      os << ii << "(" << length << ")[" << ( line - _buffer ) << "]:";
      os.write( line, length );
      os << std::endl;
   }
   return os;
}

//------------------------------------------------------------------------------
//
uint XxBuffer::computeTextWidth(
   const QFont& font,
   const uint   tabWidth,
   const bool   hideCR
)
{
   QFontMetrics fm( font );

   uint longest = 0;
   int nbLines = getNbLines();
   for ( int ii = 1; ii <= nbLines; ++ii ) {
      uint length;
      const char* lineText = getTextLine( ii, length );

      int rlength;
      const char* renderedText = 
         renderTextWithTabs( lineText, length, tabWidth, hideCR, rlength, 0 );
      QString str( renderedText );

      QRect rect = fm.boundingRect( str );
      if ( static_cast<uint>( rect.width() ) > longest ) {
         longest = static_cast<uint>( rect.width() );
      }
   }
   return longest;
}

//------------------------------------------------------------------------------
//
uint XxBuffer::computeLineNumbersWidth( const QFont& font ) const
{
   QFontMetrics fm( font );
   char lnFormat[32];
   ::snprintf( lnFormat, sizeof(lnFormat), "%%%dd", getNbDigits() );

   uint longest = 0;
   int nbLines = getNbLines();
   char buffer[12];
   for ( int ii = 1; ii <= nbLines; ++ii ) {
      XxFln fline = getDisplayLineNo( ii );
      ::snprintf( buffer, sizeof(buffer), lnFormat, fline );
      QString str( buffer );

      QRect rect = fm.boundingRect( str, str.length() );
      longest = std::max( longest, (uint)rect.width() );
   }
   return longest;
}

//------------------------------------------------------------------------------
//
const char* XxBuffer::renderTextWithTabs( 
   const char* lineText,
   const uint  length,
   const uint  tabWidth,
   const uint  hideCR,
   int&        rlength,
   int*        hordiffs
)
{
//#define LOCAL_TRACE
#ifdef LOCAL_TRACE
#define XX_LOCAL_TRACE(x) XX_TRACE(x)
#else
#define XX_LOCAL_TRACE(x)
#endif

   const char* ps = lineText; // Source pointer.
   const char* endstr = ps + length; // End of string.

   char* pd = _renderBuffer; // Destination pointer.

   // Compute the maximum number of characters added a time.
   const uint maxIncrement = std::max( (uint)2, tabWidth );

   --ps;
   while ( ++ps < endstr ) {

      XX_LOCAL_TRACE( "source char: " << *ps );

      const int curcol = pd - _renderBuffer;
      uint horoffset = 0;

      // Expand render buffer if necessary.  We use a conservative measure here,
      // that will accomodate all possible cases below.
      const int requiredSize = curcol + maxIncrement;
      if ( _renderBufferSize <= requiredSize ) {
         _renderBufferSize = 
            ( (requiredSize / renderBufferIncrement) + 1 ) *
            renderBufferIncrement;
         _renderBuffer = (char*)realloc( _renderBuffer,
                                         _renderBufferSize );
         pd = _renderBuffer + curcol;
      }

      if ( *ps == '\t' ) {
         XX_LOCAL_TRACE( "  tab" );

         //
         // We detected a tab character.
         //
         if ( tabWidth != 0 ) {
            
            // Compute amount of chars to insert.
            int bcol = pd - _renderBuffer; 
            uint nspaces = tabWidth - (bcol % tabWidth);

            // Output equivalent spaces for tab in the destination.
            for ( uint t = 0; t < nspaces; ++t ) {
               *pd++ = ' ';
               XX_CHECK( pd - _renderBuffer < _renderBufferSize );
            }

            horoffset = nspaces - 1;
         }
         else {
            continue;
         }
      }
      else if ( *ps == '\r' ) {
         if ( ! hideCR ) {
            XX_LOCAL_TRACE( "  cr" );
            *pd++ = '^';
            *pd++ = 'M';
            XX_CHECK( pd - _renderBuffer < _renderBufferSize );

            horoffset = 1;
         }
         else {
            continue;
         }
      }
      else { 
         XX_LOCAL_TRACE( "  output:" << *ps );
         *pd++ = *ps;
         XX_CHECK( pd - _renderBuffer < _renderBufferSize );
         continue; // No need to offset the horizontal diffs.
      }

      // Shift horizontal diffs if necessary.
      if ( hordiffs ) {
         for ( int* phd = hordiffs; *phd != -1; ++phd ) {
            if ( *phd > curcol ) {
               *phd += horoffset;
            }
         }
      }
   }

   const int curcol = pd - _renderBuffer;

   //
   // Invalidate horizontal diffs that are at the end if necessary.  This is
   // necessary since we can potentially be removing characters (due to hideCR)
   // and we do not want to return with some hordiffs beyond the end of the
   // rendered line (the rendering code itself does not handle this, it assumes
   // the hordiffs within the rendered line).
   //
   if ( hordiffs ) {
      for ( int* phd = hordiffs; *phd != -1; ++phd ) {
         if ( *phd > curcol ) {
            *phd = curcol;
         }
      }
   }


   //
   // Add terminator character.
   //

   // Expand render buffer if necessary.
   const int requiredSize = curcol + 1;
   if ( _renderBufferSize <= requiredSize ) {
      _renderBufferSize = 
         ( (requiredSize / renderBufferIncrement) + 1 ) *
         renderBufferIncrement;
      _renderBuffer = (char*)realloc( _renderBuffer, _renderBufferSize );
      pd = _renderBuffer + curcol;
   }

   // Add terminator to render buffer.
   *pd = '\0';

   // Return the buffer.
   rlength = pd - _renderBuffer;
   XX_LOCAL_TRACE( "--- length = " << rlength );
   XX_LOCAL_TRACE( "--- renderbuffer = " << _renderBuffer );

   return _renderBuffer;
}

//------------------------------------------------------------------------------
//
uint XxBuffer::getNbDigits() const
{
   return uint( ::ceil( ::log10( double( getNbLines() ) ) ) );
}

//------------------------------------------------------------------------------
//
const QString& XxBuffer::renderLineNumber( 
   const XxFln    lineNumber,
   const QString& format
)
{
   _lnBuffer.sprintf( format, lineNumber );
   return _lnBuffer;
}

//------------------------------------------------------------------------------
//
bool XxBuffer::searchLine( const XxFln lineno, const QString& searchText ) const
{
   bool found = false;
   uint len;
   const char* text = getTextLine( lineno, len );
   if ( strnstr( text, len, searchText.latin1() ) != 0 ) {
      found = true;
   }
   return found;
}

//------------------------------------------------------------------------------
//
bool XxBuffer::isDirectory() const
{
   return !_directoryEntries.isEmpty();
}

//------------------------------------------------------------------------------
//
const QStringList& XxBuffer::getDirectoryEntries() const
{
   return _directoryEntries;
}

//------------------------------------------------------------------------------
//
QString XxBuffer::getBufferAtLine( const XxFln lineno ) const
{
   QString filename = _name;
   if ( filename.constref( filename.length()-1 ) != '/' ) {
      filename.append( "/" );
   }
   
   XX_CHECK( lineno - 1 < XxFln(_directoryEntries.count()) );

   // Note: QStringList O(n) lookup.
   filename.append( _directoryEntries[ lineno - 1 ] );
   return filename;
}

//------------------------------------------------------------------------------
//
void XxBuffer::makeTemporary()
{
   //
   // Write contents of the buffer into the temporary file.
   //

   // Open the temporary file.
   char temporaryFilename[32] = "/var/tmp/xxdiff-tmp.XXXXXX";
   FILE* fout = XxUtil::tempfile( temporaryFilename );
   
   // Write contents.
   uint size;
   const char* buffer = getBuffer( size );
   
   if ( ::fwrite( buffer, 1, size, fout ) != size ) {
      throw XxIoError( XX_EXC_PARAMS, 
                       "Error writing to temporary file." );
   }

   // Close the temporary file.
   if ( ::fclose( fout ) != 0 ) {
      throw XxIoError( XX_EXC_PARAMS, 
                       "Error closing temporary file." );
   }
   
   // Make this buffer a temporary to be deleted.
   _name = QString(temporaryFilename);
   _deleteFile = true;
}

XX_NAMESPACE_END

/******************************************************************************\
 * $Id: buffer.cpp 250 2001-10-04 19:56:59Z blais $
 * $Date: 2001-10-04 15:56:59 -0400 (Thu, 04 Oct 2001) $
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

#include <buffer.h>
#include <exceptions.h>

#include <qfontmetrics.h>
#include <qfont.h>
#include <qrect.h>

#include <iostream>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else    
# define dirent direct
# define NAMLEN(dirent) ((dirent)->d_namlen)
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

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
   const QString& displayFilename
) :
   _name( filename ),
   _displayName( displayFilename ),
   _hiddenCR( false ),
   _temporary( false ),
   _buffer( 0 )
{
   XX_ASSERT( filename != 0 );

   init();
}

//------------------------------------------------------------------------------
//
XxBuffer::XxBuffer( 
   const QString& filename, 
   const QString& displayFilename,
   const bool     hideCR,
   const bool     deleteFile
) :
   _name( filename ),
   _displayName( displayFilename ),
   _hiddenCR( hideCR ),
   _temporary( deleteFile ),
   _buffer( 0 )
{
   XX_ASSERT( !filename.isEmpty() );

   // Stat the file.
   struct stat ss;
   if ( stat( _name.latin1(), &ss ) == -1 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }

   if ( ! S_ISDIR( ss.st_mode ) ) {
      loadFile( ss );
   }
   else {
      loadDirectory( ss );
   }

   init();
}

//------------------------------------------------------------------------------
//
void XxBuffer::init()
{
   _renderBufferSize = 256;
   _renderBuffer = (char*)malloc( _renderBufferSize );
}

//------------------------------------------------------------------------------
//
XxBuffer::~XxBuffer()
{
   delete _buffer;

   // Delete the temporary file if asked for.
   if ( _temporary == true ) {
      unlink( _name.latin1() );
   }

   if ( _renderBuffer != 0 ) {
      free( _renderBuffer );
   }
}

//------------------------------------------------------------------------------
//
void XxBuffer::loadFile( struct stat& ss )
{
   // Find out the file size.
   _bufferSize = ss.st_size;

   // Allocate buffer.
   // add one for potential added newline.
   _buffer = new char[ _bufferSize + 1 ];

   // Read file into buffer.
   FILE* fp = fopen( _name.latin1(), "r" );
   if ( fp == 0 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }

   if ( fread( _buffer, 1, _bufferSize, fp ) != _bufferSize ) {
      throw XxIoError( XX_EXC_PARAMS );
   }

   if ( fclose( fp ) != 0 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }

   // Add a final newline if there isn't one.  This will simplify indexing.
   if ( _bufferSize > 0 ) { // to support empty files.
      if ( _buffer[ _bufferSize - 1 ] != '\n' ) {
         _buffer[ _bufferSize ] = '\n';
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
void XxBuffer::loadDirectory( struct stat& /*ss*/ )
{
   register struct dirent* next;

   QStringList directoryEntries;

   /* Open the directory and check for errors.  */
   register DIR* reading = opendir( _name.latin1() );
   if ( !reading ) {
      throw XxIoError( XX_EXC_PARAMS );
   }
   
   /* Read the directory entries, and insert the subfiles
      into the `data' table.  */
   while ( ( errno = 0, ( next = readdir( reading ) ) != 0 ) ) {
      char *d_name = next->d_name;
      size_t d_size = NAMLEN(next) + 1;
         
      /* Ignore the files `.' and `..' */
      if ( d_name[0] == '.'
           && (d_name[1] == 0 || (d_name[1] == '.' && d_name[2] == 0))) {
         continue;
      }
         
      QString newString;
      newString.setLatin1( d_name, d_size );

      XX_CHECK( d_name != 0 && d_size > 0 );
      directoryEntries.append( newString );
   }
   if ( errno ) {
      int e = errno;
      closedir( reading );
      errno = e;
      throw XxIoError( XX_EXC_PARAMS );
   }
#if CLOSEDIR_VOID
   closedir( reading );
#else
   if ( closedir( reading ) != 0 ) {
      throw XxIoError( XX_EXC_PARAMS );
   }
#endif

   directoryEntries.sort();
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
   for ( QStringList::Iterator it = _directoryEntries.begin(); 
         it != _directoryEntries.end();
         ++it ) {
      _bufferSize += (*it).length() + 1;
   }

   // Allocate buffer.
   // add one for potential added newline.
   _buffer = new char[ _bufferSize + 1 ];
   
   char* bufferPtr = _buffer;
   for ( QStringList::Iterator it = _directoryEntries.begin(); 
         it != _directoryEntries.end();
         ++it ) {
      int len = (*it).length();
      ::strncpy( bufferPtr, (*it).latin1(), len );
      bufferPtr[len] = '\n';
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
   // will simply zoom through the file, instead of us having to check that
   // destPtr < srcPtr.
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
   _index.push_back( 0 );

   for ( ii = 0; ii < _bufferSize; ++ii ) {
      if ( _buffer[ii] == '\n' ) {
         _index.push_back( ii + 1 );
      }
   }
   // Make sure the file was [possibly artificially] terminated with a newline.
   XX_ASSERT( _index.back() == int(_bufferSize) );
}

//------------------------------------------------------------------------------
//
std::ostream& XxBuffer::dump( std::ostream& os ) const
{
   int nbLines = getNbLines();
   for ( XxFln ii = 1; ii <= nbLines; ++ii ) {
      uint length;
      const char* line = getTextLine( ii, length );
      os << ii << ":";
      os.write( line, length );
      os << std::endl;
   }
   return os;
}

//------------------------------------------------------------------------------
//
uint XxBuffer::computeTextWidth( const QFont& font, const uint tabWidth )
{
   QFontMetrics fm( font );

   uint longest = 0;
   int nbLines = getNbLines();
   for ( int ii = 1; ii <= nbLines; ++ii ) {
      uint length;
      const char* lineText = getTextLine( ii, length );

      int rlength;
      int lhd = 0;
      int rhd = 0;
      const char* renderedText = 
         renderTextWithTabs( lineText, length, tabWidth, rlength, lhd, rhd );
      QString str( renderedText );

      QRect rect = fm.boundingRect( str );
      longest = std::max( longest, (uint)rect.width() );
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
      ::snprintf( buffer, sizeof(buffer), lnFormat, ii );
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
   int&        rlength,
   int&        lhd,
   int&        rhd
)
{
   const int increment = 256;

   char* pd = _renderBuffer;

   uint col = 0;
   const char* ps = lineText;
   const char* endstr = ps + length;
   const char* nexttab = 0;

   // Offset rightmost char to render tabs correctly.
   int hrhd = rhd + 1;
   while ( ( nexttab = strnchr( ps, '\t', endstr ) ) != 0 ) {

      // Check for size requirements and reallocate if necessary.
      int requiredSize = (pd - _renderBuffer) + (nexttab - ps) + tabWidth;
      if ( _renderBufferSize <= requiredSize ) {
         _renderBufferSize = ( (requiredSize / increment) + 1 ) * increment;
         _renderBuffer = (char*)realloc( _renderBuffer, _renderBufferSize );
         pd = _renderBuffer + col;
      }

      // Copy chunk before tab.
      while ( ps < nexttab ) {
         *pd++ = *ps++;
         XX_CHECK( pd - _renderBuffer < _renderBufferSize );
      }
      ps++; // skip tab

      // Output tab.
      uint nspaces = 0;
      if ( tabWidth != 0 ) {
         nspaces = tabWidth - ((pd - _renderBuffer) % tabWidth);
      }

      lhd += ( lhd > (pd - _renderBuffer) ) ? nspaces - 1 : 0;
      hrhd += ( hrhd > (pd - _renderBuffer) ) ? nspaces - 1 : 0;

      for ( uint t = 0; t < nspaces; ++t ) {
         *pd++ = ' ';
         XX_CHECK( pd - _renderBuffer < _renderBufferSize );
      }
   }
   rhd = hrhd - 1;

   // Copy last chunk.

   // Check for size requirements and reallocate if necessary.
   int requiredSize = (pd - _renderBuffer) + (length) + 1;
   if ( _renderBufferSize <= requiredSize ) {
      _renderBufferSize = ( (requiredSize / increment) + 1 ) * increment;
      _renderBuffer = (char*)realloc( _renderBuffer, _renderBufferSize );
      pd = _renderBuffer + col;
   }
   while ( ps < endstr ) {
      *pd++ = *ps++;
      XX_CHECK( pd - _renderBuffer < _renderBufferSize );
   }

   // End string.
   *pd = '\0';

   rlength = pd - _renderBuffer;

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
   // FIXME this is not thread-safe because we're modifying the text buffer
   // temporarily.  Implement your own strnstr without copying.
   bool found = false;
   uint len;
   char* text = const_cast<char*>( getTextLine( lineno, len ) );
   char endchar = text[len];
   text[len] = '\0';
   if ( ::strstr( text, searchText.latin1() ) != 0 ) {
      found = true;
   }
   text[len] = endchar;
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
QString XxBuffer::getFileAtLine( 
   const XxFln lineno
) const
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

XX_NAMESPACE_END

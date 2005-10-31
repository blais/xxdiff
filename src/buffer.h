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

#ifndef INCL_XXDIFF_FILE
#define INCL_XXDIFF_FILE

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_TYPES
#include <types.h>
#endif

#ifndef INCL_STD_VECTOR
#include <vector>
#define INCL_STD_VECTOR
#endif

#ifndef INCL_STD_IOSFWD
#include <iosfwd>
#define INCL_STD_IOSFWD
#endif

#ifndef INCL_STD_STDIO
#include <stdio.h>
#define INCL_STD_STDIO
#endif

#ifndef INCL_QT_QSTRING
#include <qstring.h>
#define INCL_QT_QSTRING
#endif

#ifndef INCL_QT_QSTRINGLIST
#include <qstringlist.h>
#define INCL_QT_QSTRINGLIST
#endif

#ifndef INCL_QT_QFILEINFO
#include <qfileinfo.h>
#define INCL_QT_QFILEINFO
#endif

#ifndef INCL_STD_MEMORY
#include <memory>
#define INCL_STD_MEMORY
#endif

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QFont;

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxBuffer
 *============================================================================*/

// <summary> a class that represents a file's text </summary>

// This class represents a file's text and keeps references each new line.

class XxBuffer {

public:

   /*----- member functions -----*/

   // Passive constructor, will not build buffer contents.
   XxBuffer(
      const bool     passiveDummy, // ignored
      const QString& filename, 
      const QString& displayFilename,
      const char     newlineChar = '\n'
   );

   // Constructor.  This will load the file in memory and index the beginnings
   // of each line.
   XxBuffer( 
      const QString&   filename, 
      const QString&   displayFilename,
      const QFileInfo& fileInfo,
      const bool       temporary = false,
      const char       newlineChar = '\n'
   );

   // Constructor that can impersonate another buffer but that does not own the
   // data. This is used to share the data for unmerge mode.
   XxBuffer(
      const XxBuffer&  orig,
      const QString&   filename,
      const QString&   displayFilename,
      const QFileInfo& fileInfo
   );

   // Destructor.
   virtual ~XxBuffer();

   // Get file name.
   QString getName() const;

   // Get/set display filename.
   // <group>
   const QString& getDisplayName() const;
   void setDisplayName( const QString& );
   // </group>

   // Get/set stat info for file.
   const QFileInfo& getFileInfo() const;

   // Returns true if this file is stored as a temporary file.
   bool isTemporary() const;

   // Returns the number of lines.
   uint getNbLines() const;

   // Computes the width of the longest line when rendered with specified font,
   // in pixels.
   uint computeTextWidth(
      const QFont& font,
      const uint   tabWidth,
      const bool   hideCR
   );

   // Computes the width of the longest numbers when rendered with specified
   // font, in pixels.
   uint computeLineNumbersWidth( const QFont& ) const;

   // Returns a pointer to the beginning of a line and its length.  Important
   // note: in accordance to how diff reports line numbers, files start at line
   // 1, not line 0.  You cannot call this method for line 0.
   const char* getTextLine( 
      const XxFln lineno,
      uint&       length 
   ) const;

   // Renders text into modified text ready for display and returns a pointer to
   // a buffer that belongs to the file object (don't fool around with it).
   // rlength will contain the length of the rendered string.
   //
   // These functions will offset the horizontal diffs it is given if necessary.
   // If the hordiffs is not null, it is expecting an array ende by a -1
   // element.  This array will be modified in place.
   // <group>
   
   // Realizes tab expansion into spaces, as well as carriage returns.
   const char* renderTextWithTabs( 
      const char* lineText,
      const uint  length, 
      const uint  tabWidth, 
      const uint  hideCR,
      int&        rlength,
      int*        hordiffs
   );
   // </group>

   // Returns the number of digits required for this file.
   uint getNbDigits() const;

   // Renders the line number.  See renderTextWithTabs.
   const QString& renderLineNumber( 
      const XxFln    lineNumber,
      const QString& format
   );

   // Searches the specified line for the specified search text.
   bool searchLine( const XxFln lineno, const QString& searchText ) const;

   // Debug output.
   std::ostream& dump( std::ostream& ) const;

   // Returns true if this buffer represents a directory.
   bool isDirectory() const;

   // Returns a list of the directory files if this is a buffer representing a
   // directory.
   const QStringList& getDirectoryEntries() const;

   // Sets the directory entries and creates and indexes the buffer with them.
   void setDirectoryEntries( const QStringList& directoryEntries );

   // If this is a buffer containing a directory (i.e. a list of files), return
   // the full path of the filename at line lineno.
   QString getBufferAtLine( const XxFln lineno ) const;

   // Reindex the buffer lines, according to the given table. The table
   // indicates what current lines the new lines should point to.
   // This is used during unmerge.
   void reindex( const std::vector<XxFln>& reindexTbl );

   // Returns the line number to display for given text line.
   XxFln getDisplayLineNo( const XxFln fline ) const;

   // Returns the buffer and buffersize.
   const char* getBuffer( uint& size ) const;

   // Copy the given buffer contents into a temporary file and make this buffer
   // represent this temporary file, which will be automatically deleted.
   void makeTemporary();

private:

   /*----- member functions -----*/

   // Common initialization.
   void init();

   // Load the file in the buffer.
   // <group>
   void loadFile( const QFileInfo& );
   void loadDirectory();
   void loadStream( FILE* fin );
   // </group>

   // Process the buffer to remove the carriage returns. Do this before
   // indexing!
   void processCarriageReturns();

   // Index the file lines.
   void indexFile();

   /*----- data members -----*/

   char               _newlineChar; // default is \010

   QString            _name;
   QString            _displayName;
   QFileInfo          _fileInfo;
   bool               _hiddenCR;
   bool               _temporary;
   bool               _deleteFile;
   bool               _proxy;
   char*              _buffer;
   uint               _bufferSize;

   // Index of character offsets.
   // Entry 0 is a dummy.
   // Entry 1 is an offset to the beginning of line 1 (== 0).
   // Entry 2 is an offset to the beginning of line 2 (end of line 1 + \n).
   // The last entry is an offset to the end of the last line.
   std::vector<int>   _index;

#define XX_ENABLED_BUFFER_LINE_LENGTHS
#ifdef XX_ENABLED_BUFFER_LINE_LENGTHS
   // Important note: due to the use of the short datatype to record line
   // lengths, xxdiff is limited to lines of length up to 64k characters. We
   // consider this reasonable for all purposes. However, this could be easily
   // changed to int below if necessary.
   //
   // We had to introduce an explicit vector of lengths because since the
   // unmerge feature was introduced, we're sharing the very text buffer that
   // the multiple buffers use we cannot anymore rely on buffer lines appearing
   // next to each other in the data array.
   std::vector<short> _lengths;
#endif

   // Indirection index for reindexed files. This array contains the line
   // numbers that should be displayed, for each line.
   std::vector<XxFln> _dpyLineNos;

   char*              _renderBuffer;
   int                _renderBufferSize;
   QString            _lnBuffer;

   QStringList        _directoryEntries;

};


XX_NAMESPACE_END

#include <buffer.inline.h>

#endif

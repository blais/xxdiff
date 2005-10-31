/******************************************************************************\
 * $Id: buffer.h 2 2000-09-15 02:19:22Z blais $
 * $Date: 2000-09-14 22:19:22 -0400 (Thu, 14 Sep 2000) $
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

#ifndef INCL_XXDIFF_FILE
#define INCL_XXDIFF_FILE

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_STD_STRING
#include <string>
#define INCL_STD_STRING
#endif

#ifndef INCL_STD_VECTOR
#include <vector>
#define INCL_STD_VECTOR
#endif

#ifndef INCL_STD_IOSFWD
#include <iosfwd>
#define INCL_STD_IOSFWD
#endif

#ifndef INCL_STD_STAT
#include <sys/stat.h>
#define INCL_STD_STAT
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
      const bool  passiveDummy, // ignored
      const char* filename, 
      const char* displayFilename
   );

   // Constructor.  This will load the file in memory and index the beginnings
   // of each line.
   XxBuffer( 
      const char* filename, 
      const char* displayFilename,
      const bool  hideCR = true,
      const bool  deleteFile = false
   );

   // Destructor.
   virtual ~XxBuffer();

   // Get file name.
   const char* getName() const;

   // Get display filename.
   const char* getDisplayName() const;

   // Returns true if this file is stored as a temporary file.
   bool isTemporary() const;

   // Returns the number of lines.
   uint getNbLines() const;

   // Computes the width of the longest line when rendered with specified font,
   // in pixels.
   uint computeTextWidth( const QFont&, const uint tabWidth );

   // Computes the width of the longest numbers when rendered with specified
   // font, in pixels.
   uint computeLineNumbersWidth( const QFont& ) const;

   // Returns a pointer to the beginning of a line and its length.  Important
   // note: in accordance to how diff reports line numbers, files start at line
   // 1, not line 0.  You cannot call this method for line 0.
   const char* getTextLine( 
      const uint lineno,
      uint& 	 length 
   ) const;

   // Renders text into text width tabs expanded to spaces and returns a pointer
   // to a buffer that belongs to the file object (don't fool around with it).
   // rlength will contain the length of the rendered string.  lhd and rhd will
   // get readjusted according to the right amount of spaces that will be added.
   const char* renderTextWithTabs( 
      const char* lineText,
      const uint  length, 
      const uint  tabWidth, 
      int&	  rlength,
      int&	  lhd,
      int&	  rhd
   );

   // Returns the number of digits required for this file.
   uint getNbDigits() const;

   // Renders the line number.  See renderTextWithTabs.
   const char* renderLineNumber( 
      const int   lineNumber,
      const char* format
   );

   // Searches the specified line for the specified search text.
   bool searchLine( const uint lineno, const char* searchText ) const;

   // Debug output.
   std::ostream& dump( std::ostream& ) const;

   // Returns true if this buffer represents a directory.
   bool isDirectory() const;

   // Returns a list of the directory files if this is a buffer representing a
   // directory.
   const std::vector<const char*>& getDirectoryEntries() const;

   // Sets the directory entries and creates and indexes the buffer with them.
   void setDirectoryEntries( const std::vector<const char*>& directoryEntries );

   // If this is a buffer containing a directory (i.e. a list of files), return
   // the full path of the filename at line lineno.
   std::string getFileAtLine( 
      const uint lineno
   ) const;

private:

   /*----- member functions -----*/

   // Common initialization.
   void init();

   // Load the file in the buffer.
   void loadFile( struct stat& );
   void loadDirectory( struct stat& );

   // Process the buffer to remove the carriage returns. Do this before
   // indexing!
   void processCarriageReturns();

   // Index the file lines.
   void indexFile();

   /*----- data members -----*/

   std::string              _name;
   std::string              _displayName;
   bool                     _hiddenCR;
   bool                     _temporary;
   char*                    _buffer;
   uint                     _bufferSize;
   std::vector<int>         _index;
   char*                    _renderBuffer;
   int                      _renderBufferSize;
   char                     _lnBuffer[12];

   // Note: we explicitly avoid instantiating a std::vector<string> because that
   // instantiation increases the size of our executable file considerably.
   std::vector<const char*> _directoryEntries;

};


XX_NAMESPACE_END

#include <buffer.inline.h>

#endif

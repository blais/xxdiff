/******************************************************************************\
 * $Id: line.h 274 2001-10-16 02:05:34Z blais $
 * $Date: 2001-10-15 22:05:34 -0400 (Mon, 15 Oct 2001) $
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

#ifndef INCL_XXDIFF_LINE
#define INCL_XXDIFF_LINE

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_TYPES
#include <types.h>
#endif

#ifndef INCL_XXDIFF_RESOURCES
#include <resources.h>
#define INCL_XXDIFF_RESOURCES
#endif

#ifndef INCL_QT_QSTRING
#include <qstring.h>
#define INCL_QT_QSTRING
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * CLASS XxLine
 *============================================================================*/

// <summary> a display line </summary>

// This class represent a single line in the display.

class XxLine {

public:

   /*----- types and enumerations -----*/


   // [2-way] [3-way] [what we call them]
   // --------------------------------------------
   //                      
   //  (AA)   (AAA)         SAME
   //                      
   //         (BAA)         DIFF_1
   //         (ABA)         DIFF_2
   //         (AAB)         DIFF_3
   //                     
   //         (-AA)         DELETE_1
   //         (A-A)         DELETE_2
   //         (AA-)         DELETE_3
   //                     
   //  (A-)   (A--)         INSERT_1
   //  (-A)   (-A-)         INSERT_2
   //         (--A)         INSERT_3
   //                     
   //  (AB)   (ABC)         DIFF_ALL
   //                     
   //         (-AB)         DIFFDEL_1
   //         (A-B)         DIFFDEL_2
   //         (AB-)         DIFFDEL_3

   enum Type { 
      SAME,
      DIFF_1,
      DIFF_2,
      DIFF_3,
      DELETE_1,
      DELETE_2,
      DELETE_3,
      INSERT_1,
      INSERT_2,
      INSERT_3,
      DIFF_ALL,
      DIFFDEL_1,
      DIFFDEL_2,
      DIFFDEL_3,
      DIRECTORIES,
      NB_TYPES
   };

   // Type of selection of the line.
   enum Selection { 
      SEL1       = 0,
      SEL2       = 1,
      SEL3       = 2,
      UNSELECTED ,
      NEITHER    
   };

   /*----- member functions -----*/

   // Constructor (3-way).
   // For DIFF regions who have one side blank just specify -1 for that side.
   XxLine( 
      Type  type,
      XxFln fline0,
      XxFln fline1,
      XxFln fline2 = -1
   );

   // Returns the line number in the file NOT in the diffs.  If the line for
   // the requested file nb. does not correspond to any actual file text, return
   // -1.
   XxFln getLineNo( const XxFno no ) const;

   // Get the type.
   Type getType() const;

   // Get/set selection.
   // <group>
   Selection getSelection() const;
   void setSelection( Selection selection );
   // </group>

   // Returns the file no and line number of the selected text.  If the selected
   // text has no content, no will be set to -1.  Returns false if the content
   // is unknown, true otherwise.
   bool getSelectedText( XxFno& no, XxFln& fline ) const;

   // Compute (initialize) horizontal brackets.
   void initializeHorizontalDiff( 
      const bool  ignoreWs,
      const char* text0,
      const uint  len0,
      const char* text1,
      const uint  len1,
      const char* text2 = 0,
      const uint  len2 = 0
   );

   // Returns true if this line has horizontal diffs.
   bool hasHorizontalDiffs( const XxFno no ) const;

   // Returns the left/right horizontal diff position (in chars) for side no.
   // <group>
   int getLeftHdiffPos( const XxFno no ) const;
   int getRightHdiffPos( const XxFno no ) const;
   // </group>

   // Get/set the hunk id.
   // <group>
   XxHunk getHunkId() const;
   void setHunkId( XxHunk hunkId );
   // </group>

   // Gets the split part of a line, for a certain side.
   XxLine getSplit( const XxFno no ) const;

   // Builds a promoted line--that is, a line that has been promoted to a
   // different type with an empty side.  This surely seems weird, and it is
   // used during merging of lines.
   XxLine getPromoted( Type promoteType ) const;

   // Get display color for line text.  Returns the resource for the background
   // color.
   void getLineColorType(
      const XxIgnoreFile ignoreFile,
      const XxFno        no,
      XxColor&           dtype,
      XxColor&           dtypeSup
   ) const;

   /*----- static member functions -----*/

   // Returns true if the two types are part of the same region.
   static bool isSameRegion( Type type1, Type type2 );

   // Joins two or three lines.
   // <group>
   static XxLine join(
      const XxLine& line1,
      const XxLine& line2
   );
   static XxLine join(
      const XxLine& line1,
      const XxLine& line2,
      const XxLine& line3
   );
   // </group>

   // Debugging output functions.
   // <group>
   static QString mapToString( Type type );
   static QString mapToString( Selection selection );
   // </group>

   // Maps a line type to a file no, when possible. Returns -1 if none makes
   // sense.
   static XxFno mapTypeToFileNo( Type type );

private:

   /*----- member functions -----*/

   void init();

   // Implementations of the line color type algorithms.
   // <group>
   bool getLineColorIfSelected(
      const XxFno no,
      XxColor&    dtype,
      XxColor&    dtypeSup
   ) const;
   void getLineColorTypeStd(
      const XxLine::Type newType,
      const XxFno        no,
      XxColor&           dtype,
      XxColor&           dtypeSup
   ) const;
   // </group>

   /*----- static data members -----*/

   static Type _ignoreConvertTables[ 4 ][ NB_TYPES ];

   /*----- data members -----*/

   XxFln	_lineNo[3];
   Type		_type;
   Selection	_selection;
   XxHunk	_hunkId; // This is used for split/swap/join.

   // horizontal diff brackets

   // This is the first different character (may be length)
   int		_lefthd[3];
   // This is the last different character (may be -1)
   int		_righthd[3];

   // This means that a line without horizontal differences will have
   // (righthd-1) == lefthd.

};

//------------------------------------------------------------------------------
//
std::ostream& operator << ( 
   std::ostream& os, 
   const XxLine& line 
);


XX_NAMESPACE_END

#include <line.inline.h>

#endif

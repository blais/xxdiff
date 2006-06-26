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

#ifndef INCL_STD_IOSTREAM
#include <iostream>
#define INCL_STD_IOSTREAM
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


   // enum Misc {
   //    NUM_MASK  = 0x03,
   //    TYPE_MASK = 0xFC
   //    DIFF_TYPE = 1 << 2,
   //    DELETE_TYPE = 2 << 2,
   //    INSERT_TYPE = 3 << 2,
   //    DIFFDEL_TYPE = 4 << 2,
   // };

   enum Type { 
      SAME        /*= (0 << 2)*/,
      DIFF_1      /*= (1 << 2) + 0*/,
      DIFF_2      /*= (1 << 2) + 1*/,
      DIFF_3      /*= (1 << 2) + 2*/,
      DELETE_1    /*= (2 << 2) + 0*/,
      DELETE_2    /*= (2 << 2) + 1*/,
      DELETE_3    /*= (2 << 2) + 2*/,
      INSERT_1    /*= (3 << 2) + 0*/,
      INSERT_2    /*= (3 << 2) + 1*/,
      INSERT_3    /*= (3 << 2) + 2*/,
      DIFF_ALL    /*= (4 << 2)*/,
      DIFFDEL_1   /*= (5 << 2) + 0*/,
      DIFFDEL_2   /*= (5 << 2) + 1*/,
      DIFFDEL_3   /*= (5 << 2) + 2*/,
      DIRECTORIES /*= (6 << 2)*/,
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

   // Default constructor, initialize to an invalid line.
   XxLine();

   // Copy constructor.
   XxLine( const XxLine& copy );

   // Constructor (3-way).
   // For DIFF regions who have one side blank just specify -1 for that side.
   XxLine( 
      Type  type,
      XxFln fline0,
      XxFln fline1,
      XxFln fline2 = -1
   );

   // Destructor, non-virtual on purpose (gotta keep things small, this goes
   // into a vector).
   ~XxLine();

   // Returns the line number in the file NOT in the diffs.  If the line for
   // the requested file nb. does not correspond to any actual file text, return
   // -1.
   XxFln getLineNo( const XxFno no ) const;

   // Allows one to change a line no. Do not use this, this is meant to be used
   // by the reindexing code only, for the unmerged support.
   void setLineNo( const XxFno no, const XxFln fline );

   // Get the type.
   Type getType() const;

   // Get/set selection.
   // <group>
   Selection getSelection() const;
   void setSelection( Selection selection );
   // </group>

   // Return true if the given file is selected.  This takes care of checking
   // for the line type, and if multiple sides end up being selected, it returns
   // true for all of them.
   bool isSelected( XxFno fno ) const;

   // Get/set ignore-display flag.
   // <group>
   bool getIgnoreDisplay() const;
   void setIgnoreDisplay( const bool f );
   // </group>

   // Returns the file no and line number of the selected text.  If the selected
   // text has no content, no will be set to -1.  Returns false if the content
   // is unknown, true otherwise.
   bool getSelectedText( XxFno& no, XxFln& fline ) const;

   // Compute (initialize) horizontal brackets.
   void initializeHorizontalDiff( 
      const XxResources& resources,
      const char*        text[3],
      const uint         len[3]
   );

   // Returns true if this line has horizontal diffs.
   bool hasHorizontalDiffs( const XxFno no ) const;

   // Returns the left/right horizontal diff list for a particular file.  Format
   // is a list of ints, and could be possibly null, ends with -1 element if not
   // null, and there is always an even number of valid brackets.
   // <group>
   const int* getHorDiffs( const XxFno no ) const;
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
      XxColor&           dtypeSup,
      const bool         perHunkWsEnabled
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
      XxColor&           dtypeSup,
      const bool         perHunkWsEnabled
   ) const;
   // </group>

   /*----- static data members -----*/

   // Table used to map types for changing colors for the "ignore one file"
   // feature. This is not related to the ignore-display feature.
   static Type _ignoreConvertTables[ 4 ][ NB_TYPES ];

   // Table that specifies which columns are selected when a specific selection
   // is made.
   static int _selectMasks[ XxLine::NB_TYPES ][ 3 ];

   /*----- data members -----*/

   XxFln       _lineNo[3];
   Type        _type;
   Selection   _selection;
   XxHunk      _hunkId; // This is used for split/swap/join.

   // Note: we could join the selection and per-hunk ws members into a
   // generalized "flags" member to save memory.
   bool        _perHunkWs;

   // Horizontal diff brackets.
   const int*  _hordiffs[3];

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

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

#include <diffs.h>
#include <buffer.h>
#include <resources.h>

#include <qcstring.h>

#include <list>
#include <algorithm>
#include <iostream>
#include <stdio.h> // Should be cstdio but SGI doesn't support it.

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

//------------------------------------------------------------------------------
//
int outputLine(
   QTextStream&                   os,
   const std::auto_ptr<XxBuffer>* files,
   const XxLine&                  line,
   int                            no
)
{
   int fline = line.getLineNo( no );
   XX_ASSERT( fline != 0 );
   if ( fline != -1 ) {
      uint len;
      const char* text = files[no]->getTextLine( fline, len );
      XX_ASSERT( text != 0 ); // make this one throw

      os.writeRawBytes( text, len );
      os << endl;
      return 1;
   }
   return 0;
}

//------------------------------------------------------------------------------
//
inline int fillBufWithChars(
   const XxDiffs& diffs,
   const XxFno    fno,
   XxDln&         l1,
   uint&          c1,
   const char*&   t1,
   uint&          len1,
   char*          buf1,
   bool&          done1,
   XxBuffer&      buffer,
   XxLine::Type   ctype
)
{
#define COMPBUFSIZE 40

   int nbc1 = 0;
   while ( 1 ) {

      while ( c1 < len1 && nbc1 < COMPBUFSIZE ) {
         const char c = t1[c1++];
         if ( !isspace( c ) ) {
            buf1[nbc1++] = c;
         }
      }

      if ( nbc1 == COMPBUFSIZE ) {
         break;
      }

      // Change line.
      ++l1;
      if ( l1 > diffs.getNbLines() ) {
         buf1[nbc1] = 0; // finished.
         c1 = len1 = 0;
         done1 = true;
         break;
      }

      const XxLine& cline = diffs.getLine( l1 );
      XxFln fl1 = cline.getLineNo( fno );
      if ( diffs.getLine( l1 ).getType() != ctype || fl1 == -1 ) {
         buf1[nbc1] = 0; // finished.
         c1 = len1 = 0;
         done1 = true;
         break;
      }

      t1 = buffer.getTextLine( fl1, len1 );
      c1 = 0;
   }

   return nbc1;
}

}

XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxDiffs::SearchResult
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxDiffs::SearchResult::SearchResult() :
   _lineNo( -1 )
{
   _fline[0] = _fline[1] = _fline[2] = -1;
}

//------------------------------------------------------------------------------
//
XxDiffs::SearchResult::SearchResult(
   const XxDln lineNo,
   const XxFln fline[3]
) :
   _lineNo( lineNo )
{
   _fline[0] = fline[0];
   _fline[1] = fline[1];
   _fline[2] = fline[2];
}

//------------------------------------------------------------------------------
//
bool XxDiffs::SearchResult::isValid() const
{
   return _lineNo != -1;
}



/*==============================================================================
 * CLASS XxDiffs
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxDiffs::XxDiffs() :
   _lastInitializedDiffType( HD_NONE ),
   _lastInitializedContext( 0 ),
   _dirty( false )
{
   // Insure that at the very minimum there are 0 lines (see getNbLines()).
   _lines.push_back( XxLine() );
}

//------------------------------------------------------------------------------
//
XxDiffs::XxDiffs(
   std::vector<XxLine>& lines,
   bool                 isDirectoryDiff,
   bool                 XX_DEBUG_COMPILE( validate )
 ) :
   _lastInitializedDiffType( HD_NONE ),
   _lastInitializedContext( 0 ),
   _isDirectoryDiff( isDirectoryDiff ),
   _dirty( false )
{
   std::swap( _lines, lines );
   if ( _lines.size() == 0 ) {
      // Insure that at the very minimum there are 0 lines (see getNbLines()).
      _lines.push_back( XxLine() );
   }
#ifdef XX_DEBUG
   if ( validate ) {
      validateLineNumbers();
   }
#endif
}

//------------------------------------------------------------------------------
//
XxDiffs::~XxDiffs()
{}

//------------------------------------------------------------------------------
//
XxDln XxDiffs::getNbLinesWithContent( uint& unselectedChanges ) const
{
   uint nbLines = 0;
   bool prevKnown = true;
   int prevfline = -1;

   unselectedChanges = 0;
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {

      int no, fline;
      bool known = getLine( ii ).getSelectedText( no, fline );
      if ( known == true ) {
         if ( fline != -1 ) {
            nbLines++;
         }
         else if ( prevfline != -1 ) {
            // nothing.
         }
      }
      prevfline = fline;

      // Detect changes and insert marker.
      if ( prevKnown != known && known == false ) {
         unselectedChanges++;
      }
      prevKnown = known;
   }
   return nbLines;
}

//------------------------------------------------------------------------------
//
XxFln XxDiffs::getNbLinesWithText( const XxFno fno ) const
{
   int nbLines = 0;
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLine( ii );
      if ( line.getLineNo( fno ) != -1 ) {
         ++nbLines;
      }
   }
   return nbLines;
}

//------------------------------------------------------------------------------
//
XxDln XxDiffs::moveBackwardsVisibleLines(
   XxDln startingLine,
   uint  nbLines,
   uint  unselChangesLines
) const
{
   uint nbVisibleLines = 0;
   bool prevKnown = true;
   int prevfline = -1;

   XxDln curLine = startingLine;
   while ( curLine > 0 && nbVisibleLines < nbLines ) {

      XxFno no;
      XxFln fline;
      bool known = getLine( curLine ).getSelectedText( no, fline );
      if ( known == true ) {
         if ( fline != -1 ) {
            nbVisibleLines++;
         }
         else if ( prevfline != -1 ) {
            // nothing.
         }
      }
      prevfline = fline;

      // Detect changes and insert marker.
      if ( prevKnown != known && known == false ) {
         nbVisibleLines += unselChangesLines;
      }
      prevKnown = known;

      curLine--;
   }

   return std::min( XxDln(getNbLines()), curLine + 1 );
}

//------------------------------------------------------------------------------
//
void XxDiffs::selectLine( XxDln lineNo, XxLine::Selection selection )
{
   if ( lineNo > getNbLines() ) {
      return;
   }

   if ( _isDirectoryDiff ) {
      return;
   }

   XxLine& line = getLineNC( lineNo );
   line.setSelection( selection );

   _dirty = true;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxDiffs::selectRegion( XxDln lineNo, XxLine::Selection selection )
{
   if ( lineNo > getNbLines() ) {
      return;
   }

   if ( _isDirectoryDiff ) {
      return;
   }

   XxLine& line = getLineNC( lineNo );
   XxLine::Type type = line.getType();
   if ( type == XxLine::SAME ||
        type == XxLine::DIRECTORIES ) {
      return;
   }

   XxDln start, end;
   findRegion( lineNo, start, end );

   // Move backwards (including selected line).
   for ( XxDln ii = start; ii <= end; ++ii ) {
      XxLine& line = getLineNC( ii );
      line.setSelection( selection );
      XX_ASSERT( line.getType() != XxLine::SAME );
      XX_ASSERT( line.getType() != XxLine::DIRECTORIES );
   }

   _dirty = true;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxDiffs::selectGlobal( XxLine::Selection selection )
{
   if ( _isDirectoryDiff ) {
      return;
   }

   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      XxLine& line = getLineNC( ii );
      line.setSelection( selection );
   }

   _dirty = true;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxDiffs::selectGlobalUnselected( XxLine::Selection selection )
{
   XX_ASSERT( selection != XxLine::UNSELECTED );
   if ( _isDirectoryDiff ) {
      return;
   }

   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      XxLine& line = getLineNC( ii );
      if ( line.getSelection() == XxLine::UNSELECTED ) {
         line.setSelection( selection );
      }
   }

   _dirty = true;
   emit changed();
}

//------------------------------------------------------------------------------
//
XxLine::Type XxDiffs::findRegion(
   XxDln  lineNo,
   XxDln& regionStart,
   XxDln& regionEnd
) const
{
   XX_ASSERT( lineNo <= getNbLines() );

   const XxLine& rline = getLine( lineNo );
   XxLine::Type type = rline.getType();

   // Move backwards (including selected line).
   XxDln cur = lineNo;
   while ( cur > 0 ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), type ) ||
           rline.getHunkId() != line.getHunkId() ) {
         break;
      }
      cur--;
   }
   regionStart = cur + 1;

   // Move forwards.
   cur = lineNo + 1;
   while ( cur <= getNbLines() ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), type ) ||
           rline.getHunkId() != line.getHunkId() ) {
         break;
      }
      cur++;
   }
   regionEnd = cur - 1;
   return type;
}

//------------------------------------------------------------------------------
//
XxLine::Type XxDiffs::findRegionWithSel(
   XxDln  lineNo,
   XxDln& regionStart,
   XxDln& regionEnd
) const
{
   XX_ASSERT( lineNo <= getNbLines() );

   const XxLine& rline = getLine( lineNo );
   XxLine::Type type = rline.getType();

   // Move backwards (including selected line).
   XxDln cur = lineNo;
   while ( cur > 0 ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), type ) ||
           rline.getHunkId() != line.getHunkId() ||
           rline.getSelection() != line.getSelection() ) {
         break;
      }
      cur--;
   }
   regionStart = cur + 1;

   // Move forwards.
   cur = lineNo + 1;
   while ( cur <= getNbLines() ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), type ) ||
           rline.getHunkId() != line.getHunkId() ||
           rline.getSelection() != line.getSelection() ) {
         break;
      }
      cur++;
   }
   regionEnd = cur - 1;
   return type;
}

//------------------------------------------------------------------------------
//
XxDln XxDiffs::findNextDifference( XxDln lineNo ) const
{
   if ( lineNo > getNbLines() ) {
      return -1;
   }

   const XxLine& line = getLine( lineNo );
   XxLine::Type type = line.getType();

   // Seek to end of current region.
   XxDln cur = lineNo + 1;
   while ( cur <= getNbLines() ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), type ) ) {
         break;
      }
      cur++;
   }
   if ( cur > getNbLines() ) {
      return -1;
   }

   // Seek to next difference.
   while ( cur <= getNbLines() ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), XxLine::SAME ) ) {
         break;
      }
      cur++;
   }
   if ( cur > getNbLines() ) {
      return -1;
   }

   return cur;
}

//------------------------------------------------------------------------------
//
XxDln XxDiffs::findPreviousDifference( XxDln lineNo ) const
{
   if ( lineNo > getNbLines() ) {
      return -1;
   }

   const XxLine& line = getLine( lineNo );
   XxLine::Type type = line.getType();

   // Seek to beginning of current region.
   XxDln cur = lineNo - 1;
   while ( cur > 0 ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), type ) ) {
         break;
      }
      cur--;
   }
   if ( cur == 0 ) {
      return -1;
   }

   // Seek to next difference.
   while ( cur > 0 ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), XxLine::SAME ) ) {
         break;
      }
      cur--;
   }
   if ( cur == 0 ) {
      return -1;
   }
   return cur;
}

//------------------------------------------------------------------------------
//
XxDln XxDiffs::findNextUnselected( XxDln lineNo ) const
{
   // Note: lineNo could be outside boundaries in order to search for the
   // first/last unselected difference.

   XxDln cur = lineNo;
   if ( 1 <= lineNo && lineNo <= getNbLines() ) {
      // Skip current selection block.
      const XxLine& line = getLine( lineNo );
      XxLine::Type type = line.getType();
      XxLine::Selection sel = line.getSelection();

      cur = lineNo;
      while ( cur <= getNbLines() ) {
         const XxLine& line = getLine( cur );
         if ( !XxLine::isSameRegion( line.getType(), type ) ||
              line.getSelection() != sel ) {
            break;
         }
         cur++;
      }
   }

   // Seek to next unselected line.
   cur = std::max( 1, cur );
   while ( cur <= getNbLines() ) {
      const XxLine& line = getLine( cur );
      if ( line.getType() != XxLine::SAME &&
           line.getType() != XxLine::DIRECTORIES &&
           line.getSelection() == XxLine::UNSELECTED ) {
         break;
      }
      cur++;
   }
   if ( cur > getNbLines() ) {
      return -1;
   }
   return cur;
}

//------------------------------------------------------------------------------
//
XxDln XxDiffs::findPreviousUnselected( XxDln lineNo ) const
{
   // Note: lineNo could be outside boundaries in order to search for the
   // first/last unselected difference.

   XxDln cur = lineNo;
   if ( 1 <= lineNo && lineNo <= getNbLines() ) {
      // Skip current selection block.
      const XxLine& line = getLine( lineNo );
      XxLine::Type type = line.getType();
      XxLine::Selection sel = line.getSelection();

      cur = lineNo;
      while ( cur > 0 ) {
         const XxLine& line = getLine( cur );
         if ( !XxLine::isSameRegion( line.getType(), type ) ||
              line.getSelection() != sel ) {
            break;
         }
         cur--;
      }
   }

   // Seek to next unselected line.
   cur = std::min( getNbLines(), cur );
   while ( cur > 0 ) {
      const XxLine& line = getLine( cur );
      if ( line.getType() != XxLine::SAME &&
           line.getType() != XxLine::DIRECTORIES &&
           line.getSelection() == XxLine::UNSELECTED ) {
         break;
      }
      cur--;
   }
   if ( cur <= 0 ) {
      return -1;
   }
   return cur;
}


//------------------------------------------------------------------------------
//
XxDln XxDiffs::findNextNonSameLine( XxDln lineNo ) const
{
   if ( lineNo > getNbLines() ) {
      return -1;
   }

   // Seek to next line that is not same.
   XxDln cur = lineNo + 1;
   while ( cur <= getNbLines() ) {
      const XxLine& line = getLine( cur );
      if ( line.getType() != XxLine::SAME ) {
         break;
      }
      cur++;
   }
   if ( cur > getNbLines() ) {
      return -1;
   }

   return cur;
}

//------------------------------------------------------------------------------
//
uint XxDiffs::getNbFileLines(
   XxFno no,
   XxDln start,
   XxDln end
) const
{
   XX_ASSERT( no == 0 || no == 1 || no == 2 );
   XX_ASSERT( start <= getNbLines() );
   XX_ASSERT( end <= getNbLines() );
   XX_ASSERT( start <= end );

   int count = 0;
   for ( XxDln ii = start; ii <= end; ++ii ) {
      const XxLine& line = getLine( ii );
      if ( line.getLineNo( no ) != -1 ) {
         count++;
      }
   }
   return count;
}

//------------------------------------------------------------------------------
//
XxFln XxDiffs::getBufferLine(
   XxFno no,
   XxDln lineNo,
   bool& actuallyEmpty
) const
{
   XX_ASSERT( no == 0 || no == 1 || no == 2 );
   if ( getNbLines() < 1 ) {
      actuallyEmpty = true;
      return 0;
   }
   actuallyEmpty = false;

   if ( lineNo > getNbLines() ) {
      actuallyEmpty = true;
      return 0;
   }

   // This is one place where our data structure is annoying and we have to do a
   // stupid search.

   // Look backward, empty regions will get assigned the first encountered line
   // number.
   XxFln fline = -1;
   XxDln ii;
   for ( ii = lineNo; ii > 0; --ii ) {
      const XxLine& line = getLine( ii );
      fline = line.getLineNo( no );
      if ( fline != -1 ) {
         break;
      }
   }
   if ( ii != lineNo ) {
      actuallyEmpty = true;
   }

   // In the extremely unlikely situation where no line was found by looking
   // backwards, look forward.
   if ( fline == -1 ) {
      actuallyEmpty = true;
      for ( ii = lineNo; ii <= getNbLines(); ++ii ) {
         const XxLine& line = getLine( ii );
         fline = line.getLineNo( no );
         if ( fline != -1 ) {
            break;
         }
      }
   }

   // The only case where no valid line has been found is when a file is empty.
   return fline;
}

//------------------------------------------------------------------------------
//
std::ostream& XxDiffs::dump( std::ostream& os ) const
{
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      os << _lines[ii] << std::endl;
   }
   return os;
}

//------------------------------------------------------------------------------
//
bool XxDiffs::isSomeSelected() const
{
   // Look for a selected line.
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLine( ii );
      XxLine::Type type = line.getType();
      if ( type != XxLine::SAME &&
           type != XxLine::DIRECTORIES ) {
         XxLine::Selection selection = line.getSelection();
         if ( selection != XxLine::UNSELECTED ) {
            return true;
         }
      }
   }
   return false;
}

//------------------------------------------------------------------------------
//
bool XxDiffs::hasSelectionsOtherThan( XxFno no ) const
{
   // Look for a selected line.
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLine( ii );
      XxLine::Type type = line.getType();
      if ( type != XxLine::SAME &&
           type != XxLine::DIRECTORIES ) {

         if ( ! line.isSelected( no ) && 
              ( line.getSelection() == XxLine::SEL1 ||
                line.getSelection() == XxLine::SEL2 ||
                line.getSelection() == XxLine::SEL3 ) ) {

            return true;
         }
      }
   }
   return false;
}

//------------------------------------------------------------------------------
//
bool XxDiffs::isAllSelected() const
{
   // Make sure that there are no more unselected regions.
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLine( ii );
      XxLine::Type type = line.getType();
      if ( type != XxLine::SAME &&
           type != XxLine::DIRECTORIES ) {
         XxLine::Selection selection = line.getSelection();
         if ( selection == XxLine::UNSELECTED ) {
            return false;
         }
      }
   }
   return true;
}

//------------------------------------------------------------------------------
//
uint XxDiffs::countRemainingUnselected() const
{
   // Make sure that there are no more unselected regions.
   bool inhunk = false;
   XxHunk curHunk = 0;
   XxLine::Type curType = XxLine::SAME; /* just to make compiler happy */
   uint nbunsel = 0;
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLine( ii );
      XxLine::Type type = line.getType();

      /*XX_TRACE( "no " << ii << " - type " << type << " - hunk " << inhunk );*/

      if ( !inhunk ) {

         if ( type != XxLine::SAME &&
              type != XxLine::DIRECTORIES &&
              line.getSelection() == XxLine::UNSELECTED ) {

            ++nbunsel;
            inhunk = true;
            curHunk = line.getHunkId();
            curType = type;

            /*XX_TRACE( "entering hunk" );*/
         }
      }
      else {

         if ( type == XxLine::SAME ||
              type == XxLine::DIRECTORIES ||
              line.getSelection() != XxLine::UNSELECTED ) {

            inhunk = false;
            /*XX_TRACE( "leaving hunk" );*/
         }
         else if ( line.getHunkId() != curHunk || line.getType() != curType ) {

            ++nbunsel;
            curHunk = line.getHunkId();
            curType = line.getType();
            /*XX_TRACE( "switching hunks" );*/
         }
      }
   }
   return nbunsel;
}

//------------------------------------------------------------------------------
//
bool XxDiffs::save(
   const XxResources&             resources,
   QTextStream&                   os,
   const std::auto_ptr<XxBuffer>* files,
   const bool                     useConditionals,
   const bool                     removeEmptyConditionals,
   const QString                  conditionals[3]
) const
{
   XX_ASSERT( files != 0 );
   XX_ASSERT( files[0].get() != 0 );
   XX_ASSERT( files[1].get() != 0 );
   int nbFiles = files[2].get() != 0 ? 3 : 2;

   QString tags[4];
   if ( useConditionals == false ) {
      for ( int ii = 0; ii < 3; ++ii ) {
         tags[ii] = resources.getTag( TAG_CONFLICT_SEPARATOR );
      }
      tags[3] = resources.getTag( TAG_CONFLICT_END );
   }
   else {
      tags[0] = resources.getTag( TAG_CONDITIONAL_IF );
      tags[1] = resources.getTag( TAG_CONDITIONAL_ELSEIF );
      tags[2] = resources.getTag( TAG_CONDITIONAL_ELSE );
      tags[3] = resources.getTag( TAG_CONDITIONAL_ENDIF );
   }

   for ( int ii = 0; ii < nbFiles; ++ii ) {
      int pos = tags[ii].find( "%d" );
      if ( pos != -1 ) {
         tags[ii].replace( pos, 2, QString::number( ii+1 ) );
      }

      if ( ! useConditionals ) {
         pos = tags[ii].find( "%s" );
         if ( pos != -1 ) {
            tags[ii].replace( pos, 2, files[ii]->getDisplayName() );
         }
      }
   }

   // for ( int ii = 0; ii < nbFiles+1; ++ii ) {
   //    XX_TRACE( tags[ii].latin1() );
   // }

   bool foundUnsel = false;
   bool insideUnsel = false;
   uint unselBegin = 0;
   uint unselEnd;
   XxDln uii;
   enum State { IF = 0, ELSIF = 1, ELSE = 2 };
   for ( uii = 1; uii <= getNbLines(); ++uii ) {
      const XxLine& line = getLine( uii );

      XxLine::Selection selection = line.getSelection();
      if ( line.getType() == XxLine::SAME ||
           line.getType() == XxLine::DIRECTORIES ||
           selection != XxLine::UNSELECTED ) {

         if ( insideUnsel == true ) {
            // Output the unselected portion.
            unselEnd = uii;
            XX_ASSERT( unselEnd - unselBegin > 0 );
            int state = IF;
            for ( uint f = 0; f < 3; ++f ) {
               if ( files[f].get() != 0 ) {

                  // Note: I know this is lame, but SGI STL doesn't support
                  // formatted output from ostream.
                  QString cond;
                  cond.sprintf( tags[state].latin1(),
                                conditionals[f].latin1() );

                  QByteArray line;
                  QTextOStream oss( line );
                  oss << cond << endl;

                  int nbOutlines = 0;
                  for ( uint iii = unselBegin; iii < unselEnd; ++iii ) {
                     const XxLine& cline = getLine( iii );
                     nbOutlines += outputLine( oss, files, cline, f );
                  }

                  if ( removeEmptyConditionals && nbOutlines == 0 ) {
                     continue;
                  }

                  if ( useConditionals ) {
                     switch ( state ) {
                        case IF: state = ELSIF; break;
                        case ELSIF: /* stay as ELSIF */ break;
                        case ELSE: break;
                     }
                  }
                  else {
                     ++state;
                  }
                  oss << '\0' << flush; // end string and flush
                  os << static_cast<const char*>( line );
               }
            }

            os << tags[3] << endl;

            insideUnsel = false;
         }

         if ( selection != XxLine::NEITHER ) {
            int no = int(selection);
            if ( line.getType() == XxLine::SAME ||
                 line.getType() == XxLine::DIRECTORIES ) {
               no = 0;
            }
            outputLine( os, files, line, no );
         }
         // else, for the NEITHER case, don't output anything.
      }
      else {
         // Found the beginning of an unselected block.
         if ( insideUnsel == false ) {
            insideUnsel = true;
            unselBegin = uii;
         }
         foundUnsel = true;
      }
   }

   // If file ends with a hunk, make sure a pending unselected hunk is output.
   if ( insideUnsel == true ) {
      // Output the unselected portion.
      unselEnd = uii;
      XX_ASSERT( unselEnd - unselBegin > 0 );
      for ( uint f = 0; f < 3; ++f ) {
         if ( files[f].get() != 0 ) {

            os << tags[f] << endl;

            for ( uint iii = unselBegin; iii < unselEnd; ++iii ) {
               const XxLine& cline = getLine( iii );
               outputLine( os, files, cline, f );
            }
         }
      }

      os << tags[nbFiles] << endl;

      insideUnsel = false;
   }

   return !foundUnsel;
}

//------------------------------------------------------------------------------
//
bool XxDiffs::saveSelectedOnly(
   QTextStream&                   os,
   const std::auto_ptr<XxBuffer>* files
) const
{
   XX_ASSERT( files != 0 );
   XX_ASSERT( files[0].get() != 0 );
   XX_ASSERT( files[1].get() != 0 );

   bool some = false;
   bool prevOut = false;
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLine( ii );
      XxLine::Selection selection = line.getSelection();
      if ( selection == XxLine::SEL1 ||
           selection == XxLine::SEL2 ||
           selection == XxLine::SEL3 ) {

         int no = int(selection);
         int fline = line.getLineNo( no );
         XX_ASSERT( fline != 0 );
         if ( fline != -1 ) {
            uint len;
            const char* text = files[no]->getTextLine( fline, len );
            XX_ASSERT( text != 0 ); // make this one throw

            os << ( no == 0 ? '<' : '>' ) << fline << ": ";
            os.writeRawBytes( text, len );
            os << endl;

            some = true;
            prevOut = true;
         }
      }
      else if ( prevOut == true ) {
         os << endl;
         prevOut = false;
      }
   }
   return some;
}

//------------------------------------------------------------------------------
//
void XxDiffs::search(
   const QString&                 searchText,
   const int                      nbFiles,
   const std::auto_ptr<XxBuffer>* files
)
{
   XX_ASSERT( files != 0 );
   for ( int iii = 0; iii < nbFiles; ++iii ) {
      XX_ASSERT( files[iii].get() != 0 );
   }

   // Note: we could easily avoid the stupid buffer copy by implementing
   // what would be strnstr.

   _searchResults.clear();

   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLineNC( ii );
      SearchResult scurrent; // init as invalid

      // Look in all files.
      for ( int ni = 0; ni < nbFiles; ++ni ) {

         int fline = line.getLineNo( ni );
         if ( fline != -1 ) {
            if ( files[ni]->searchLine( fline, searchText ) == true ) {
               // We have a hit.
               scurrent._lineNo = ii;
               scurrent._fline[ni] = fline;
            }
         }
      }

      if ( scurrent.isValid() ) {
         _searchResults.push_back( scurrent );
      }
   }

   // This is just used to trigger overview area redraw.
   emit changed();
}

//------------------------------------------------------------------------------
//
const std::vector<XxDiffs::SearchResult>& XxDiffs::getSearchResults() const
{
   return _searchResults;
}

//------------------------------------------------------------------------------
//
XxDiffs::SearchResult XxDiffs::findNextSearch( XxDln lineNo ) const
{
   if ( lineNo < 1 || lineNo > getNbLines() ) {
      return SearchResult();
   }

   // Stupid linear search.
   for ( uint ii = 0; ii < _searchResults.size(); ++ii ) {
      if ( _searchResults[ii]._lineNo > XxDln(lineNo) ) {
         return _searchResults[ii];
      }
   }
   // else return invalid.
   return SearchResult();
}

//------------------------------------------------------------------------------
//
XxDiffs::SearchResult XxDiffs::findPreviousSearch( XxDln lineNo ) const
{
   if ( lineNo < 1 || lineNo > getNbLines() ) {
      return SearchResult();
   }

   // Stupid linear search.
   for ( int ii = _searchResults.size() - 1; ii >= 0; --ii ) {
      if ( _searchResults[ii]._lineNo < XxDln(lineNo) ) {
         return _searchResults[ii];
      }
   }
   // else return invalid.
   return SearchResult();
}

//------------------------------------------------------------------------------
//
bool XxDiffs::splitSwapJoin( XxDln lineNo, uint nbFiles )
{
   if ( lineNo < 1 || lineNo > getNbLines() ) {
      return false;
   }

   bool wasJoin = false;

   // Find contiguous hunks that are not SAME with same hunk id.  There should
   // be three at most (we cannot split to more than three).
   std::vector<XxDln> lstart;
   std::vector<XxDln> lend;
   XxDln start, end;

   // Save hunk id we're restricted to.
   XxHunk hunkId = getLine( lineNo ).getHunkId();

   // Find current hunk.
   XxLine::Type type = findRegion( lineNo, start, end );
   if ( type == XxLine::SAME ||
        type == XxLine::DIRECTORIES ) {
      return false;
   }
   lstart.push_back( start );
   lend.push_back( end );

   // Look for hunks before current hunk.
   while ( start > 1 ) {
      if ( getLine( start - 1 ).getHunkId() == hunkId ) {
         findRegion( start - 1, start, end );
         lstart.insert( lstart.begin(), start );
         lend.insert( lend.begin(), end );
      }
      else {
         break;
      }
   }

   // Look for hunks after current hunk.
   end = lend.back();
   while ( end < XxDln(getNbLines()) ) {
      if ( getLine( end + 1 ).getHunkId() == hunkId ) {
         findRegion( end + 1, start, end );
         lstart.push_back( start );
         lend.push_back( end );
      }
      else {
         break;
      }
   }

   int nbHunks = lstart.size();

   std::vector<XxLine> newLines;

   if ( nbHunks == 1 ) {

      switch ( getLine( *( lstart.begin() ) ).getType() ) {
         case XxLine::SAME:
         case XxLine::DIRECTORIES:
         case XxLine::DELETE_1:
         case XxLine::DELETE_2:
         case XxLine::DELETE_3:
         case XxLine::INSERT_1:
         case XxLine::INSERT_2:
         case XxLine::INSERT_3: {
            return false;
         } break;

         case XxLine::DIFF_1: {
            splitTwoRegions( newLines, lstart[0], lend[0], 0, 1 );
         } break;

         case XxLine::DIFF_2: {
            splitTwoRegions( newLines, lstart[0], lend[0], 1, 0 );
         } break;

         case XxLine::DIFF_3: {
            splitTwoRegions( newLines, lstart[0], lend[0], 2, 0 );
         } break;

         case XxLine::DIFFDEL_1: {
            splitTwoRegions( newLines, lstart[0], lend[0], 1, 2 );
         } break;

         case XxLine::DIFFDEL_2: {
            splitTwoRegions( newLines, lstart[0], lend[0], 0, 2 );
         } break;

         case XxLine::DIFFDEL_3: {
            splitTwoRegions( newLines, lstart[0], lend[0], 0, 1 );
         } break;

         case XxLine::DIFF_ALL: {
            splitTwoRegions( newLines, lstart[0], lend[0], 0, 1 );
            if ( nbFiles > 2 ) {
               for ( XxDln ii = lstart[0]; ii <= lend[0]; ++ii ) {
                  const XxLine& cline = getLine( ii );
                  if ( cline.getLineNo( 2 ) != -1 ) {
                     newLines.push_back( cline.getSplit( 2 ) );
                  }
               }
            }
         } break;

         default: {
         } break;
      }

   }
   else if ( nbHunks == 2 ) {
      XxLine::Type type1 = getLine( lstart[0] ).getType();
      XxLine::Type type2 = getLine( lstart[1] ).getType();
      if ( ( type1 == XxLine::INSERT_1 && type2 == XxLine::DELETE_1 ) ||
           ( type1 == XxLine::INSERT_2 && type2 == XxLine::DELETE_2 ) ||
           ( type1 == XxLine::INSERT_3 && type2 == XxLine::DELETE_3 ) ||
           ( type1 == XxLine::INSERT_1 && type2 == XxLine::INSERT_2 ) ||
           ( type1 == XxLine::INSERT_1 && type2 == XxLine::INSERT_3 ) ||
           ( type1 == XxLine::INSERT_2 && type2 == XxLine::INSERT_3 ) ) {
         // Swap two regions.
         XxDln ii;
         for ( ii = lstart[1]; ii <= lend[1]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( ii = lstart[0]; ii <= lend[0]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
      }
      else if ( ( type1 == XxLine::DELETE_1 && type2 == XxLine::INSERT_1 ) ||
                ( type1 == XxLine::DELETE_2 && type2 == XxLine::INSERT_2 ) ||
                ( type1 == XxLine::DELETE_3 && type2 == XxLine::INSERT_3 ) ||
                ( type1 == XxLine::INSERT_2 && type2 == XxLine::INSERT_1 ) ||
                ( type1 == XxLine::INSERT_3 && type2 == XxLine::INSERT_1 ) ||
                ( type1 == XxLine::INSERT_3 && type2 == XxLine::INSERT_2 ) ) {
         // Join common lines.
         wasJoin = true;

         uint len0 = lend[0] - lstart[0];
         uint len1 = lend[1] - lstart[1];
         uint nbmax = std::max( len0, len1 );
         uint nbmin = std::min( len0, len1 );
         for ( uint uii = 0; uii <= nbmin; ++uii ) {
            XxLine nline = XxLine::join( getLine( lstart[0] + uii ),
                                          getLine( lstart[1] + uii ) );
            if ( nbFiles > 2 ) {
               newLines.push_back( nline );
            }
            else {
               newLines.push_back( nline.getPromoted( XxLine::DIFF_ALL ) );
            }
         }

         // Join remainder.
         XxLine::Type promoteType = XxLine::SAME; // remove warning.
         if ( type1 == XxLine::DELETE_1 && type2 == XxLine::INSERT_1 ) {
            promoteType = XxLine::DIFF_1;
         }
         else if ( type1 == XxLine::DELETE_2 && type2 == XxLine::INSERT_2 ) {
            promoteType = XxLine::DIFF_2;
         }
         else if ( type1 == XxLine::DELETE_3 && type2 == XxLine::INSERT_3 ) {
            promoteType = XxLine::DIFF_3;
         }
         else if ( type1 == XxLine::INSERT_2 && type2 == XxLine::INSERT_1 ) {
            if ( nbFiles > 2 ) {
               promoteType = XxLine::DIFFDEL_3;
            }
            else {
               promoteType = XxLine::DIFF_ALL;
            }
         }
         else if ( type1 == XxLine::INSERT_3 && type2 == XxLine::INSERT_1 ) {
            if ( nbFiles > 2 ) {
               promoteType = XxLine::DIFFDEL_2;
            }
            else {
               promoteType = XxLine::DIFF_ALL;
            }
         }
         else if ( type1 == XxLine::INSERT_3 && type2 == XxLine::INSERT_2 ) {
            if ( nbFiles > 2 ) {
               promoteType = XxLine::DIFFDEL_1;
            }
            else {
               promoteType = XxLine::DIFF_ALL;
            }
         }

         int longerIdx = ( len0 < len1 ) ? 1 : 0;
         for ( uint ii = nbmin + 1; ii <= nbmax; ++ii ) {
            newLines.push_back(
               getLine( lstart[longerIdx] + ii ).getPromoted( promoteType )
            );
         }
      }
      else {
         XX_ASSERT( false );
      }

   }
   else if ( nbHunks == 3 ) {
      XxLine::Type type1 = getLine( lstart[0] ).getType();
      XxLine::Type type2 = getLine( lstart[1] ).getType();
      XxLine::Type type3 = getLine( lstart[2] ).getType();
      if ( ( type1 == XxLine::INSERT_1 &&
             type2 == XxLine::INSERT_2 &&
             type3 == XxLine::INSERT_3 ) ||
           ( type1 == XxLine::INSERT_2 &&
             type2 == XxLine::INSERT_1 &&
             type3 == XxLine::INSERT_3 ) ||
           ( type1 == XxLine::INSERT_3 &&
             type2 == XxLine::INSERT_1 &&
             type3 == XxLine::INSERT_2 ) ) {
         // Swap the two last regions.
         XxDln ii;
         for ( ii = lstart[0]; ii <= lend[0]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( ii = lstart[2]; ii <= lend[2]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( ii = lstart[1]; ii <= lend[1]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
      }
      else if ( type1 == XxLine::INSERT_1 &&
                type2 == XxLine::INSERT_3 &&
                type3 == XxLine::INSERT_2 ) {
         // Roll down.
         XxDln ii;
         for ( ii = lstart[2]; ii <= lend[2]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( ii = lstart[0]; ii <= lend[0]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( ii = lstart[1]; ii <= lend[1]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
      }
      else if ( type1 == XxLine::INSERT_2 &&
                type2 == XxLine::INSERT_3 &&
                type3 == XxLine::INSERT_1 ) {
         // Roll up.
         XxDln ii;
         for ( ii = lstart[1]; ii <= lend[1]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( ii = lstart[2]; ii <= lend[2]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( ii = lstart[0]; ii <= lend[0]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
      }
      else if ( type1 == XxLine::INSERT_3 &&
                type2 == XxLine::INSERT_2 &&
                type3 == XxLine::INSERT_1 ) {
         // Join common lines.
         wasJoin = true;

         uint len[3];
         uint ord[3] = { 0, 1, 2 };
         for ( int iii = 0; iii < 3; ++iii ) {
            len[iii] = lend[iii] - lstart[iii];
         }
         uint swap;

         // Sort lengths, keep orders.
         if ( len[1] < len[0] ) {
            swap = len[0]; len[0]= len[1]; len[1] = swap;
            swap = ord[0]; ord[0]= ord[1]; ord[1] = swap;
         }
         if ( len[2] < len[0] ) {
            swap = len[0]; len[0]= len[2]; len[2] = swap;
            swap = ord[0]; ord[0]= ord[2]; ord[2] = swap;
         }
         if ( len[2] < len[1] ) {
            swap = len[1]; len[1]= len[2]; len[2] = swap;
            swap = ord[1]; ord[1]= ord[2]; ord[2] = swap;
         }

         uint ii;
         for ( ii = 0; ii <= len[0]; ++ii ) {
            XxLine nline = XxLine::join(
               getLine( lstart[ord[0]] + ii ),
               getLine( lstart[ord[1]] + ii ),
               getLine( lstart[ord[2]] + ii )
            );
            newLines.push_back( nline );
         }

         for ( ii = len[0]+1; ii <= len[1]; ++ii ) {
            XxLine nline = XxLine::join(
               getLine( lstart[ord[1]] + ii ),
               getLine( lstart[ord[2]] + ii )
            );
            newLines.push_back( nline.getPromoted( XxLine::DIFF_ALL ) );
         }

         for ( ii = len[1]+1; ii <= len[2]; ++ii ) {
            XxLine nline = getLine( lstart[ord[2]] + ii );
            newLines.push_back( nline.getPromoted( XxLine::DIFF_ALL ) );
         }
      }
      else {
         XX_ASSERT( false );
      }
   }
   else {
      XX_ASSERT( false );
   }

   // Erase old lines.
   std::vector<XxLine>::iterator istart = _lines.begin();
   istart += lstart.front();
   std::vector<XxLine>::iterator iend = _lines.begin();
   iend += lend.back();

   _lines.erase( istart, iend + 1 );
   _lines.insert( istart, newLines.begin(), newLines.end() );

   emit nbLinesChanged();
   _dirty = true;
   emit changed();
   // No need to clear search results, the overview looks the same.

   // Mark for redoing the horizontal diffs.
   _lastInitializedDiffType = HD_NONE;
   _lastInitializedContext = 0;

   return wasJoin;
}

//------------------------------------------------------------------------------
//
void XxDiffs::initializeHorizontalDiffs(
   const XxResources&             resources,
   const std::auto_ptr<XxBuffer>* files,
   const bool                     force
)
{
   XX_ASSERT( files[0].get() != 0 );
   XX_ASSERT( files[1].get() != 0 );

   if ( force == true ||
        _lastInitializedDiffType != resources.getHordiffType() ||
        _lastInitializedContext != resources.getHordiffContext()
   ) {

      for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
         const char* text[3];
         uint len[3];
         XxLine& line = getLineNC( ii );
         for ( int iif = 0; iif < 3; ++iif ) {
            XxFln fline = line.getLineNo( XxFln(iif) );
            if ( fline != -1 ) {
               text[iif] = files[iif]->getTextLine( fline, len[iif] );
            }
            else {
               text[iif] = 0;
               len[iif] = 0;
            }
         }

         line.initializeHorizontalDiff( resources, text, len );
      }

      _lastInitializedDiffType = resources.getHordiffType();
      _lastInitializedContext = resources.getHordiffContext();
   }
}

//------------------------------------------------------------------------------
//
void XxDiffs::splitTwoRegions(
   std::vector<XxLine>& newLines,
   XxDln                start,
   XxDln                end,
   XxFno                s1,
   XxFno                s2
) const
{
   XxDln ii;
   for ( ii = start; ii <= end; ++ii ) {
      const XxLine& cline = getLine( ii );
      if ( cline.getLineNo( s1 ) != -1 ) {
         newLines.push_back( cline.getSplit( s1 ) );
      }
   }
   for ( ii = start; ii <= end; ++ii ) {
      const XxLine& cline = getLine( ii );
      if ( cline.getLineNo( s2 ) != -1 ) {
         newLines.push_back( cline.getSplit( s2 ) );
      }
   }
}

//------------------------------------------------------------------------------
//
int XxDiffs::merge( uint nbFiles )
{
   int result = 0;
   if ( _isDirectoryDiff ) {
      return 2;
   }
   else if ( nbFiles == 2 ) {
      // In 2-way diffs, the algorithm is simple: select insert side of all
      // inserts/deletes, and leave unselected the change regions.

      for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
         XxLine& line = getLineNC( ii );
         switch ( line.getType() ) {
            // Ignored lines.
            case XxLine::SAME:
            case XxLine::DIRECTORIES: {
               // Leave as is unselected.
            } break;

            case XxLine::DIFF_1:
            case XxLine::DIFF_2:
            case XxLine::DELETE_1:
            case XxLine::DELETE_2:
            case XxLine::DIFF_3:
            case XxLine::DELETE_3:
            case XxLine::INSERT_3:
            case XxLine::DIFFDEL_1:
            case XxLine::DIFFDEL_2:
            case XxLine::DIFFDEL_3: {
               // Leave as is unselected.
               result = 1;
            } break;

            // Left insert.
            case XxLine::INSERT_1: {
               line.setSelection( XxLine::SEL1 );
            } break;

            // Right insert.
            case XxLine::INSERT_2: {
               line.setSelection( XxLine::SEL2 );
            } break;

            // Overlaps.
            case XxLine::DIFF_ALL: {
               line.setSelection( XxLine::UNSELECTED );
               result = 1;
            } break;

            default: {
               XX_CHECK( false );
            }
         }
      }
      _dirty = true;
   }
   else if ( nbFiles == 3 ) {

      /*
       * Digested diff3 docs:
       *
       * There are two types of reported hunks by the merge feature:
       *
       *   `conflict': where only OLDER differs.
       *   `overlaps': where all three files differ.
       *
       * In all cases, where we don't select a change, we select MINE (SEL1).
       * diff3 options, those must be used along with the -m (--merge) option:
       *   -e : `selects' conflicts and overlaps
       *   -3 : `selects' conflicts only
       *   -x : `selects' overlaps only
       *   -A : `selects' conflict, overlaps and merged changes
       *
       *   -E and -X : versions of -e and -x that produce less output when not
       *               using the -m option.
       *
       * So our algorithm is simple, two choices:
       *
       *  1) first select SEL1 globally, then spawn diff3 -m, with the
       *  appropriate option, find all the edits and unselect the reported
       *  edits.  Upside: we don't perform _any_ algorithmic work.  Downside: we
       *  need to write a new parser because the output of diff3 -m is an ed
       *  script.
       *
       *  2) do it by hand and work through all the lines, selecting lines as
       *  below:
       *
       *                  diff3-m    diff3-m-e  diff3-m-3  diff3-m-x  xxdiff
       * AAA (SAME)       UNSELECTED UNSELECTED UNSELECTED UNSELECTED UNSELECTED
       * BAA (DIFF_1)     SEL1       SEL1       SEL1       SEL1       SEL1
       * ABA (DIFF_2)     conflict   SEL1/3     SEL1/3     SEL1/3     SEL1/3
       * AAB (DIFF_3)     SEL3       SEL3       SEL3       SEL1       SEL3
       * -AA (DELETE_1)   SEL1       SEL1       SEL1       SEL1       SEL1
       * A-A (DELETE_2)   conflict   SEL1/3     SEL1/3     SEL1/3     SEL1/3
       * AA- (DELETE_3)   SEL3       SEL3       SEL3       SEL1       SEL3
       * A-- (INSERT_1)   SEL1       SEL1       SEL1       SEL1       SEL1
       * -A- (INSERT_2)   conflict   SEL1/3     SEL1/3     SEL1/3     SEL1/3
       * --A (INSERT_3)   SEL3       SEL3       SEL3       SEL1       SEL3
       * ABC (DIFF_ALL)   overlap    SEL3       SEL1       SEL3       UNSELECTED
       * AB- (DIFFDEL_1)  overlap    SEL3       SEL1       SEL3       UNSELECTED
       * A-B (DIFFDEL_2)  overlap    SEL3       SEL1       SEL3       UNSELECTED
       * -AB (DIFFDEL_3)  overlap    SEL3       SEL1       SEL3       UNSELECTED
       *
       * This is way simpler to implement and more efficient.
       *
       */

      for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
         XxLine& line = getLineNC( ii );
         switch ( line.getType() ) {
            // Ignored lines.
            case XxLine::SAME:
            case XxLine::DIRECTORIES: {
               // Leave unselected.
            } break;

            // Unambiguous lines.
            case XxLine::DIFF_1:
            case XxLine::DIFF_2:
            case XxLine::DELETE_1:
            case XxLine::DELETE_2:
            case XxLine::INSERT_1:
            case XxLine::INSERT_2: {
               line.setSelection( XxLine::SEL1 );
            } break;

            // Conflicts.
            case XxLine::DIFF_3:
            case XxLine::DELETE_3:
            case XxLine::INSERT_3: {
               line.setSelection( XxLine::SEL3 );
            } break;

            // Overlaps.
            case XxLine::DIFF_ALL:
            case XxLine::DIFFDEL_1:
            case XxLine::DIFFDEL_2:
            case XxLine::DIFFDEL_3: {
               line.setSelection( XxLine::UNSELECTED );
               result = 1;
            } break;

            default: {
               XX_CHECK( false );
            }
         }
      }
      _dirty = true;
   }

   emit changed();
   return result;
}

//------------------------------------------------------------------------------
//
void XxDiffs::validateLineNumbers() const
{
   int fline[3] = { 0, 0, 0 };
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLine( ii );
      for ( XxFno l = 0; l < 3; ++l ) {
         XxFln flineNo = line.getLineNo( l );
         if ( flineNo != -1 ) {
            fline[l]++;
            XX_ASSERT( flineNo == fline[l] );
         }
      }
   }
}

//------------------------------------------------------------------------------
//
bool XxDiffs::checkSelections( const XxLine::Selection sel ) const
{
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLine( ii );

      if ( line.getType() != XxLine::SAME &&
           line.getType() != XxLine::DIRECTORIES &&
           line.getSelection() != sel ) {
         return false;
      }
   }
   return true;
}

//------------------------------------------------------------------------------
//
XxDln XxDiffs::getDisplayLine(
   const XxFln     rdline,
   const XxBuffer& buffer,
   const XxFno     fno
) const
{
   for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
      const XxLine& line = getLine( ii );

      XxFln fline = line.getLineNo( fno );
      if ( fline == -1 ) {
         continue;
      }
      XxFln dfline = buffer.getDisplayLineNo( fline );
      if ( dfline >= rdline ) {
         return ii;
      }
   }
   return -1;
}

//------------------------------------------------------------------------------
//
void XxDiffs::reindex(
   const std::auto_ptr<XxBuffer>& file1,
   const std::auto_ptr<XxBuffer>& file2,
   const std::auto_ptr<XxBuffer>& file3
)
{
   XxFno nbFiles = ( file3.get() == 0 ) ? 2 : 3;

   // Go through the diffs on each side and generate re-indexing tables.
   for ( XxFno fi = 0; fi < nbFiles; ++fi ) {
      std::vector<XxFln> reindexTbl;
      reindexTbl.push_back( -1 );
      int fline = 0;
      for ( XxDln ii = 1; ii <= getNbLines(); ++ii ) {
         XxLine& line = getLineNC( ii );
         XxFln lno = line.getLineNo( fi );
         if ( lno != -1 ) {
            reindexTbl.push_back( line.getLineNo( fi ) );
            ++fline;
            line.setLineNo( fi, fline );
         }
      }

      if ( fi == 0 && file1.get() != 0 ) {
         file1->reindex( reindexTbl );
      }
      else if ( fi == 1 && file2.get() != 0 ) {
         file2->reindex( reindexTbl );
      }
      else if ( fi == 2 && file3.get() != 0 ) {
         file3->reindex( reindexTbl );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxDiffs::computeIgnoreDisplay(
   const int                      nbFiles,
   const std::auto_ptr<XxBuffer>* files
)
{
   XxDln lineNo = 1;
   while ( 1 ) {
      lineNo = findNextDifference( lineNo );
      if ( lineNo == -1 ) {
         break;
      }

      const XxLine& cline = getLine( lineNo );
      const XxLine::Type ctype = cline.getType();

      XxFno fno0, fno1;
      int nbDiffFiles;
      if ( nbFiles == 2 ) {
         if ( ctype != XxLine::DIFF_ALL ) {
            continue;
         }
         nbDiffFiles = 2;  fno0 = 0;  fno1 = 1;
      }
      else {
         XX_ASSERT( nbFiles == 3 );

         switch ( ctype ) {
            case XxLine::DIFF_ALL:
               nbDiffFiles = 3;  fno0 = 0;  fno1 = 1; /* 2 hard-coded */ break;
            case XxLine::DIFF_1:
               nbDiffFiles = 2;  fno0 = 0;  fno1 = 1; break;
            case XxLine::DIFF_2:
               nbDiffFiles = 2;  fno0 = 0;  fno1 = 1; break;
            case XxLine::DIFF_3:
               nbDiffFiles = 2;  fno0 = 0;  fno1 = 2; break;
            case XxLine::DIFFDEL_1:
               nbDiffFiles = 2;  fno0 = 1;  fno1 = 2; break;
            case XxLine::DIFFDEL_2:
               nbDiffFiles = 2;  fno0 = 0;  fno1 = 2; break;
            case XxLine::DIFFDEL_3:
               nbDiffFiles = 2;  fno0 = 0;  fno1 = 1; break;
            default:
               continue;
         }
      }

      //
      // Check just this hunk for ignore-display.
      //
      // We use temporary, fixed-size buffers for performing the text
      // comparisons without whitespace because if requires much many less
      // branches, which should be more efficient.
      //

      char buf1[ COMPBUFSIZE ];
      char buf2[ COMPBUFSIZE ];

      XxDln l1 = lineNo;
      XxFln fl1 = cline.getLineNo( fno0 );
      uint c1 = 0;
      uint len1;
      const char* t1 = files[fno0]->getTextLine( fl1, len1 );

      XxDln l2 = lineNo;
      XxFln fl2 = cline.getLineNo( fno1 );
      uint c2 = 0;
      uint len2;
      const char* t2 = files[fno1]->getTextLine( fl2, len2 );

      bool different = false;
      bool done1 = false;
      bool done2 = false;

      if ( nbDiffFiles == 2 ) {

         while ( !different && !done1 && !done2 ) {

            //
            // Fill up the non-ws comparison buffers.
            //
            int nbc1 = fillBufWithChars(
               *this, fno0, l1, c1, t1, len1, buf1, done1, *(files[fno0]), ctype
            );

            int nbc2 = fillBufWithChars(
               *this, fno1, l2, c2, t2, len2, buf2, done2, *(files[fno1]), ctype
            );

            //
            // Here our two buffers should be full or null-terminated.
            // Perform the comparison of the buffers.
            //
            if ( nbc1 != nbc2 ) {
               different = true;
            }
            else {
               for ( int ii = 0; ii < nbc1; ++ii ) {
                  if ( buf1[ii] != buf2[ii] ) {
                     different = true;
                     break;
                  }
               }
            }
         }

      }
      else {
         XX_CHECK( nbDiffFiles == 3 );

         const XxFno fno2 = 2;

         char buf3[ COMPBUFSIZE ];

         XxDln l3 = lineNo;
         XxFln fl3 = cline.getLineNo( fno2 );
         uint c3 = 0;
         uint len3;
         const char* t3 = files[fno2]->getTextLine( fl3, len3 );

         bool done3 = false;

         while ( !different && !done1 && !done2 ) {

            //
            // Fill up the non-ws comparison buffers.
            //
            int nbc1 = fillBufWithChars(
               *this, fno0, l1, c1, t1, len1, buf1, done1, *(files[fno0]), ctype
            );

            int nbc2 = fillBufWithChars(
               *this, fno1, l2, c2, t2, len2, buf2, done2, *(files[fno1]), ctype
            );

            int nbc3 = fillBufWithChars(
               *this, fno2, l3, c3, t3, len3, buf3, done3, *(files[fno2]), ctype
            );

            //
            // Here our two buffers should be full or null-terminated.
            // Perform the comparison of the buffers.
            //
            if ( nbc1 != nbc2 || nbc1 != nbc3 ) {
               different = true;
            }
            else {
               for ( int ii = 0; ii < nbc1; ++ii ) {
                  if ( buf1[ii] != buf2[ii] || buf1[ii] != buf3[ii] ) {
                     different = true;
                     break;
                  }
               }
            }
         }
      }

      if ( !different ) {
         while ( lineNo <= getNbLines() ) {
            XxLine& line = getLineNC( lineNo );
            if ( line.getType() != ctype ) {
               break;
            }
            line.setIgnoreDisplay( true );
            ++lineNo;
         }
      }

   }
}

XX_NAMESPACE_END

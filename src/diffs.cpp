/******************************************************************************\
 * $Id: diffs.cpp 56 2000-12-25 20:15:47Z  $

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

#include <diffs.h>
#include <buffer.h>
#include <resources.h>

#include <list>
#include <algorithm>
#include <iostream>
#include <stdio.h>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

//------------------------------------------------------------------------------
//
void outputLine(
   std::ostream&                os,
   const std::auto_ptr<XxBuffer>* files,
   const XxLine&                line,
   int                          no
)
{
   int fline = line.getLineNo( no );
   XX_ASSERT( fline != 0 );
   if ( fline != -1 ) {
      uint len;
      const char* text = files[no]->getTextLine( fline, len );
      XX_ASSERT( text != 0 ); // make this one throw

      os.write( text, len );
      os.put( '\n' );
   }
}

//------------------------------------------------------------------------------
//
std::string buildTag( 
   const char*                    tag,
   int                            nbFiles,
   int                            number,
   bool                           useNumber, 
   bool                           useString,
   bool                           useConditionals,
   const std::string&             conditional1,
   const std::string&             conditional2,
   const std::auto_ptr<XxBuffer>& file
)
{
   char buf[ 1024 ];
   char buf2[ 1024 ];
   ::strcpy( buf, tag );
   
   // FIXME this could lead to a bug.

   if ( useNumber ) {
      ::strcpy( buf2, buf );
      ::sprintf( buf, buf2, number );
   }
   if ( useString ) {
      ::strcpy( buf2, buf );
      if ( useConditionals ) {
         if ( nbFiles == 3 && ( number == 1 || number == 2 ) ) {
            ::sprintf( buf, buf2, conditional2.c_str() );
         }
         else {
            ::sprintf( buf, buf2, conditional1.c_str() );
         }
      }
      else {
         ::sprintf( buf, buf2, file->getDisplayName() );
      }
   }

   return std::string( buf );
}

}

XX_NAMESPACE_BEGIN


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxDiffs::SeaResult
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxDiffs::SeaResult::SeaResult() :
   _lineNo( -1 )
{
   _fline[0] = _fline[1] = _fline[2] = -1;
}

//------------------------------------------------------------------------------
//
XxDiffs::SeaResult::SeaResult( int lineNo, int fline[3] ) :
   _lineNo( lineNo )
{
   _fline[0] = fline[0];
   _fline[1] = fline[1];
   _fline[2] = fline[2];
}

//------------------------------------------------------------------------------
//
bool XxDiffs::SeaResult::isValid() const
{
   return _lineNo != -1;
}



/*==============================================================================
 * CLASS XxDiffs
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxDiffs::XxDiffs() :
   _initializedHorizontalDiffs( false ),
   _dirty( false )
{}

//------------------------------------------------------------------------------
//
XxDiffs::XxDiffs( std::vector<XxLine>& lines, bool isDirectoryDiff ) :
   _initializedHorizontalDiffs( false ),
   _isDirectoryDiff( isDirectoryDiff ),
   _dirty( false )
{
   std::swap( _lines, lines );
#ifdef XX_DEBUG
   validateLineNumbers();
#endif
}

//------------------------------------------------------------------------------
//
XxDiffs::~XxDiffs()
{}

//------------------------------------------------------------------------------
//
uint XxDiffs::getNbLinesWithContent( uint& unselectedChanges ) const
{
   uint nbLines = 0;
   bool prevKnown = true;
   int prevfline = -1;

   unselectedChanges = 0;
   for ( uint ii = 1; ii <= _lines.size(); ++ii ) {

      int no, fline;
      bool known = _lines[ii].getSelectedText( no, fline );
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
uint XxDiffs::moveBackwardsVisibleLines(
   uint startingLine,
   uint nbLines,
   uint unselChangesLines
) const
{
   uint nbVisibleLines = 0;
   bool prevKnown = true;
   int prevfline = -1;

   uint curLine = startingLine;
   while ( curLine > 0 && nbVisibleLines < nbLines ) {

      int no, fline;
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

   return std::min( getNbLines(), curLine + 1 );
}

//------------------------------------------------------------------------------
//
void XxDiffs::selectLine( uint lineNo, XxLine::Selection selection )
{
   XX_ASSERT( lineNo <= _lines.size() );
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
void XxDiffs::selectRegion( uint lineNo, XxLine::Selection selection )
{
   XX_ASSERT( lineNo <= _lines.size() );
   if ( _isDirectoryDiff ) {
      return;
   }

   XxLine& line = getLineNC( lineNo );
   XxLine::Type type = line.getType();
   if ( type == XxLine::SAME ||
        type == XxLine::DIRECTORIES ) {
      return;
   }

   uint start, end;
   findRegion( lineNo, start, end );

   // Move backwards (including selected line).
   for ( uint ii = start; ii <= end; ++ii ) {
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

   for ( uint ii = 1; ii <= _lines.size(); ++ii ) {
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

   for ( uint ii = 1; ii <= _lines.size(); ++ii ) {
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
   uint  lineNo,
   uint& regionStart,
   uint& regionEnd
) const
{
   XX_ASSERT( lineNo <= _lines.size() );

   const XxLine& rline = getLine( lineNo );
   XxLine::Type type = rline.getType();

   // Move backwards (including selected line).
   uint cur = lineNo;
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
   while ( cur <= _lines.size() ) {
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
   uint  lineNo,
   uint& regionStart,
   uint& regionEnd
) const
{
   XX_ASSERT( lineNo <= _lines.size() );

   const XxLine& rline = getLine( lineNo );
   XxLine::Type type = rline.getType();

   // Move backwards (including selected line).
   uint cur = lineNo;
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
   while ( cur <= _lines.size() ) {
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
int XxDiffs::findNextDifference( uint lineNo ) const
{
   XX_ASSERT( lineNo <= _lines.size() );

   const XxLine& line = getLine( lineNo );
   XxLine::Type type = line.getType();

   // Seek to end of current region.
   uint cur = lineNo + 1;
   while ( cur <= _lines.size() ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), type ) ) {
         break;
      }
      cur++;
   }
   if ( cur > _lines.size() ) {
      return -1;
   }

   // Seek to next difference.
   while ( cur <= _lines.size() ) {
      const XxLine& line = getLine( cur );
      if ( !XxLine::isSameRegion( line.getType(), XxLine::SAME ) ) {
         break;
      }
      cur++;
   }
   if ( cur > _lines.size() ) {
      return -1;
   }
   return cur;
}

//------------------------------------------------------------------------------
//
int XxDiffs::findPreviousDifference( uint lineNo ) const
{
   XX_ASSERT( lineNo <= _lines.size() );

   const XxLine& line = getLine( lineNo );
   XxLine::Type type = line.getType();

   // Seek to beginning of current region.
   uint cur = lineNo - 1;
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
int XxDiffs::findNextUnselected( uint lineNo ) const
{
   // Note: lineNo could be outside boundaries in order to search for the
   // first/last unselected difference.

   uint cur = lineNo;
   if ( 1 <= lineNo && lineNo <= _lines.size() ) {
      // Skip current selection block.
      const XxLine& line = getLine( lineNo );
      XxLine::Type type = line.getType();
      XxLine::Selection sel = line.getSelection();

      cur = lineNo;
      while ( cur <= _lines.size() ) {
         const XxLine& line = getLine( cur );
         if ( !XxLine::isSameRegion( line.getType(), type ) ||
              line.getSelection() != sel ) {
            break;
         }
         cur++;
      }
   }

   // Seek to next unselected line.
   cur = std::max( uint(1), cur );
   while ( cur <= _lines.size() ) {
      const XxLine& line = getLine( cur ); 
      if ( line.getType() != XxLine::SAME &&
           line.getType() != XxLine::DIRECTORIES &&
           line.getSelection() == XxLine::UNSELECTED ) {
         break;
      }
      cur++;
   }
   if ( cur > _lines.size() ) {
      return -1;
   }
   return cur;
}

//------------------------------------------------------------------------------
//
int XxDiffs::findPreviousUnselected( uint lineNo ) const
{
   // Note: lineNo could be outside boundaries in order to search for the
   // first/last unselected difference.

   uint cur = lineNo;
   if ( 1 <= lineNo && lineNo <= _lines.size() ) {
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
   cur = std::min( (int)_lines.size(), (int)cur );
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
uint XxDiffs::getNbFileLines(
   uint no,
   uint start,
   uint end
) const
{
   XX_ASSERT( no == 0 || no == 1 || no == 2 );
   XX_ASSERT( start <= _lines.size() );
   XX_ASSERT( end <= _lines.size() );
   XX_ASSERT( start <= end );

   int count = 0;
   for ( uint ii = start; ii <= end; ++ii ) {
      const XxLine& line = getLine( ii );
      if ( line.getLineNo( no ) != -1 ) {
         count++;
      }
   }
   return count;
}

//------------------------------------------------------------------------------
//
uint XxDiffs::getFileLine(
   uint  no,
   uint  lineNo,
   bool& actuallyEmpty
) const
{
   XX_ASSERT( no == 0 || no == 1 || no == 2 );
   if ( _lines.size() == 0 ) {
      actuallyEmpty = true;
      return 0;
   }
   actuallyEmpty = false;

   if ( lineNo > _lines.size() ) {
      actuallyEmpty = true;
      return 0;
   }

   // This is one place where our data structure is annoying and we have to do a
   // stupid search.

   // Look backward, empty regions will get assigned the first encountered line
   // number.
   int fline = -1;
   int ii;
   for ( ii = lineNo; ii > 0; --ii ) {
      const XxLine& line = getLine( ii );
      fline = line.getLineNo( no );
      if ( fline != -1 ) {
         break;
      }
   }
   if ( ii != int(lineNo) ) {
      actuallyEmpty = true;
   }

   // In the extremely unlikely situation where no line was found by looking
   // backwards, look forward.
   if ( fline == -1 ) {
      actuallyEmpty = true;
      for ( ii = lineNo; ii <= int(_lines.size()); ++ii ) {
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
   for ( uint ii = 0; ii < _lines.size(); ++ii ) {
      os << _lines[ii] << std::endl;
   }
   return os;
}

//------------------------------------------------------------------------------
//
bool XxDiffs::isSomeSelected() const
{
   // Look for a selected line.
   for ( uint ii = 0; ii < _lines.size(); ++ii ) {
      XxLine::Type type = _lines[ii].getType();
      if ( type != XxLine::SAME &&
           type != XxLine::DIRECTORIES ) {
         XxLine::Selection selection = _lines[ii].getSelection();
         if ( selection != XxLine::UNSELECTED ) {
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
   for ( uint ii = 0; ii < _lines.size(); ++ii ) {
      XxLine::Type type = _lines[ii].getType();
      if ( type != XxLine::SAME && 
           type != XxLine::DIRECTORIES ) {
         XxLine::Selection selection = _lines[ii].getSelection();
         if ( selection == XxLine::UNSELECTED ) {
            return false;
         }
      }
   }
   return true;
}

//------------------------------------------------------------------------------
//
bool XxDiffs::save( 
   std::ostream&                  os, 
   const std::auto_ptr<XxBuffer>* files,
   bool                           useConditionals,
   const std::string&             conditional1,
   const std::string&             conditional2
) const
{
   XX_ASSERT( files != 0 );
   XX_ASSERT( files[0].get() != 0 );
   XX_ASSERT( files[1].get() != 0 );
   int nbFiles = files[2].get() != 0 ? 3 : 2;

   const XxResources* resources = XxResources::getInstance();
   std::string tags[4];
   if ( useConditionals == false ) {
      for ( int ii = 0; ii < nbFiles; ++ii ) {
         tags[ii] = resources->getTag( XxResources::TAG_CONFLICT_SEPARATOR );
      }
      tags[nbFiles] = resources->getTag( XxResources::TAG_CONFLICT_END );
   }
   else {
      tags[0] = resources->getTag( XxResources::TAG_CONDITIONAL_IFDEF );
      if ( nbFiles == 2 ) {
         tags[1] = resources->getTag( XxResources::TAG_CONDITIONAL_ELSE );
         tags[2] = resources->getTag( XxResources::TAG_CONDITIONAL_ENDIF );
      }
      else {
         tags[1] = resources->getTag( XxResources::TAG_CONDITIONAL_ELSEIF );
         tags[2] = resources->getTag( XxResources::TAG_CONDITIONAL_ELSE );
         tags[3] = resources->getTag( XxResources::TAG_CONDITIONAL_ENDIF );
      }
   }

   for ( int ii = 0; ii < nbFiles; ++ii ) {
      std::string::size_type pos = tags[ii].find( "%d" );
      if ( pos != std::string::npos ) {
         char buf[12];
         ::sprintf( buf, "%d", ii+1 );
         tags[ii].replace( pos, 2, buf );
      }

      if ( useConditionals == false ) {
         pos = tags[ii].find( "%s" );
         if ( pos != std::string::npos ) {
            tags[ii].replace( pos, 2, files[ii]->getDisplayName() );
         }
      }
   }

   if ( useConditionals == true ) {
      std::string::size_type pos = tags[0].find( "%s" );
      if ( pos != std::string::npos ) {
         tags[0].replace( pos, 2, conditional1 );
      }

      if ( nbFiles == 2 ) {
         pos = tags[1].find( "%s" );
         if ( pos != std::string::npos ) {
            tags[1].replace( pos, 2, conditional1 );
         }

         pos = tags[2].find( "%s" );
         if ( pos != std::string::npos ) {
            tags[2].replace( pos, 2, conditional1 );
         }
      }
      else {
         pos = tags[1].find( "%s" );
         if ( pos != std::string::npos ) {
            tags[1].replace( pos, 2, conditional2 );
         }

         pos = tags[2].find( "%s" );
         if ( pos != std::string::npos ) {
            tags[2].replace( pos, 2, conditional2 );
         }

         pos = tags[3].find( "%s" );
         if ( pos != std::string::npos ) {
            tags[3].replace( pos, 2, conditional1 );
         }
      }
   }

   for ( int ii = 0; ii < nbFiles+1; ++ii ) {
      XX_TRACE( tags[ii] );
   }

   bool foundUnsel = false;
   bool insideUnsel = false;
   uint unselBegin = 0;
   uint unselEnd;
   uint ii;
   for ( ii = 1; ii <= _lines.size(); ++ii ) {
      const XxLine& line = getLine( ii );

      XxLine::Selection selection = line.getSelection();
      if ( line.getType() == XxLine::SAME ||
           line.getType() == XxLine::DIRECTORIES ||
           selection != XxLine::UNSELECTED ) {

         if ( insideUnsel == true ) {
            // Output the unselected portion.
            unselEnd = ii;
            XX_ASSERT( unselEnd - unselBegin > 0 );
            for ( uint f = 0; f < 3; ++f ) {
               if ( files[f].get() != 0 ) {

                  os << tags[f] << std::endl;

                  for ( uint iii = unselBegin; iii < unselEnd; ++iii ) {
                     const XxLine& cline = getLine( iii );
                     outputLine( os, files, cline, f );
                  }
               }
            }

            os << tags[nbFiles] << std::endl;

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
            unselBegin = ii;
         }
         foundUnsel = true;
      }
   }

   // If file ends with a hunk, make sure a pending unselected hunk is output.
   if ( insideUnsel == true ) {
      // Output the unselected portion.
      unselEnd = ii;
      XX_ASSERT( unselEnd - unselBegin > 0 );
      for ( uint f = 0; f < 3; ++f ) {
         if ( files[f].get() != 0 ) {

            os << tags[f] << std::endl;

            for ( uint iii = unselBegin; iii < unselEnd; ++iii ) {
               const XxLine& cline = getLine( iii );
               outputLine( os, files, cline, f );
            }
         }
      }

      os << tags[nbFiles] << std::endl;

      insideUnsel = false;
   }

   return !foundUnsel;
}

//------------------------------------------------------------------------------
//
bool XxDiffs::saveSelectedOnly(
   std::ostream&                os,
   const std::auto_ptr<XxBuffer>* files
) const
{
   XX_ASSERT( files != 0 );
   XX_ASSERT( files[0].get() != 0 );
   XX_ASSERT( files[1].get() != 0 );

   bool some = false;
   bool prevOut = false;
   for ( uint ii = 1; ii <= _lines.size(); ++ii ) {
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
            os.write( text, len );
            os << std::endl;

            some = true;
            prevOut = true;
         }
      }
      else if ( prevOut == true ) {
         os << std::endl;
         prevOut = false;
      }
   }
   return some;
}

//------------------------------------------------------------------------------
//
void XxDiffs::search(
   const char*                  searchText,
   const int                    nbFiles,
   const std::auto_ptr<XxBuffer>* files
)
{
   XX_ASSERT( files != 0 );
   for ( int ii = 0; ii < nbFiles; ++ii ) {
      XX_ASSERT( files[ii].get() != 0 );
   }

   // Note: we could easily avoid the stupid buffer copy by implementing
   // what would be strnstr.

   _searchResults.clear();

   for ( uint ii = 1; ii <= _lines.size(); ++ii ) {
      const XxLine& line = getLineNC( ii );
      SeaResult scurrent; // init as invalid

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
const std::vector<XxDiffs::SeaResult>& XxDiffs::getSearchResults() const
{
   return _searchResults;
}

//------------------------------------------------------------------------------
//
XxDiffs::SeaResult XxDiffs::findNextSearch( uint lineNo ) const
{
   XX_ASSERT( lineNo > 0 && lineNo <= _lines.size() );

   // Stupid linear search.
   for ( uint ii = 0; ii < _searchResults.size(); ++ii ) {
      if ( _searchResults[ii]._lineNo > int(lineNo) ) {
         return _searchResults[ii];
      }
   }
   // else return invalid.
   return SeaResult();
}

//------------------------------------------------------------------------------
//
XxDiffs::SeaResult XxDiffs::findPreviousSearch( uint lineNo ) const
{
   XX_ASSERT( lineNo > 0 && lineNo <= _lines.size() );

   // Stupid linear search.
   for ( int ii = _searchResults.size() - 1; ii >= 0; --ii ) {
      if ( _searchResults[ii]._lineNo < int(lineNo) ) {
         return _searchResults[ii];
      }
   }
   // else return invalid.
   return SeaResult();
}

//------------------------------------------------------------------------------
//
bool XxDiffs::splitSwapJoin( uint lineNo, uint nbFiles )
{
   bool wasJoin = false;

   // Find contiguous hunks that are not SAME with same hunk id.  There should
   // be three at most (we cannot split to more than three).
   std::vector<uint> lstart;
   std::vector<uint> lend;
   uint start, end;

   // Save hunk id we're restricted to.
   int hunkId = getLine( lineNo ).getHunkId();

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
   while ( end < getNbLines() ) {
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
               for ( uint ii = lstart[0]; ii <= lend[0]; ++ii ) {
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
         for ( uint ii = lstart[1]; ii <= lend[1]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( uint ii = lstart[0]; ii <= lend[0]; ++ii ) {
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
         for ( uint ii = 0; ii <= nbmin; ++ii ) {
            XxLine nline = XxLine::join( getLine( lstart[0] + ii ),
                                          getLine( lstart[1] + ii ) );
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
         for ( uint ii = lstart[0]; ii <= lend[0]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( uint ii = lstart[2]; ii <= lend[2]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( uint ii = lstart[1]; ii <= lend[1]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
      }
      else if ( type1 == XxLine::INSERT_1 &&
                type2 == XxLine::INSERT_3 &&
                type3 == XxLine::INSERT_2 ) {
         // Roll down.
         for ( uint ii = lstart[2]; ii <= lend[2]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( uint ii = lstart[0]; ii <= lend[0]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( uint ii = lstart[1]; ii <= lend[1]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
      }
      else if ( type1 == XxLine::INSERT_2 &&
                type2 == XxLine::INSERT_3 &&
                type3 == XxLine::INSERT_1 ) {
         // Roll up.
         for ( uint ii = lstart[1]; ii <= lend[1]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( uint ii = lstart[2]; ii <= lend[2]; ++ii ) {
            newLines.push_back( getLine( ii ) );
         }
         for ( uint ii = lstart[0]; ii <= lend[0]; ++ii ) {
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
         for ( int ii = 0; ii < 3; ++ii ) {
            len[ii] = lend[ii] - lstart[ii];
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

         for ( uint ii = 0; ii <= len[0]; ++ii ) {
            XxLine nline = XxLine::join(
               getLine( lstart[ord[0]] + ii ),
               getLine( lstart[ord[1]] + ii ),
               getLine( lstart[ord[2]] + ii )
            );
            newLines.push_back( nline );
         }

         for ( uint ii = len[0]+1; ii <= len[1]; ++ii ) {
            XxLine nline = XxLine::join(
               getLine( lstart[ord[1]] + ii ),
               getLine( lstart[ord[2]] + ii )
            );
            newLines.push_back( nline.getPromoted( XxLine::DIFF_ALL ) );
         }

         for ( uint ii = len[1]+1; ii <= len[2]; ++ii ) {
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
   std::vector<XxLine>::iterator istart = & getLineNC( lstart.front() );
   std::vector<XxLine>::iterator iend = & getLineNC( lend.back() );
   _lines.erase( istart, iend + 1 );
   _lines.insert( istart, newLines.begin(), newLines.end() );

   emit nbLinesChanged();
   _dirty = true;
   emit changed();
   // No need to clear search results, the overview looks the same.

   // Mark for redoing the horizontal diffs.
   _initializedHorizontalDiffs = false;

   return wasJoin;
}


//------------------------------------------------------------------------------
//
void XxDiffs::initializeHorizontalDiffs(
   const std::auto_ptr<XxBuffer>* files,
   const bool                     force
)
{
   XX_ASSERT( files[0].get() != 0 );
   XX_ASSERT( files[1].get() != 0 );

   if ( force == true || _initializedHorizontalDiffs == false ) {

      const XxResources* resources = XxResources::getInstance();
      const bool ignoreWs =
         resources->getBoolOpt( XxResources::IGNORE_HORIZONTAL_WS );

      for ( uint ii = 0; ii < _lines.size(); ++ii ) {
         int line0 = _lines[ii].getLineNo( 0 );
         const char* text0 = 0;
         uint len0 = 0;
         if ( line0 != -1 ) {
            text0 = files[0]->getTextLine( line0, len0 );
         }

         int line1 = _lines[ii].getLineNo( 1 );
         const char* text1 = 0;
         uint len1 = 0;
         if ( line1 != -1 ) {
            text1 = files[1]->getTextLine( line1, len1 );
         }

         int line2 = _lines[ii].getLineNo( 2 );
         const char* text2 = 0;
         uint len2 = 0;
         if ( line2 != -1 ) {
            text2 = files[2]->getTextLine( line2, len2 );
         }

         _lines[ii].initializeHorizontalDiff(
            ignoreWs, text0, len0, text1, len1, text2, len2
         );
      }

      _initializedHorizontalDiffs = true;
   }
}

//------------------------------------------------------------------------------
//
void XxDiffs::splitTwoRegions(
   std::vector<XxLine>& newLines,
   uint start,
   uint end,
   uint s1,
   uint s2
) const
{
   for ( uint ii = start; ii <= end; ++ii ) {
      const XxLine& cline = getLine( ii );
      if ( cline.getLineNo( s1 ) != -1 ) {
         newLines.push_back( cline.getSplit( s1 ) );
      }
   }
   for ( uint ii = start; ii <= end; ++ii ) {
      const XxLine& cline = getLine( ii );
      if ( cline.getLineNo( s2 ) != -1 ) {
         newLines.push_back( cline.getSplit( s2 ) );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxDiffs::merge()
{
   if ( _isDirectoryDiff ) {
      return;
   }

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
    *  1) first select SEL1 globally, then spawn diff3 -m, with the appropriate
    *  option, find all the edits and unselect the reported edits.  Upside: we
    *  don't perform _any_ algorithmic work.  Downside: we need to write a new
    *  parser because the output of diff3 -m is an ed script.
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

   for ( uint ii = 1; ii <= _lines.size(); ++ii ) {
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
         } break;

         default: {
            XX_CHECK( false );
         }
      }
   }

   _dirty = true;
   emit changed();
}

//------------------------------------------------------------------------------
//
void XxDiffs::validateLineNumbers() const
{
   int fline[3] = { 0, 0, 0 };
   for ( uint ii = 1; ii <= _lines.size(); ++ii ) {
      const XxLine& line = getLine( ii );
      for ( int l = 0; l < 3; ++l ) {
         int flineNo = line.getLineNo( l );
         if ( flineNo != -1 ) {
            fline[l]++;
            XX_ASSERT( flineNo == fline[l] );
         }
      }
   }
}

//------------------------------------------------------------------------------
//
bool XxDiffs::checkSelections( uint no ) const
{
   for ( uint ii = 1; ii <= _lines.size(); ++ii ) {
      const XxLine& line = getLine( ii );

      if ( line.getType() != XxLine::SAME &&
           line.getType() != XxLine::DIRECTORIES &&
           line.getSelection() != XxLine::Selection( no ) ) {
         return false;
      }
   }
   return true;
}

XX_NAMESPACE_END

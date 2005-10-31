/******************************************************************************\
 * $Id: line.cpp 2 2000-09-15 02:19:22Z blais $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <line.h>

#include <iostream>
#include <ctype.h>
#include <string>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

const char* typeString[ 16 ] = { 
   "SAME",
   "DIFF_1",
   "DIFF_2",
   "DIFF_3",
   "DELETE_1",
   "DELETE_2",
   "DELETE_3",
   "INSERT_1",
   "INSERT_2",
   "INSERT_3",
   "DIFF_ALL",
   "DIFFDEL_1",
   "DIFFDEL_2",
   "DIFFDEL_3",
   "DIFFDEL_3",
   "DIRECTORIES"
};

const char* selectionString[ 5 ] = { 
   "SEL1",
   "SEL2",
   "SEL3",
   "UNSELECTED",
   "NEITHER"
};

}

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxLine
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxLine::XxLine( 
   Type type,
   int  fline0,
   int  fline1,
   int  fline2
) :
   _type( type ),
   _selection( UNSELECTED ),
   _hunkId( 0 )
{
   _lineNo[0] = fline0;
   _lineNo[1] = fline1;
   _lineNo[2] = fline2;

   init();

#ifdef XX_DEBUG
   switch ( _type ) {
      case SAME:
         XX_ASSERT( fline0 != -1 && fline1 != -1 /*&& fline2 != -1*/ );
         break;
      case DIFF_1:
         XX_ASSERT( fline0 != -1 || fline1 != -1 || fline2 != -1 );
         break;
      case DIFF_2:
         XX_ASSERT( fline0 != -1 || fline1 != -1 || fline2 != -1 );
         break;
      case DIFF_3:
         XX_ASSERT( fline0 != -1 || fline1 != -1 || fline2 != -1 );
         break;
      case DELETE_1:
         XX_ASSERT( fline0 == -1 && fline1 != -1 && fline2 != -1 );
         break;
      case DELETE_2:
         XX_ASSERT( fline0 != -1 && fline1 == -1 && fline2 != -1 );
         break;
      case DELETE_3:
         XX_ASSERT( fline0 != -1 && fline1 != -1 && fline2 == -1 );
         break;
      case INSERT_1:
         XX_ASSERT( fline0 != -1 && fline1 == -1 && fline2 == -1 );
         break;
      case INSERT_2:
         XX_ASSERT( fline0 == -1 && fline1 != -1 && fline2 == -1 );
         break;
      case INSERT_3:
         XX_ASSERT( fline0 == -1 && fline1 == -1 && fline2 != -1 );
         break;
      case DIFF_ALL:
         XX_ASSERT( fline0 != -1 || fline1 != -1 || fline2 != -1 );
         break;
      case DIFFDEL_1: 
         XX_ASSERT( fline0 == -1 && ( fline1 != -1 || fline2 != -1 ) );
         break;
      case DIFFDEL_2:
         XX_ASSERT( fline1 == -1 && ( fline0 != -1 || fline2 != -1 ) );
         break;
      case DIFFDEL_3:
         XX_ASSERT( fline2 == -1 && ( fline0 != -1 || fline1 != -1 ) );
         break;
      case DIRECTORIES:
         XX_ASSERT( fline0 != -1 && fline1 != -1 /*&& fline2 != -1*/ );
         break;
   }
#endif
}

//------------------------------------------------------------------------------
//
std::ostream& operator << ( 
   std::ostream& os, 
   const XxLine& line 
)
{
   os << "Line: type=";
   os << typeString[ int( line.getType() ) ];

   os << line.getLineNo(0) << "  ";
   os << line.getLineNo(1) << "  ";
   os << line.getLineNo(2) << "  ";

   os << "  Selection=";
   os << selectionString[ int( line.getSelection() ) ];

   return os;
}

//------------------------------------------------------------------------------
//
bool XxLine::isSameRegion( Type type1, Type type2 )
{
   return type1 == type2;
   // FIXME should we make SAME == DIRECTORIES?
}

//------------------------------------------------------------------------------
//
void XxLine::initializeHorizontalDiff( 
   const bool  ignoreWs,
   const char* text0,
   const uint  len0,
   const char* text1,
   const uint  len1,
   const char* text2,
   const uint  len2
)
{
   // Count nb. of empty files and do something appropriate.
   int nbEmpty = 0;
   nbEmpty += ( text0 == 0 ) ? 1 : 0;
   nbEmpty += ( text1 == 0 ) ? 1 : 0;
   nbEmpty += ( text2 == 0 ) ? 1 : 0;

   if ( nbEmpty == 0 ) {

      // Left brackets.
      const char* pl0 = text0;
      const char* pl1 = text1;
      const char* pl2 = text2;

      const char* end0 = text0 + len0;
      const char* end1 = text1 + len1;
      const char* end2 = text2 + len2;

      bool allbegin = true;
      while ( true ) {

         if ( ignoreWs && 
              ( allbegin || 
                ( isspace( *pl0 ) && isspace( *pl1 ) && isspace( *pl2 ) ) ) ) {
            while ( isspace( *pl0 ) && pl0 < end0 ) { pl0++; }
            while ( isspace( *pl1 ) && pl1 < end1 ) { pl1++; }
            while ( isspace( *pl2 ) && pl2 < end2 ) { pl2++; }
         }

         if ( !( *pl0 == *pl1 &&
                 *pl1 == *pl2 &&
                 pl0 < end0 &&
                 pl1 < end1 &&
                 pl2 < end2 ) ) {
            break;
         }
         pl0++;
         pl1++;
         pl2++;

         allbegin = false;
      }
      
      _lefthd[0] = pl0 - text0;
      _lefthd[1] = pl1 - text1;
      _lefthd[2] = pl2 - text2;

      // Right brackets.
      const char* pr0 = end0 - 1;
      const char* pr1 = end1 - 1;
      const char* pr2 = end2 - 1;

      bool allend = true;

      while ( true ) {

         if ( ignoreWs &&
              ( allend ||
                ( isspace( *pr0 ) && isspace( *pr1 ) && isspace( *pr2 ) ) ) ) {
            while ( isspace( *pr0 ) && pr0 >= pl0 && pr0 >= text0 ) { pr0--; }
            while ( isspace( *pr1 ) && pr1 >= pl1 && pr1 >= text1 ) { pr1--; }
            while ( isspace( *pr2 ) && pr2 >= pl2 && pr2 >= text2 ) { pr2--; }
         }

         if ( !( *pr0 == *pr1 && 
                 *pr1 == *pr2 && 
                 pr0 >= pl0 &&
                 pr1 >= pl1 &&
                 pr2 >= pl2 &&
                 pr0 >= text0 &&
                 pr1 >= text1 &&
                 pr2 >= text2 ) ) {
            break;
         }
         pr0--;
         pr1--;
         pr2--;

         allend = false;
      }

      _righthd[0] = pr0 - text0;
      _righthd[1] = pr1 - text1;
      _righthd[2] = pr2 - text2;

      XX_CHECK( pr0+1 >= pl0 );
      XX_CHECK( pr1+1 >= pl1 );
      XX_CHECK( pr2+1 >= pl2 );
   }
   else if ( nbEmpty == 1 ) {

      const char* begin0 = 0;
      const char* begin1 = 0;
      const char* end0 = 0;
      const char* end1 = 0;
      int* lhd[3];
      int* rhd[3];
      if ( text0 == 0 ) {
         begin0 = text1;
         begin1 = text2;
         end0 = begin0 + len1;
         end1 = begin1 + len2;
         lhd[0] = & _lefthd[1];
         lhd[1] = & _lefthd[2];
         lhd[2] = & _lefthd[0];
         rhd[0] = & _righthd[1];
         rhd[1] = & _righthd[2];
         rhd[2] = & _righthd[0];
      }
      else if ( text1 == 0 ) {
         begin0 = text0;
         begin1 = text2;
         end0 = begin0 + len0;
         end1 = begin1 + len2;
         lhd[0] = & _lefthd[0];
         lhd[1] = & _lefthd[2];
         lhd[2] = & _lefthd[1];
         rhd[0] = & _righthd[0];
         rhd[1] = & _righthd[2];
         rhd[2] = & _righthd[1];
      }
      else if ( text2 == 0 ) {
         begin0 = text0;
         begin1 = text1;
         end0 = begin0 + len0;
         end1 = begin1 + len1;
         lhd[0] = & _lefthd[0];
         lhd[1] = & _lefthd[1];
         lhd[2] = & _lefthd[2];
         rhd[0] = & _righthd[0];
         rhd[1] = & _righthd[1];
         rhd[2] = & _righthd[2];
      }

      // Left brackets.
      const char* pl0 = begin0;
      const char* pl1 = begin1;
      bool allbegin = true;

      while ( true ) {

         if ( ignoreWs &&
              ( allbegin ||
                ( isspace( *pl0 ) && isspace( *pl1 ) ) ) ) {
            while ( isspace( *pl0 ) && pl0 < end0 ) { pl0++; }
            while ( isspace( *pl1 ) && pl1 < end1 ) { pl1++; }
         }

         if ( !( *pl0 == *pl1 &&
                 pl0 < end0 &&
                 pl1 < end1 ) ) {
            break;
         }
         pl0++;
         pl1++;

         allbegin = false;
      }

      *(lhd[0]) = pl0 - begin0;
      *(lhd[1]) = pl1 - begin1;
      *(lhd[2]) = -1;

      // Right brackets.
      const char* pr0 = end0 - 1;
      const char* pr1 = end1 - 1;
      bool allend = true;

      while ( true ) {

         if ( ignoreWs &&
              ( allend ||
                ( isspace( *pr0 ) && isspace( *pr1 ) ) ) ) {
            while ( isspace( *pr0 ) && pr0 >= pl0 && pr0 >= text0 ) { pr0--; }
            while ( isspace( *pr1 ) && pr1 >= pl1 && pr1 >= text1 ) { pr1--; }
         }
         
         if ( !( *pr0 == *pr1 && 
                 pr0 >= pl0 &&
                 pr1 >= pl1 &&
                 pr0 >= begin0 &&
                 pr1 >= begin1 ) ) {
            break;
         }
         pr0--;
         pr1--;

         allend = false;
      }
      *(rhd[0]) = pr0 - begin0; 
      *(rhd[1]) = pr1 - begin1;
      *(rhd[2]) = -1;

      XX_CHECK( pr0+1 >= pl0 );
      XX_CHECK( pr1+1 >= pl1 );
   }
   else {
      // nbEmpty > 1
      _lefthd[0] = _lefthd[1] = _lefthd[2] = -1;
      _righthd[0] = _righthd[1] = _righthd[2] = -1;
      return;
   }
}

//------------------------------------------------------------------------------
//
XxLine XxLine::getSplit( const int no ) const
{
   XxLine newline( *this ); // no default const.

   switch ( _type ) {
      case SAME:
      case DIRECTORIES: {
         newline = XxLine( *this ); // No split.
      } break;

      case DIFF_1: {
         switch ( no ) {
            case 0: {
               newline = XxLine( INSERT_1, _lineNo[0], -1, -1 );
            } break;
            case 1:
            case 2: {
               newline = XxLine( DELETE_1, -1, _lineNo[1], _lineNo[2] );
            } break;
         }
      } break;
      case DIFF_2: {
         switch ( no ) {
            case 1: {
               newline = XxLine( INSERT_2, -1, _lineNo[1], -1 );
            } break;
            case 0:
            case 2: {
               newline = XxLine( DELETE_2, _lineNo[0], -1, _lineNo[2] );
            } break;
         }
      } break;
      case DIFF_3: {
         switch ( no ) {
            case 2: {
               newline = XxLine( INSERT_3, -1, -1, _lineNo[2] );
            } break;
            case 0:
            case 1: {
               newline = XxLine( DELETE_3, _lineNo[0], _lineNo[1], -1 );
            } break;
         }
      } break;
      case DELETE_1: {
         newline = XxLine( *this ); // No split.
      } break;
      case DELETE_2: {
         newline = XxLine( *this ); // No split.
      } break;
      case DELETE_3: {
         newline = XxLine( *this ); // No split.
      } break;
      case INSERT_1: {
         newline = XxLine( *this ); // No split.
      } break;
      case INSERT_2: {
         newline = XxLine( *this ); // No split.
      } break;
      case INSERT_3: {
         newline = XxLine( *this ); // No split.
      } break;
      case DIFF_ALL: {
         switch ( no ) {
            case 0: {
               newline = XxLine( INSERT_1, _lineNo[0], -1, -1 );
            } break;
            case 1: {
               newline = XxLine( INSERT_2, -1, _lineNo[1], -1 );
            } break;
            case 2: {
               newline = XxLine( INSERT_3, -1, -1, _lineNo[2] );
            } break;
         }
      } break;
      case DIFFDEL_1: {
         switch ( no ) {
            case 0: {
               newline = XxLine( *this ); // No split.
            } break;
            case 1: {
               newline = XxLine( INSERT_2, -1, _lineNo[1], -1 );
            } break;
            case 2: {
               newline = XxLine( INSERT_3, -1, -1, _lineNo[2] );
            } break;
         }
      } break;
      case DIFFDEL_2: {
         switch ( no ) {
            case 0: {
               newline = XxLine( INSERT_1, _lineNo[0], -1, -1 );
            } break;
            case 1: {
               newline = XxLine( *this ); // No split.
            } break;
            case 2: {
               newline = XxLine( INSERT_3, -1, -1, _lineNo[2] );
            } break;
         }
      } break;
      case DIFFDEL_3: {
         switch ( no ) {
            case 0: {
               newline = XxLine( INSERT_1, _lineNo[0], -1, -1 );
            } break;
            case 1: {
               newline = XxLine( INSERT_2, -1, _lineNo[1], -1 );
            } break;
            case 2: {
               newline = XxLine( *this ); // No split.
            } break;
         }
      } break;

      default: {
      } break;
   }

   newline.setSelection( _selection );
   newline.setHunkId( _hunkId );
   return newline;
}

//------------------------------------------------------------------------------
//
XxLine XxLine::join(
   const XxLine& line1,
   const XxLine& line2
)
{
   XX_ASSERT( line1.getHunkId() == line2.getHunkId() );

   XxLine::Type type1 = line1.getType();
   XxLine::Type type2 = line2.getType();
    
   XxLine ll1 = line1;
   XxLine ll2 = line2;
   if ( ( type1 == DELETE_1 && type2 == INSERT_1 ) ||
        ( type1 == DELETE_2 && type2 == INSERT_2 ) ||
        ( type1 == DELETE_3 && type2 == INSERT_3 ) ||
        ( type1 == INSERT_2 && type2 == INSERT_1 ) ||
        ( type1 == INSERT_3 && type2 == INSERT_1 ) ||
        ( type1 == INSERT_3 && type2 == INSERT_2 ) ) {
      // Swap lines.
      ll1 = line2;
      ll2 = line1;
      type1 = ll1.getType();
      type2 = ll2.getType();
   }

   XxLine joined( line1 ); // no default const.
   if ( type1 == INSERT_1 && type2 == DELETE_1 ) {
      joined = XxLine( DIFF_1,
                       ll1._lineNo[0], ll2._lineNo[1], ll2._lineNo[2] );
   }
   else if ( type1 == INSERT_2 && type2 == DELETE_2 ) {
      joined = XxLine( DIFF_2,
                       ll2._lineNo[0], ll1._lineNo[1], ll2._lineNo[2] );
   }
   else if ( type1 == INSERT_3 && type2 == DELETE_3 ) {
      joined = XxLine( DIFF_3,
                       ll2._lineNo[0], ll2._lineNo[1], ll1._lineNo[2] );
   }
   else if ( type1 == INSERT_1 && type2 == INSERT_2 ) {
      joined = XxLine( DIFFDEL_3,
                       ll1._lineNo[0], ll2._lineNo[1], -1 );
   }
   else if ( type1 == INSERT_1 && type2 == INSERT_3 ) {
      joined = XxLine( DIFFDEL_2,
                       ll1._lineNo[0], -1, ll2._lineNo[2] );
   }
   else if ( type1 == INSERT_2 && type2 == INSERT_3 ) {
      joined = XxLine( DIFFDEL_1,
                       -1, ll1._lineNo[1], ll2._lineNo[2] );
   }
   else { 
      XX_ASSERT( false );
   }

   if ( ll1.getSelection() == ll2.getSelection() ) {
      joined.setSelection( ll1.getSelection() );
   }

   joined.setHunkId( line1.getHunkId() );
   return joined;
}

//------------------------------------------------------------------------------
//
XxLine XxLine::join(
   const XxLine& line1,
   const XxLine& line2,
   const XxLine& line3
)
{
   XX_CHECK( line1.getType() == INSERT_1 ||
             line1.getType() == INSERT_2 ||
             line1.getType() == INSERT_3 );
   XX_CHECK( line2.getType() == INSERT_1 ||
             line2.getType() == INSERT_2 ||
             line2.getType() == INSERT_3 );
   XX_CHECK( line3.getType() == INSERT_1 ||
             line3.getType() == INSERT_2 ||
             line3.getType() == INSERT_3 );
   XX_ASSERT( line1.getHunkId() == line2.getHunkId() );
   XX_ASSERT( line2.getHunkId() == line3.getHunkId() );

   XxLine::Type type1 = line1.getType();
   XxLine::Type type2 = line2.getType();
   XxLine::Type type3 = line3.getType();
    
   // Order lines.
   XxLine ll1 = line1;
   XxLine ll2 = line2;
   XxLine ll3 = line3;
   XxLine swap( line1 ); // no default const.
   if ( type1 > type2 ) {
      swap = ll1; ll1 = ll2; ll2 = swap;
      type1 = ll1.getType();
      type2 = ll2.getType();
   }
   if ( type1 > type3 ) {
      swap = ll1; ll1 = ll3; ll3 = swap;
      type1 = ll1.getType();
      type3 = ll3.getType();
   }
   if ( type2 > type3 ) {
      swap = ll2; ll2 = ll3; ll3 = swap;
      type2 = ll2.getType();
      type3 = ll3.getType();
   }
   
   XxLine joined( DIFF_ALL, ll1._lineNo[0], ll2._lineNo[1], ll3._lineNo[2] );
   if ( ll1.getSelection() == ll2.getSelection() &&
        ll2.getSelection() == ll3.getSelection() ) {
      joined.setSelection( ll1.getSelection() );
   }
   joined.setHunkId( ll1.getHunkId() );
   return joined;
}

//------------------------------------------------------------------------------
//
XxLine XxLine::getPromoted( Type promoteType ) const
{
   XxLine nline( promoteType, _lineNo[0], _lineNo[1], _lineNo[2] );
   nline.setSelection( _selection );
   nline.setHunkId( _hunkId );
   return nline;
}

//------------------------------------------------------------------------------
//
std::string XxLine::mapToString( Type type )
{
   return std::string( typeString[ type ] );
}

//------------------------------------------------------------------------------
//
std::string XxLine::mapToString( Selection sel )
{
   return std::string( selectionString[ sel ] );
}

XX_NAMESPACE_END

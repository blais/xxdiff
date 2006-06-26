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

#include <line.h>
#include <hordiffImp.h>

#include <iostream>
#include <ctype.h>

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

//------------------------------------------------------------------------------
//
inline const int* allocBounds( int lefthd, int righthd )
{
   if ( lefthd != -1 && righthd != -1 ) {
      // Leave only brackets.
      int* ll = new int[3];
      ll[0] = lefthd;
      ll[1] = righthd;
      ll[2] = -1; // end list
      return ll;
   }
   return 0;
}

//------------------------------------------------------------------------------
//
inline const int* copyHordiffs( const int* src )
{
   //
   // Copy horizontal diffs. Returns newly allocated copy.
   //
   if ( src == 0 ) {
      return 0;
   }

   // Compute size of array.
   const int* pp = src;
   int cc = 0;
   while ( pp[cc] != -1 ) { ++cc; }
   int* ll = new int[cc+1];
   
   cc = 0;
   while ( pp[cc] != -1 ) {
      ll[cc] = pp[cc];
      ++cc;
   }
   ll[cc] = -1;
   return ll;
}

}

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxLine
 *============================================================================*/

XxLine::Type XxLine::_ignoreConvertTables[4][ XxLine::NB_TYPES ] = {
   // No ignore.
   {                 XxLine::SAME,
                     XxLine::DIFF_1,
                     XxLine::DIFF_2,
                     XxLine::DIFF_3,
                     XxLine::DELETE_1,
                     XxLine::DELETE_2,
                     XxLine::DELETE_3,
                     XxLine::INSERT_1,
                     XxLine::INSERT_2,
                     XxLine::INSERT_3,
                     XxLine::DIFF_ALL,
                     XxLine::DIFFDEL_1,
                     XxLine::DIFFDEL_2,
                     XxLine::DIFFDEL_3,
                     XxLine::DIRECTORIES
   },
   // Ignoring file 0.
   { /*SAME*/        XxLine::SAME,
     /*DIFF_1*/      XxLine::SAME,
     /*DIFF_2*/      XxLine::DIFF_ALL,
     /*DIFF_3*/      XxLine::DIFF_ALL,
     /*DELETE_1*/    XxLine::SAME,
     /*DELETE_2*/    XxLine::INSERT_3,
     /*DELETE_3*/    XxLine::INSERT_2,
     /*INSERT_1*/    XxLine::INSERT_1,
     /*INSERT_2*/    XxLine::INSERT_2,
     /*INSERT_3*/    XxLine::INSERT_3,
     /*DIFF_ALL*/    XxLine::DIFF_ALL,
     /*DIFFDEL_1*/   XxLine::DIFF_ALL,
     /*DIFFDEL_2*/   XxLine::INSERT_3,
     /*DIFFDEL_3*/   XxLine::INSERT_2,
     /*DIRECTORIES*/ XxLine::DIRECTORIES
   },
   // Ignoring file 1.
   { /*SAME*/        XxLine::SAME,
     /*DIFF_1*/      XxLine::DIFF_ALL,
     /*DIFF_2*/      XxLine::SAME,
     /*DIFF_3*/      XxLine::DIFF_ALL,
     /*DELETE_1*/    XxLine::INSERT_3,
     /*DELETE_2*/    XxLine::SAME,
     /*DELETE_3*/    XxLine::INSERT_1,
     /*INSERT_1*/    XxLine::INSERT_1,
     /*INSERT_2*/    XxLine::INSERT_2,
     /*INSERT_3*/    XxLine::INSERT_3,
     /*DIFF_ALL*/    XxLine::DIFF_ALL,
     /*DIFFDEL_1*/   XxLine::INSERT_3,
     /*DIFFDEL_2*/   XxLine::DIFF_ALL,
     /*DIFFDEL_3*/   XxLine::INSERT_1,
     /*DIRECTORIES*/ XxLine::DIRECTORIES
   },
   // Ignoring file 2.
   { /*SAME*/        XxLine::SAME,
     /*DIFF_1*/      XxLine::DIFF_ALL,
     /*DIFF_2*/      XxLine::DIFF_ALL,
     /*DIFF_3*/      XxLine::SAME,
     /*DELETE_1*/    XxLine::INSERT_2,
     /*DELETE_2*/    XxLine::INSERT_1,
     /*DELETE_3*/    XxLine::SAME,
     /*INSERT_1*/    XxLine::INSERT_1,
     /*INSERT_2*/    XxLine::INSERT_2,
     /*INSERT_3*/    XxLine::INSERT_3,
     /*DIFF_ALL*/    XxLine::DIFF_ALL,
     /*DIFFDEL_1*/   XxLine::INSERT_2,
     /*DIFFDEL_2*/   XxLine::INSERT_1,
     /*DIFFDEL_3*/   XxLine::DIFF_ALL,
     /*DIRECTORIES*/ XxLine::DIRECTORIES
   }
};

int XxLine::_selectMasks[ XxLine::NB_TYPES ][ 3 ] = {
   /* SAME        */ { 7, 7, 7 },
   /* DIFF_1      */ { 1, 6, 6 },
   /* DIFF_2      */ { 5, 2, 5 },
   /* DIFF_3      */ { 3, 3, 4 },
   /* DELETE_1    */ { 1, 6, 6 },
   /* DELETE_2    */ { 5, 2, 5 },
   /* DELETE_3    */ { 3, 3, 4 },
   /* INSERT_1    */ { 1, 6, 6 },
   /* INSERT_2    */ { 5, 2, 5 },
   /* INSERT_3    */ { 3, 3, 4 },
   /* DIFF_ALL    */ { 1, 2, 4 },
   /* DIFFDEL_1   */ { 1, 2, 4 },
   /* DIFFDEL_2   */ { 1, 2, 4 },
   /* DIFFDEL_3   */ { 1, 2, 4 },
   /* DIRECTORIES */ { 0, 0, 0 },
};


//------------------------------------------------------------------------------
//
XxLine::XxLine( 
   Type  type,
   XxFln fline0,
   XxFln fline1,
   XxFln fline2
) :
   _type( type ),
   _selection( UNSELECTED ),
   _hunkId( 0 ),
   _perHunkWs( false )
{
   _lineNo[0] = fline0;
   _lineNo[1] = fline1;
   _lineNo[2] = fline2;

   init();

#ifdef XX_DEBUG
   switch ( _type ) {
      case SAME:
#if KEPT_FOR_HISTORY  // FIXME remove, this could not be with -B
         XX_ASSERT( fline0 != -1 && fline1 != -1 /*&& fline2 != -1*/ );
#endif
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
      case NB_TYPES:
         XX_ABORT();
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
   os << "Line: type=" << typeString[ int( line.getType() ) ] << "  ";

   os << line.getLineNo(0) << "  ";
   os << line.getLineNo(1) << "  ";
   os << line.getLineNo(2) << "  ";

   os << "  Selection=" << selectionString[ int( line.getSelection() ) ];
   os << "  IgnDisp=" << line.getIgnoreDisplay();
   return os;
}

//------------------------------------------------------------------------------
//
bool XxLine::isSameRegion( Type type1, Type type2 )
{
   return type1 == type2;
   // Note: prehaps we should also make SAME == DIRECTORIES?
}

//------------------------------------------------------------------------------
//
void XxLine::initializeHorizontalDiff( 
   const XxResources& resources,
   const char*        text[3],
   const uint         len[3]
)
{
   //
   // HD_NONE: remove horizontal diffs and return.
   //
   if ( resources.getHordiffType() == HD_NONE ) {
      return;
   }

   // Don't waste time in here.
   if ( _type == SAME || 
        _type == DELETE_1 || _type == DELETE_2 || _type == DELETE_3 ||
        _type == INSERT_1 || _type == INSERT_2 || _type == INSERT_3 ||
        _type == DIRECTORIES ) {
      return;
   }

   //
   // What this code is supposed to do:
   //
   //   * compute the number of potentially different non-empty sides of text.
   //   note that this is independent of the type of line this is, since one
   //   side can have text or can be empty, even if it is marked otherwise (but
   //   never the opposite, i.e. two sides that are marked to be the same will
   //   always be the same, and we don't even bother checking that).
   //
   //   * if there is only one non-empty side
   //      - nothing to do.  return.
   //
   //   * if there are two potentially different non-empty sides (understand
   //   that this could mean three non-empty sides, only two of which *
   //   potentially differ).
   //      - compute bounds between the two
   //      - if multiple horizontal diffs are enabled, compute them
   //      - if needed, copy one of the hordiff results
   //
   //   * if there are three potentially different non-empty sides (this will
   //   * only and always happen on a DIFF_ALL in three-way diffs mode)
   //      - compute bounds between the three
   //      - if needed, copy one of the hordiff results
   //
   //      (we do not know at this point how to compute multiple horizontal
   //      diffs between three potentially different lines).
   //

   // Remove current horizontal diffs.
   int ii;
   for ( ii = 0; ii < 3; ++ii ) {
#ifndef WINDOWS
      delete[] _hordiffs[ii];
#else
      delete[] const_cast<int*>( _hordiffs[ii] );
#endif
      _hordiffs[ii] = 0;
   }

   // Count nb. of potentially different non-empty files.
   int idiff[3] = { -1, -1, -1 };
   int icopyfrom = -1;
   int icopyto = -1;
   switch ( _type ) {
      case DIFF_1:    { 
         idiff[0] = 0; idiff[1] = 1;
         icopyfrom = 1; icopyto = 2;
      } break;

      case DIFF_2:    { 
         idiff[0] = 1; idiff[1] = 0;
         icopyfrom = 0; icopyto = 2;
      } break;

      case DIFF_3:    { 
         idiff[0] = 2; idiff[1] = 0;
         icopyfrom = 0; icopyto = 1;
      } break;

      case DIFFDEL_1: { 
         idiff[0] = 1; idiff[1] = 2;
      } break;

      case DIFFDEL_2: { 
         idiff[0] = 0; idiff[1] = 2;
      } break;

      case DIFFDEL_3: { 
         idiff[0] = 0; idiff[1] = 1;
      } break;
      
      case DIFF_ALL: {
         idiff[0] = 0; idiff[1] = 1; idiff[2] = 2;
      } break;
      
      case SAME:
      case DELETE_1:
      case DELETE_2:
      case DELETE_3:
      case INSERT_1:
      case INSERT_2:
      case INSERT_3:
      case DIRECTORIES:
      case NB_TYPES:
         XX_ABORT();
   }

   // Gather the non-null requested diffs.
   int nndiff[3];
   int nn = 0;
   for ( ii = 0; ii < 3; ++ii ) {
      if ( idiff[ii] != -1 && text[ idiff[ii] ] != 0 ) {
         nndiff[nn++] = idiff[ii];
      }
   }

   if ( nn < 2 ) {
      return; // nothing to do.
   }

   // 3-way hordiff, only compute bounds.
   else if ( nn == 3 ) {
      XX_CHECK( _type == DIFF_ALL );
      int lefthd[3];
      int righthd[3];
      XxHordiffImp::boundsHordiff3( resources, text, len, lefthd, righthd );

      for ( ii = 0; ii < 3; ++ii ) {
         _hordiffs[ii] = allocBounds( lefthd[ii], righthd[ii] );
      }
      return;
   }

   // 2-way hordiff, potentially with multiple diffs.
   else {
      XX_CHECK( nn == 2 );

      const char* ttext[2];
      uint tlen[2];
      for ( ii = 0; ii < 2; ++ii ) {
         ttext[ii] = text[ nndiff[ii] ];
         tlen[ii] = len[ nndiff[ii] ];
      }
      int lefthd[2];
      int righthd[2];
      XxHordiffImp::boundsHordiff2( resources, ttext, tlen, lefthd, righthd );

      if ( resources.getHordiffType() == HD_SINGLE ) {
         //
         // HD_SINGLE: simply use the horizontal diff bounds.
         //
         for ( ii = 0; ii < 2; ++ii ) {
            _hordiffs[ nndiff[ii] ] = allocBounds( lefthd[ii], righthd[ii] );
         }
      }
      else {
         //
         // HD_MULTIPLE: compute and create lcs for diff region.
         //
         int blen[2];
         for ( ii = 0; ii < 2; ++ii ) {
            blen[ii] = righthd[ii] - lefthd[ii];
         }

         const int hordiffthr = resources.getHordiffContext();
         if ( blen[0] >= hordiffthr && blen[1] >= hordiffthr ) {
            XxHordiffImp::multipleHordiffs2(
               resources,
               _hordiffs[ nndiff[0] ], ttext[0], lefthd[0], righthd[0],
               _hordiffs[ nndiff[1] ], ttext[1], lefthd[1], righthd[1]
            );
         }
         else {
            // The region is not long enough for multiple hordiffs, just use
            // brackets.
            for ( ii = 0; ii < 2; ++ii ) {
               _hordiffs[ nndiff[ii] ] = allocBounds( lefthd[ii], righthd[ii] );
            }
         }
      }
      
      // Copy to third region if requested.
      if ( icopyfrom != -1 ) {
         _hordiffs[ icopyto ] = copyHordiffs( _hordiffs[ icopyfrom ] );
      }
   }
}

//------------------------------------------------------------------------------
//
XxLine XxLine::getSplit( const XxFno no ) const
{
   XxLine newline;  /*( *this );*/ // no default const.

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
   newline.setIgnoreDisplay( _perHunkWs );
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
   joined.setIgnoreDisplay(
      line1.getIgnoreDisplay() && line2.getIgnoreDisplay()
   );
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
   joined.setIgnoreDisplay( line1.getIgnoreDisplay() && 
                            line2.getIgnoreDisplay() && 
                            line3.getIgnoreDisplay() );
   return joined;
}

//------------------------------------------------------------------------------
//
XxLine XxLine::getPromoted( Type promoteType ) const
{
   XxLine nline( promoteType, _lineNo[0], _lineNo[1], _lineNo[2] );
   nline.setSelection( _selection );
   nline.setHunkId( _hunkId );
   nline.setIgnoreDisplay( _perHunkWs );
   return nline;
}

//------------------------------------------------------------------------------
//
QString XxLine::mapToString( Type type )
{
   return QString( typeString[ type ] );
}

//------------------------------------------------------------------------------
//
QString XxLine::mapToString( Selection sel )
{
   return QString( selectionString[ sel ] );
}

//------------------------------------------------------------------------------
//
void XxLine::getLineColorType(
   const XxIgnoreFile ignoreFile,
   const XxFno        no,
   XxColor&           dtype,
   XxColor&           dtypeSup,
   const bool         perHunkWsEnabled
) const
{
   if ( ignoreFile == IGNORE_NONE ) {
      getLineColorTypeStd( getType(), no, dtype, dtypeSup, perHunkWsEnabled );
   }
   else {
      if ( no == (ignoreFile - 1) ) {
         dtype = dtypeSup = COLOR_IGNORED;
      }
      else {
         XxLine::Type newType = 
            _ignoreConvertTables[ int(ignoreFile) ][ getType() ];
         getLineColorTypeStd( newType, no, dtype, dtypeSup, perHunkWsEnabled );
      }
   }
}

//------------------------------------------------------------------------------
//
bool XxLine::getLineColorIfSelected(
   const XxFno no,
   XxColor&    dtype,
   XxColor&    dtypeSup
) const
{
   XxLine::Selection sel = getSelection();
   if ( sel == XxLine::SEL1 ||
        sel == XxLine::SEL2 ||
        sel == XxLine::SEL3 ) {

      if ( XxFno(sel) == no ) {
         dtype = COLOR_SELECTED;
         dtypeSup = COLOR_SELECTED_SUP;
         return true;
      }
      else if ( 
         ( ( getType() == XxLine::DELETE_1 || 
             getType() == XxLine::DIFF_1 ||
             getType() == XxLine::INSERT_1 ) && 
           no != 0 && int(sel) != XxLine::SEL1 ) ||

         ( ( getType() == XxLine::DELETE_2 || 
             getType() == XxLine::DIFF_2 || 
             getType() == XxLine::INSERT_2 ) &&
           no != 1 && int(sel) != XxLine::SEL2 ) ||

         ( ( getType() == XxLine::DELETE_3 || 
             getType() == XxLine::DIFF_3 ||
             getType() == XxLine::INSERT_3 ) && 
           no != 2 && int(sel) != XxLine::SEL3 ) 
      ) {
         // For regions that are not selected but whose text is the same as the
         // ones that are selected, color as selected as well.
         dtype = COLOR_SELECTED;
         dtypeSup = COLOR_SELECTED_SUP;
         return true;
      }
      else {
         dtype = COLOR_DELETED;
         dtypeSup = COLOR_DELETED_SUP;
         return true;
      }
   }
   else if ( sel == XxLine::NEITHER ) {
      dtype = COLOR_DELETED;
      dtypeSup = COLOR_DELETED_SUP;
      return true;
   }
   // else
   return false;
}

//------------------------------------------------------------------------------
//
void XxLine::getLineColorTypeStd(
   const XxLine::Type newType,
   const XxFno        no,
   XxColor&           dtype,
   XxColor&           dtypeSup,
   const bool         perHunkWsEnabled
) const
{
   if ( getLineColorIfSelected( no, dtype, dtypeSup ) ) {
      return;
   }
   // else

   if ( _perHunkWs ) {
      if ( perHunkWsEnabled ) {
         dtype = COLOR_IGNORE_DISPLAY;
         dtypeSup = COLOR_IGNORE_DISPLAY_SUP;
         return;
      }
   }

   int lno = mapTypeToFileNo( newType );

   switch ( newType ) {

      case XxLine::SAME: {
         dtype = dtypeSup = COLOR_SAME;
         if ( getLineNo(no) == -1 ) {
            dtype = dtypeSup = COLOR_SAME_BLANK;
         }
         return;
      }

      case XxLine::DIFF_1: 
      case XxLine::DIFF_2: 
      case XxLine::DIFF_3: {
         if ( no == lno ) {
            if ( getLineNo(no) == -1 ) {
               dtype = dtypeSup = COLOR_DIFF_ONE_NONLY;
               return;
            }
            else if ( getLineNo((no+1)%3) == -1 ) {
               dtype = dtypeSup = COLOR_DIFF_ONE_ONLY;
               return;
            }
            else {
               dtype = COLOR_DIFF_ONE;
               dtypeSup = COLOR_DIFF_ONE_SUP;
               return;
            }
         }
         else {
            if ( getLineNo(no) == -1 ) {
               dtype = dtypeSup = COLOR_DIFF_TWO_NONLY;
               return;
            }
            else if ( getLineNo((no+1)%3) == -1 ) {
               dtype = dtypeSup = COLOR_DIFF_TWO_ONLY;
               return;
            }
            else {
               dtype = COLOR_DIFF_TWO;
               dtypeSup = COLOR_DIFF_TWO_SUP;
               return;
            }
         }
      }

      case XxLine::DELETE_1: 
      case XxLine::DELETE_2:
      case XxLine::DELETE_3: {
         if ( no == lno ) {
            dtype = dtypeSup = COLOR_DELETE_BLANK;
            return;
         }
         else {
            dtype = dtypeSup = COLOR_DELETE;
            return;
         }
      }

      case XxLine::INSERT_1:
      case XxLine::INSERT_2:
      case XxLine::INSERT_3: {
         if ( no == lno ) {
            dtype = dtypeSup = COLOR_INSERT;
            return;
         }
         else {
            dtype = dtypeSup = COLOR_INSERT_BLANK;
            return;
         }
      }

      case XxLine::DIFF_ALL: {
         if ( getLineNo(no) == -1 ) {
            dtype = dtypeSup = COLOR_DIFF_ALL_NONLY;
            return;
         }
         else if ( getLineNo((no+1)%3) == -1 &&
                   getLineNo((no+2)%3) == -1 ) {
            dtype = dtypeSup = COLOR_DIFF_ALL_ONLY;
            return;
         }
         else {
            dtype = COLOR_DIFF_ALL;
            dtypeSup = COLOR_DIFF_ALL_SUP;
            return;
         }
      }

      case XxLine::DIFFDEL_1:
      case XxLine::DIFFDEL_2:
      case XxLine::DIFFDEL_3: {
         if ( no == lno ) {
            dtype = dtypeSup = COLOR_DIFFDEL_BLANK;
            return;
         }
         // else
         if ( getLineNo(no) == -1 ) {
            dtype = dtypeSup = COLOR_DIFFDEL_NONLY;
            return;
         }
         else if ( getLineNo((no+1)%3) == -1 &&
                   getLineNo((no+2)%3) == -1 ) {
            dtype = dtypeSup = COLOR_DIFFDEL_ONLY;
            return;
         }
         else {
            dtype = COLOR_DIFFDEL;
            dtypeSup = COLOR_DIFFDEL_SUP;
            return;
         }
      }

      case XxLine::DIRECTORIES: {
         dtype = dtypeSup = COLOR_DIRECTORIES;
         return;
      }

      case XxLine::NB_TYPES: {
         XX_ABORT();
      }
   }

   dtype = dtypeSup = COLOR_SAME; // unreached.
}

XX_NAMESPACE_END

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

#include <hordiffImp.h>
#include <resources.h>

#include <qcstring.h>

#ifndef COMPILER_MIPSPRO
#include <cctype> // isspace()
#else
#include <ctype.h> // isspace()
#endif

// #define XX_LOCAL_HDIFF_DEBUG
#ifdef XX_LOCAL_HDIFF_DEBUG
#include <iostream>
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

//------------------------------------------------------------------------------
//
inline int strncpyNoWs( char* dest, const char* src, unsigned int len )
{
   // Copy a string into another, but compressing the whitespace to one space.
   // Returns the length of the copied string.

   int ic = 0;
   for ( unsigned int ii = 0; ii < len; ++ii ) {
      if ( isspace( src[ii] ) ) {
         dest[ic++] = ' ';
         int sc = 0;
         unsigned int iid = ii + 1;
         while ( isspace( src[iid] ) && iid < len ) { ++sc; ++iid; }
         ii += sc;
      }
      else {
         dest[ic++] = src[ii];
      }
   }
   dest[ic] = 0;

   return ic;
}

//------------------------------------------------------------------------------
//
inline void adjustWs( int* hordiff, const char* text, int len )
{
   //    {
   //       cerr << "len = " << len << endl;
   //       for ( int ii = 0; hordiff[ii] != -1; ++ii ) {
   //          cerr << hordiff[ii] << " ";
   //       }
   //       cerr << endl;
   //    }

   int curoff = 0; // current offset
   int ccc = 0; // current hor diff
   int is = 0; // src index
   int id = 0; // dest index
   for ( ; id < len && hordiff[ccc] != -1; ++id ) {

      // Check if the current diff is affected by the current offset, advance if
      // necessary.
      while ( hordiff[ccc] <= is && hordiff[ccc] != -1 ) {
         hordiff[ccc++] += curoff; // adjusting here.
      }

      // Compare and adjust current offset.
      if ( isspace( text[id] ) ) {
         // Count the number of extra ws chars.
         int iid = id + 1;
         int sc = 0;
         while ( isspace( text[iid] ) && iid < len ) { ++iid; ++sc; }
         curoff += sc;
         id += sc;
      }

      ++is; // one space in the src or one non-space char.
   }

   while ( hordiff[ccc] != -1 ) {
      hordiff[ccc++] += curoff;
   }

   //    {
   //       for ( int ii = 0; hordiff[ii] != -1; ++ii ) {
   //          cerr << hordiff[ii] << " ";
   //       }
   //       cerr << endl;
   //    }
}

#ifdef XX_LOCAL_HDIFF_DEBUG
void printMatches( 
   int         c,
   int*        hbuffer0,
   const char* ctext0,
   int*        hbuffer1,
   const char* ctext1
)
{
   using namespace std;
   cerr << endl << "nb.matches " << c << endl;

   cerr << "matches left ";
   for ( int ii = c/2-1; ii >= 0; --ii ) {
      cerr << " " << hbuffer0[ii*2] << "|";
      for ( int ic = hbuffer0[ii*2]; ic < hbuffer0[ii*2+1]; ++ic ) {
         cerr << ctext0[ic+1];
      }
      cerr << "|" << hbuffer0[ii*2+1] << " ";
   }
   cerr << endl;

   cerr << "matches right ";
   for ( int ii = c/2-1; ii >= 0; --ii ) {
      cerr << " " << hbuffer1[ii*2] << "|";
      for ( int ic = hbuffer1[ii*2]; ic < hbuffer1[ii*2+1]; ++ic ) {
         cerr << ctext1[ic+1];
      }
      cerr << "|" << hbuffer1[ii*2+1] << " ";
   }
   cerr << endl;
}
#endif

};

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxHordiff
 *============================================================================*/

//------------------------------------------------------------------------------
//
void XxHordiffImp::boundsHordiff2(
   const XxResources& resources,
   const char*        text[2],
   const uint         len[2],
   int                lefthd[2],
   int                righthd[2]
)
{
   const bool ignoreWs = resources.getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS );
   int i;

   // Left brackets.
   const char* begin[2];
   const char* end[2];
   const char* pl[2];
   for ( i = 0; i < 2; ++i ) {
      begin[i] = text[ i ];
      end[i] = begin[i] + len[ i ];
      pl[i] = begin[i];

      // If ignoreWs is on, skip all whitespace at the beginning of line.
      if ( ignoreWs ) {
         while ( isspace( *pl[i] ) && pl[i] < end[i] )
            { pl[i]++; }
      }
   }

   while ( true ) {

      int noff[2];
      char c[2];
      for ( i = 0; i < 2; ++i ) {
         c[i] = *pl[i];
         if ( ignoreWs && isspace( c[i] ) ) {
            // Compress whitespace if necessary.
            const char* pp = pl[i];
            int sc = 0;
            while ( isspace( *pp ) && pp < end[i] ) { pp++; sc++; }
            noff[i] = sc;
            c[i] = ' ';
         }
         else {
            noff[i] = 1;
         }
      }

      if ( !( c[0] == c[1] &&
              pl[0] < end[0] &&
              pl[1] < end[1] ) ) {
         break;
      }
      pl[0] += noff[0];
      pl[1] += noff[1];
   }

   for ( i = 0; i < 2; ++i ) {
      lefthd[ i ] = pl[i] - begin[i];
   }

   // Right brackets.
   const char* pr[2];
   for ( i = 0; i < 2; ++i ) {
      pr[i] = end[i] - 1;

      // If ignoreWs is on, skip all whitespace at the end of all lines.
      if ( ignoreWs ) {
         while ( isspace( *pr[i] ) && pr[i] >= pl[i] && pr[i] >= text[i] )
            { pr[i]--; }
      }
   }

   while ( true ) {

      int noff[2];
      char c[2];
      for ( i = 0; i < 2; ++i ) {
         c[i] = *pr[i];
         if ( ignoreWs && isspace( c[i] ) ) {
            // Compress whitespace if necessary.
            const char* pp = pr[i];
            int sc = 0;
            while ( isspace( *pp ) && pp >= pl[i] && pp >= text[i] )
               { pp--; sc++; }
            noff[i] = sc;
            c[i] = ' ';
         }
         else {
            noff[i] = 1;
         }
      }
         
      if ( !( c[0] == c[1] && 
              pr[0] >= pl[0] &&
              pr[1] >= pl[1] &&
              pr[0] >= begin[0] &&
              pr[1] >= begin[1] ) ) {
         break;
      }
      pr[0] -= noff[0];
      pr[1] -= noff[1];
   }

   for ( i = 0; i < 2; ++i ) {
      righthd[ i ] = pr[i] - begin[i];
      XX_CHECK( pr[i]+1 >= pl[i] );
   }

   // Normalize.
   for ( int ii = 0; ii < 2; ++ii ) {
      if ( !( lefthd[ii] == -1 && righthd[ii] == -1 ) ) {
         righthd[ii]++; // +1 because righthd is the last char.
      }
   }
   
// #define XX_LOCAL_BOUNDS_DEBUG
#ifdef XX_LOCAL_BOUNDS_DEBUG
   using namespace std;
   for ( int ii = 0; ii < 2; ++ii ) {
      cerr << ii << " : " << lefthd[ii] << " " << righthd[ii] << endl;
   }
#endif
}

//------------------------------------------------------------------------------
//
void XxHordiffImp::boundsHordiff3(
   const XxResources& resources,
   const char*        text[3],
   const uint         len[3],
   int                lefthd[3],
   int                righthd[3]
)
{
   const bool ignoreWs = resources.getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS );
   int i;

   // Left brackets.
   const char* pl[3];
   const char* end[3];
   for ( i = 0; i < 3; ++i ) {
      pl[i] = text[i];
      end[i] = text[i] + len[i];

      // If ignoreWs is on, skip all whitespace at the beginning of line.
      if ( ignoreWs ) {
         while ( isspace( *pl[i] ) && pl[i] < end[i] ) 
            { pl[i]++; }
      }
   }
           
   while ( true ) {

      int noff[3];
      char c[3];
      for ( i = 0; i < 3; ++i ) {
         c[i] = *pl[i];
         if ( ignoreWs && isspace( c[i] ) ) {
            // Compress whitespace if necessary.
            const char* pp = pl[i];
            int sc = 0;
            while ( isspace( *pp ) && pp < end[i] ) { pp++; sc++; }
            noff[i] = sc;
            c[i] = ' ';
         }
         else {
            noff[i] = 1;
         }
      }

      /*cerr << "cbeg " << c[0] << "|" << c[1] << "|" << c[2] << endl;*/
      if ( !( c[0] == c[1] &&
              c[1] == c[2] &&
              pl[0] < end[0] &&
              pl[1] < end[1] &&
              pl[2] < end[2] ) ) {
         break;
      }
      pl[0] += noff[0];
      pl[1] += noff[1];
      pl[2] += noff[2];
   }
      
   for ( i = 0; i < 3; ++i ) {
      lefthd[i] = pl[i] - text[i];
   }

   // Right brackets.
   const char* pr[3];
   for ( i = 0; i < 3; ++i ) {
      pr[i] = end[i] - 1;

      // If ignoreWs is on, skip all whitespace at the end of all lines.
      if ( ignoreWs ) {
         while ( isspace( *pr[i] ) && pr[i] >= pl[i] && pr[i] >= text[i] )
            { pr[i]--; }
      }
   }

   while ( true ) {

      int noff[3];
      char c[3];
      for ( i = 0; i < 3; ++i ) {
         c[i] = *pr[i];
         if ( ignoreWs && isspace( c[i] ) ) {
            // Compress whitespace if necessary.
            const char* pp = pr[i];
            int sc = 0;
            while ( isspace( *pp ) && pp >= pl[i] && pp >= text[i] )
               { pp--; sc++; }
            noff[i] = sc;
            c[i] = ' ';
         }
         else {
            noff[i] = 1;
         }
      }
            
      /*cerr << "cend " << c[0] << "|" << c[1] << "|" << c[2] << endl;*/
      if ( !( c[0] == c[1] && 
              c[1] == c[2] && 
              pr[0] >= pl[0] &&
              pr[1] >= pl[1] &&
              pr[2] >= pl[2] &&
              pr[0] >= text[0] &&
              pr[1] >= text[1] &&
              pr[2] >= text[2] ) ) {
         break;
      }
      pr[0] -= noff[0];
      pr[1] -= noff[1];
      pr[2] -= noff[2];
   }

   for ( i = 0; i < 3; ++i ) {
      righthd[i] = pr[i] - text[i];
      XX_CHECK( pr[i]+1 >= pl[i] );
   }

   for ( int ii = 0; ii < 3; ++ii ) {
      if ( righthd[ii] != -1 ) {
         righthd[ii]++; // +1 because righthd is the last char.
      }
   }
}

//------------------------------------------------------------------------------
//
void XxHordiffImp::multipleHordiffs2(
   const XxResources& resources,
   const int*&        hordiff0, 
   const char*        text0,
   const int          lefthd0, 
   const int          righthd0,
   const int*&        hordiff1,
   const char*        text1,
   const int          lefthd1, 
   const int          righthd1
)
{
#ifdef XX_LOCAL_HDIFF_DEBUG
   using namespace std;
#endif

   int len0 = righthd0 - lefthd0;
   text0 += lefthd0-1; // we set to -1 because our table starts at -1
   int len1 = righthd1 - lefthd1;
   text1 += lefthd1-1;

#ifdef XX_LOCAL_HDIFF_DEBUG
   {
      QCString s1( text0 + 1, len0+1 );
      QCString s2( text1 + 1, len1+1 );
      cerr << "Comparing " << s1 << " to " << s2 << endl;
   }
#endif
   
   const bool ignoreWs = resources.getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS );

   int* hbuffer0;
   int* hbuffer1;
#ifdef XX_DEBUG
   int hbufferSize = 
#endif
      resources.getHordiffBuffers( hbuffer0, hbuffer1 );
   XX_ASSERT( hbuffer0 && hbuffer1 );

   const char* ctext0;
   const char* ctext1;
   int clen0, clen1;
   if ( ignoreWs ) {
      // Get buffer and first copy text into that buffer, stripping away
      // whitespace, before computing the multiple horizontal diffs.
      char* hlinebuf0;
      char* hlinebuf1;
      resources.getHordiffLineBuffers( hlinebuf0, hlinebuf1 );
      
      ctext0 = hlinebuf0;
      ctext1 = hlinebuf1;
      clen0 =
         strncpyNoWs( const_cast<char*>( &(ctext0[1]) ), &(text0[1]), len0 );
      clen1 =
         strncpyNoWs( const_cast<char*>( &(ctext1[1]) ), &(text1[1]), len1 );

#ifdef XX_LOCAL_HDIFF_DEBUG
      cerr << "==============================" << endl;
      QCString stext0( text0+1, len0+1 );
      cerr << stext0 << endl;
      cerr << ctext0+1 << endl;
      cerr << "------------------------------" << endl;
      QCString stext1( text1+1, len1+1 );
      cerr << stext1 << endl;
      cerr << ctext1+1 << endl;
      cerr << "------------------------------" << endl;
#endif
   }
   else { 
      ctext0 = text0;
      ctext1 = text1;
      clen0 = len0;
      clen1 = len1;
   }

   XX_CHECK( clen0 > 0 );
   XX_CHECK( clen1 > 0 );
   int tlen0 = clen0 + 1;
   int tlen1 = clen1 + 1;

   int* htable = resources.getDynProgTable( tlen0, tlen1 );
   if ( htable ) {
      // Initialize the table for longest common subsequence.
      int x, y;
      for ( x = 0; x < tlen0; ++x ) {
         htable[ x ] = 0;
      }
      for ( y = 0; y < tlen1; ++y ) {
         htable[ y*tlen0 ] = 0;
      }

      // Compute the table.
      for ( x = 1; x < tlen0; ++x ) {
         for ( y = 1; y < tlen1; ++y ) {
            if ( ctext0[x] == ctext1[y] ) {
               htable[ x + y*tlen0 ] = htable[ x-1 + (y-1)*tlen0 ] + 1;
            }
            else {
               htable[ x + y*tlen0 ] = 
                  std::max( htable[ x-1 + y*tlen0 ],
                            htable[ x + (y-1)*tlen0 ] );
            }
         }
      }

#ifdef XX_LOCAL_HDIFF_DEBUG
      // Print out the table for debugging.
      fprintf( stderr, "\n        " );
      for ( int ix = 1; ix < tlen0; ++ix ) {
         fprintf( stderr, " %c ", ctext0[ix] );
      }
      fprintf( stderr, "\n     " );
      for ( int ix = 0; ix < tlen0; ++ix ) {
         fprintf( stderr, "%2d ", ix );
      }
      fprintf( stderr, "\n\n" );
         
      for ( int iy = 0; iy < tlen1; ++iy ) {
         if ( iy == 0 ) 
            fprintf( stderr, "    " );
         else {
            fprintf( stderr, " \"%c\"", ctext1[iy] );
         }
         for ( int ix = 0; ix < tlen0; ++ix ) {
            fprintf( stderr, " %2d", htable[ ix + iy*tlen0 ] );
         }
         fprintf( stderr, "      %2d", iy );
         fprintf( stderr, "\n" );
      }
      fprintf( stderr, "\n" );
      fprintf( stderr, "\n" );
#endif

      const int hordiffthr = resources.getHordiffContext();

      // Traceback: look at only one path, this should be enough for our
      // purposes, but we can come back to it later.
      x = tlen0-1;
      y = tlen1-1;
      int c = 0;
      int beg[2] = { -1, -1 };
      int end[2] = { -1, -1 };
      bool inword = false;
      while ( x > -1 && y > -1 ) {
#ifdef XX_LOCAL_HDIFF_DEBUG
         cerr << x << " " << y << " = " << htable[ x + y*tlen0 ] << endl;
#endif

         if ( 
            ( htable[ x + y*tlen0 ] == ( htable[ x-1 + (y-1)*tlen0 ] + 1 ) /*||
              htable[ x + y*tlen0 ] == htable[ x-1 + (y-1)*tlen0 ]*/ ) &&
            ctext0[x] == ctext1[y] 

#if KEPT_FOR_HISTORY
            /* We don't need this anymore (2004-03-22).
               We post-process instead. */
            /* This next extra condition contributed to fix subtle problem,
               reproduce with these two lines:
               return (abcdefghijklmnopqrstuvwxyz + gosh + fiji + kill + \
                   mono + paques + roast + suv + wax + fly + zoo);
               return abcdefghijklmnopqrstuvwxyz;
               See bug [ 896045 ] for details.
            */
            &&
            ( htable[ x + y*tlen0 ] > htable[ x + (y-1)*tlen0 ] &&
              htable[ x + y*tlen0 ] > htable[ (x-1) + y*tlen0 ] )
#endif

         ) {
            if ( !inword ) {
               inword = true;
               end[0] = x;
               end[1] = y;
            }
            --x; --y;
         }
         else {
            if ( inword ) {
               beg[0] = x;
               beg[1] = y;

               if ( ( end[0] - beg[0] ) >= hordiffthr ) {
                  // Normally they should be equal.
                  XX_CHECK( (end[1] - beg[1]) == (end[0] - beg[0]) );

                  hbuffer0[c] = beg[0];
                  hbuffer0[c+1] = end[0];

                  hbuffer1[c] = beg[1];
                  hbuffer1[c+1] = end[1];

                  XX_CHECK( c < hbufferSize );
                  c += 2;
               }

               inword = false;
            }

            if ( htable[ x-1 + y*tlen0 ] > htable[ x + (y-1)*tlen0 ] ) {
               --x;
            }
            else { 
               --y;
            }
         }
      }

      // Add last word if was still active.
      if ( inword ) {
         beg[0] = x;
         beg[1] = y;

         hbuffer0[c] = beg[0];
         hbuffer0[c+1] = end[0];

         hbuffer1[c] = beg[1];
         hbuffer1[c+1] = end[1];

         XX_CHECK( c < hbufferSize );
         c += 2;
         inword = false;
      }


#ifdef XX_LOCAL_HDIFF_DEBUG
      printMatches( c, hbuffer0, ctext0, hbuffer1, ctext1 );
#endif
      int ii;

#if KEPT_FOR_HISTORY // IMPORTANT NOTE: this does not work, with following case.

// In file "f1":
// ------------------------------
// 1: ABCDEFG
// 2: ABCDEFG
// 3: sprintf( myString, "Number is %d, myInteger );
// 
// In file "f2":
// ------------------------------
// 1: _A_B_ABCDEFG;
// 2: ;ABCDEFG_F_G_
// 3: snprintf( myString, sizeof(myString), "Number is %d, myInteger );
// 

      // Attempt to elongate the end of the matches as much as possible, because
      // the algorithm functions from the end of the matches.
      int stop0 = tlen0;
      int stop1 = tlen1;
      for ( ii = c/2-1; ii >= 0; --ii ) {
         int end0 = hbuffer0[ii*2 + 1] + 1;
         int end1 = hbuffer1[ii*2 + 1] + 1;

         while ( end0 < tlen0 && end1 < tlen1 &&
                 ctext0[end0] == ctext1[end1] ) {
            ++end0; ++end1;
         }
         hbuffer0[ii*2 + 1] = end0 - 1;
         hbuffer1[ii*2 + 1] = end1 - 1;

         stop0 = hbuffer0[ii*2];
         stop1 = hbuffer1[ii*2];

#ifdef XX_LOCAL_HDIFF_DEBUG
      printMatches( c, hbuffer0, ctext0, hbuffer1, ctext1 );
#endif
      }
#endif


      // Create horizontal diffs (allocate and copy results into output, and
      // reorder).
      int* tmpdiff0 = new int[c + 3];
      int* tmpdiff1 = new int[c + 3];
      int cc = 0;
      tmpdiff0[cc] = 0;
      tmpdiff1[cc] = 0;
      ++cc;
      for ( int ii = c/2-1; ii >= 0; --ii ) {
         tmpdiff0[cc] = hbuffer0[ii*2];
         tmpdiff1[cc] = hbuffer1[ii*2];
         ++cc;

         tmpdiff0[cc] = hbuffer0[ii*2+1];
         tmpdiff1[cc] = hbuffer1[ii*2+1];
         ++cc;
      }
      tmpdiff0[cc] = clen0;
      tmpdiff1[cc] = clen1;
      ++cc;
      tmpdiff0[cc] = -1;
      tmpdiff1[cc] = -1;

#ifdef XX_LOCAL_HDIFF_DEBUG
      cerr << "------------------------------" << endl;

      for ( ii = 0; ii < cc; ++ii ) {
         cerr << tmpdiff0[ii] << " ";
      }
      cerr << endl;

      for ( ii = 0; ii < cc; ++ii ) {
         cerr << tmpdiff1[ii] << " ";
      }
      cerr << endl;
#endif

      // Adjust for removed whitespaces (shift back indices).
      if ( ignoreWs ) {
         adjustWs( tmpdiff0, &(text0[1]), len0 );
         adjustWs( tmpdiff1, &(text1[1]), len1 );
      }

      // Adjust hordiffs for left bracket.
      for ( ii = 0; ii < cc; ++ii ) {
         tmpdiff0[ii] += lefthd0;
         tmpdiff1[ii] += lefthd1;
      }

#ifdef XX_LOCAL_HDIFF_DEBUG
      for ( ii = 0; ii < cc; ++ii ) {
         cerr << tmpdiff0[ii] << " ";
      }
      cerr << endl;

      for ( ii = 0; ii < cc; ++ii ) {
         cerr << tmpdiff1[ii] << " ";
      }
      cerr << endl;
#endif

      hordiff0 = tmpdiff0;
      hordiff1 = tmpdiff1;
   }
   else {
      // Ouch!  The table cannot grow that big, so we just bracket the
      // horizontal diffs.

      int* tmpdiff0 = new int[3];
      tmpdiff0[0] = lefthd0;
      tmpdiff0[1] = righthd0;
      tmpdiff0[2] = -1;

      int* tmpdiff1 = new int[3];
      tmpdiff1[0] = lefthd1;
      tmpdiff1[1] = righthd1;
      tmpdiff1[2] = -1;

      hordiff0 = tmpdiff0;
      hordiff1 = tmpdiff1;
   }
}

XX_NAMESPACE_END


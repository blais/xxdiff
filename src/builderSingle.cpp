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

#include <builderSingle.h>
#include <diffs.h>
#include <buffer.h>

#include <qtextstream.h>

#include <stdio.h>
#include <iostream>

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxBuilderSingle
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxBuilderSingle::XxBuilderSingle() :
   XxBuilder()
{}

//------------------------------------------------------------------------------
//
XxBuilderSingle::~XxBuilderSingle()
{}

//------------------------------------------------------------------------------
//
std::auto_ptr<XxDiffs> XxBuilderSingle::process( const XxBuffer& buffer )
{
   initLines();

   QTextOStream errors( &_errors );

   _curHunk = 0;
   createInsertLeftBlock( 1, buffer.getNbLines() );

   // Set a succesful status.
   _status = 0;

   std::auto_ptr<XxDiffs> ap( new XxDiffs( _lines, false, false ) );
   return ap;
}


//------------------------------------------------------------------------------
//
void XxBuilderSingle::createInsertLeftBlock( 
   XxFln fline1,
   uint  fsize
)
{
   for ( uint ii = 0; ii < fsize; ++ii ) {
      XxLine line( XxLine::INSERT_1, fline1 + ii, -1 );
      line.setHunkId( _curHunk );
      addLine( line );
   }
   _curHunk++;
}

XX_NAMESPACE_END

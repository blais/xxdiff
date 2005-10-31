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

#ifndef INCL_XXDIFF_BUILDERSINGLE
#define INCL_XXDIFF_BUILDERSINGLE

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_BUILDER
#include <builder.h>
#endif

#ifndef INCL_XXDIFF_DIFFS
#include <diffs.h>
#endif

#ifndef INCL_XXDIFF_LINE
#include <line.h>
#endif

#ifndef INCL_STD_MEMORY
#include <memory>
#define INCL_STD_MEMORY
#endif

#ifndef INCL_STD_STDIO
#include <stdio.h>
#define INCL_STD_STDIO
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxResources;

/*==============================================================================
 * CLASS XxBuilderSingle
 *============================================================================*/

// <summary> a class to build the diffs data structure </summary>

// This class will spawn the diff program on the files and parse the output to
// produce a XxDiffs data structure that defines the difference type of each
// line of the files.  The files themselves are not parsed here.  They are
// referenced to by using line numbers only.

class XxBuilderSingle : public XxBuilder {

public:

   /*----- member functions -----*/

   // Constructor.
   XxBuilderSingle();

   // Destructor.
   virtual ~XxBuilderSingle();

   // Process the single file and fake no differences.
   std::auto_ptr<XxDiffs> process( const XxBuffer& buffer );

private:

   /*----- member functions -----*/

   void createInsertLeftBlock( 
      XxFln fline1,
      uint  fsize
   );

   /*----- data members -----*/

   int _curHunk;

};

XX_NAMESPACE_END

#endif

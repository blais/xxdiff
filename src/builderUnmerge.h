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

#ifndef INCL_XXDIFF_BUILDERUNMERGE
#define INCL_XXDIFF_BUILDERUNMERGE

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

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class QString;

XX_NAMESPACE_BEGIN

/*==============================================================================
 * FORWARD DECLARATIONS
 *============================================================================*/

class XxResources;

/*==============================================================================
 * CLASS XxBuilderUnmerge
 *============================================================================*/

// <summary> a class to build the diffs data structure </summary>

// This class will spawn the diff program on the files and parse the output to
// produce a XxDiffs data structure that defines the difference type of each
// line of the files.  The files themselves are not parsed here.  They are
// referenced to by using line numbers only.

class XxBuilderUnmerge : public XxBuilder {

public:

   /*----- member functions -----*/

   // Constructor.
   XxBuilderUnmerge();

   // Destructor.
   virtual ~XxBuilderUnmerge();

   // Process the file (unmerge it) and return a displayable diffs data
   // structure.  If an error occurs, this will throw.  Otherwise you are
   // guaranteed a valid newly allocated XxDiffs object.  However, there might
   // be warnings, so you may want to check hasErrors() if you want to display
   // those.
   std::auto_ptr<XxDiffs> process(
      const XxBuffer&    buffer,
      const XxResources& resources,
      QString&           outFileLeft,
      QString&           outFileRight
   );

   // Like the above process() method, but unmerge in 3 files instead of 2.
   std::auto_ptr<XxDiffs> process(
      const XxBuffer&    buffer,
      const XxResources& resources,
      QString&           outFileLeft,
      QString&           outFileMiddle,
      QString&           outFileRight
   );

private:

   /*----- member functions -----*/

   // Create the individual lines for different types of blocks.
   // <group>
   void createIgnoreBlock( 
      XxFln fline1,
      XxFln fline2,
      uint  fsize
   );
   void createIgnoreBlock( 
      XxFln fline1,
      XxFln fline2,
      XxFln fline3,
      uint  fsize
   );

   void createChangeBlock( 
      XxFln fline1,
      XxFln fline2,
      uint  fsize1,
      uint  fsize2
   );

   void createInsertLeftBlock( 
      XxFln fline1,
      uint  fsize
   );

   void createInsertRightBlock( 
      XxFln fline2,
      uint  fsize
   );
   // </group>

   // Create a DIFF_ALL or DIFF_DEL block.
   void create3Block( 
      XxLine::Type type,
      XxFln        f1n1,
      XxFln        f1n2,
      XxFln        f2n1,
      XxFln        f2n2,
      XxFln        f3n1,
      XxFln        f3n2
   );

   /*----- data members -----*/

   int _curHunk;

};

XX_NAMESPACE_END

#endif

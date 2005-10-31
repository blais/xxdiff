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

#ifndef INCL_XXDIFF_BUILDERFILES3
#define INCL_XXDIFF_BUILDERFILES3

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

class XxBuffer;

/*==============================================================================
 * CLASS XxBuilderFiles3
 *============================================================================*/

// <summary> a class to build the diffs data structure </summary>

// The three-way diff (diff3) output parser.

class XxBuilderFiles3 : public XxBuilder {

public:

   /*----- member functions -----*/

   // Constructor.  The filenames must be valid.
   XxBuilderFiles3();

   // Destructor.
   virtual ~XxBuilderFiles3();

   // Process the files and return a displayable diffs data structure.  If an
   // error occurs, this will throw.  Otherwise you are guaranteed a valid newly
   // allocated XxDiffs object.  However, there might be warnings, so you may
   // want to check hasErrors() if you want to display those.
   std::auto_ptr<XxDiffs> process(
      const QString& command,
      const XxBuffer& buffer1,
      const XxBuffer& buffer2,
      const XxBuffer& buffer3
   );

private:

   /*----- member functions -----*/

   // Create the individual lines for different types of blocks.
   // <group>
   void createIgnoreBlock( 
      XxFln fline1,
      XxFln fline2,
      XxFln fline3,
      uint  fsize
   );
   // </group>

   /*----- data members -----*/

   int                 _curHunk;
};

XX_NAMESPACE_END

#endif

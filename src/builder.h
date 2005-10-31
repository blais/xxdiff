/******************************************************************************\
 * $Id: builder.h 2 2000-09-15 02:19:22Z blais $
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

#ifndef INCL_XXDIFF_BUILDER2
#define INCL_XXDIFF_BUILDER2

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

#ifndef INCL_XXDIFF_DIFFS
#include <diffs.h>
#endif

#ifndef INCL_STD_MEMORY
#include <memory>
#define INCL_STD_MEMORY
#endif

#ifndef INCL_STD_STRING
#include <string>
#define INCL_STD_STRING
#endif

#ifndef INCL_STD_STDIO
#include <stdio.h>
#define INCL_STD_STDIO
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * CLASS XxBuilder
 *============================================================================*/

// <summary> base class for diffs structure builders </summary>

class XxBuilder {

public:

   /*----- member functions -----*/

   // Constructor.
   XxBuilder();

   // Destructor.
   virtual ~XxBuilder();

   // Returns the error text that occurred on the last process.
   // <group>
   bool hasErrors() const;
   const char* getErrors() const;
   // </group>

   // Returns the status of the diff command.
   // Note: the return value of xxdiff matches the one that the spawned diff
   // returns.  e.g. a statuc of 1 does not indicate an error.
   int getStatus() const;

protected:

   /*----- data members -----*/

   std::string         _errors;
   int                 _status;

};


XX_NAMESPACE_END

#include <builder.inline.h>

#endif

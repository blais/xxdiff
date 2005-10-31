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

#ifndef INCL_STD_VECTOR
#include <vector>
#define INCL_STD_VECTOR
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
   const QString& getErrors() const;
   // </group>

   // Returns the status of the diff command.
   // Note: the return value of xxdiff matches the one that the spawned diff
   // returns.  e.g. a statuc of 1 does not indicate an error.
   int getStatus() const;

protected:

   /*----- member functions -----*/

   // Initializes the line array. Call this before processing.
   void initLines();

   // Add a new XxLine.
   void addLine( const XxLine& line );

   /*----- data members -----*/

   QString             _errors;
   int                 _status;
   std::vector<XxLine> _lines;
   bool                _debugPrint;

};

XX_NAMESPACE_END

#include <builder.inline.h>

#endif

/******************************************************************************\
 * $Id: cmdline.h 331 2001-11-01 07:16:19Z blais $
 * $Date: 2001-11-01 02:16:19 -0500 (Thu, 01 Nov 2001) $
 *
 * Copyright (C) 1999-2001  Martin Blais <blais@iro.umontreal.ca>
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

#ifndef INCL_XXDIFF_CMDLINE
#define INCL_XXDIFF_CMDLINE

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DEFS
#include <defs.h>
#endif

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxCmdline
 *============================================================================*/

// <summary> command line parsing data </summary>

// This is used to store information parsed in the cmdline so that you don't
// have to do this in the XxApp constructor, thus if you exit without popping
// opening windows you do not need a connection to X.

class XxCmdline {

public:

   /*----- types and enumerations -----*/

   struct Option {
      const char* _longname;
      char        _shortname;
      bool        _has_arg;
      int         _val;
      const char* _help;
   };

   /*----- member functions -----*/

   // Constructor, makes a cmdline object with default values.
   XxCmdline();

   // Parse command line options and removes the recognized ones.  fileidx will
   // contain the beginning of the filenames on the command line.
   // This may throw.
   // If this returns false, don't run the app.
   bool parseCommandLine( int& argc, char**& argv );

   // Destructor.
   virtual ~XxCmdline();

   /*----- static member functions -----*/

   // Returns access to the list of options.
   static Option* getOptionList( int& nbOptions );

private:

   /*----- static member functions -----*/

   // List the resources on stdout.
   static void listResources();

   // Prints out HTML help on stdout.
   static void printHtmlHelp();

   /*----- friends -----*/

   friend class XxApp;

   /*----- data members -----*/

   // Cmdline-related variables.
   bool       _forceStyle;
   bool       _originalXdiff;
   QString    _userFilenames[3];
   QString    _stdinFilename;
   bool       _useRcfile;
   QString    _extraDiffArgs;
   QByteArray _cmdlineResources;
   bool       _mergeRequested;

   int        _nbFilenames;
   QString    _filenames[3];

   /*----- static data members -----*/

   static Option _options[];

};

XX_NAMESPACE_END

#endif


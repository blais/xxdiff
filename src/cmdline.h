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

   enum OptType {
      OPT_GENERIC = 0,
      OPT_XXDIFF = 1,
      OPT_DISPLAY = 2,
      OPT_DIFF = 3,
      OPT_QT = 4,
      OPT_DEFAULT = ( 1<<0 | 1<<1 | 1<<2 | 1<<3 ),
      OPT_ALL = ( 1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 )
   };

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

   // Parse command line options. This may throw.  If this returns false, don't
   // run the app after parsing the command-line.
   bool parseCommandLine( const int argc, char* const* argv );

   // Returns options suitable for QApplication consumption.
   void getQtOptions( int& argc, char**& argv );
   
   // Destructor.
   virtual ~XxCmdline();

   /*----- static member functions -----*/

   // Returns access to the list of options.
   static Option* getOptionList( OptType otype, int& nbOptions );

private:

   /*----- static member functions -----*/

   // List the resources on stdout.
   static void listResources();

   // Prints out HTML help on stdout.
   static void printHtmlHelp();

   // Prints out version info stdout.
   static void printVersion();

   // Prints out invocation txt help.
   static void printInvocation();

   // Search for an option by code character.  Returns -1 if not found.
   int searchForOption( 
      const XxCmdline::Option* options,
      const int                nbOptions,
      const int                c
   );

   /*----- friends -----*/

   friend class XxApp;

   /*----- data members -----*/

   // Cmdline-related variables.
   bool          _forceStyle;
   bool          _forceGeometry;
   bool          _forceFont;

   bool          _originalXdiff;
   QString       _userFilenames[3];
   QString       _stdinFilename;
   bool          _useRcfile;
   QString       _extraDiffArgs;
   QString       _mergedFilename;
   QByteArray    _cmdlineResources;

   int           _nbFilenames;
   QString       _filenames[3];

   bool          _single;
   bool          _unmerge;
   // Nb.files to unmerge into.
   int           _unmergeNbFiles;

   bool          _forceDecision;
   bool          _macNewlines;
   bool          _indicateInputProcessed;
   bool          _useTemporaryFiles;
   bool          _promptForFiles;

   /*----- static data members -----*/

   static Option _optionsGeneric[];
   static Option _optionsXxdiff[];
   static Option _optionsDisplay[];
   static Option _optionsDiff[];
   static Option _optionsQt[];

   int           _nbQtOptions;
   char*         _qtOptions[64]; // 64 Qt options! Way beyond you'll ever need!
   char*         _qtOptionsCopy[64];

};

XX_NAMESPACE_END

#endif

/******************************************************************************\
 * $Id: cmdline.cpp 417 2001-11-26 23:06:52Z blais $
 * $Date: 2001-11-26 18:06:52 -0500 (Mon, 26 Nov 2001) $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <cmdline.h>
#include <exceptions.h>
#include <resParser.h>
#include <help.h>

#ifndef INCL_RESPARSER_Y
#include <resParser.y.h> // For FONT_TEXT
#define INCL_RESPARSER_Y
#endif

#include <qapplication.h>
#include <qcstring.h>

/*#define getopt xxdiff_getopt*/
#include <getopt.h>
#include <stdlib.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

//------------------------------------------------------------------------------
//
int initOptions(
   const XxCmdline::Option* options,
   const int                nbOptions,
   struct option*           go_longopts,
   QString&                 go_shortopts
)
{
   // Note: no bounds checking is done.
   int ii;
   for ( ii = 0; ii < nbOptions; ++ii ) {
      go_longopts[ii].name = const_cast<char*>( options[ii]._longname );
      // const_cast is needed for solaris-cc compiler.
      go_longopts[ii].has_arg = ( options[ii]._has_arg ? 1 : 0 );
      go_longopts[ii].flag = 0;
      go_longopts[ii].val = options[ii]._val;

      if ( options[ii]._shortname != 0 ) {
         go_shortopts.append( options[ii]._shortname );
         if ( options[ii]._has_arg ) {
            go_shortopts.append( ":" );
         }
      }
   }
   go_longopts[ii].name = 0;
   go_longopts[ii].has_arg = 0;
   go_longopts[ii].flag = 0;
   go_longopts[ii].val = 0;

   return nbOptions;
}

};

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxCmdline
 *============================================================================*/

/*----- static data members -----*/

//
// Generic options.
//
XxCmdline::Option XxCmdline::_optionsGeneric[] = {
   { "help", 0, false, 'h',
     "Show help about options."
   }, 
   { "help-qt", 0, false, 'Z',
     "Show Qt specific options."
   }, 
   { "help-all", 0, false, 'Y',
     "Show all options."
   }, 
   { "help-html", 0, false, 'H',
     "Output documentation in HTML."
   }, 
//#define XX_KDE_CMDLINE
#ifdef XX_KDE_CMDLINE
   { "author", 0, false, 'X', 
     "Show author information."
   }, 
#endif
   { "version", 'v', false, 'v', 
     "Show version information."
   }, 
#ifdef XX_KDE_CMDLINE
   { "license", 0, false, 'W', 
     "Show license information."
   }, 
#endif
};

//
// Xxdiff options
//
XxCmdline::Option XxCmdline::_optionsXxdiff[] = {
   { "no-rcfile", 0, false, 'n', 
     "Don't query rcfile resources (.xxdiffrc)."
   }, 
   { "list-resources", 0, false, 'l', 
     "Lists all the supported resources and default values."
   }, 
   { "exit-on-same", 'D', false, 'D', 
     "If there are no differences then exit quietly with exit code of 0."
   }, 
   { "merge", 'm', false, 'm', 
     "Automatically select regions that would end up being selected by "
     "an automatic merge."
   }, 
#if 0 // do this soon.
   { "conflict", 'C', false, 'C', 
     "Invoke on a single file with CVS merge conflicts, splitting the "
     "conflicts into two files for display.  If this is specified, only a "
     "single file can then be given as argument."
   }, 
#endif
   { "title1", 0, true, '1', 
     "Display 'str' instead of filename in filename label 1 (left)."
   }, 
   { "title2", 0, true, '2', 
     "Display 'str' instead of filename in filename label 2 (middle)."
   }, 
   { "title3", 0, true, '3',
     "Display 'str' instead of filename in filename label 3 (right)."
   }, 
   { "titlein", 'N', true, 'N', // This is kept for xdiff compatibility only.
     "Display 'str' for filename in given in stdin."
   }, 
   { "resource", 0, true, 'R',
     "Pass on string 'str' to resource parser.  Resources given in this manner "
     "on the command line supersede other resource mechanisms.  One can "
     "specify multiple resource settings by repeating this option."
   }, 
   { "args", 'A', true, 'A',
     "Pass on argument to the subordinate diff program."
   }, 
   { "orig-xdiff", 0, false, 'o',
     "Used settings as close as possible to original xdiff "
     "(for the romantics longing the old days of SGI... snif snif)."
   },
};

//
// GNU diff options.
//
XxCmdline::Option XxCmdline::_optionsDiff[] = {
   { "ignore-all-space", 'w', false, 'w',
     "Option passed to 2-files diff(1). "
     "Ignore white space when comparing lines."
   }, 
   { "ignore-space-change", 'b', false, 'b', 
     "Option passed to 2-files diff(1). "
     "Ignore changes in amount of white space."
   }, 
   { "ignore-case", 'i', false, 'i', 
     "Option passed to 2-files diff(1). "
     "Ignore changes in case; consider upper- and lower-case to be the same."
   }, 

#if 0 // Ignore blank lines... make this work eventually (see TODO file).
   { "ignore-blank-lines", 'B', false, 'B', 
     "Option passed to 2-files diff(1). "
     "Ignore changes that just insert or delete blank lines."
   }, 
#endif
   { "recursive", 'r', false, 'r',
     "Option passed to 2-files diff(1). "
     "This is only meaningful for directory diffs."
   }, 
   { "text", 'a', false, 'a',
     "Option passed to 2-files diff(1). "
     "Treat all files as text and compare them "
     "line-by-line, even if they do not appear to be text."
   }, 

};

//
// Qt options.
//
XxCmdline::Option XxCmdline::_optionsQt[] = {
   { "qt", 'Q', true, 'Q',
     "Set options to be given to Qt application options parser. "
     "See Qt manual, class QApplication for a list of supported options."
   }, 
   { "display", 0, true, 'd',
     "Sets the X display (default is $DISPLAY)."
   }, 
   { "style", 0, true, 's',
     "Sets the application GUI style. Possible values are motif, windows, "
     "and platinum. "
   }, 
   { "geometry", 0, true, 'g',
     "Sets the client geometry of the main widget."
   }, 
   { "font", 0, true, 'M',
     "Defines the application font (for widgets)."
   }, 
   { "textfont", 0, true, 'J',
     "Defines the text font (for diff text)."
   }, 
   { "name", 0, false, 'L',
     "Sets the application name."
   }, 
   { "visual", 0, true, 'V',
     "Forces the application to use a particular visual on an 8-bit display "
     "(e.g. TrueColor)."
   },
   { "ncols", 0, true, 'O',
     "Limits the number of colors allocated in the color cube on a 8-bit "
     "display."
     // ", if the application is using the QApplication::ManyColor color "
     // "specification. If count is 216 then a 6x6x6 color cube is used (ie. 6 "
     // "levels of red, 6 of green, and 6 of blue); for other values, a cube "
     // "approximately proportional to a 2x3x1 cube is used."
   },
   { "cmap", 0, false, 'P',
     "Causes the application to install a private color map on an 8-bit "
     "display."
   }, 
   { "nograb", 0, false, 'S',
     "Tells Qt to never grab the mouse or the keyboard."
   }, 
   { "dograb", 0, false, 'T',
     "Running under a debugger can cause an implicit -nograb, use -dograb "
     "to override."
   }, 
   { "sync", 0, false, 'U',
     "Switches to synchronous mode for debugging."
   }, 
}; 

//------------------------------------------------------------------------------
//
XxCmdline::XxCmdline() :
   _forceStyle( false ),
   _forceGeometry( false ),
   _forceFont( false ),
   _originalXdiff( false ),
   _useRcfile( true ), 
   _extraDiffArgs( "" ),
   _mergeRequested( false ),
   _conflict( false ),
   _nbQtOptions( 0 ),
   _qtOptions()
{
   _userFilenames[0] = "";
   _userFilenames[1] = "";
   _userFilenames[2] = "";
   _stdinFilename = "(stdin)";
}

//------------------------------------------------------------------------------
//
XxCmdline::~XxCmdline()
{
   for ( int ii = 0; ii < _nbQtOptions; ++ii ) {
      XX_ASSERT( _qtOptions[ii] != 0 );
      delete[] _qtOptions[ii];
   }
}

//------------------------------------------------------------------------------
//
bool XxCmdline::parseCommandLine( const int argc, char* const* argv )
{
   XX_ASSERT( argc > 0 );
   _qtOptions[ _nbQtOptions++ ] = qstrdup( argv[0] );

   // The command-line parsing has to be carried out before the resource
   // building because some of the command-line options affect the way resources
   // are built up (e.g. --no-rcfile).  So command-line options that should
   // supersede resources and explicit resources specified on the command-line
   // are stored in the _cmdlineResources.

   int optionIndex = 0;
   const int nbTotalOptions = 
      ( sizeof(_optionsGeneric) + 
        sizeof(_optionsXxdiff) + 
        sizeof(_optionsDiff) + 
        sizeof(_optionsQt) ) / sizeof(XxCmdline::Option);
   struct option longOptions[ nbTotalOptions + 1 ];
   QString shortOptions = "";
   int nbopt = 0;
   nbopt += initOptions(
      _optionsGeneric,
      sizeof(_optionsGeneric) / sizeof(XxCmdline::Option),
      &( longOptions[nbopt] ),
      shortOptions
   );
   nbopt += initOptions(
      _optionsXxdiff,
      sizeof(_optionsXxdiff) / sizeof(XxCmdline::Option),
      &( longOptions[nbopt] ),
      shortOptions
   );
   nbopt += initOptions(
      _optionsDiff,
      sizeof(_optionsDiff) / sizeof(XxCmdline::Option),
      &( longOptions[nbopt] ),
      shortOptions
   );
   nbopt += initOptions(
      _optionsQt,
      sizeof(_optionsQt) / sizeof(XxCmdline::Option),
      &( longOptions[nbopt] ),
      shortOptions
   );
   
   // Do the parsing.
   while ( true ) {
      int c = getopt_long( argc,
                           argv,
                           shortOptions.latin1(),
                           longOptions,
                           &optionIndex );
      if ( c == -1 ) {
         break;
      }
              
      switch ( c ) {
         //
         // Generic options.
         //
         case 'h':
            throw XxUsageError( XX_EXC_PARAMS, QString::null, OPT_DEFAULT );

         case 'Z':
            throw XxUsageError( XX_EXC_PARAMS,
                                QString::null, 
                                1 << OPT_GENERIC | 1 << OPT_QT );

         case 'Y':
            throw XxUsageError( XX_EXC_PARAMS, QString::null, OPT_ALL );

         case 'H': {
            printHtmlHelp();
            return false;
         } break;

         case 'v': {
            printVersion();
            return false;
         } break;

         //
         // Xxdiff options.
         //
         case 'n':
            _useRcfile = false;
            break;

         case 'l': {
            listResources();
            return false;
         } break;

         case 'D': {
            QTextStream oss( _cmdlineResources, IO_WriteOnly | IO_Append );
            oss << XxResParser::getBoolOptName( BOOL_EXIT_ON_SAME )
                << ": true" << endl;
         } break;

         case 'm': {
            _mergeRequested = true;
         } break;

         case 'C': {
            _conflict = true;
         } break;

         case '1':
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS,
                                   "Missing argument for title option." );
            }
            _userFilenames[0] = optarg;
            break;

         case '2':
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS, 
                                   "Missing argument for title option." );
            }
            _userFilenames[1] = optarg;
            break;

         case '3':
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS, 
                                   "Missing argument for title option." );
            }
            _userFilenames[2] = optarg;
            break;

         case 'N':
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS,
                                   "Missing argument for title option." );
            }
            _stdinFilename = optarg;
            break;

         case 'R': {
            QTextStream oss( _cmdlineResources, IO_WriteOnly | IO_Append );
            oss << optarg << endl << flush;
         } break;

         case 'A': {
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS,
                                   "Missing argument for sub. diff program." );
            }
            _extraDiffArgs.append( optarg );
         } break;
         
         case 'o': {
            _originalXdiff = true;
         } break;

         //
         // GNU diff options.
         //
         case 'w':
         case 'b':
         case 'i':
         case 'a':
            char optionString[4];
            optionString[0] = ' ';
            optionString[1] = '-';
            optionString[2] = c;
            optionString[3] = 0;
            _extraDiffArgs.append( optionString );
            break;

         case 'r': {
            QTextStream oss( _cmdlineResources, IO_WriteOnly | IO_Append );
            oss << XxResParser::getBoolOptName( BOOL_DIRDIFF_RECURSIVE ) 
                << ": true" << endl;
         } break;

         //
         // Qt options.
         //
         case 'Q': {
            // Split string to spaces and add as individual options.
            QCString qtopts( optarg );
            qtopts.simplifyWhiteSpace();
            int previdx = 0;
            int idx = -1;
            while ( ( idx = qtopts.find( ' ', idx+1 ) ) != -1 ) {
               QCString qtopt = qtopts.mid( previdx, idx - previdx );
               _qtOptions[ _nbQtOptions++ ] = 
                  qstrdup( static_cast<const char*>( qtopt ) );
               XX_ASSERT( _nbQtOptions < 64 ); // just check
               previdx = idx+1;
            }
            QCString qtopt = qtopts.mid( previdx );
            _qtOptions[ _nbQtOptions++ ] = 
               qstrdup( static_cast<const char*>( qtopt ) );
            XX_ASSERT( _nbQtOptions < (64-1) ); // just check
         } break;

         case 'd':
         case 's':
         case 'g':
         case 'M':
         case 'L':
         case 'V':
         case 'O':
         case 'P':
         case 'S':
         case 'T':
         case 'U': {
            int oidx = searchForOption( 
               _optionsQt, sizeof(_optionsQt)/sizeof(Option), c
            );
            XX_ASSERT( oidx != -1 );

            QCString qtopt( "-" );
            qtopt += _optionsQt[ oidx ]._longname;

            // Add argument too.

            if ( _optionsQt[ oidx ]._has_arg == true ) {
               _qtOptions[ _nbQtOptions++ ] =
                  qstrdup( static_cast<const char*>( qtopt ) );
               XX_ASSERT( _nbQtOptions < (64-1) ); // just check
               
               _qtOptions[ _nbQtOptions++ ] = qstrdup( optarg );
               XX_ASSERT( _nbQtOptions < (64-1) ); // just check
            }
            else {
               _qtOptions[ _nbQtOptions++ ] =
                  qstrdup( static_cast<const char*>( qtopt ) );
               XX_ASSERT( _nbQtOptions < (64-1) ); // just check
            }

         } break;

         case 'J': {
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS,
                                   "Missing argument for text font." );
            }

            QTextStream oss( _cmdlineResources, IO_WriteOnly | IO_Append );
            oss << XxResParser::getKwdName( FONT_TEXT )
                << ": \"" << optarg << "\"" << endl;
         } break;

         case 0:
            throw XxInternalError( XX_EXC_PARAMS );

         case ':':
         case '?': {
            throw XxUsageError( XX_EXC_PARAMS, "Argument error." );
         } break;

         default:
            throw XxUsageError( XX_EXC_PARAMS );
      }
   }

   // Count number of filenames specified on the cmdline.
   _nbFilenames = argc - optind;
      
   // End qt options with a marker.
   _qtOptions[ _nbQtOptions ] = 0;

   // Check if there are too many.
   if ( _conflict && _nbFilenames != 1 ) {
      throw XxUsageError(
         XX_EXC_PARAMS, 
         "You need to specify a single filename in conflict mode."
      );
   }

   int minfn = _conflict ? 1 : 2;
   int maxfn = _conflict ? 1 : 3;

   if ( _nbFilenames < minfn ) {
      QString msg;
      {
         QTextOStream oss( &msg );
         oss << "You must specify at least " << minfn << " filenames.";
      }
      throw XxUsageError( XX_EXC_PARAMS, msg );
   }

   if ( _nbFilenames > maxfn ) {
      QString msg;
      {
         QTextOStream oss( &msg );
         oss << "You can specify at most " << maxfn << " filenames." << endl;
         oss << "Extra arguments: \"";
         for ( int ii = maxfn; ii < _nbFilenames; ++ii ) {
            oss << " " << argv[ optind + ii ];
         }
         oss << "\"";
      }
      throw XxUsageError( XX_EXC_PARAMS, msg );
   }

   // Disable conflict until implemented.
   if ( _conflict ) {
      throw XxUsageError( XX_EXC_PARAMS, "Not yet implemented." );
   }

   // Read filenames.
   int ii;
   for ( ii = 0; ii < _nbFilenames; ++ii ) {
      _filenames[ ii ].setLatin1( argv[ optind + ii ] );
      _filenames[ ii ] = _filenames[ ii ].stripWhiteSpace();
   }
   
   // Detect qt options.
   for ( ii = 0; ii < _nbQtOptions; ++ii ) {
      if ( strncmp( _qtOptions[ ii ], "-style", 6 ) == 0 ) {
         _forceStyle = true;
      }
      else if ( strncmp( _qtOptions[ ii ], "-geometry", 9 ) == 0 ) {
         _forceGeometry = true;
      }
      else if ( strncmp( _qtOptions[ ii ], "-fn", 3 ) == 0 ||
                strncmp( _qtOptions[ ii ], "-font", 5 ) == 0 ) {
         _forceFont = true;
      }
   }

   return true;
}

//------------------------------------------------------------------------------
//
void XxCmdline::getQtOptions( int& argc, char**& argv )
{
   // We make a copy because Qt likes to remove the options it parses.
   for ( int ii = 0; ii < _nbQtOptions; ++ii ) {
      _qtOptionsCopy[ii] = _qtOptions[ii];
   }

   argc = _nbQtOptions;
   argv = _qtOptionsCopy;
}

//------------------------------------------------------------------------------
//
void XxCmdline::listResources()
{
   // Open display connection, we need that for font and color conversions.
   int argc = 0;
   QApplication app( argc, 0 );

   // This lists the default resources, not the ones parsed from the
   // rcfile or command-line.
   QTextStream oss_cout( stdout, IO_WriteOnly );
   XxResParser::listResources( oss_cout );
   oss_cout << flush;
}

//------------------------------------------------------------------------------
//
void XxCmdline::printHtmlHelp()
{
   // We need a QApplication object to be able to list the resources
   // documentation.  Perhaps we could revise this.
   int argc = 0;
   QApplication app( argc, 0 );

   {
      QTextStream os( stdout, IO_WriteOnly );
      os << "<html>" << endl
         << "<head>" << endl
         << "<title>xxdiff documentation</title>" << endl
         << "</head>" << endl
         << "<body>" << endl
         << XxHelp::getManual() << endl
         << "</body>" << endl
         << "</html>" << endl;
   }
}

//------------------------------------------------------------------------------
//
void XxCmdline::printVersion()
{
   QTextStream oss_cout( stdout, IO_WriteOnly );
   oss_cout << "xxdiff " << XxHelp::getVersion()
#ifdef XX_DEBUG
            << " [debug]"
#endif
            << endl;
   oss_cout << QString("  (Qt: %1)").arg( qVersion() ) << endl;
   oss_cout << "  Written by Martin Blais <blais@iro.umontreal.ca>" << endl
            << flush;
}

//------------------------------------------------------------------------------
//
XxCmdline::Option* XxCmdline::getOptionList( OptType otype, int& nbOptions )
{
   XxCmdline::Option* options = 0;
   switch ( otype ) {
      case OPT_GENERIC: {
         nbOptions = sizeof(_optionsGeneric);
         options = _optionsGeneric;
      } break;
      case OPT_XXDIFF: {
         nbOptions = sizeof(_optionsXxdiff);
         options = _optionsXxdiff;
      } break;
      case OPT_DIFF: {
         nbOptions = sizeof(_optionsDiff);
         options = _optionsDiff;
      } break;
      case OPT_QT: {
         nbOptions = sizeof(_optionsQt);
         options = _optionsQt;
      } break;
      default: XX_ABORT();
   }
   nbOptions /= sizeof(XxCmdline::Option);
   return options;
}

//------------------------------------------------------------------------------
//
int XxCmdline::searchForOption( 
   const XxCmdline::Option* options,
   const int                nbOptions,
   const int                c
)
{
   for ( int ii = 0; ii < nbOptions; ++ii ) {
      if ( options[ii]._val == c ) {
         return ii;
      }
   }
   return -1;
}

XX_NAMESPACE_END


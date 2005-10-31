/******************************************************************************\
 * $Id: cmdline.cpp 338 2001-11-05 05:37:35Z blais $
 * $Date: 2001-11-05 00:37:35 -0500 (Mon, 05 Nov 2001) $
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

#include <qapplication.h>

/*#define getopt xxdiff_getopt*/
#include <getopt.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

//------------------------------------------------------------------------------
//
void initOptions(
   const XxCmdline::Option* options,
   const int                nbOptions,
   struct option*           go_longopts,
   QString&                 go_shortopts
)
{
   // Note: no bounds checking is done.
   go_shortopts = "";
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
}

};

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxCmdline
 *============================================================================*/

/*----- static data members -----*/

XxCmdline::Option XxCmdline::_options[] = {
   { "help", 'h', false, 'h',
     "Show help message."
   }, 
   { "help-html", 0, false, 'H',
     "Output documentation in HTML (this is mainly used by the author to "
     "generate up-to-date web documentation)."
   }, 
   { "version", 'v', false, 'v', 
     "Show the program version and compilation options."
   }, 
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

   //
   // GNU diff options.
   //
   { "ignore-all-space", 'w', false, 'w',
     "Option passed to diff(1). "
     "Ignore white space when comparing lines."
   }, 
   { "ignore-space-change", 'b', false, 'b', 
     "Option passed to diff(1). "
     "Ignore changes in amount of white space."
   }, 
   { "ignore-case", 'i', false, 'i', 
     "Option passed to diff(1). "
     "Ignore changes in case; consider upper- and lower-case to be the same."
   }, 
// FIXME add this eventually.
#if 0 
   { "ignore-blank-lines", 'B', false, 'B', 
     "Option passed to diff(1). "
     "Ignore changes that just insert or delete blank lines."
   }, 
#endif
   { "recursive", 'r', false, 'r',
     "Option passed to diff(1). "
     "This is only meaningful for directory diffs."
   }, 
   { "text", 'a', false, 'a',
     "Option passed to diff(1). "
     "Treat all files as text and compare them "
     "line-by-line, even if they do not appear to be text."
   }, 

}; 

//------------------------------------------------------------------------------
//
XxCmdline::XxCmdline() :
   _forceStyle( false ),
   _originalXdiff( false ),
   _useRcfile( true ), 
   _extraDiffArgs( "" ),
   _mergeRequested( false )
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
}

//------------------------------------------------------------------------------
//
bool XxCmdline::parseCommandLine( int& argc, char**& argv )
{
   // Search original arguments to see if the style has been explicitly
   // forced.
   int aa;
   for ( aa = 0; aa < argc; ++aa ) {
      if ( ::strncmp( argv[aa], "-style", 6 ) == 0 ) {
         break;
      }
   }
   _forceStyle = (aa == argc);

   // The command-line parsing has to be carried out before the resource
   // building because some of the command-line options affect the way resources
   // are built up (e.g. --no-rcfile).  So command-line options that should
   // supersede resources and explicit resources specified on the command-line
   // are stored in the _cmdlineResources.

   int optionIndex = 0;
   struct option longOptions[ sizeof(_options)/sizeof(Option) + 1 ];
   QString shortOptions;
   initOptions(
      _options,
      sizeof(_options)/sizeof(XxCmdline::Option),
      longOptions,
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
         case 'h':
            throw XxUsageError( XX_EXC_PARAMS, QString::null, true );

         case 'H': {
            printHtmlHelp();
            return false;
         } break;
            
         case 'v':
            throw XxUsageError( XX_EXC_PARAMS, QString::null, true, true );

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

         case 'r': {
            QTextStream oss( _cmdlineResources, IO_WriteOnly | IO_Append );
            oss << XxResParser::getBoolOptName( BOOL_DIRDIFF_RECURSIVE ) 
                << ": true" << endl;
         } break;

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

         case 'A':
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS );
            }
            _extraDiffArgs.append( optarg );
            break;
            
         case 'm':
            _mergeRequested = true;
            break;

         case '1':
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS );
            }
            _userFilenames[0] = optarg;
            break;

         case '2':
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS );
            }
            _userFilenames[1] = optarg;
            break;

         case '3':
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS );
            }
            _userFilenames[2] = optarg;
            break;

         case 'N':
            if ( !optarg ) {
               throw XxUsageError( XX_EXC_PARAMS );
            }
            _stdinFilename = optarg;
            break;

         case 'R': {
            QTextStream oss( _cmdlineResources, IO_WriteOnly | IO_Append );
            oss << optarg << endl << flush;
         } break;
         
         case 'o': {
            _originalXdiff = true;
         } break;

         case 0:
            throw XxInternalError( XX_EXC_PARAMS );

         case ':':
         case '?':
            throw XxUsageError( XX_EXC_PARAMS );
            
         default:
            throw XxUsageError( XX_EXC_PARAMS );
      }
   }

   // Read filenames.
   _nbFilenames = argc - optind;
   for ( int ii = 0; ii < _nbFilenames; ++ii ) {
      _filenames[ ii ].setLatin1( argv[ optind + ii ] );
      _filenames[ ii ] = _filenames[ ii ].stripWhiteSpace();
   }

   return true;
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
XxCmdline::Option* XxCmdline::getOptionList( int& nbOptions )
{
   nbOptions = sizeof(_options)/sizeof(XxCmdline::Option);
   return _options;
}

XX_NAMESPACE_END


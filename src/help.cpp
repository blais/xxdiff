/******************************************************************************\
 * $Id: help.cpp 56 2000-12-25 20:15:47Z  $
 * $Date: 2000-12-25 15:15:47 -0500 (Mon, 25 Dec 2000) $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <help.h>
#include <line.h>
#include <resources.h>

#ifndef XX_USE_RUNTIME_HELP
#include <man.h>
#endif

#include <qdialog.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpalette.h>
#include <qtextview.h>
#include <qscrollview.h>

#include <sstream>
#include <iostream>
#include <stdio.h>


/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

/*----- variables -----*/

const char* whatsThisStrings[] = {

   //  VSCROLL
   "This is the vertical scrollbar used to navigate the files.",

   //  HSCROLL
   "This is the horizontal scrollbar used for all files simultaneously.",
   
   //  OVERVIEW
   "This is the overview area\n\n"
   "This area contains a pictorial representation of the diff hunks "
   "as well as outlines of the visible regions and the current cursor "
   "position.",

   //  FILENAME
   "This is the filename label.\n\n"
   "Clicking on it pastes the filename into the clipboard for cut-n-paste.",

   //  LINENO
   "This is the line number label.\n\n"
   "It display the line number at the cursor in the appropriate file",

   //  TEXT_VIEW
   "This is the text view.\n\n"
   "It contains the text highlighted in hunks of different types:\n\n"
   "IGNORE regions: the same text in all files.\n"
   "CHANGE regions: text is present in all files but different.\n"
   "INSERT/DELETE regions: text is present in only one of the files.\n",

   //  MERGED_VIEW
   "This is the merged view.\n\n"
   "The merged view contains the file that results from the selections."

};



/*==============================================================================
 * LOCAL CLASS XxAboutDialog
 *============================================================================*/

// <summary> the about dialog </summary>

class XxAboutDialog : public QMessageBox {

public:

   /*----- member functions -----*/

   // Constructor.
   XxAboutDialog( QWidget* parent, QString& text );

   // See base class.
   virtual void done( int r );

};

//------------------------------------------------------------------------------
//
XxAboutDialog::XxAboutDialog( QWidget* parent, QString& text ) :
   QMessageBox( 
      "About xxdiff.", text, QMessageBox::Information, 
      1, 0, 0, parent, 0, false 
   )
{}

//------------------------------------------------------------------------------
//
void XxAboutDialog::done( int )
{
   delete this;
}


/*==============================================================================
 * LOCAL CLASS XxColorLegend
 *============================================================================*/

// <summary> the color legend dialog </summary>

class XxColorLegend : public QDialog {

public:

   /*----- member functions -----*/

   // Constructor.
   XxColorLegend( QWidget* parent );

   // See base class.
   virtual void accept();
   
private:

   /*----- member functions -----*/

   // Creates a label.
   QLabel* createLabel( 
      const char* text, 
      QColor& fore,
      QColor& back,
      const QFont& font
   );

};

//------------------------------------------------------------------------------
//
XxColorLegend::XxColorLegend( 
   QWidget* parent
) :
   QDialog( parent )
{
   QVBoxLayout* toplay = new QVBoxLayout( this );
   QLabel* l = new QLabel( "Color legend:", this );
   l->adjustSize();
   l->setMinimumHeight( l->height() + 10 );
   toplay->addWidget( l );

   const XxResources* resources = XxResources::getInstance();
   const QFont& font = resources->getTextFont();
   QColor fore, back;

   const struct Expl {
      XxResources::Resource _type;
      const char*           _expl;
   } explanations[] = {
      { XxResources::COLOR_BACK_SAME,
        " Identical text " },
      { XxResources::COLOR_BACK_DIFF_ONE,
        " Different in one file " },
      { XxResources::COLOR_BACK_DIFF_ONE_SUP,
        " Different in one file (shadowed) " },
      { XxResources::COLOR_BACK_DIFF_ONE_ONLY,
        " Different in one file (only text on lines) " },
      { XxResources::COLOR_BACK_DIFF_ONE_NONLY,
        " Different in one file (blank side)" },
      { XxResources::COLOR_BACK_DIFF_TWO,
        " Common text in two files only " },
      { XxResources::COLOR_BACK_DIFF_TWO_SUP,
        " Common text in two files only (shadowed) " },
      { XxResources::COLOR_BACK_DIFF_TWO_ONLY,
        " Common text in two files only (only text on lines) " },
      { XxResources::COLOR_BACK_DIFF_TWO_NONLY,
        " Common text in two files only (blank side) " },
      { XxResources::COLOR_BACK_DELETE,
        " Delete text (side with text) " },
      { XxResources::COLOR_BACK_DELETE_BLANK,
        " Delete text (blank side) " },
      { XxResources::COLOR_BACK_INSERT,
        " Insert text (side with text) " },
      { XxResources::COLOR_BACK_INSERT_BLANK,
        " Insert text (blank side) " },
      { XxResources::COLOR_BACK_DIFF_ALL,
        " Different in all files " },
      { XxResources::COLOR_BACK_DIFF_ALL_SUP,
        " Different in all files (shadowed) " },
      { XxResources::COLOR_BACK_DIFF_ALL_ONLY,
        " Different in all files (only text on lines) " },
      { XxResources::COLOR_BACK_DIFF_ALL_NONLY,
        " Different in all files (blank side) " },
      { XxResources::COLOR_BACK_DIFFDEL,
        " Different and delete text " },
      { XxResources::COLOR_BACK_DIFFDEL_SUP,
        " Different and delete text (shadowed) " },
      { XxResources::COLOR_BACK_DIFFDEL_ONLY,
        " Different and delete text (only text on lines) " },
      { XxResources::COLOR_BACK_DIFFDEL_NONLY,
        " Different and delete text (blank side) " },
      { XxResources::COLOR_BACK_DIFFDEL_BLANK,
        " Different and delete text (empty side) " },
      { XxResources::COLOR_BACK_SELECTED,
        " Selected text " },
      { XxResources::COLOR_BACK_SELECTED_SUP,
        " Selected text (shadowed) " },
      { XxResources::COLOR_BACK_DELETED,
        " Deleted text " },
      { XxResources::COLOR_BACK_DELETED_SUP,
        " Deleted text (shadowed) " },
      { XxResources::COLOR_BACK_DIRECTORIES,         
        " Directories in directory diffs " },
      { XxResources::COLOR_BACK_MERGED_UNDECIDED,
        " Merged view undecided text " },
      { XxResources::COLOR_BACK_MERGED_DECIDED_1,
        " Merged view decided text, file 1 " },
      { XxResources::COLOR_BACK_MERGED_DECIDED_2,
        " Merged view decided text, file 2 " },
      { XxResources::COLOR_BACK_MERGED_DECIDED_3,
        " Merged view decided text, file 3 " },

   };
   const int nbExplanations = 28;
   for ( int ii = 0; ii < nbExplanations; ++ii ) {
      resources->getRegionColor( explanations[ii]._type, back, fore );
      l = createLabel( explanations[ii]._expl, fore, back, font );
      toplay->addWidget( l );
   }

   QPushButton* b1 = new QPushButton( "Close", this );
   toplay->addWidget( b1 );
   connect( b1, SIGNAL(clicked()), this, SLOT(accept()) );
}

//------------------------------------------------------------------------------
//
QLabel* XxColorLegend::createLabel( 
   const char* text, 
   QColor& fore,
   QColor& back,
   const QFont& font
) 
{
   QLabel* l = new QLabel( QString(text), this );
   QColorGroup g;
   g.setColor( QColorGroup::Foreground, fore );
   g.setColor( QColorGroup::Background, back );
   QPalette p( g, g, g );
   l->setPalette( p );
   l->setFont( font );
   l->adjustSize();
   l->setMinimumHeight( l->height() + 4 );
   return l;
}

//------------------------------------------------------------------------------
//
void XxColorLegend::accept()
{
   QDialog::accept();
   delete this;
}

/*==============================================================================
 * LOCAL CLASS XxManPageDialog
 *============================================================================*/

// <summary> the man pag dialog </summary>

class XxManPageDialog : public QDialog {

public:

   /*----- member functions -----*/

   // Constructor.
   XxManPageDialog( QWidget* parent, const QString& text );

   // See base class.
   virtual void accept();

   // Returns a newly allocated block of text containing the manual page text.
   // Ownership is passed to the caller (you must delete).
   static char* getManualText( const char* command );

};


//------------------------------------------------------------------------------
//
XxManPageDialog::XxManPageDialog( 
   QWidget* parent, 
   const QString& text
) :
   QDialog( parent )
{
   QVBoxLayout* toplay = new QVBoxLayout( this );
   QTextView* tv = new QTextView( text, QString::null, this, "name" );
   tv->setMinimumSize( 500, 700 );
   toplay->addWidget( tv );
   
   QPushButton* b1 = new QPushButton( "Close", this );
   b1->setDefault( true );
   toplay->addWidget( b1 );
   connect( b1, SIGNAL(clicked()), this, SLOT(accept()) );
}

//------------------------------------------------------------------------------
//
void XxManPageDialog::accept()
{
   QDialog::accept();
   delete this;
}

//------------------------------------------------------------------------------
//
char* XxManPageDialog::getManualText( const char* command )
{
#ifdef XX_USE_RUNTIME_HELP
   XX_ASSERT( command != 0 );

   // This code fetched from mgdiff.
   FILE* f;
   char* retval;
   int size, bytes;
   char buffer[BUFSIZ];
   
   if ( ( f = popen( command, "r" ) ) == 0 ) {
      return 0;
   }
   
   for ( size = 0, retval = 0; !feof (f); ) {
      if ( ( bytes = fread( (void*)buffer, 1, BUFSIZ, f ) ) != 0 ) {
         if ( size ) {
            retval = (char *) realloc ((void *) retval, size + bytes + 1);
         }
         else {
            retval = (char *) malloc (bytes + 1);
         }
         memcpy( (void*)&retval[size], (void*)buffer, bytes );
         retval[size + bytes] = '\0';
         size += bytes;
      }
   }
   
   pclose( f );
   return retval;
#else
   return manText;
#endif
}

}


XX_NAMESPACE_BEGIN
using namespace std;


/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxHelp
 *============================================================================*/

//------------------------------------------------------------------------------
//
QDialog* XxHelp::getAboutDialog( QWidget* parent )
{
   ostringstream oss;
   oss << "xxdiff" << endl 
       << endl
       << "A graphical file comparator and merge tool." << endl
       << endl
       << "Author: Martin Blais <blais@iro.umontreal.ca>" << endl
       << "Home page: http://xxdiff.sourceforge.net" << endl
       << "Version: " << XX_VERSION << endl << ends;
   QString text( oss.str().c_str() );
   QDialog* box = new XxAboutDialog( parent, text );
   return box;
}

//------------------------------------------------------------------------------
//
QDialog* XxHelp::getColorLegend( QWidget* parent )
{
   QDialog* box = new XxColorLegend( parent );
   return box;
}

//------------------------------------------------------------------------------
//
QDialog* XxHelp::getManPageDialog( QWidget* parent )
{
   const XxResources* resources = XxResources::getInstance();
   const char* command = resources->getCommand( XxResources::COMMAND_MANUAL );
   char* manualText = XxManPageDialog::getManualText( command );
   if ( manualText != 0 ) {
      QString text( manualText );
      QDialog* box = new XxManPageDialog( parent, text );
#ifdef XX_USE_RUNTIME_HELP
      free( manualText );
#endif
      return box;
   }
   return 0;
}

//------------------------------------------------------------------------------
//
const char* XxHelp::getWhatsThisText( WhatsThisTextType type )
{
   return whatsThisStrings[ type ];
}

//------------------------------------------------------------------------------
//
void XxHelp::dumpUsage( std::ostream& os )
{
   os << 
"Usage: \n\
   xxdiff [OPTION] <file1> <file2>\n\
	where <fileX> can be - for stdin\n\
\n\
Options: \n\
   -h, --help                  Show this help message.\n\
   -v, --version               Show the program version and compilation \n\
                               options.\n\
";
#ifdef XX_USE_XRM
   os << "\
       --no-xrm                Don't query X resources.\n\
";
#endif
#ifdef XX_USE_RCFILE
   os << "\
       --no-rcfile             Don't query rcfile resources (.xxdiffrc).\n\
";
#endif
   os << "\
       --list-resources        Lists all the supported resources and default \n\
                               values.\n\
       --title<X>=<string>     Display <string> instead of filename in \n\
                               filename label <X>, where <X> is 1, 2 or 3 \n\
                               for left, middle or right file.\n\
   -N, --titlein=<string>      Display <string> instead of filename given \n\
                               in stdin.\n\
   -D, --exit-on-same          If there are no differences then exit quietly\n\
                               with exit code of 0.\n\
   -i, --ignore-case           Passed on to diff(1). \n\
                               Consider upper- and lower-case to be the same.\n\
   -w, --ignore-all-space      Passed on to diff(1). \n\
                               Ignore all white space.\n\
   -b, --ignore-space-change   Passed on to diff(1).\n\
                               Ignore changes in the amount of white space.\n\
                               when comparing the two files.\n\
   -r, --recursive             Passed on to diff(1).\n\
                               This is only meaningful for directory diffs.\n\
       --resource=<string>     Pass on string to resource parser.\n\
                               Resources given in this manner on the command\n\
                               line supersede other resource mechanisms.\n\
                               One can specify multiple resource settings by\n\
                               repeating this option.\n\
\n\
";
}

//------------------------------------------------------------------------------
//
void XxHelp::dumpVersion( std::ostream& os )
{
   os << "xxdiff - version " << XX_VERSION << endl;
   os << "Compile options:";
#ifdef XX_DEBUG
   os << " debug";
#endif
#ifdef XX_LINKSTATIC
   os << " linkstatic";
#endif
#ifdef XX_USE_XRM
   os << " xrm";
#endif
#ifdef XX_USE_RCFILE
   os << " rcfile";
#endif
#ifdef XX_USE_NAMESPACE
   os << " namespace";
#endif
#ifdef XX_USE_RUNTIME_HELP
   os << " runtime_help";
#endif
   os << "." << endl;
}

//------------------------------------------------------------------------------
//
void XxHelp::dumpResources( std::ostream& os )
{
   int nbres = int( XxResources::RESOURCE_LAST - XxResources::RESOURCE_FIRST );
   for ( int ii = 0; ii < nbres; ++ii ) {
      const char* doc =
         XxResources::getResourceDoc( XxResources::Resource( ii ) );
      if ( doc != 0 ) {
         if ( strcmp( doc, "()" ) != 0 ) {
            os << doc << std::endl;
            os << XxResources::getResourceName( XxResources::Resource( ii ) );
            os << std::endl << std::endl;
         }
      }
   }   
}

XX_NAMESPACE_END

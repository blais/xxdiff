/******************************************************************************\
 * $Id: help.cpp 291 2001-10-20 22:15:00Z blais $
 * $Date: 2001-10-20 18:15:00 -0400 (Sat, 20 Oct 2001) $
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

#include <version.h>
#include <help.h>
#include <line.h>
#include <resources.h>

#include <man.h>

#include <qdialog.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpalette.h>
#include <qtextview.h>
#include <qscrollview.h>

#include <iostream>
#include <stdio.h>

// Pixmaps.
#include "pixmaps/xxdiff.xpm"

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

/*----- variables -----*/
const QString whatsThisStrings[] = {

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
{
   QPixmap pm_xxdiff_logo( const_cast<const char**>( xxdiff_xpm ) );
   setIconPixmap( pm_xxdiff_logo );
}

//------------------------------------------------------------------------------
//
void XxAboutDialog::done( int )
{
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
   static QString getManualText( const QString& command = QString::null );

};


//------------------------------------------------------------------------------
//
XxManPageDialog::XxManPageDialog( 
   QWidget*       parent, 
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
QString XxManPageDialog::getManualText( const QString& /*command*/ )
{
   return QString(manText);
}

}

XX_NAMESPACE_BEGIN

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
   QString text;
   QTextOStream oss( &text );
   oss << "xxdiff" << endl 
       << endl
       << "A graphical file comparator and merge tool." << endl
       << endl
       << "Author: Martin Blais <blais@iro.umontreal.ca>" << endl
       << "Home page: http://xxdiff.sourceforge.net" << endl
       << "Version: " << XX_VERSION << endl;
   QDialog* box = new XxAboutDialog( parent, text );
   return box;
}

//------------------------------------------------------------------------------
//
QDialog* XxHelp::getManPageDialog( QWidget* parent )
{
   QString manualText = XxManPageDialog::getManualText();
   if ( !manualText.isEmpty() ) {
      QDialog* box = new XxManPageDialog( parent, manualText );
      return box;
   }
   return 0;
}

//------------------------------------------------------------------------------
//
const QString& XxHelp::getWhatsThisText( WhatsThisTextType type )
{
   return whatsThisStrings[ type ];
}

//------------------------------------------------------------------------------
//
void XxHelp::dumpUsage( QTextStream& os )
{
   os << 
"Usage: \n\
   xxdiff [OPTION] <file1> <file2> [<file3>]\n\
	where <fileX> can be - for stdin.\n\
        Filenames can be either 2 directories, or 3 files.\n\
\n\
Options: \n\
   -h, --help                  Show this help message.\n\
   -v, --version               Show the program version and compilation \n\
                               options.\n\
";
   os << "\
       --no-rcfile             Don't query rcfile resources (.xxdiffrc).\n\
";
   os << "\
       --list-resources        Lists all the supported resources and default \n\
                               values.\n\
       --resource=<str>        Pass on string <str> to resource parser.\n\
                               Resources given in this manner on the command\n\
                               line supersede other resource mechanisms.\n\
                               One can specify multiple resource settings by\n\
                               repeating this option.\n\
       --title<X>=<str>        Display <str> instead of filename in \n\
                               filename label <X>, where <X> is 1, 2 or 3 \n\
                               for left, middle or right file.\n\
   -N<str>, --titlein=<str>    Display <str> instead of filename given \n\
                               in stdin.\n\
   -D, --exit-on-same          If there are no differences then exit quietly\n\
                               with exit code of 0.\n\
   -A<str>, --args=<str>       Pass on arguments in <str> to the subordinate\n\
                               diff program.\n\
\n\
   Options passed on diff program as GNU diff options:\n\
\n\
   -i, --ignore-case           Passed on to diff(1). \n\
                               Consider upper- and lower-case to be the same.\n\
   -w, --ignore-all-space      Passed on to diff(1). \n\
                               Ignore all white space.\n\
   -b, --ignore-space-change   Passed on to diff(1).\n\
                               Ignore changes in the amount of white space.\n\
                               when comparing the two files.\n\
   -a, --as-text               Passed on to diff(1).\n\
                               Treat all files as text and compare them\n\
                               line-by-line, even if they do not seem to be\n\
                               text.\n\
   -r, --recursive             Passed on to diff(1).\n\
                               This is only meaningful for directory diffs.\n\
\n\
";
}

//------------------------------------------------------------------------------
//
void XxHelp::dumpVersion( QTextStream& os )
{
   os << "xxdiff - version " << XX_VERSION << endl;
   os << "Compile options:";
#ifdef XX_DEBUG
   os << " debug";
#endif
#ifdef XX_LINKSTATIC
   os << " linkstatic";
#endif
   os << " rcfile";
   os << "." << endl;
}

XX_NAMESPACE_END


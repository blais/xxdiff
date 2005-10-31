/******************************************************************************\
 * $Id: help.cpp 347 2001-11-06 06:30:32Z blais $
 * $Date: 2001-11-06 01:30:32 -0500 (Tue, 06 Nov 2001) $
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
#include <cmdline.h>
#include <resParser.h>

namespace XX_NAMESPACE_PREFIX { namespace Manual {
#include <doc.h>
}}

#include <qdialog.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpalette.h>
#include <qtextbrowser.h>
#include <qscrollview.h>

#include <iostream>
#include <stdio.h>
#include <string.h>

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

//------------------------------------------------------------------------------
//
QString formatOptionsPlain(
   const XxCmdline::Option* options,
   const int                nbOptions,
   const unsigned int       width = 80
)
{
   QString outs;
   QTextOStream oss( &outs );

   // Compute maximum width of long-option.
   int maxw = 0;
   int ii;
   for ( ii = 0; ii < nbOptions; ++ii ) {
      int len = 2;
      if ( options[ii]._shortname != 0 ) {
         len += 4; // "-o, "
      }
      len += 2 + ::strlen( options[ii]._longname ); // "--opt"
      if ( options[ii]._has_arg ) {
         len += 6; // " <arg>"
      }
      if ( len > maxw ) {
         maxw = len;
      }
   }

   unsigned int startw = maxw + 2;
   XX_CHECK( startw < width );

   for ( ii = 0; ii < nbOptions; ++ii ) {
      // Output option name.
      int len = 2;
      oss << "  ";
      if ( options[ii]._shortname != 0 ) {
         oss << "-" << options[ii]._shortname << ", ";
         len += 4;
      }
      len += 2 + ::strlen( options[ii]._longname );
      oss << "--" << options[ii]._longname;
      if ( options[ii]._has_arg ) {
         oss << " <arg>";
         len += 6;
      }
      for ( unsigned int iii = len; iii < startw; ++iii ) { oss << " "; }

      // Output formatted help.
      {
         QString helpstr( options[ii]._help );
         QTextIStream iss( &helpstr );
         QString word;
         int cch = startw;
         while ( !iss.atEnd() ) {
            iss >> word;

            // prevent infinite loop
            XX_ASSERT( ( startw + word.length() + 1 ) < width ); 

            if ( ( cch + word.length() + 1 ) >= width ) {
               oss << endl;
               for ( unsigned int iii = 0; iii < startw; ++iii ) { oss << " "; }
               cch = startw;
            }
            oss << word << " ";
            cch += word.length() + 1;
         }
      }
      oss << endl << endl;
   }
   return outs;
}


//------------------------------------------------------------------------------
//
QString formatOptionsQml(
   const XxCmdline::Option* options,
   const int                nbOptions
)
{
   QString outs;
   QTextOStream oss( &outs );

   oss << "<table cellpadding=5>" << endl << endl;
   for ( int ii = 0; ii < nbOptions; ++ii ) {
      oss << "<tr width=20%>" << endl;

      // Output option name.
      oss << "<td><tt>";
      if ( options[ii]._shortname != 0 ) {
         oss << "-" << options[ii]._shortname << ", ";
      }
      oss << "--" << options[ii]._longname;
      if ( options[ii]._has_arg ) {
         oss << " &lt;arg&gt;";
      }
      oss << "</tt></td>" << endl;


      // Output formatted help.
      oss << "<td>" << endl;
      oss << options[ii]._help << endl;
      oss << "</td>" << endl;

      oss << "</tr>" << endl << endl;
   }
   oss << "</table>" << endl << endl;
   oss << flush;
   
   return outs;
}

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
   QTextBrowser* tv = new QTextBrowser( this, "name" );
   tv->setText( text, QString::null );
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
const QString& XxHelp::getWhatsThisText( WhatsThisTextType type )
{
   return whatsThisStrings[ type ];
}

//------------------------------------------------------------------------------
//
QString XxHelp::getVersion()
{
   return QString( XX_VERSION );
}

//------------------------------------------------------------------------------
//
QString XxHelp::getUsage( bool plain )
{
   int nbOptions;
   XxCmdline::Option* options = XxCmdline::getOptionList( nbOptions );

   QString usage;
   if ( plain ) {
      {
         QTextOStream oss( &usage );
         oss << "Usage:" << endl
             << "  xxdiff [OPTIONS] file1 file2 [file3]" << endl
             << endl
             << "  where any specified file can be `-' for stdin." << endl
             << "  Filenames can be either 2 directories, or 3 files." << endl
             << endl
             << "Options:" << endl
             << endl;
      }

      usage += formatOptionsPlain( options, nbOptions );
   }
   else {
      usage += formatOptionsQml( options, nbOptions );
   }
   return usage;
}

//------------------------------------------------------------------------------
//
QString XxHelp::getManual()
{
   QString srcManual( XX_NAMESPACE_PREFIX :: Manual :: text );
   QString manual;
   int idx = 0;

   // Fill in the invocation section.
   QString vertag( "<version>" );
   int idxver = srcManual.find( vertag, idx );
   if ( idxver != -1 ) {
      manual += srcManual.mid( idx, idxver );
      manual += getVersion();
      idx = idxver + vertag.length();
   }

   // Fill in the invocation section.
   QString invtag( "<invocation>" );
   int idxinv = srcManual.find( invtag, idx );
   if ( idxinv != -1 ) {
      manual += srcManual.mid( idx, idxinv - idx );
      manual += getUsage( false );
      idx = idxinv + invtag.length();
   }

   // Fill in the resource reference section.
   QString restag( "<resourceref>" );
   int idxres = srcManual.find( restag, idx );
   if ( idxres != -1 ) {
      manual += srcManual.mid( idx, idxres - idx );
      manual += XxResParser::getResourceRef();
      idx = idxres + restag.length();
   }

   // Add rest of documentation.
   manual += srcManual.mid( idx );

   return manual;
}

//------------------------------------------------------------------------------
//
void XxHelp::dumpVersion( QTextStream& os )
{
   os << "xxdiff --- version " << getVersion();
#ifdef XX_DEBUG
   os << " (debug)";
#endif
}

//------------------------------------------------------------------------------
//
void XxHelp::dumpUsage( QTextStream& os )
{
   os << getUsage( true );
}

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
       << "Version: " << getVersion() << endl;
   QDialog* box = new XxAboutDialog( parent, text );
   return box;
}

//------------------------------------------------------------------------------
//
QDialog* XxHelp::getManPageDialog( QWidget* parent )
{
   QString docstr;
   {
      QTextOStream oss( &docstr );
      oss << "<qt title=\"xxdiff documentation\">" << endl
          << getManual() << endl
          << "</qt" << endl;
   }
   QDialog* box = new XxManPageDialog( parent, docstr );
   return box;
}

//------------------------------------------------------------------------------
//
QString XxHelp::xmlize( const QString& in )
{
   const char* inc = in.latin1();
   QString out;
   for ( unsigned int ii = 0; ii < in.length(); ++ii ) {
      if ( inc[ii] == '<' ) {
         out.append( "&lt;" );
      }
      else if ( inc[ii] == '>' ) {
         out.append( "&gt;" );
      }
      else {
         out.append( inc[ii] );
      }
   }
   return out;
}

XX_NAMESPACE_END


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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <proginfo.h>
#include <help.h>
#include <line.h>
#include <resources.h>
#include <cmdline.h>
#include <resParser.h>

#include <kdeSupport.h>
#include <QPixmap>
#include <QTextStream>
#include <QVBoxLayout>

namespace XX_NAMESPACE_PREFIX { namespace Manual {
#if !defined(WINDOWS) && !defined(__CYGWIN__)
#include <doc.h>
#else
char text[]="<h1>xxdiff documentation</h1><p>Not available under Windows.</p>";
#endif
}}

#include <QDialog>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QPalette>
#include <QTextBrowser>
#include <QByteArray>
#include <QLineEdit>

#include <iostream>
#include <stdio.h>
#include <string.h>

// Pixmaps.
#include "pixmaps/xxdiff.xpm"

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace XxHelpNS {

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
   const unsigned int       mincol = 29,
   const unsigned int       width = 80
)
{
   QString outs;
   QTextStream oss( &outs );

   // Compute maximum width of long-option.
   unsigned int maxw = 0;
   int ii;
   for ( ii = 0; ii < nbOptions; ++ii ) {
      unsigned int len = 2;
      len += 2 + ::strlen( options[ii]._longname ); // "--opt"
      if ( options[ii]._shortname != 0 ) {
         len += 4; // ", -o"
      }
      if ( options[ii]._has_arg ) {
         len += 6; // " <arg>"
      }

      if ( len > maxw ) {
         maxw = len;
      }
   }

   unsigned int startw = ( mincol >= maxw + 2 ) ? mincol : maxw + 2;
   XX_CHECK( startw < width );

   for ( ii = 0; ii < nbOptions; ++ii ) {
      // Output option name.
      int len = 2;
      oss << "  ";
      oss << "--" << options[ii]._longname;
      len += 2 + ::strlen( options[ii]._longname );
      if ( options[ii]._shortname != 0 ) {
         oss << ", -" << options[ii]._shortname;
         len += 4;
      }
      if ( options[ii]._has_arg ) {
         oss << " <arg>";
         len += 6;
      }
      for ( unsigned int iii = len; iii < startw; ++iii ) { oss << " "; }

      // Output formatted help.
      {
         QString helpstr( options[ii]._help );
         QTextStream iss( &helpstr );
         QString word;
         unsigned int cch = startw;
         while ( !iss.atEnd() ) {
            iss >> word;

            // prevent infinite loop
            XX_ASSERT( ( startw + word.length() + 1 ) < width ); 

            if ( ( cch + word.length() + 1 ) >= width ) {
               oss << Qt::endl;
               for ( unsigned int iii = 0; iii < startw; ++iii ) { oss << " "; }
               cch = startw;
            }
            oss << word << " ";
            cch += word.length() + 1;
         }
      }
      oss << Qt::endl;

      // Add an extra line between the options, there are so much that it will
      // make it clearer.
      oss << Qt::endl;
   }
   oss << Qt::endl;
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
   QTextStream oss( &outs );

   oss << "<table cellpadding=5 width=\"100%\">" << Qt::endl << Qt::endl;
   for ( int ii = 0; ii < nbOptions; ++ii ) {
      oss << "<tr>" << Qt::endl;

      // Output option name.
      oss << "<td width=\"30%\"><tt>";
      oss << "--" << options[ii]._longname;
      if ( options[ii]._shortname != 0 ) {
         oss << ", -" << options[ii]._shortname;
      }
      if ( options[ii]._has_arg ) {
         oss << " &lt;arg&gt;";
      }
      oss << "</tt></td>" << Qt::endl;


      // Output formatted help.
      oss << "<td align=left>" << Qt::endl;
      oss << options[ii]._help << Qt::endl;
      oss << "</td>" << Qt::endl;

      oss << "</tr>" << Qt::endl << Qt::endl;
   }
   oss << "</table>" << Qt::endl << Qt::endl;
   oss << Qt::flush;
   
   return outs;
}

/*==============================================================================
 * LOCAL CLASS XxAboutDialog
 *============================================================================*/

// <summary> the about dialog </summary>

#ifdef XX_KDE
class XxAboutDialog : public KAboutApplication {
#else
class XxAboutDialog : public QMessageBox {
#endif

public:

   /*----- member functions -----*/

   // Constructor.
#ifdef XX_KDE
   XxAboutDialog( QWidget* parent, const KAboutData* aboutData );   
#else   
   XxAboutDialog( QWidget* parent, QString& text );
#endif

#ifdef XX_KDE
   // See base class.
   virtual void done( int r );
#endif

};

//------------------------------------------------------------------------------
//

#ifdef XX_KDE

XxAboutDialog::XxAboutDialog( QWidget* parent, const KAboutData* aboutData ) :
   KAboutApplication(aboutData, parent)
{
}

#else

XxAboutDialog::XxAboutDialog( QWidget* parent, QString& text ) :
   QMessageBox( 
      QMessageBox::Information, "About xxdiff.", text,
      QMessageBox::Close, parent
   )
{
   QPixmap pm_xxdiff_logo( const_cast<const char**>( xxdiff_xpm ) );
   setIconPixmap( pm_xxdiff_logo );
   setModal( 0 );
   setAttribute( Qt::WA_DeleteOnClose );
}

#endif

//------------------------------------------------------------------------------
//
#ifdef XX_KDE
void XxAboutDialog::done( int )
{
   delete this; // Will probably crash too in KDE, as it does in Qt4
}
#endif

/*==============================================================================
 * LOCAL CLASS XxManPageDialog
 *============================================================================*/



//------------------------------------------------------------------------------
//
XxManPageDialog::XxManPageDialog(
   QWidget*       parent, 
   const QString& text
) :
   QDialog( parent )
{
   setAttribute( Qt::WA_DeleteOnClose );
   QVBoxLayout* toplay = new QVBoxLayout( this );
   toplay->setContentsMargins( 0,0,0,0 );
   toplay->setSpacing( 0 );
   _textBrowser = new QkTextBrowser();
   _textBrowser->setText( text );
   _textBrowser->setMinimumSize( 500, 700 );
   toplay->addWidget( _textBrowser );
   
   QHBoxLayout* sbLayout = new QHBoxLayout;
   toplay->addLayout( sbLayout );
   sbLayout->setContentsMargins( 10,10,10,10 );
   sbLayout->setSpacing( 3 );
   _lineEdit = new QkLineEdit;
   sbLayout->addWidget( new QLabel( "Search String:") );
   sbLayout->addWidget( _lineEdit );
   _lineEdit->setFocus();
   connect( _lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(searchFirst(const QString&)) );
   connect( _lineEdit, SIGNAL(returnPressed()), this, SLOT(searchNext()) );
   
   QHBoxLayout* btLayout = new QHBoxLayout;
   toplay->addLayout( btLayout );
   QkPushButton* b1 = new QkPushButton( "Close", this );
   b1->setAutoDefault( false );

   btLayout->addItem( new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
   btLayout->addWidget( b1 );
   btLayout->addItem( new QSpacerItem( 10, 1, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
   connect( b1, SIGNAL(clicked()), this, SLOT(accept()) );
   toplay->addItem( new QSpacerItem( 1, 10, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
}

//------------------------------------------------------------------------------
//
void XxManPageDialog::searchFirst(const QString& text)
{
    QTextCursor c = _textBrowser->textCursor();
    c.movePosition( QTextCursor::Start);
    _textBrowser->setTextCursor( c );
    _textBrowser->find( text );
}

//------------------------------------------------------------------------------
//
void XxManPageDialog::searchNext()
{
    if ( ! _textBrowser->find( _lineEdit->text() ) ) {
        searchFirst( _lineEdit->text() );
    }
}

}

XX_NAMESPACE_BEGIN

using namespace XxHelpNS;

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
   return QString( xx_version );
}

//------------------------------------------------------------------------------
//
QString XxHelp::getUsage( int helpMask, bool plain )
{
   int nbOptions;
   XxCmdline::Option* options;

   QString usage;
   if ( plain ) {
      QTextStream oss( &usage, QIODevice::WriteOnly | QIODevice::Append );
      oss << "Usage: "
          << "xxdiff [OPTIONS] file1 file2 [file3]" << Qt::endl
          << Qt::endl
          << "A graphical file and directories comparison and merge tool."
          << Qt::endl << Qt::endl;

      if ( helpMask & (1 << XxCmdline::OPT_GENERIC) ) {
         oss << "Generic options:" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_GENERIC, nbOptions );
         oss << formatOptionsPlain( options, nbOptions );
      }

      if ( helpMask & (1 << XxCmdline::OPT_QT) ) {
         oss << "Qt options:" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_QT, nbOptions );
         oss << formatOptionsPlain( options, nbOptions );
      }

      if ( helpMask & (1 << XxCmdline::OPT_DIFF) ) {
         oss << "GNU diff options (2-files only):" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_DIFF, nbOptions );
         oss << formatOptionsPlain( options, nbOptions );
         oss << Qt::endl;
      }

      if ( helpMask & (1 << XxCmdline::OPT_XXDIFF) ) {
         oss << "Options:" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_XXDIFF, nbOptions );
         oss << formatOptionsPlain( options, nbOptions );
      }

      if ( helpMask & (1 << XxCmdline::OPT_DISPLAY) ) {
         oss << "Display Options:" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_DISPLAY, nbOptions );
         oss << formatOptionsPlain( options, nbOptions );
      }

      oss << "Any one specified file can be `-' for stdin." << Qt::endl
          << "Filenames can be either 2 directories, 2 files or 3 files."
          << Qt::endl;
   }
   else {
      QTextStream oss( &usage, QIODevice::WriteOnly | QIODevice::Append );
      if ( helpMask & (1 << XxCmdline::OPT_GENERIC) ) {
         oss << "<h4>Generic options</h4>" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_GENERIC, nbOptions );
         oss << formatOptionsQml( options, nbOptions );
      }

      if ( helpMask & (1 << XxCmdline::OPT_QT) ) {
         oss << "<h4>Qt options</h4>" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_QT, nbOptions );
         oss << formatOptionsQml( options, nbOptions );
      }

      if ( helpMask & (1 << XxCmdline::OPT_DIFF) ) {
         oss << "<h4>GNU diff options (2-files only)</h4>" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_DIFF, nbOptions );
         oss << formatOptionsQml( options, nbOptions );
         oss << Qt::endl;
      }

      if ( helpMask & (1 << XxCmdline::OPT_XXDIFF) ) {
         oss << "<h4>Options</h4>" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_XXDIFF, nbOptions );
         oss << formatOptionsQml( options, nbOptions );
      }

      if ( helpMask & (1 << XxCmdline::OPT_DISPLAY) ) {
         oss << "<h4>Display Options</h4>" << Qt::endl;
         options =
            XxCmdline::getOptionList( XxCmdline::OPT_DISPLAY, nbOptions );
         oss << formatOptionsQml( options, nbOptions );
      }

   }
   return usage;
}

//------------------------------------------------------------------------------
//
QString XxHelp::getManual()
{
   QString srcManual( XX_NAMESPACE_PREFIX Manual :: text );
   QString manual;
   int idx = 0;

   // Fill in the invocation section.
   QString vertag( "<version/>" );
   int idxver = srcManual.indexOf( vertag, idx );
   if ( idxver != -1 ) {
      manual += srcManual.mid( idx, idxver );
      manual += getVersion();
      idx = idxver + vertag.length();
   }
   else {
      std::cerr << "Warning: cannot find version tag in documentation text" 
                << std::endl;
   }

   // Fill in the invocation section.
   QString invtag( "<invocation/>" );
   int idxinv = srcManual.indexOf( invtag, idx );
   if ( idxinv != -1 ) {
      manual += srcManual.mid( idx, idxinv - idx );
      manual += getUsage( XxCmdline::OPT_ALL, false );
      idx = idxinv + invtag.length();
   }
   else {
      std::cerr << "Warning: cannot find invocation tag in documentation text" 
                << std::endl;
   }

   // Fill in the resource reference section.
   QString restag( "<resourceref/>" );
   int idxres = srcManual.indexOf( restag, idx );
   if ( idxres != -1 ) {
      manual += srcManual.mid( idx, idxres - idx );
      manual += XxResParser::getResourceRef();
      idx = idxres + restag.length();
   }
   else {
      std::cerr << "Warning: cannot find resourceref tag in documentation text" 
                << std::endl;
   }

   // Add rest of documentation.
   manual += srcManual.mid( idx );

   return manual;
}

//------------------------------------------------------------------------------
//
QDialog* XxHelp::getAboutDialog( QWidget* parent )
{
#ifdef XX_KDE
   
   KAboutData* aboutData = new KAboutData(
      xx_name,
      xx_name,
      xx_version,
      xx_description,
      KAboutData::License_GPL_V2,
      xx_copyright,
      NULL,
      xx_homepage,
      xx_bugs_email);

   aboutData->addAuthor(
      "Martin Blais",
      "Original author, core developer, and maintainer",
      "blais@furius.ca",
      "http://furius.ca");
   aboutData->addAuthor("Trevor Harmon", "KDE port", "trevor@vocaro.com");
   
   QDialog* box = new XxAboutDialog( parent, aboutData );

   delete aboutData;
   
#else
   
   QString text;
   QTextStream oss( &text );
   oss << xx_name << Qt::endl 
       << Qt::endl
       << xx_description << Qt::endl
       << Qt::endl
       << "Author: Martin Blais <blais@furius.ca>" << Qt::endl
       << xx_homepage << Qt::endl
       << "Version: " << getVersion() << Qt::endl;

   QDialog* box = new XxAboutDialog( parent, text );

#endif
   
   return box;
}

//------------------------------------------------------------------------------
//
QDialog* XxHelp::getManPageDialog( QWidget* parent )
{
   QString docstr;
   {
      QTextStream oss( &docstr );
      oss << "<qt title=\"xxdiff documentation\">" << Qt::endl
          << getManual() << Qt::endl
          << "</qt" << Qt::endl;
   }
   QDialog* box = new XxManPageDialog( parent, docstr );
   return box;
}

//------------------------------------------------------------------------------
//
QString XxHelp::xmlize( const QString& in )
{
   QByteArray inBa =  in.toLocal8Bit();
   const char* inc = inBa.constData();
   QString out;
   for ( int ii = 0; ii < in.length(); ++ii ) {
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

//------------------------------------------------------------------------------
//
QString XxHelp::getInvocationTxt()
{
   // Return invocation section as plain text.
   return getUsage( XxCmdline::OPT_ALL, true );
}

XX_NAMESPACE_END


/******************************************************************************\
 * $Id: dialogs.cpp 2 2000-09-15 02:19:22Z blais $
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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#include <app.h>
#include <dialogs.h>
#include <resources.h>
#include <diffs.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qpushbutton.h>

#include <sstream>


XX_NAMESPACE_BEGIN
using namespace std;

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxSearchDialog
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxSearchDialog::XxSearchDialog( 
   XxApp*   app,
   QWidget* parent
) :
   QDialog( parent ),
   _app( app )
{
   XX_CHECK( _app != 0 );
   QVBoxLayout* toplay = new QVBoxLayout( this );
   toplay->setMargin( 10 );

   QLabel* l = new QLabel( "Search pattern:", this );
   toplay->addWidget( l );
   _e = new QLineEdit( this );
   toplay->addWidget( _e );
   QHBoxLayout* butlay = new QHBoxLayout( toplay );
   QPushButton* b1 = new QPushButton( "Search", this );
   butlay->addWidget( b1 );
   QPushButton* b2 = new QPushButton( "Close", this );
   butlay->addWidget( b2 );

   connect( _e, SIGNAL(returnPressed()), this, SLOT(onSearch()) );
   connect( b1, SIGNAL(clicked()), this, SLOT(onSearch()) );
   connect( b2, SIGNAL(clicked()), this, SLOT(onCancel()) );
}

//------------------------------------------------------------------------------
//
void XxSearchDialog::onSearch()
{
   XxDiffs* diffs = _app->getDiffs();
   const char* searchText = _e->text().ascii();
   if ( diffs != 0 && searchText != 0 ) {
      diffs->search( searchText, _app->getNbFiles(), _app->getFiles() );
   }
}

//------------------------------------------------------------------------------
//
void XxSearchDialog::onCancel()
{
   hide();
}

/*==============================================================================
 * CLASS XxTabWidthDialog
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxTabWidthDialog::XxTabWidthDialog( 
   XxApp*   app,
   QWidget* parent
) :
   QDialog( parent ),
   _app( app )
{
   XX_CHECK( _app != 0 );

   QVBoxLayout* toplay = new QVBoxLayout( this );
   toplay->setMargin( 10 );
   
   QLabel* l = new QLabel( "Tab width:", this );
   toplay->addWidget( l );
   _e = new QLineEdit( this );
   QIntValidator* validator = new QIntValidator( this );
   validator->setRange( 0, 100 );
   _e->setValidator( validator );
   toplay->addWidget( _e );

   QHBoxLayout* butlay = new QHBoxLayout( toplay );
   QPushButton* b1 = new QPushButton( "Set", this );
   butlay->addWidget( b1 );
   QPushButton* b2 = new QPushButton( "Close", this );
   butlay->addWidget( b2 );

   connect( _e, SIGNAL(returnPressed()), this, SLOT(onSet()) );
   connect( b1, SIGNAL(clicked()), this, SLOT(onSet()) );
   connect( b2, SIGNAL(clicked()), this, SLOT(onClose()) );
}

//------------------------------------------------------------------------------
//
void XxTabWidthDialog::onSet()
{
   int value = atoi( _e->text() );
   value = std::max( 0, std::min( 100, value ) );
   _app->setTabWidth( value );
}

//------------------------------------------------------------------------------
//
void XxTabWidthDialog::onClose()
{
   hide();
}

//------------------------------------------------------------------------------
//
void XxTabWidthDialog::show()
{
   const XxResources* resources = XxResources::getInstance();
   XX_CHECK( resources != 0 );
   std::ostringstream oss;
   oss << resources->getTabWidth() << std::ends;
   _e->setText( oss.str().c_str() );
   QDialog::show();
}

XX_NAMESPACE_END

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

#include <searchDialog.h>
#include <resources.h>
#include <diffs.h>
#include <app.h>

#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>

XX_NAMESPACE_BEGIN

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
   BaseClass( parent ),
   _app( app )
{
   // Make connections.
   connect( _buttonApply, SIGNAL( clicked() ),
            this, SLOT( onApply() ) );

   connect( _buttonNext, SIGNAL( clicked() ),
            _app, SLOT( searchForward() ) );
   connect( _buttonPrevious, SIGNAL( clicked() ),
            _app, SLOT( searchBackward() ) );

   connect( _buttonGotoLine, SIGNAL( clicked() ),
            this, SLOT( onGotoLine() ) );


   if ( _app->getNbFiles() == 2 ) {
      _comboGotoWhichFile->removeItem( 2 );
   }
}

//------------------------------------------------------------------------------
//
void XxSearchDialog::onApply()
{
   XxDiffs* diffs = _app->getDiffs();
   const QString searchText = _lineeditSearchString->text();
   if ( diffs != 0 && !searchText.isEmpty() ) {
      diffs->search( searchText, _app->getNbFiles(), _app->getBuffers() );
   }
}

//------------------------------------------------------------------------------
//
void XxSearchDialog::onGotoLine()
{
   bool ok;
   int gline = _lineeditGotoLine->text().toInt( &ok );
   
   static int ind[3] = { 0, 2, 1 };
   XX_ASSERT( _comboGotoWhichFile->currentItem() < 3 );
   XxFno fno = ind[ _comboGotoWhichFile->currentItem() ];
   if ( fno == 2 && _app->getNbFiles() == 2 ) {
      fno = 1;
   }

   XxDiffs* diffs = _app->getDiffs();
   XxBuffer* buffer = _app->getBuffer( fno );
   XxDln dline = diffs->getDisplayLine( gline, *buffer, fno );
   if ( dline != -1 ) {
      _app->setCursorLine( dline );
   }
}

//------------------------------------------------------------------------------
//
void XxSearchDialog::show()
{
   _lineeditSearchString->setFocus();
   BaseClass::show();
}

XX_NAMESPACE_END


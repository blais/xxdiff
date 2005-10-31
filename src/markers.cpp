/******************************************************************************\
 * $Id: markers.cpp 48 2000-10-03 04:43:36Z  $
 * $Date: 2000-10-03 00:43:36 -0400 (Tue, 03 Oct 2000) $
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

#include <markersFileDialog.h>

#include <qfiledialog.h>
#include <qurloperator.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qdir.h>

#include <qradiobutton.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qlabel.h>

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxMarkersWidget
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxMarkersWidget::XxMarkersWidget( 
   QWidget* parent,
   bool     threeWay
) :
   BaseClass( parent )
{
   // Make connections.
   connect( _checkboxConflicts, SIGNAL( clicked() ),
            this, SLOT( onConflicts() ) );
   connect( _checkboxConditionals, SIGNAL( clicked() ),
            this, SLOT( onConditionals() ) );

   if ( ! threeWay ) {
      _lineeditConditional2->hide();
      _labelConditional2->hide();
   }

   _buttongroup->setButton( _buttongroup->id( _checkboxConflicts ) );
   onConflicts();
}

//------------------------------------------------------------------------------
//
void XxMarkersWidget::onConflicts()
{
   _lineeditConditional1->setEnabled( false );
   _labelConditional1->setEnabled( false );
   _lineeditConditional2->setEnabled( false );
   _labelConditional2->setEnabled( false );
}

//------------------------------------------------------------------------------
//
void XxMarkersWidget::onConditionals()
{
   _lineeditConditional1->setEnabled( true );
   _labelConditional1->setEnabled( true );
   _lineeditConditional2->setEnabled( true );
   _labelConditional2->setEnabled( true );
}

//------------------------------------------------------------------------------
//
bool XxMarkersWidget::isUseConditionals() const
{
   return _checkboxConditionals->isChecked();
}

//------------------------------------------------------------------------------
//
QString XxMarkersWidget::getConditionalVariable1() const
{
   return _lineeditConditional1->text();
}

//------------------------------------------------------------------------------
//
QString XxMarkersWidget::getConditionalVariable2() const
{
   return _lineeditConditional2->text();
}


/*==============================================================================
 * CLASS XxMarkersFileDialog
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxMarkersFileDialog::XxMarkersFileDialog( 
   const QString& dirName,
   const QString& filter,
   QWidget*       parent,
   const char*    name,
   bool           modal,
   bool           threeWay
) :
   BaseClass( dirName, filter, parent, name, modal )
{
   _markersWidget = new XxMarkersWidget( this, threeWay );
   addWidgets( 0, _markersWidget, 0 );
}

//------------------------------------------------------------------------------
//
QString XxMarkersFileDialog::getSaveFileName( 
   const QString& startWith,
   const QString& filter,
   QWidget*       parent,
   bool           threeWay,
   bool&          useConditionals,
   std::string&   conditionalVariable1,
   std::string&   conditionalVariable2
)
{
   // From Qt's source code:
   QString initialSelection;
   static QString workingDirectory;
   if ( !startWith.isEmpty() ) {
      QUrlOperator u( startWith );
      if ( u.isLocalFile() && QFileInfo( u.path() ).isDir() ) {
         //workingDirectory = startWith;
      }
      else {
         //workingDirectory = u.toString();
         initialSelection = QString::null;//u.fileName();
      }
   }
   //if ( workingDirectory.isNull() ) {
   //   workingDirectory = QDir::currentDirPath();
   //}


   XxMarkersFileDialog* dlg = new XxMarkersFileDialog(
      startWith, filter, parent, QString::null, TRUE, threeWay
   );
   CHECK_PTR( dlg );
   dlg->setCaption( QFileDialog::tr( "Save as" ) );

   QString result;
   dlg->setMode( QFileDialog::AnyFile );

   if ( !initialSelection.isEmpty() ) {
      dlg->setSelection( initialSelection );
   }

   if ( dlg->exec() == QDialog::Accepted ) {
      result = dlg->selectedFile();
      //workingDirectory = dlg->d->url;
   }

   //XxMarkersWidget* markers = dlg->getMarkersWidget();
   useConditionals = dlg->_markersWidget->isUseConditionals();
   if ( useConditionals == true ) {
      conditionalVariable1 = dlg->_markersWidget->getConditionalVariable1();
      conditionalVariable2 = dlg->_markersWidget->getConditionalVariable2();
   }

   delete dlg;

   return result;
}

XX_NAMESPACE_END

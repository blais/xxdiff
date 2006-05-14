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

#include <markers.h>

#include <qfiledialog.h>
#include <qurloperator.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qdir.h>

#include <qradiobutton.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qcheckbox.h>

#include <qlayout.h>
#include <qpushbutton.h>

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
      _lineeditConditional3->hide();
      _labelConditional3->hide();
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
   _lineeditConditional3->setEnabled( false );
   _labelConditional3->setEnabled( false );
   _removeEmptyConditionals->setEnabled( false );
}

//------------------------------------------------------------------------------
//
void XxMarkersWidget::onConditionals()
{
   _lineeditConditional1->setEnabled( true );
   _labelConditional1->setEnabled( true );
   _lineeditConditional2->setEnabled( true );
   _labelConditional2->setEnabled( true );
   _lineeditConditional3->setEnabled( true );
   _labelConditional3->setEnabled( true );
   _removeEmptyConditionals->setEnabled( true );
}

//------------------------------------------------------------------------------
//
bool XxMarkersWidget::useConditionals() const
{
   return _checkboxConditionals->isChecked();
}

//------------------------------------------------------------------------------
//
bool XxMarkersWidget::removeEmptyConditionals() const
{
   return _removeEmptyConditionals->isChecked();
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

//------------------------------------------------------------------------------
//
QString XxMarkersWidget::getConditionalVariable3() const
{
   return _lineeditConditional3->text();
}

/*==============================================================================
 * CLASS XxMarkersDialog
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxMarkersDialog::XxMarkersDialog(
   QWidget*    parent,
   const char* name,
   const bool  threeWay
) :
   BaseClass( parent, name, true )
{
   QVBoxLayout* vlayout;
   QHBoxLayout* hlayout;

   //resize( 627, 262 ); 
   //setCaption( trUtf8( "Form1" ) );
   vlayout = new QVBoxLayout( this, 11, 6, "vlayout"); 

   _markersWidget = new XxMarkersWidget( this, threeWay );
   vlayout->addWidget( _markersWidget );

   hlayout = new QHBoxLayout( 0, 0, 6, "hlayout");
   QSpacerItem* spacer =
      new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
   hlayout->addItem( spacer );

   _buttonOk = new QPushButton( this, "_buttonOk" );
#if (QT_VERSION >= 0x030000)
   _buttonOk->setText( trUtf8( "Ok" ) );
#else
   _buttonOk->setText( "Ok" );
#endif
   _buttonOk->setDefault( TRUE );
   hlayout->addWidget( _buttonOk );
   QSpacerItem* spacer_2 =
      new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
   hlayout->addItem( spacer_2 );

   _buttonCancel = new QPushButton( this, "_buttonCancel" );
#if (QT_VERSION >= 0x030000)
   _buttonCancel->setText( trUtf8( "Cancel" ) );
#else
   _buttonCancel->setText( "Cancel" );
#endif
   hlayout->addWidget( _buttonCancel );
   QSpacerItem* spacer_3 =
      new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
   hlayout->addItem( spacer_3 );
   vlayout->addLayout( hlayout );

   QObject::connect( _buttonOk, SIGNAL(clicked()), this, SLOT(accept()) );
   QObject::connect( _buttonCancel, SIGNAL(clicked()), this, SLOT(reject()) );
}

//------------------------------------------------------------------------------
//
void XxMarkersDialog::disableCancel()
{
   _buttonCancel->setEnabled( false );
}

//------------------------------------------------------------------------------
//
void XxMarkersDialog::reject()
{
   if ( _buttonCancel->isEnabled() ) {
      BaseClass::reject();
   }
   // else noop.
}

//------------------------------------------------------------------------------
//
bool XxMarkersDialog::getMarkers(
   QWidget*    parent,
   const char* name,
   bool        threeWay,
   bool&       useConditionals,
   bool&       removeEmptyConditionals,
   QString     conditionals[3],
   bool        noCancel
)
{
   bool result = false;

   XxMarkersDialog* dlg = new XxMarkersDialog( parent, name, threeWay );
   if ( noCancel ) {
      dlg->disableCancel();
   }

   if ( dlg->exec() == QDialog::Accepted || noCancel ) {
      result = true;
   }

   useConditionals = dlg->_markersWidget->useConditionals();
   if ( useConditionals == true ) {
      conditionals[0] = dlg->_markersWidget->getConditionalVariable1();
      conditionals[1] = dlg->_markersWidget->getConditionalVariable2();
      conditionals[2] = dlg->_markersWidget->getConditionalVariable3();
   }
      
   removeEmptyConditionals = dlg->_markersWidget->removeEmptyConditionals();

   delete dlg;

   return result;
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
   const char*    name,
   bool           threeWay,
   bool&          useConditionals,
   bool&          removeEmptyConditionals,
   QString        conditionals[3]
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
      startWith, filter, parent, name, TRUE, threeWay
   );
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
   useConditionals = dlg->_markersWidget->useConditionals();
   if ( useConditionals == true ) {
      conditionals[0] = dlg->_markersWidget->getConditionalVariable1();
      conditionals[1] = dlg->_markersWidget->getConditionalVariable2();
      conditionals[2] = dlg->_markersWidget->getConditionalVariable3();
   }

   removeEmptyConditionals = dlg->_markersWidget->removeEmptyConditionals();

   delete dlg;

   return result;
}

XX_NAMESPACE_END

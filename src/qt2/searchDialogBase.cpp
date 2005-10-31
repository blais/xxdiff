/****************************************************************************
** Form implementation generated from reading ui file 'searchDialogBase.ui'
**
** Created: Fri Jan 9 17:31:35 2004
**      by: The User Interface Compiler ($Id: searchDialogBase.cpp 713 2004-01-12 06:16:16Z blais $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "searchDialogBase.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a XxSearchDialogBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
XxSearchDialogBase::XxSearchDialogBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "XxSearchDialogBase" );
    setSizeGripEnabled( TRUE );
    XxSearchDialogBaseLayout = new QVBoxLayout( this, 11, 6, "XxSearchDialogBaseLayout"); 

    Layout3 = new QHBoxLayout( 0, 0, 6, "Layout3"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    Layout3->addWidget( TextLabel1 );

    _lineeditSearchString = new QLineEdit( this, "_lineeditSearchString" );
    Layout3->addWidget( _lineeditSearchString );
    XxSearchDialogBaseLayout->addLayout( Layout3 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QHBoxLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    _checkboxCaseSensitive = new QCheckBox( GroupBox1, "_checkboxCaseSensitive" );
    _checkboxCaseSensitive->setEnabled( FALSE );
    GroupBox1Layout->addWidget( _checkboxCaseSensitive );

    _checkboxUseRegexp = new QCheckBox( GroupBox1, "_checkboxUseRegexp" );
    _checkboxUseRegexp->setEnabled( FALSE );
    GroupBox1Layout->addWidget( _checkboxUseRegexp );
    XxSearchDialogBaseLayout->addWidget( GroupBox1 );

    Layout5 = new QHBoxLayout( 0, 0, 6, "Layout5"); 

    _buttonApply = new QPushButton( this, "_buttonApply" );
    _buttonApply->setAutoDefault( TRUE );
    _buttonApply->setDefault( TRUE );
    Layout5->addWidget( _buttonApply );

    _buttonPrevious = new QPushButton( this, "_buttonPrevious" );
    Layout5->addWidget( _buttonPrevious );

    _buttonNext = new QPushButton( this, "_buttonNext" );
    Layout5->addWidget( _buttonNext );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout5->addItem( spacer );

    _buttonClose = new QPushButton( this, "_buttonClose" );
    _buttonClose->setAutoDefault( TRUE );
    Layout5->addWidget( _buttonClose );
    XxSearchDialogBaseLayout->addLayout( Layout5 );

    Layout5_2 = new QHBoxLayout( 0, 0, 6, "Layout5_2"); 

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    Layout5_2->addWidget( TextLabel1_2 );

    _lineeditGotoLine = new QLineEdit( this, "_lineeditGotoLine" );
    Layout5_2->addWidget( _lineeditGotoLine );

    _comboGotoWhichFile = new QComboBox( FALSE, this, "_comboGotoWhichFile" );
    Layout5_2->addWidget( _comboGotoWhichFile );

    _buttonGotoLine = new QPushButton( this, "_buttonGotoLine" );
    Layout5_2->addWidget( _buttonGotoLine );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout5_2->addItem( spacer_2 );
    XxSearchDialogBaseLayout->addLayout( Layout5_2 );
    languageChange();
    resize( QSize(619, 232).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( _buttonClose, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
XxSearchDialogBase::~XxSearchDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void XxSearchDialogBase::languageChange()
{
    setCaption( tr( "Search" ) );
    TextLabel1->setText( tr( "Search string:" ) );
    GroupBox1->setTitle( tr( "Options" ) );
    _checkboxCaseSensitive->setText( tr( "Case sensitive" ) );
    _checkboxUseRegexp->setText( tr( "Regexp search" ) );
    _buttonApply->setText( tr( "&Apply" ) );
    _buttonPrevious->setText( tr( "<< Previous" ) );
    _buttonNext->setText( tr( "Next >>" ) );
    _buttonClose->setText( tr( "&Close" ) );
    TextLabel1_2->setText( tr( "Goto line:" ) );
    _lineeditGotoLine->setText( tr( "1" ) );
    _comboGotoWhichFile->clear();
    _comboGotoWhichFile->insertItem( tr( "left" ) );
    _comboGotoWhichFile->insertItem( tr( "right" ) );
    _comboGotoWhichFile->insertItem( tr( "middle" ) );
    _buttonGotoLine->setText( tr( ">> Goto line" ) );
}


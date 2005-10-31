/****************************************************************************
** Form implementation generated from reading ui file 'markersWidgetBase.ui'
**
** Created: Fri Jan 9 17:31:24 2004
**      by: The User Interface Compiler ($Id: markersWidgetBase.cpp 713 2004-01-12 06:16:16Z blais $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "markersWidgetBase.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a XxMarkersWidgetBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
XxMarkersWidgetBase::XxMarkersWidgetBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "XxMarkersWidgetBase" );
    XxMarkersWidgetBaseLayout = new QVBoxLayout( this, 11, 6, "XxMarkersWidgetBaseLayout"); 

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    QFont TextLabel1_2_font(  TextLabel1_2->font() );
    TextLabel1_2_font.setFamily( "helvetica" );
    TextLabel1_2_font.setPointSize( 14 );
    TextLabel1_2->setFont( TextLabel1_2_font ); 
    TextLabel1_2->setAlignment( int( QLabel::AlignCenter ) );
    XxMarkersWidgetBaseLayout->addWidget( TextLabel1_2 );

    _buttongroup = new QButtonGroup( this, "_buttongroup" );
    _buttongroup->setColumnLayout(0, Qt::Vertical );
    _buttongroup->layout()->setSpacing( 6 );
    _buttongroup->layout()->setMargin( 11 );
    _buttongroupLayout = new QVBoxLayout( _buttongroup->layout() );
    _buttongroupLayout->setAlignment( Qt::AlignTop );

    _checkboxConflicts = new QRadioButton( _buttongroup, "_checkboxConflicts" );
    _buttongroupLayout->addWidget( _checkboxConflicts );

    Layout3 = new QHBoxLayout( 0, 0, 6, "Layout3"); 

    _checkboxConditionals = new QRadioButton( _buttongroup, "_checkboxConditionals" );
    Layout3->addWidget( _checkboxConditionals );

    _removeEmptyConditionals = new QCheckBox( _buttongroup, "_removeEmptyConditionals" );
    Layout3->addWidget( _removeEmptyConditionals );
    _buttongroupLayout->addLayout( Layout3 );

    Layout13 = new QHBoxLayout( 0, 0, 6, "Layout13"); 

    _labelConditional1 = new QLabel( _buttongroup, "_labelConditional1" );
    Layout13->addWidget( _labelConditional1 );

    _lineeditConditional1 = new QLineEdit( _buttongroup, "_lineeditConditional1" );
    Layout13->addWidget( _lineeditConditional1 );
    _buttongroupLayout->addLayout( Layout13 );

    Layout13_2 = new QHBoxLayout( 0, 0, 6, "Layout13_2"); 

    _labelConditional2 = new QLabel( _buttongroup, "_labelConditional2" );
    Layout13_2->addWidget( _labelConditional2 );

    _lineeditConditional2 = new QLineEdit( _buttongroup, "_lineeditConditional2" );
    Layout13_2->addWidget( _lineeditConditional2 );
    _buttongroupLayout->addLayout( Layout13_2 );

    Layout13_2_2 = new QHBoxLayout( 0, 0, 6, "Layout13_2_2"); 

    _labelConditional3 = new QLabel( _buttongroup, "_labelConditional3" );
    Layout13_2_2->addWidget( _labelConditional3 );

    _lineeditConditional3 = new QLineEdit( _buttongroup, "_lineeditConditional3" );
    Layout13_2_2->addWidget( _lineeditConditional3 );
    _buttongroupLayout->addLayout( Layout13_2_2 );
    XxMarkersWidgetBaseLayout->addWidget( _buttongroup );
    languageChange();
    resize( QSize(537, 253).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
XxMarkersWidgetBase::~XxMarkersWidgetBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void XxMarkersWidgetBase::languageChange()
{
    setCaption( tr( "Form3" ) );
    TextLabel1_2->setText( tr( "Some regions were found that are still UNSELECTED." ) );
    _buttongroup->setTitle( tr( "Unselected lines" ) );
    _checkboxConflicts->setText( tr( "Save as merge conflicts" ) );
    _checkboxConditionals->setText( tr( "Save with conditionals" ) );
    _removeEmptyConditionals->setText( tr( "Remove empty conditionals" ) );
    _labelConditional1->setText( tr( "Conditional variable:" ) );
    _lineeditConditional1->setText( tr( "FILE1" ) );
    _labelConditional2->setText( tr( "Conditional variable:" ) );
    _lineeditConditional2->setText( tr( "FILE2" ) );
    _labelConditional3->setText( tr( "Conditional variable:" ) );
    _lineeditConditional3->setText( tr( "FILE3" ) );
}


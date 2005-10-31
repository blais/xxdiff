/****************************************************************************
** Form implementation generated from reading ui file 'optionsDialogBase.ui'
**
** Created: Fri Jan 9 17:31:30 2004
**      by: The User Interface Compiler ($Id: optionsDialogBase.cpp 713 2004-01-12 06:16:16Z blais $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "optionsDialogBase.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qtextview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a XxOptionsDialogBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
XxOptionsDialogBase::XxOptionsDialogBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "XxOptionsDialogBase" );
    setSizeGripEnabled( TRUE );
    XxOptionsDialogBaseLayout = new QVBoxLayout( this, 11, 6, "XxOptionsDialogBaseLayout"); 

    _tabWidget = new QTabWidget( this, "_tabWidget" );

    Widget2 = new QWidget( _tabWidget, "Widget2" );
    Widget2Layout = new QVBoxLayout( Widget2, 11, 6, "Widget2Layout"); 

    GroupBox10 = new QGroupBox( Widget2, "GroupBox10" );
    GroupBox10->setColumnLayout(0, Qt::Vertical );
    GroupBox10->layout()->setSpacing( 6 );
    GroupBox10->layout()->setMargin( 11 );
    GroupBox10Layout = new QVBoxLayout( GroupBox10->layout() );
    GroupBox10Layout->setAlignment( Qt::AlignTop );

    Layout16 = new QHBoxLayout( 0, 0, 6, "Layout16"); 

    TestLabel1 = new QLabel( GroupBox10, "TestLabel1" );
    Layout16->addWidget( TestLabel1 );

    _lineeditCommandFiles2 = new QLineEdit( GroupBox10, "_lineeditCommandFiles2" );
    Layout16->addWidget( _lineeditCommandFiles2 );
    GroupBox10Layout->addLayout( Layout16 );

    _groupBoxGnuDiffOptions = new QGroupBox( GroupBox10, "_groupBoxGnuDiffOptions" );
    _groupBoxGnuDiffOptions->setColumnLayout(0, Qt::Vertical );
    _groupBoxGnuDiffOptions->layout()->setSpacing( 6 );
    _groupBoxGnuDiffOptions->layout()->setMargin( 11 );
    _groupBoxGnuDiffOptionsLayout = new QHBoxLayout( _groupBoxGnuDiffOptions->layout() );
    _groupBoxGnuDiffOptionsLayout->setAlignment( Qt::AlignTop );

    Layout20 = new QVBoxLayout( 0, 0, 6, "Layout20"); 

    _checkboxIgnoreTrailingBlanks = new QCheckBox( _groupBoxGnuDiffOptions, "_checkboxIgnoreTrailingBlanks" );
    Layout20->addWidget( _checkboxIgnoreTrailingBlanks );

    _checkboxIgnoreWhitespace = new QCheckBox( _groupBoxGnuDiffOptions, "_checkboxIgnoreWhitespace" );
    Layout20->addWidget( _checkboxIgnoreWhitespace );

    _checkboxIgnoreCase = new QCheckBox( _groupBoxGnuDiffOptions, "_checkboxIgnoreCase" );
    Layout20->addWidget( _checkboxIgnoreCase );

    _checkboxIgnoreBlankLines = new QCheckBox( _groupBoxGnuDiffOptions, "_checkboxIgnoreBlankLines" );
    _checkboxIgnoreBlankLines->setEnabled( TRUE );
    Layout20->addWidget( _checkboxIgnoreBlankLines );
    _groupBoxGnuDiffOptionsLayout->addLayout( Layout20 );

    ButtonGroup8 = new QButtonGroup( _groupBoxGnuDiffOptions, "ButtonGroup8" );
    ButtonGroup8->setColumnLayout(0, Qt::Vertical );
    ButtonGroup8->layout()->setSpacing( 6 );
    ButtonGroup8->layout()->setMargin( 11 );
    ButtonGroup8Layout = new QVBoxLayout( ButtonGroup8->layout() );
    ButtonGroup8Layout->setAlignment( Qt::AlignTop );

    _radiobuttonQualityNormal = new QRadioButton( ButtonGroup8, "_radiobuttonQualityNormal" );
    ButtonGroup8Layout->addWidget( _radiobuttonQualityNormal );

    _radiobuttonQualityFastest = new QRadioButton( ButtonGroup8, "_radiobuttonQualityFastest" );
    ButtonGroup8Layout->addWidget( _radiobuttonQualityFastest );

    _radiobuttonQualityHighest = new QRadioButton( ButtonGroup8, "_radiobuttonQualityHighest" );
    ButtonGroup8Layout->addWidget( _radiobuttonQualityHighest );
    _groupBoxGnuDiffOptionsLayout->addWidget( ButtonGroup8 );
    GroupBox10Layout->addWidget( _groupBoxGnuDiffOptions );
    Widget2Layout->addWidget( GroupBox10 );

    GroupBox11 = new QGroupBox( Widget2, "GroupBox11" );
    GroupBox11->setColumnLayout(0, Qt::Vertical );
    GroupBox11->layout()->setSpacing( 6 );
    GroupBox11->layout()->setMargin( 11 );
    GroupBox11Layout = new QHBoxLayout( GroupBox11->layout() );
    GroupBox11Layout->setAlignment( Qt::AlignTop );

    Layout16_3 = new QHBoxLayout( 0, 0, 6, "Layout16_3"); 

    TestLabel1_3_2_2 = new QLabel( GroupBox11, "TestLabel1_3_2_2" );
    Layout16_3->addWidget( TestLabel1_3_2_2 );

    _lineeditCommandFiles3 = new QLineEdit( GroupBox11, "_lineeditCommandFiles3" );
    Layout16_3->addWidget( _lineeditCommandFiles3 );
    GroupBox11Layout->addLayout( Layout16_3 );
    Widget2Layout->addWidget( GroupBox11 );
    _tabWidget->insertTab( Widget2, QString("") );

    Widget4 = new QWidget( _tabWidget, "Widget4" );
    Widget4Layout = new QVBoxLayout( Widget4, 11, 6, "Widget4Layout"); 

    Layout16_2 = new QHBoxLayout( 0, 0, 6, "Layout16_2"); 

    TestLabel1_2 = new QLabel( Widget4, "TestLabel1_2" );
    Layout16_2->addWidget( TestLabel1_2 );

    _lineeditCommandDirs = new QLineEdit( Widget4, "_lineeditCommandDirs" );
    Layout16_2->addWidget( _lineeditCommandDirs );
    Widget4Layout->addLayout( Layout16_2 );

    Layout16_2_2 = new QHBoxLayout( 0, 0, 6, "Layout16_2_2"); 

    TestLabel1_2_2 = new QLabel( Widget4, "TestLabel1_2_2" );
    Layout16_2_2->addWidget( TestLabel1_2_2 );

    _lineeditCommandDirsRecursive = new QLineEdit( Widget4, "_lineeditCommandDirsRecursive" );
    Layout16_2_2->addWidget( _lineeditCommandDirsRecursive );
    Widget4Layout->addLayout( Layout16_2_2 );

    GroupBox62 = new QGroupBox( Widget4, "GroupBox62" );

    _checkboxRecursive = new QCheckBox( GroupBox62, "_checkboxRecursive" );
    _checkboxRecursive->setGeometry( QRect( 20, 30, 104, 20 ) );
    Widget4Layout->addWidget( GroupBox62 );
    _tabWidget->insertTab( Widget4, QString("") );

    Widget5 = new QWidget( _tabWidget, "Widget5" );
    Widget5Layout = new QVBoxLayout( Widget5, 11, 6, "Widget5Layout"); 

    Layout24 = new QHBoxLayout( 0, 0, 6, "Layout24"); 

    _groupboxFileDiffs = new QGroupBox( Widget5, "_groupboxFileDiffs" );
    _groupboxFileDiffs->setColumnLayout(0, Qt::Vertical );
    _groupboxFileDiffs->layout()->setSpacing( 6 );
    _groupboxFileDiffs->layout()->setMargin( 11 );
    _groupboxFileDiffsLayout = new QVBoxLayout( _groupboxFileDiffs->layout() );
    _groupboxFileDiffsLayout->setAlignment( Qt::AlignTop );

    Layout23 = new QHBoxLayout( 0, 0, 6, "Layout23"); 

    TextLabel1 = new QLabel( _groupboxFileDiffs, "TextLabel1" );
    Layout23->addWidget( TextLabel1 );

    _comboHordiffType = new QComboBox( FALSE, _groupboxFileDiffs, "_comboHordiffType" );
    Layout23->addWidget( _comboHordiffType );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout23->addItem( spacer );
    _groupboxFileDiffsLayout->addLayout( Layout23 );

    Layout21 = new QHBoxLayout( 0, 0, 6, "Layout21"); 

    TextLabel2 = new QLabel( _groupboxFileDiffs, "TextLabel2" );
    Layout21->addWidget( TextLabel2 );

    _spinboxHordiffContext = new QSpinBox( _groupboxFileDiffs, "_spinboxHordiffContext" );
    _spinboxHordiffContext->setMaxValue( 20 );
    _spinboxHordiffContext->setMinValue( 1 );
    _spinboxHordiffContext->setValue( 5 );
    Layout21->addWidget( _spinboxHordiffContext );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout21->addItem( spacer_2 );
    _groupboxFileDiffsLayout->addLayout( Layout21 );

    _checkboxIgnoreHorizontalWhitespace = new QCheckBox( _groupboxFileDiffs, "_checkboxIgnoreHorizontalWhitespace" );
    _groupboxFileDiffsLayout->addWidget( _checkboxIgnoreHorizontalWhitespace );

    _checkboxHideCarriageReturns = new QCheckBox( _groupboxFileDiffs, "_checkboxHideCarriageReturns" );
    _groupboxFileDiffsLayout->addWidget( _checkboxHideCarriageReturns );

    Layout23_2 = new QHBoxLayout( 0, 0, 6, "Layout23_2"); 

    _tabWidthlabel = new QLabel( _groupboxFileDiffs, "_tabWidthlabel" );
    Layout23_2->addWidget( _tabWidthlabel );

    _spinboxTabWidth = new QSpinBox( _groupboxFileDiffs, "_spinboxTabWidth" );
    _spinboxTabWidth->setMaxValue( 48 );
    _spinboxTabWidth->setMinValue( 1 );
    _spinboxTabWidth->setValue( 8 );
    Layout23_2->addWidget( _spinboxTabWidth );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout23_2->addItem( spacer_3 );
    _groupboxFileDiffsLayout->addLayout( Layout23_2 );

    Layout17 = new QHBoxLayout( 0, 0, 6, "Layout17"); 

    _checkboxDrawVerticalLine = new QCheckBox( _groupboxFileDiffs, "_checkboxDrawVerticalLine" );
    Layout17->addWidget( _checkboxDrawVerticalLine );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout17->addItem( spacer_4 );

    _vlinePosLabel = new QLabel( _groupboxFileDiffs, "_vlinePosLabel" );
    Layout17->addWidget( _vlinePosLabel );

    _spinboxVlinePos = new QSpinBox( _groupboxFileDiffs, "_spinboxVlinePos" );
    _spinboxVlinePos->setMaxValue( 512 );
    _spinboxVlinePos->setMinValue( 1 );
    _spinboxVlinePos->setValue( 80 );
    Layout17->addWidget( _spinboxVlinePos );
    _groupboxFileDiffsLayout->addLayout( Layout17 );

    Layout22 = new QHBoxLayout( 0, 0, 6, "Layout22"); 

    TextLabel1_3 = new QLabel( _groupboxFileDiffs, "TextLabel1_3" );
    Layout22->addWidget( TextLabel1_3 );

    _comboIgnoreFile = new QComboBox( FALSE, _groupboxFileDiffs, "_comboIgnoreFile" );
    Layout22->addWidget( _comboIgnoreFile );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout22->addItem( spacer_5 );
    _groupboxFileDiffsLayout->addLayout( Layout22 );
    Layout24->addWidget( _groupboxFileDiffs );

    _groupboxDirectoryDiffs = new QGroupBox( Widget5, "_groupboxDirectoryDiffs" );
    _groupboxDirectoryDiffs->setColumnLayout(0, Qt::Vertical );
    _groupboxDirectoryDiffs->layout()->setSpacing( 6 );
    _groupboxDirectoryDiffs->layout()->setMargin( 11 );
    _groupboxDirectoryDiffsLayout = new QVBoxLayout( _groupboxDirectoryDiffs->layout() );
    _groupboxDirectoryDiffsLayout->setAlignment( Qt::AlignTop );

    Layout24_2 = new QHBoxLayout( 0, 0, 6, "Layout24_2"); 

    _checkboxIgnoreFileChanges = new QCheckBox( _groupboxDirectoryDiffs, "_checkboxIgnoreFileChanges" );
    Layout24_2->addWidget( _checkboxIgnoreFileChanges );
    QSpacerItem* spacer_6 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout24_2->addItem( spacer_6 );
    _groupboxDirectoryDiffsLayout->addLayout( Layout24_2 );
    Layout24->addWidget( _groupboxDirectoryDiffs );
    Widget5Layout->addLayout( Layout24 );

    GroupBox8_2_2 = new QGroupBox( Widget5, "GroupBox8_2_2" );
    GroupBox8_2_2->setColumnLayout(0, Qt::Vertical );
    GroupBox8_2_2->layout()->setSpacing( 6 );
    GroupBox8_2_2->layout()->setMargin( 11 );
    GroupBox8_2_2Layout = new QVBoxLayout( GroupBox8_2_2->layout() );
    GroupBox8_2_2Layout->setAlignment( Qt::AlignTop );

    Layout15 = new QHBoxLayout( 0, 0, 4, "Layout15"); 

    _checkboxFormatClipboardText = new QCheckBox( GroupBox8_2_2, "_checkboxFormatClipboardText" );
    Layout15->addWidget( _checkboxFormatClipboardText );
    QSpacerItem* spacer_7 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout15->addItem( spacer_7 );

    TextLabel1_2 = new QLabel( GroupBox8_2_2, "TextLabel1_2" );
    Layout15->addWidget( TextLabel1_2 );

    _lineeditClipboardFormat = new QLineEdit( GroupBox8_2_2, "_lineeditClipboardFormat" );
    Layout15->addWidget( _lineeditClipboardFormat );
    GroupBox8_2_2Layout->addLayout( Layout15 );
    Widget5Layout->addWidget( GroupBox8_2_2 );
    _tabWidget->insertTab( Widget5, QString("") );

    Widget6 = new QWidget( _tabWidget, "Widget6" );
    Widget6Layout = new QHBoxLayout( Widget6, 11, 6, "Widget6Layout"); 

    _listboxColors = new QListBox( Widget6, "_listboxColors" );
    Widget6Layout->addWidget( _listboxColors );

    Layout30 = new QVBoxLayout( 0, 0, 6, "Layout30"); 

    GroupBox7 = new QGroupBox( Widget6, "GroupBox7" );
#if 0 
    GroupBox7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupBox7->sizePolicy().hasHeightForWidth() ) );
#endif
    GroupBox7->setColumnLayout(0, Qt::Vertical );
    GroupBox7->layout()->setSpacing( 6 );
    GroupBox7->layout()->setMargin( 11 );
    GroupBox7Layout = new QHBoxLayout( GroupBox7->layout() );
    GroupBox7Layout->setAlignment( Qt::AlignTop );

    _labelEditBack = new QLabel( GroupBox7, "_labelEditBack" );
    _labelEditBack->setMinimumSize( QSize( 100, 0 ) );
    _labelEditBack->setFrameShape( QLabel::Box );
    _labelEditBack->setFrameShadow( QLabel::Sunken );
    _labelEditBack->setLineWidth( 2 );
    GroupBox7Layout->addWidget( _labelEditBack );

    _buttonEditBack = new QPushButton( GroupBox7, "_buttonEditBack" );
#if 0 
    _buttonEditBack->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, _buttonEditBack->sizePolicy().hasHeightForWidth() ) );
#endif
    GroupBox7Layout->addWidget( _buttonEditBack );
    Layout30->addWidget( GroupBox7 );

    GroupBox7_2 = new QGroupBox( Widget6, "GroupBox7_2" );
#if 0 
    GroupBox7_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupBox7_2->sizePolicy().hasHeightForWidth() ) );
#endif
    GroupBox7_2->setMinimumSize( QSize( 0, 0 ) );
    GroupBox7_2->setFrameShape( QGroupBox::Box );
    GroupBox7_2->setFrameShadow( QGroupBox::Sunken );
    GroupBox7_2->setLineWidth( 1 );
    GroupBox7_2->setColumnLayout(0, Qt::Vertical );
    GroupBox7_2->layout()->setSpacing( 6 );
    GroupBox7_2->layout()->setMargin( 11 );
    GroupBox7_2Layout = new QHBoxLayout( GroupBox7_2->layout() );
    GroupBox7_2Layout->setAlignment( Qt::AlignTop );

    _labelEditFore = new QLabel( GroupBox7_2, "_labelEditFore" );
    _labelEditFore->setMinimumSize( QSize( 100, 0 ) );
    _labelEditFore->setFrameShape( QLabel::Box );
    _labelEditFore->setFrameShadow( QLabel::Sunken );
    _labelEditFore->setLineWidth( 2 );
    GroupBox7_2Layout->addWidget( _labelEditFore );

    _buttonEditFore = new QPushButton( GroupBox7_2, "_buttonEditFore" );
#if 0 
    _buttonEditFore->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, _buttonEditFore->sizePolicy().hasHeightForWidth() ) );
#endif
    GroupBox7_2Layout->addWidget( _buttonEditFore );
    Layout30->addWidget( GroupBox7_2 );

    GroupBox2 = new QGroupBox( Widget6, "GroupBox2" );
#if 0 
    GroupBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, GroupBox2->sizePolicy().hasHeightForWidth() ) );
#endif
    GroupBox2->setMinimumSize( QSize( 0, 100 ) );
    GroupBox2->setColumnLayout(0, Qt::Vertical );
    GroupBox2->layout()->setSpacing( 6 );
    GroupBox2->layout()->setMargin( 11 );
    GroupBox2Layout = new QHBoxLayout( GroupBox2->layout() );
    GroupBox2Layout->setAlignment( Qt::AlignTop );

    _labelDescription = new QTextView( GroupBox2, "_labelDescription" );
    _labelDescription->setFrameShape( QTextView::Box );
    _labelDescription->setFrameShadow( QTextView::Plain );
    _labelDescription->setTextFormat( QTextView::RichText );
    GroupBox2Layout->addWidget( _labelDescription );
    Layout30->addWidget( GroupBox2 );
    Widget6Layout->addLayout( Layout30 );
    _tabWidget->insertTab( Widget6, QString("") );

    tab = new QWidget( _tabWidget, "tab" );
    tabLayout = new QVBoxLayout( tab, 11, 6, "tabLayout"); 

    GroupBox25_3 = new QGroupBox( tab, "GroupBox25_3" );
    GroupBox25_3->setColumnLayout(0, Qt::Vertical );
    GroupBox25_3->layout()->setSpacing( 6 );
    GroupBox25_3->layout()->setMargin( 11 );
    GroupBox25_3Layout = new QVBoxLayout( GroupBox25_3->layout() );
    GroupBox25_3Layout->setAlignment( Qt::AlignTop );

    Layout16_5 = new QHBoxLayout( 0, 0, 6, "Layout16_5"); 

    _labelFontTextName = new QLabel( GroupBox25_3, "_labelFontTextName" );
    Layout16_5->addWidget( _labelFontTextName );
    QSpacerItem* spacer_8 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout16_5->addItem( spacer_8 );

    _editFontText = new QPushButton( GroupBox25_3, "_editFontText" );
#if 0 
    _editFontText->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, _editFontText->sizePolicy().hasHeightForWidth() ) );
#endif
    Layout16_5->addWidget( _editFontText );
    GroupBox25_3Layout->addLayout( Layout16_5 );

    _labelFontText = new QLabel( GroupBox25_3, "_labelFontText" );
    _labelFontText->setFrameShape( QLabel::Box );
    _labelFontText->setFrameShadow( QLabel::Plain );
    _labelFontText->setTextFormat( QLabel::PlainText );
    _labelFontText->setAlignment( int( QLabel::AlignCenter ) );
    GroupBox25_3Layout->addWidget( _labelFontText );
    tabLayout->addWidget( GroupBox25_3 );

    GroupBox25_2_2 = new QGroupBox( tab, "GroupBox25_2_2" );
    GroupBox25_2_2->setColumnLayout(0, Qt::Vertical );
    GroupBox25_2_2->layout()->setSpacing( 6 );
    GroupBox25_2_2->layout()->setMargin( 11 );
    GroupBox25_2_2Layout = new QVBoxLayout( GroupBox25_2_2->layout() );
    GroupBox25_2_2Layout->setAlignment( Qt::AlignTop );

    Layout16_4_2 = new QHBoxLayout( 0, 0, 6, "Layout16_4_2"); 

    _labelFontAppName = new QLabel( GroupBox25_2_2, "_labelFontAppName" );
    Layout16_4_2->addWidget( _labelFontAppName );
    QSpacerItem* spacer_9 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout16_4_2->addItem( spacer_9 );

    _editFontApp = new QPushButton( GroupBox25_2_2, "_editFontApp" );
#if 0 
    _editFontApp->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, _editFontApp->sizePolicy().hasHeightForWidth() ) );
#endif
    Layout16_4_2->addWidget( _editFontApp );
    GroupBox25_2_2Layout->addLayout( Layout16_4_2 );

    _labelFontApp = new QLabel( GroupBox25_2_2, "_labelFontApp" );
    _labelFontApp->setFrameShape( QLabel::Box );
    _labelFontApp->setFrameShadow( QLabel::Plain );
    _labelFontApp->setTextFormat( QLabel::PlainText );
    _labelFontApp->setAlignment( int( QLabel::AlignCenter ) );
    GroupBox25_2_2Layout->addWidget( _labelFontApp );
    tabLayout->addWidget( GroupBox25_2_2 );
    _tabWidget->insertTab( tab, QString("") );
    XxOptionsDialogBaseLayout->addWidget( _tabWidget );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 
    QSpacerItem* spacer_10 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer_10 );

    _buttonApply = new QPushButton( this, "_buttonApply" );
    _buttonApply->setAutoDefault( TRUE );
    Layout1->addWidget( _buttonApply );

    _buttonOk = new QPushButton( this, "_buttonOk" );
    _buttonOk->setAutoDefault( TRUE );
    _buttonOk->setDefault( TRUE );
    Layout1->addWidget( _buttonOk );

    _buttonCancel = new QPushButton( this, "_buttonCancel" );
    _buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( _buttonCancel );
    XxOptionsDialogBaseLayout->addLayout( Layout1 );
    languageChange();
    resize( QSize(687, 435).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( _buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( _buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
XxOptionsDialogBase::~XxOptionsDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void XxOptionsDialogBase::languageChange()
{
    setCaption( tr( "Options" ) );
    GroupBox10->setTitle( tr( "2-way diff" ) );
    TestLabel1->setText( tr( "Command:" ) );
    _groupBoxGnuDiffOptions->setTitle( tr( "GNU diff options" ) );
    _checkboxIgnoreTrailingBlanks->setText( tr( "Ignore trailing blanks" ) );
    _checkboxIgnoreWhitespace->setText( tr( "Ignore whitespace" ) );
    _checkboxIgnoreCase->setText( tr( "Ignore case" ) );
    _checkboxIgnoreBlankLines->setText( tr( "Ignore blank lines" ) );
    ButtonGroup8->setTitle( tr( "Quality" ) );
    _radiobuttonQualityNormal->setText( tr( "Normal" ) );
    _radiobuttonQualityFastest->setText( tr( "Fastest" ) );
    _radiobuttonQualityHighest->setText( tr( "Highest" ) );
    GroupBox11->setTitle( tr( "3-way diff" ) );
    TestLabel1_3_2_2->setText( tr( "Command:" ) );
    _tabWidget->changeTab( Widget2, tr( "File diff" ) );
    TestLabel1_2->setText( tr( "Shallow command:" ) );
    _lineeditCommandDirs->setText( QString::null );
    TestLabel1_2_2->setText( tr( "Recursive command:" ) );
    GroupBox62->setTitle( tr( "Options" ) );
    _checkboxRecursive->setText( tr( "Recursive" ) );
    _tabWidget->changeTab( Widget4, tr( "Directory diff" ) );
    _groupboxFileDiffs->setTitle( tr( "File diffs" ) );
    TextLabel1->setText( tr( "Horizontal diff:" ) );
    _comboHordiffType->clear();
    _comboHordiffType->insertItem( tr( "None" ) );
    _comboHordiffType->insertItem( tr( "Single" ) );
    _comboHordiffType->insertItem( tr( "Multiple" ) );
    TextLabel2->setText( tr( "Hor.diff. min.context:" ) );
    _checkboxIgnoreHorizontalWhitespace->setText( tr( "Ignore horizontal whitespace" ) );
    _checkboxHideCarriageReturns->setText( tr( "Hide carriage returns" ) );
    _tabWidthlabel->setText( tr( "Tab width:" ) );
    _checkboxDrawVerticalLine->setText( tr( "Draw vertical line" ) );
    _vlinePosLabel->setText( tr( "Position:" ) );
    TextLabel1_3->setText( tr( "Ignore file (3-way only):" ) );
    _comboIgnoreFile->clear();
    _comboIgnoreFile->insertItem( tr( "Ignore none" ) );
    _comboIgnoreFile->insertItem( tr( "Ignore left file" ) );
    _comboIgnoreFile->insertItem( tr( "Ignore middle file" ) );
    _comboIgnoreFile->insertItem( tr( "Ignore right file" ) );
    _groupboxDirectoryDiffs->setTitle( tr( "Directory diffs" ) );
    _checkboxIgnoreFileChanges->setText( tr( "Ignore file changes" ) );
    GroupBox8_2_2->setTitle( tr( "Common" ) );
    _checkboxFormatClipboardText->setText( tr( "Format clipboard text" ) );
    TextLabel1_2->setText( tr( "Clipboard format:" ) );
    _tabWidget->changeTab( Widget5, tr( "Display options" ) );
    GroupBox7->setTitle( tr( "Background" ) );
    _labelEditBack->setText( QString::null );
    _buttonEditBack->setText( tr( "Edit" ) );
    GroupBox7_2->setTitle( tr( "Foreground" ) );
    _labelEditFore->setText( QString::null );
    _buttonEditFore->setText( tr( "Edit" ) );
    GroupBox2->setTitle( tr( "Description" ) );
    _tabWidget->changeTab( Widget6, tr( "Colors" ) );
    GroupBox25_3->setTitle( tr( "Text Font" ) );
    _labelFontTextName->setText( QString::null );
    _editFontText->setText( tr( "Edit" ) );
    _labelFontText->setText( tr( "Playing jazz vibe chords quickly excites my wife." ) );
    GroupBox25_2_2->setTitle( tr( "App Font" ) );
    _labelFontAppName->setText( QString::null );
    _editFontApp->setText( tr( "Edit" ) );
    _labelFontApp->setText( tr( "Playing jazz vibe chords quickly excites my wife." ) );
    _tabWidget->changeTab( tab, tr( "Fonts" ) );
    _buttonApply->setText( tr( "Apply (redo diff)" ) );
    _buttonOk->setText( tr( "&OK" ) );
    _buttonCancel->setText( tr( "&Cancel" ) );
}


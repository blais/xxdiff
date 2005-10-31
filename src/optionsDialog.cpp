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

#include <optionsDialog.h>
#include <resParser.h>
#include <resources.h>
#include <diffs.h>
#include <app.h>

#include <kdeSupport.h>

#include <qtabwidget.h>
#include <qtextview.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qpainter.h>

#include <stdlib.h>

/*==============================================================================
 * LOCAL DECLARATIONS
 *============================================================================*/

namespace {

/*----- variables -----*/

const char* pangrams[] = {
   "Portez ce vieux whisky au juge blond qui fume.",
   "Playing jazz vibe chords quickly excites my wife.",
   "A very bad quack might jinx zippy fowls. ",
   "Big july earthquakes confound zany experimental vow.",
   "Faquir pompeux, vidangez l'abject whisky.",
   "Five wine experts jokingly quizzed sample chablis.",
   "Jackdaws love my big sphinx of quartz.",
   "Six big juicy steaks sizzled in a pan as five workmen left the quarry.",
   "The quick brown fox jumps over a lazy dog.",
   "The five boxing wizards jump quickly.",
   "The exodus of jazzy pigeons is craved by squeamish walkers.",
   "Suez sailor vomits jauntily abaft while waxing parquet decks."
};

/*----- classes -----*/

/*==============================================================================
 * CLASS XxColoredItem
 *============================================================================*/

class XxColoredItem : public QListBoxText {

public:

   /*----- member functions -----*/

   // Ctor/dtor.
   // <group>
   XxColoredItem( const XxResources* resources, XxColor color );
   virtual ~XxColoredItem();
   // </group>

   XxColor getColor() const;

protected:

   /*----- member functions -----*/
   
   // Overriden height to use our own text font.
   virtual int height( const QListBox* lb ) const;

   // Overriden paint to setup the colors right.
   virtual void paint( QPainter* painter );

private:

   /*----- data members -----*/

   const XxResources* _resources;

public: // let the dialog access all of this

   /*----- data members -----*/

   // Current state.
   const XxColor      _color;
   bool               _modified;
   QColor             _foreColor;
   QColor             _backColor;

};

//------------------------------------------------------------------------------
//
XxColoredItem::XxColoredItem( const XxResources* resources, XxColor color ) :
   QListBoxText( XxResParser::getColorName( color ) ),
   _resources( resources ),
   _color( color ),
   _modified( false ),
   _foreColor(),
   _backColor()
{
   XX_ASSERT( _resources );
}

//------------------------------------------------------------------------------
//
XxColoredItem::~XxColoredItem()
{
}

//------------------------------------------------------------------------------
//
inline XxColor XxColoredItem::getColor() const
{
   return _color;
}

//------------------------------------------------------------------------------
//
int XxColoredItem::height( const QListBox* /*lb*/ ) const
{
   const QFont& font = _resources->getFontText();
   QFontMetrics fm( font );
   return fm.height() + 2; // allow space for the listbox cursor.
}

//------------------------------------------------------------------------------
//
void XxColoredItem::paint( QPainter* ppainter )
{
   QPainter& pp = *ppainter;
   const int w = pp.window().width();

   // Font.
   pp.setFont( _resources->getFontText() );
   QFontMetrics fm = pp.fontMetrics();

   // Don't draw background of chars since we'll draw first.
   pp.setBackgroundMode( Qt::TransparentMode );

   QBrush brush( _backColor );
   pp.setPen( _foreColor );

   pp.fillRect( 0, 0, w, fm.height() + 2, brush );
   pp.drawText( 10, 1 + fm.ascent(), text() );
}

}

XX_NAMESPACE_BEGIN

/*==============================================================================
 * PUBLIC FUNCTIONS
 *============================================================================*/

/*==============================================================================
 * CLASS XxOptionsDialog
 *============================================================================*/

//------------------------------------------------------------------------------
//
XxOptionsDialog::XxOptionsDialog( 
   XxApp*   app,
   QWidget* parent
) :
   BaseClass( parent ),
   _app( app )
{
   XX_CHECK( _app != 0 );
   
   // This disables the irrelevant tabs.
   // Don't, let the user edit all fields.

   // XxDiffs* diffs = _app->getDiffs();
   // if ( diffs != 0 ) {
   //    int cpage = getDiffPageIndex();
   // 
   //    // Disable other modes' options.
   //    for ( int ii = 0; ii < 3; ++ii ) {
   //       _tabWidget->setCurrentPage( ii );
   // 
   //       // QWidget* widget = _tabWidget->currentPage();
   //       // if ( ii != cpage && widget ) {
   //       //    widget->setEnabled( false );
   //       // }
   //    }
   // 
   //    _tabWidget->setCurrentPage( cpage );
   // }
   // 
   // if ( diffs->isDirectoryDiff() == true ) {
   //    _groupboxFileDiffs->setEnabled( false );
   // }
   // else {
   //    _groupboxDirectoryDiffs->setEnabled( false );
   // }

   //
   // Make connections.
   //

   connect( _buttonApply, SIGNAL( clicked() ), this, SLOT( onApply() ) );
   connect( _buttonOk, SIGNAL( clicked() ), this, SLOT( onApply() ) );

   //---------------------------------------------------------------------------
   // Files 2

   connect( _checkboxIgnoreTrailingBlanks, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreTrailingBlanks(int) ) );
   connect( _checkboxIgnoreWhitespace, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreWhitespace(int) ) );
   connect( _checkboxIgnoreCase, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreCase(int) ) );
   connect( _checkboxIgnoreBlankLines, SIGNAL( stateChanged(int) ), 
            this, SLOT( checkboxIgnoreBlankLines(int) ) );

   connect( _radiobuttonQualityNormal, SIGNAL( stateChanged(int) ), 
            this, SLOT( radiobuttonQualityNormal(int) ) );
   connect( _radiobuttonQualityFastest, SIGNAL( stateChanged(int) ), 
            this, SLOT( radiobuttonQualityFastest(int) ) );
   connect( _radiobuttonQualityHighest, SIGNAL( stateChanged(int) ), 
            this, SLOT( radiobuttonQualityHighest(int) ) );

   //---------------------------------------------------------------------------
   // Files 3

   //---------------------------------------------------------------------------
   // Directories

   //---------------------------------------------------------------------------
   // Display

   connect( _editFontApp, SIGNAL( clicked() ),
            this, SLOT( editFontApp() ) );
   connect( _editFontText, SIGNAL( clicked() ),
            this, SLOT( editFontText() ) );

   //---------------------------------------------------------------------------
   // Colors

   // Fill up listbox with color names.
   const XxResources* resourcesPtr = &(_app->getResources());
   for ( int ii = 0; ii < int(COLOR_LAST); ++ii ) {
      QListBoxItem* lbi = 
         new XxColoredItem( resourcesPtr, XxColor(ii) );
      _listboxColors->insertItem( lbi );
   }

   connect( _listboxColors, SIGNAL( highlighted(const QString&) ), 
            this, SLOT( listboxColors(const QString&) ) );

   connect( _buttonEditFore, SIGNAL( clicked() ),
            this, SLOT( editColorFore() ) );
   connect( _buttonEditBack, SIGNAL( clicked() ),
            this, SLOT( editColorBack() ) );

   // Make this dialog an observer of the resources.
   connect( resourcesPtr, SIGNAL( changed() ), this, SLOT( synchronize() ) );
}

//------------------------------------------------------------------------------
//
XxOptionsDialog::~XxOptionsDialog()
{
   // Remove as observer.
   const XxResources* resourcesPtr = &(_app->getResources());
   disconnect( resourcesPtr, SIGNAL( changed() ), this, SLOT( synchronize() ) );
}

//------------------------------------------------------------------------------
//
int XxOptionsDialog::getDiffPageIndex() const
{
   XxDiffs* diffs = _app->getDiffs();
   return ( diffs->isDirectoryDiff() == true ) ? 1 : 0;
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::show()
{
   // Set random pangram for font widgets.
   const unsigned int max = sizeof(pangrams)/sizeof(const char*);

   const unsigned int rn1 = 
      static_cast<unsigned int>( rand()/float(RAND_MAX) * max );
   unsigned int rn2;
   while ( 1 ) { // choose different pangrams
      rn2 = static_cast<unsigned int>( rand()/float(RAND_MAX) * max );
      if ( rn2 != rn1 ) {
         break;
      }
   }

   _labelFontApp->setText( pangrams[rn1] );
   _labelFontText->setText( pangrams[rn2] );

   synchronize();
   BaseClass::show();
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::synchronize()
{
   const XxResources& resources = _app->getResources();

   //---------------------------------------------------------------------------
   // Files 2

   XxCommand cmdResId = CMD_DIFF_FILES_2;

   _lineeditCommandFiles2->setText( 
      QString( resources.getCommand( cmdResId ) )
   );

   _checkboxIgnoreTrailingBlanks->setChecked( 
      resources.isCommandSwitch( cmdResId, 
                                  CMDSW_FILES_IGNORE_TRAILING )
   );
   _checkboxIgnoreWhitespace->setChecked( 
      resources.isCommandSwitch( cmdResId, 
                                  CMDSW_FILES_IGNORE_WHITESPACE )
   );
   _checkboxIgnoreCase->setChecked( 
      resources.isCommandSwitch( cmdResId, 
                                  CMDSW_FILES_IGNORE_CASE )
   );
   _checkboxIgnoreBlankLines->setChecked( 
      resources.isCommandSwitch( cmdResId, 
                                  CMDSW_FILES_IGNORE_BLANK_LINES )
   );

   QString rcmd = resources.getCommand( cmdResId );
   XxQuality quality = resources.getQuality( rcmd );
   _radiobuttonQualityNormal->setChecked( 
      quality == QUALITY_NORMAL 
   );
   _radiobuttonQualityFastest->setChecked( 
      quality == QUALITY_FASTEST 
   );
   _radiobuttonQualityHighest->setChecked( 
      quality == QUALITY_HIGHEST 
   );

   //---------------------------------------------------------------------------
   // Files 3

   _lineeditCommandFiles3->setText(
      QString( resources.getCommand( CMD_DIFF_FILES_3 ) )
   );

   //---------------------------------------------------------------------------
   // Directories

   _lineeditCommandDirs->setText(
      QString( resources.getCommand( CMD_DIFF_DIRECTORIES ) )
   );

   _lineeditCommandDirsRecursive->setText(
      QString( resources.getCommand( 
         CMD_DIFF_DIRECTORIES_REC 
      ) )
   );

   _checkboxRecursive->setChecked(
      resources.getBoolOpt( BOOL_DIRDIFF_RECURSIVE )
   );

   //---------------------------------------------------------------------------
   // Display

   _comboHordiffType->setCurrentItem( int( resources.getHordiffType() ) );
   _spinboxHordiffContext->setValue( resources.getHordiffContext() );

   _checkboxIgnoreHorizontalWhitespace->setChecked(
      resources.getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS )
   );
   _checkboxHideCarriageReturns->setChecked(
      resources.getBoolOpt( BOOL_HIDE_CR )
   );

   _spinboxTabWidth->setValue( resources.getTabWidth() );

   _checkboxDrawVerticalLine->setChecked(
      resources.getShowOpt( SHOW_VERTICAL_LINE )
   );

   _comboIgnoreFile->setCurrentItem( int( resources.getIgnoreFile() ) );


   _checkboxIgnoreFileChanges->setChecked(
      resources.getBoolOpt( BOOL_DIRDIFF_IGNORE_FILE_CHANGES )
   );

   _checkboxFormatClipboardText->setChecked(
      resources.getBoolOpt( BOOL_FORMAT_CLIPBOARD_TEXT )
   );

   _lineeditClipboardFormat->setText(
      resources.getClipboardLineFormat()
   );

   _fontApp = resources.getFontApp();
   _labelFontApp->setFont( _fontApp );
   _editFontAppName->setText( getFontDisplayText(_fontApp) );

   _fontText = resources.getFontText();
   _labelFontText->setFont( _fontText );
   _editFontTextName->setText( getFontDisplayText(_fontText) );

   //---------------------------------------------------------------------------
   // Colors

   for ( int ii = 0; ii < int(COLOR_LAST); ++ii ) {
      XxColoredItem* coli =
         static_cast<XxColoredItem*>( _listboxColors->item( ii ) );
      XX_ASSERT( coli );
      coli->_foreColor = resources.getColor( XxColor(ii), true );
      coli->_backColor = resources.getColor( XxColor(ii), false );
      coli->_modified = false;
   }        

   {
      int idx = _listboxColors->currentItem();
      if ( idx != -1 ) {
         XxColoredItem* coli = 
            static_cast<XxColoredItem*>( _listboxColors->item( idx ) );
         XX_ASSERT( coli );
         _labelEditFore->setBackgroundColor( coli->_foreColor );
         _labelEditBack->setBackgroundColor( coli->_backColor );
         _labelEditFore->update();
         _labelEditBack->update();
      }
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::selectDiffOptions()
{
   _tabWidget->setCurrentPage( getDiffPageIndex() );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::selectDisplayOptions()
{
   _tabWidget->setCurrentPage( 2 );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::onApply()
{
   // Remove temporarily as observer, while we're setting the resources.
   const XxResources* resourcesPtr = &(_app->getResources());
   disconnect( resourcesPtr, SIGNAL( changed() ), this, SLOT( synchronize() ) );


   XxResources& resources = _app->getResourcesNC();

   bool redoDiff = false;

   //---------------------------------------------------------------------------
   // Files 2

   // First set resources, then redodiff.
   redoDiff |= maybeSetCommand( 
      CMD_DIFF_FILES_2,
      _lineeditCommandFiles2->text() 
   );

   //---------------------------------------------------------------------------
   // Files 3

   redoDiff |= maybeSetCommand( 
      CMD_DIFF_FILES_3,
      _lineeditCommandFiles3->text()
   );

   //---------------------------------------------------------------------------
   // Directories

   redoDiff |= maybeSetCommand( 
      CMD_DIFF_DIRECTORIES,
      _lineeditCommandDirs->text()
   );
   redoDiff |= maybeSetCommand( 
      CMD_DIFF_DIRECTORIES_REC,
      _lineeditCommandDirsRecursive->text()
   );

   if ( resources.getBoolOpt( BOOL_DIRDIFF_RECURSIVE ) !=
        _checkboxRecursive->isChecked() ) {
      resources.setBoolOpt( BOOL_DIRDIFF_RECURSIVE, 
                            _checkboxRecursive->isChecked() );
      redoDiff = true;
   }
                          
   //---------------------------------------------------------------------------
   // Display

   bool reinitHorizontalDiffs = false;
   XxHordiff newhdtype = XxHordiff( _comboHordiffType->currentItem() );
   if ( resources.getHordiffType() != newhdtype ) {
      resources.setHordiffType( newhdtype );
      reinitHorizontalDiffs = true;
   }

   if ( resources.getHordiffContext() !=
        uint( _spinboxHordiffContext->value() ) ) {
      resources.setHordiffContext( _spinboxHordiffContext->value() );
      reinitHorizontalDiffs = true;
   }

   if ( resources.getBoolOpt( BOOL_IGNORE_HORIZONTAL_WS ) !=
        _checkboxIgnoreHorizontalWhitespace->isChecked() ) {
      resources.setBoolOpt( BOOL_IGNORE_HORIZONTAL_WS, 
                             _checkboxIgnoreHorizontalWhitespace->isChecked() );
      reinitHorizontalDiffs = true;
   }

   resources.setBoolOpt( BOOL_HIDE_CR, 
                         _checkboxHideCarriageReturns->isChecked() );

   resources.setTabWidth( _spinboxTabWidth->value() );

   resources.setShowOpt( SHOW_VERTICAL_LINE, 
                         _checkboxDrawVerticalLine->isChecked() );

   resources.setVerticalLinePos( _spinboxVlinePos->value() );

   resources.setIgnoreFile(
      XxIgnoreFile(_comboIgnoreFile->currentItem())
   );

   if ( resources.getBoolOpt( BOOL_DIRDIFF_IGNORE_FILE_CHANGES ) !=
        _checkboxIgnoreFileChanges->isChecked() == true ) {
      resources.setBoolOpt( BOOL_DIRDIFF_IGNORE_FILE_CHANGES, 
                            _checkboxIgnoreFileChanges->isChecked() );
      redoDiff = true;
   }

   resources.setBoolOpt( BOOL_FORMAT_CLIPBOARD_TEXT, 
                         _checkboxFormatClipboardText->isChecked() );

   QString oldClipboardFormat = resources.getClipboardLineFormat();
   QString newClipboardFormat = _lineeditClipboardFormat->text();
   if ( newClipboardFormat != oldClipboardFormat ) {
      resources.setClipboardLineFormat( newClipboardFormat );
   }

   if ( !XxResources::compareFonts( _fontApp,
                                    resources.getFontApp() ) ) {
      resources.setFontApp( _fontApp );
      _app->setFont( _fontApp, true );
   }
   if ( !XxResources::compareFonts( _fontText,
                                    resources.getFontText() ) ) {
      resources.setFontText( _fontText );
      _app->invalidateTextWidth();
   }

   //---------------------------------------------------------------------------
   // Colors

   for ( int ii = 0; ii < int(COLOR_LAST); ++ii ) {
      XxColoredItem* coli =
         static_cast<XxColoredItem*>( _listboxColors->item( ii ) );
      XX_ASSERT( coli );
      if ( coli->_modified ) {
         resources.setColor( XxColor(ii), true, coli->_foreColor );
         resources.setColor( XxColor(ii), false, coli->_backColor );
         coli->_modified = false;
      }
   }        

   //---------------------------------------------------------------------------

   _app->synchronizeUI();
   
   if ( reinitHorizontalDiffs == true ) {
      XxDiffs* diffs = _app->getDiffs();
      diffs->initializeHorizontalDiffs( resources, _app->getBuffers(), true );
   }

   if ( redoDiff == true ) {
      _app->onRedoDiff();
   }
   else {
      _app->updateWidgets();
   }

   // Reconnect.
   connect( resourcesPtr, SIGNAL( changed() ), this, SLOT( synchronize() ) );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::onClose()
{
   hide();
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreTrailingBlanks( int state )
{
   setFileDiffOptions( CMDSW_FILES_IGNORE_TRAILING, 
                       state != 0 );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreWhitespace( int state )
{
   setFileDiffOptions( CMDSW_FILES_IGNORE_WHITESPACE, 
                       state != 0 );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreCase( int state )
{
   setFileDiffOptions( CMDSW_FILES_IGNORE_CASE, 
                       state != 0 );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::checkboxIgnoreBlankLines( int state )
{
   setFileDiffOptions( CMDSW_FILES_IGNORE_BLANK_LINES, 
                       state != 0 );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::setFileDiffOptions(
   XxCommandSwitch cmdOptionId,
   bool            state
) const
{
   const XxResources& resources = _app->getResources();

   QLineEdit* le = _lineeditCommandFiles2; // 2-way diffs only.
   QString cmd = le->text();
   
   const QString opt = resources.getCommandSwitch( cmdOptionId );

   if ( state == true ) {
      XxOptionsDialog::addToCommand( cmd, opt );
   }
   else {
      XxOptionsDialog::removeFromCommand( cmd, opt );
   }

   le->setText( cmd );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::radiobuttonQualityNormal( int state )
{
   if ( state != 0 ) {
      setFileDiffQuality( QUALITY_NORMAL );
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::radiobuttonQualityFastest( int state )
{
   if ( state != 0 ) {
      setFileDiffQuality( QUALITY_FASTEST );
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::radiobuttonQualityHighest( int state )
{
   if ( state != 0 ) {
      setFileDiffQuality( QUALITY_HIGHEST );
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::setFileDiffQuality(
   XxQuality quality
) const
{
   XxResources& resources = _app->getResourcesNC();

   QLineEdit* le = 0;
   if ( _app->getNbFiles() == 2 ) {
      le = _lineeditCommandFiles2;
   }
   else {
      le = _lineeditCommandFiles3;
   }
   QString cmd = le->text();
   
   resources.setQuality( cmd, quality );

   le->setText( cmd );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::listboxColors( const QString& )
{
   int idx = _listboxColors->currentItem();
   if ( idx == -1 ) {
      return;
   }
   XxColoredItem* coli = 
      static_cast<XxColoredItem*>( _listboxColors->item( idx ) );
   XX_ASSERT( coli );
   
   // Set the buttons backgrounds to the given color.
   _labelEditFore->setBackgroundColor( coli->_foreColor );
   _labelEditBack->setBackgroundColor( coli->_backColor );
   _labelEditFore->update();
   _labelEditBack->update();

   _buttonEditFore->setEnabled(
      !( coli->_color == COLOR_BACKGROUND || 
         coli->_color == COLOR_CURSOR ||
         coli->_color == COLOR_VERTICAL_LINE )
   );

   QString desc = XxResParser::getColorDescription( XxColor(idx) );
   _labelDescription->setText( desc );
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::editFontApp()
{
   const XxResources& resources = _app->getResources();

   bool ok;
   QFont newFont;
   
#ifdef XX_KDE
   newFont = resources.getFontApp();
   ok = KFontDialog::getFont( newFont, false, this ) == QDialog::Accepted;
#else
   newFont =
      QFontDialog::getFont( &ok, resources.getFontApp(), this, "font_dialog" );
#endif

   if ( ok ) {
      if ( 
         !XxResources::compareFonts( newFont, resources.getFontApp() )
      ) {
         _fontApp = newFont;
         _labelFontApp->setFont( _fontApp );
         _editFontAppName->setText( getFontDisplayText( _fontApp ) );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::editFontText()
{
   const XxResources& resources = _app->getResources();

   bool ok;
   QFont newFont;
   
#ifdef XX_KDE
   newFont = resources.getFontText();
   ok = KFontDialog::getFont( newFont, false, this ) == QDialog::Accepted;
#else
   newFont =
      QFontDialog::getFont( &ok, resources.getFontText(), this, "font_dialog" );
#endif
   
   if ( ok ) {
      if ( 
         !XxResources::compareFonts( newFont, resources.getFontText() )
      ) {
         _fontText = newFont;
         _labelFontText->setFont( _fontText );
         _editFontTextName->setText( getFontDisplayText( _fontText ) );
      }
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::editColorFore()
{
   int idx = _listboxColors->currentItem();
   if ( idx == -1 ) {
      return;
   }
   XxColoredItem* coli = 
      static_cast<XxColoredItem*>( _listboxColors->item( idx ) );
   XX_ASSERT( coli );

#ifdef XX_KDE
   QColor newColor = coli->_foreColor;
   if ( KColorDialog::getColor( newColor, this ) == QDialog::Rejected ) {
      newColor = coli->_foreColor;
   }
#else
   QColor newColor = QColorDialog::getColor( coli->_foreColor, this );
#endif
      
   if ( newColor.isValid() && newColor != coli->_foreColor ) {
      coli->_foreColor = newColor;
      coli->_modified = true;
      _listboxColors->update();
      _labelEditFore->setBackgroundColor( coli->_foreColor );
      _labelEditFore->update();
   }
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::editColorBack()
{
   int idx = _listboxColors->currentItem();
   if ( idx == -1 ) {
      return;
   }
   XxColoredItem* coli = 
      static_cast<XxColoredItem*>( _listboxColors->item( idx ) );
   XX_ASSERT( coli );

#ifdef XX_KDE
   QColor newColor = coli->_backColor;
   if ( KColorDialog::getColor( newColor, this ) == QDialog::Rejected ) {
      newColor = coli->_backColor;
   }
#else
   QColor newColor = QColorDialog::getColor( coli->_backColor, this );
#endif
   
   if ( newColor.isValid() && newColor != coli->_backColor ) {
      coli->_backColor = newColor;
      coli->_modified = true;
      _listboxColors->update();
      _labelEditBack->setBackgroundColor( coli->_backColor );
      _labelEditBack->update();
   }
}

//------------------------------------------------------------------------------
//
bool XxOptionsDialog::isInCommand(
   const QString& command,
   const QString& option
)
{
   return command.find( option ) != -1;
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::addToCommand(
   QString&       command,
   const QString& option
)
{
   // Look for option in command, if not found, add it to command.
   int pos = command.find( option );
   if ( pos == -1 ) {
      command.append( " " );
      command.append( option );
   }
   command = command.simplifyWhiteSpace();
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::removeFromCommand( 
   QString&       command,
   const QString& option
)
{
   // Look for option in command, if found, remove it from command.
   int pos = command.find( option );
   if ( pos != -1 ) {
      command.remove( pos, option.length() );
   }
   command = command.simplifyWhiteSpace();
}

//------------------------------------------------------------------------------
//
int XxOptionsDialog::isInCommand(
   const QString& command,
   const QString& option1,
   const QString& option2,
   const QString& option3
)
{
   int l1 = option1.length();
   int l2 = option2.length();
   int l3 = option3.length();

   bool f1 = l1 > 0 && XxOptionsDialog::isInCommand( command, option1 );
   bool f2 = l2 > 0 && XxOptionsDialog::isInCommand( command, option2 );
   bool f3 = l3 > 0 && XxOptionsDialog::isInCommand( command, option3 );

   if ( f1 || f2 || f3 ) {
      if ( f1 ) {
         return 1;
      }
      if ( f2 ) {
         return 2;
      }
      if ( f3 ) {
         return 3;
      }
   }
   else {
      if ( l1 == 0 ) {
         return 1;
      }
      if ( l2 == 0 ) {
         return 2;
      }
      if ( l3 == 0 ) {
         return 3;
      }
   }

   return 0;
}

//------------------------------------------------------------------------------
//
void XxOptionsDialog::setOneOfInCommand(
   QString&       command,
   const QString& optionNew,
   const QString& optionOther1,
   const QString& optionOther2
)
{
   int lNew = optionNew.length();
   int lOther1 = optionOther1.length();
   int lOther2 = optionOther2.length();

   if ( lOther1 > 0 ) {
      XxOptionsDialog::removeFromCommand( command, optionOther1 );
   }
   if ( lOther2 > 0 ) {
      XxOptionsDialog::removeFromCommand( command, optionOther2 );
   }

   if ( lNew > 0 ) {
      XxOptionsDialog::addToCommand( command, optionNew );
   }
}

//------------------------------------------------------------------------------
//
bool XxOptionsDialog::maybeSetCommand( 
   XxCommand      commandId,
   const QString& commandString
)
{
   XxResources& resources = _app->getResourcesNC();

   if ( resources.getCommand( commandId ) != commandString ) {
      resources.setCommand( commandId, commandString );
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
//
QString XxOptionsDialog::getFontDisplayText(
      const QFont& font
) const
{
   QString displayText;
#if (QT_VERSION >= 0x030000)
   displayText = font.toString();
#else
   displayText = font.rawName();
#endif

#if KEPT_FOR_HISTORY
   QFontInfo fontInfo( font );

   displayText = "Font: ";
   displayText += fontInfo.family();
   displayText += "  Size: ";
   displayText += QString::number( fontInfo.pointSize() );
   displayText += "  Style: ";
   
   if ( !fontInfo.bold() && !fontInfo.italic() )
   {
      displayText += "Regular";
   }
   else if ( fontInfo.bold() && !fontInfo.italic() )
   {
      displayText += "Bold";
   }
   else if ( fontInfo.bold() && !fontInfo.italic() )
   {
      displayText += "Italic";
   }
   else if ( fontInfo.bold() && !fontInfo.italic() )
   {
      displayText += "Bold + Italic";
   }
   else
   {
      displayText += "(unknown)";
   }
#endif
   
   return displayText;
}

XX_NAMESPACE_END

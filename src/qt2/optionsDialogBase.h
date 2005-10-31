/****************************************************************************
** Form interface generated from reading ui file 'optionsDialogBase.ui'
**
** Created: Fri Jan 9 17:31:20 2004
**      by: The User Interface Compiler ($Id: optionsDialogBase.h 713 2004-01-12 06:16:16Z blais $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef XXOPTIONSDIALOGBASE_H
#define XXOPTIONSDIALOGBASE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QTabWidget;
class QWidget;
class QGroupBox;
class QLabel;
class QLineEdit;
class QCheckBox;
class QButtonGroup;
class QRadioButton;
class QComboBox;
class QSpinBox;
class QListBox;
class QListBoxItem;
class QPushButton;
class QTextView;

class XxOptionsDialogBase : public QDialog
{
    Q_OBJECT

public:
    XxOptionsDialogBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~XxOptionsDialogBase();

    QTabWidget* _tabWidget;
    QWidget* Widget2;
    QGroupBox* GroupBox10;
    QLabel* TestLabel1;
    QLineEdit* _lineeditCommandFiles2;
    QGroupBox* _groupBoxGnuDiffOptions;
    QCheckBox* _checkboxIgnoreTrailingBlanks;
    QCheckBox* _checkboxIgnoreWhitespace;
    QCheckBox* _checkboxIgnoreCase;
    QCheckBox* _checkboxIgnoreBlankLines;
    QButtonGroup* ButtonGroup8;
    QRadioButton* _radiobuttonQualityNormal;
    QRadioButton* _radiobuttonQualityFastest;
    QRadioButton* _radiobuttonQualityHighest;
    QGroupBox* GroupBox11;
    QLabel* TestLabel1_3_2_2;
    QLineEdit* _lineeditCommandFiles3;
    QWidget* Widget4;
    QLabel* TestLabel1_2;
    QLineEdit* _lineeditCommandDirs;
    QLabel* TestLabel1_2_2;
    QLineEdit* _lineeditCommandDirsRecursive;
    QGroupBox* GroupBox62;
    QCheckBox* _checkboxRecursive;
    QWidget* Widget5;
    QGroupBox* _groupboxFileDiffs;
    QLabel* TextLabel1;
    QComboBox* _comboHordiffType;
    QLabel* TextLabel2;
    QSpinBox* _spinboxHordiffContext;
    QCheckBox* _checkboxIgnoreHorizontalWhitespace;
    QCheckBox* _checkboxHideCarriageReturns;
    QLabel* _tabWidthlabel;
    QSpinBox* _spinboxTabWidth;
    QCheckBox* _checkboxDrawVerticalLine;
    QLabel* _vlinePosLabel;
    QSpinBox* _spinboxVlinePos;
    QLabel* TextLabel1_3;
    QComboBox* _comboIgnoreFile;
    QGroupBox* _groupboxDirectoryDiffs;
    QCheckBox* _checkboxIgnoreFileChanges;
    QGroupBox* GroupBox8_2_2;
    QCheckBox* _checkboxFormatClipboardText;
    QLabel* TextLabel1_2;
    QLineEdit* _lineeditClipboardFormat;
    QWidget* Widget6;
    QListBox* _listboxColors;
    QGroupBox* GroupBox7;
    QLabel* _labelEditBack;
    QPushButton* _buttonEditBack;
    QGroupBox* GroupBox7_2;
    QLabel* _labelEditFore;
    QPushButton* _buttonEditFore;
    QGroupBox* GroupBox2;
    QTextView* _labelDescription;
    QWidget* tab;
    QGroupBox* GroupBox25_3;
    QLabel* _labelFontTextName;
    QPushButton* _editFontText;
    QLabel* _labelFontText;
    QGroupBox* GroupBox25_2_2;
    QLabel* _labelFontAppName;
    QPushButton* _editFontApp;
    QLabel* _labelFontApp;
    QPushButton* _buttonApply;
    QPushButton* _buttonOk;
    QPushButton* _buttonCancel;

protected:
    QVBoxLayout* XxOptionsDialogBaseLayout;
    QVBoxLayout* Widget2Layout;
    QVBoxLayout* GroupBox10Layout;
    QHBoxLayout* Layout16;
    QHBoxLayout* _groupBoxGnuDiffOptionsLayout;
    QVBoxLayout* Layout20;
    QVBoxLayout* ButtonGroup8Layout;
    QHBoxLayout* GroupBox11Layout;
    QHBoxLayout* Layout16_3;
    QVBoxLayout* Widget4Layout;
    QHBoxLayout* Layout16_2;
    QHBoxLayout* Layout16_2_2;
    QVBoxLayout* Widget5Layout;
    QHBoxLayout* Layout24;
    QVBoxLayout* _groupboxFileDiffsLayout;
    QHBoxLayout* Layout23;
    QHBoxLayout* Layout21;
    QHBoxLayout* Layout23_2;
    QHBoxLayout* Layout17;
    QHBoxLayout* Layout22;
    QVBoxLayout* _groupboxDirectoryDiffsLayout;
    QHBoxLayout* Layout24_2;
    QVBoxLayout* GroupBox8_2_2Layout;
    QHBoxLayout* Layout15;
    QHBoxLayout* Widget6Layout;
    QVBoxLayout* Layout30;
    QHBoxLayout* GroupBox7Layout;
    QHBoxLayout* GroupBox7_2Layout;
    QHBoxLayout* GroupBox2Layout;
    QVBoxLayout* tabLayout;
    QVBoxLayout* GroupBox25_3Layout;
    QHBoxLayout* Layout16_5;
    QVBoxLayout* GroupBox25_2_2Layout;
    QHBoxLayout* Layout16_4_2;
    QHBoxLayout* Layout1;

protected slots:
    virtual void languageChange();

};

#endif // XXOPTIONSDIALOGBASE_H

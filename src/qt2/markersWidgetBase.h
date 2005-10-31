/****************************************************************************
** Form interface generated from reading ui file 'markersWidgetBase.ui'
**
** Created: Fri Jan 9 17:31:20 2004
**      by: The User Interface Compiler ($Id: markersWidgetBase.h 713 2004-01-12 06:16:16Z blais $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef XXMARKERSWIDGETBASE_H
#define XXMARKERSWIDGETBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QButtonGroup;
class QRadioButton;
class QCheckBox;
class QLineEdit;

class XxMarkersWidgetBase : public QWidget
{
    Q_OBJECT

public:
    XxMarkersWidgetBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~XxMarkersWidgetBase();

    QLabel* TextLabel1_2;
    QButtonGroup* _buttongroup;
    QRadioButton* _checkboxConflicts;
    QRadioButton* _checkboxConditionals;
    QCheckBox* _removeEmptyConditionals;
    QLabel* _labelConditional1;
    QLineEdit* _lineeditConditional1;
    QLabel* _labelConditional2;
    QLineEdit* _lineeditConditional2;
    QLabel* _labelConditional3;
    QLineEdit* _lineeditConditional3;

protected:
    QVBoxLayout* XxMarkersWidgetBaseLayout;
    QVBoxLayout* _buttongroupLayout;
    QHBoxLayout* Layout3;
    QHBoxLayout* Layout13;
    QHBoxLayout* Layout13_2;
    QHBoxLayout* Layout13_2_2;

protected slots:
    virtual void languageChange();

};

#endif // XXMARKERSWIDGETBASE_H

/****************************************************************************
** XxApp meta object code from reading C++ file 'app.h'
**
** Created: Tue Jan 13 23:58:24 2004
**      by: The Qt MOC ($Id: moc_app.cpp 715 2004-01-14 05:06:05Z blais $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "app.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *XxApp::className() const
{
    return "XxApp";
}

QMetaObject *XxApp::metaObj = 0;

void XxApp::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QApplication::className(), "QApplication") != 0 )
	badSuperclassWarning("XxApp","QApplication");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString XxApp::tr(const char* s)
{
    return qApp->translate( "XxApp", s, 0 );
}

QString XxApp::tr(const char* s, const char * c)
{
    return qApp->translate( "XxApp", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* XxApp::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QApplication::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (XxApp::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (XxApp::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (XxApp::*m1_t2)();
    typedef void (QObject::*om1_t2)();
    typedef void (XxApp::*m1_t3)();
    typedef void (QObject::*om1_t3)();
    typedef void (XxApp::*m1_t4)();
    typedef void (QObject::*om1_t4)();
    typedef void (XxApp::*m1_t5)();
    typedef void (QObject::*om1_t5)();
    typedef void (XxApp::*m1_t6)();
    typedef void (QObject::*om1_t6)();
    typedef void (XxApp::*m1_t7)();
    typedef void (QObject::*om1_t7)();
    typedef void (XxApp::*m1_t8)();
    typedef void (QObject::*om1_t8)();
    typedef void (XxApp::*m1_t9)();
    typedef void (QObject::*om1_t9)();
    typedef void (XxApp::*m1_t10)();
    typedef void (QObject::*om1_t10)();
    typedef void (XxApp::*m1_t11)();
    typedef void (QObject::*om1_t11)();
    typedef void (XxApp::*m1_t12)();
    typedef void (QObject::*om1_t12)();
    typedef void (XxApp::*m1_t13)();
    typedef void (QObject::*om1_t13)();
    typedef void (XxApp::*m1_t14)();
    typedef void (QObject::*om1_t14)();
    typedef void (XxApp::*m1_t15)();
    typedef void (QObject::*om1_t15)();
    typedef void (XxApp::*m1_t16)();
    typedef void (QObject::*om1_t16)();
    typedef void (XxApp::*m1_t17)();
    typedef void (QObject::*om1_t17)();
    typedef void (XxApp::*m1_t18)();
    typedef void (QObject::*om1_t18)();
    typedef void (XxApp::*m1_t19)();
    typedef void (QObject::*om1_t19)();
    typedef void (XxApp::*m1_t20)();
    typedef void (QObject::*om1_t20)();
    typedef void (XxApp::*m1_t21)();
    typedef void (QObject::*om1_t21)();
    typedef void (XxApp::*m1_t22)();
    typedef void (QObject::*om1_t22)();
    typedef void (XxApp::*m1_t23)();
    typedef void (QObject::*om1_t23)();
    typedef void (XxApp::*m1_t24)();
    typedef void (QObject::*om1_t24)();
    typedef void (XxApp::*m1_t25)();
    typedef void (QObject::*om1_t25)();
    typedef void (XxApp::*m1_t26)();
    typedef void (QObject::*om1_t26)();
    typedef void (XxApp::*m1_t27)();
    typedef void (QObject::*om1_t27)();
    typedef void (XxApp::*m1_t28)();
    typedef void (QObject::*om1_t28)();
    typedef void (XxApp::*m1_t29)();
    typedef void (QObject::*om1_t29)();
    typedef void (XxApp::*m1_t30)();
    typedef void (QObject::*om1_t30)();
    typedef void (XxApp::*m1_t31)();
    typedef void (QObject::*om1_t31)();
    typedef void (XxApp::*m1_t32)();
    typedef void (QObject::*om1_t32)();
    typedef void (XxApp::*m1_t33)();
    typedef void (QObject::*om1_t33)();
    typedef void (XxApp::*m1_t34)();
    typedef void (QObject::*om1_t34)();
    typedef void (XxApp::*m1_t35)();
    typedef void (QObject::*om1_t35)();
    typedef void (XxApp::*m1_t36)();
    typedef void (QObject::*om1_t36)();
    typedef void (XxApp::*m1_t37)();
    typedef void (QObject::*om1_t37)();
    typedef void (XxApp::*m1_t38)();
    typedef void (QObject::*om1_t38)();
    typedef void (XxApp::*m1_t39)();
    typedef void (QObject::*om1_t39)();
    typedef void (XxApp::*m1_t40)();
    typedef void (QObject::*om1_t40)();
    typedef int (XxApp::*m1_t41)();
    typedef int (QObject::*om1_t41)();
    typedef void (XxApp::*m1_t42)();
    typedef void (QObject::*om1_t42)();
    typedef void (XxApp::*m1_t43)();
    typedef void (QObject::*om1_t43)();
    typedef void (XxApp::*m1_t44)();
    typedef void (QObject::*om1_t44)();
    typedef void (XxApp::*m1_t45)();
    typedef void (QObject::*om1_t45)();
    typedef void (XxApp::*m1_t46)();
    typedef void (QObject::*om1_t46)();
    typedef void (XxApp::*m1_t47)();
    typedef void (QObject::*om1_t47)();
    typedef void (XxApp::*m1_t48)();
    typedef void (QObject::*om1_t48)();
    typedef void (XxApp::*m1_t49)();
    typedef void (QObject::*om1_t49)();
    typedef void (XxApp::*m1_t50)();
    typedef void (QObject::*om1_t50)();
    typedef void (XxApp::*m1_t51)();
    typedef void (QObject::*om1_t51)();
    typedef void (XxApp::*m1_t52)();
    typedef void (QObject::*om1_t52)();
    typedef void (XxApp::*m1_t53)();
    typedef void (QObject::*om1_t53)();
    typedef void (XxApp::*m1_t54)();
    typedef void (QObject::*om1_t54)();
    typedef void (XxApp::*m1_t55)();
    typedef void (QObject::*om1_t55)();
    typedef void (XxApp::*m1_t56)();
    typedef void (QObject::*om1_t56)();
    typedef void (XxApp::*m1_t57)();
    typedef void (QObject::*om1_t57)();
    typedef void (XxApp::*m1_t58)();
    typedef void (QObject::*om1_t58)();
    typedef void (XxApp::*m1_t59)();
    typedef void (QObject::*om1_t59)();
    typedef void (XxApp::*m1_t60)();
    typedef void (QObject::*om1_t60)();
    typedef void (XxApp::*m1_t61)();
    typedef void (QObject::*om1_t61)();
    typedef void (XxApp::*m1_t62)();
    typedef void (QObject::*om1_t62)();
    typedef void (XxApp::*m1_t63)();
    typedef void (QObject::*om1_t63)();
    typedef void (XxApp::*m1_t64)();
    typedef void (QObject::*om1_t64)();
    typedef void (XxApp::*m1_t65)();
    typedef void (QObject::*om1_t65)();
    typedef void (XxApp::*m1_t66)();
    typedef void (QObject::*om1_t66)();
    typedef void (XxApp::*m1_t67)();
    typedef void (QObject::*om1_t67)();
    typedef void (XxApp::*m1_t68)();
    typedef void (QObject::*om1_t68)();
    typedef void (XxApp::*m1_t69)();
    typedef void (QObject::*om1_t69)();
    typedef void (XxApp::*m1_t70)();
    typedef void (QObject::*om1_t70)();
    typedef void (XxApp::*m1_t71)();
    typedef void (QObject::*om1_t71)();
    typedef void (XxApp::*m1_t72)();
    typedef void (QObject::*om1_t72)();
    typedef void (XxApp::*m1_t73)();
    typedef void (QObject::*om1_t73)();
    typedef void (XxApp::*m1_t74)();
    typedef void (QObject::*om1_t74)();
    typedef void (XxApp::*m1_t75)();
    typedef void (QObject::*om1_t75)();
    typedef void (XxApp::*m1_t76)();
    typedef void (QObject::*om1_t76)();
    typedef void (XxApp::*m1_t77)();
    typedef void (QObject::*om1_t77)();
    typedef void (XxApp::*m1_t78)();
    typedef void (QObject::*om1_t78)();
    typedef void (XxApp::*m1_t79)();
    typedef void (QObject::*om1_t79)();
    typedef void (XxApp::*m1_t80)();
    typedef void (QObject::*om1_t80)();
    typedef void (XxApp::*m1_t81)();
    typedef void (QObject::*om1_t81)();
    typedef void (XxApp::*m1_t82)();
    typedef void (QObject::*om1_t82)();
    typedef void (XxApp::*m1_t83)();
    typedef void (QObject::*om1_t83)();
    typedef void (XxApp::*m1_t84)();
    typedef void (QObject::*om1_t84)();
    typedef void (XxApp::*m1_t85)();
    typedef void (QObject::*om1_t85)();
    typedef void (XxApp::*m1_t86)();
    typedef void (QObject::*om1_t86)();
    typedef void (XxApp::*m1_t87)();
    typedef void (QObject::*om1_t87)();
    typedef void (XxApp::*m1_t88)();
    typedef void (QObject::*om1_t88)();
    typedef void (XxApp::*m1_t89)();
    typedef void (QObject::*om1_t89)();
    typedef void (XxApp::*m1_t90)();
    typedef void (QObject::*om1_t90)();
    typedef void (XxApp::*m1_t91)();
    typedef void (QObject::*om1_t91)();
    typedef void (XxApp::*m1_t92)();
    typedef void (QObject::*om1_t92)();
    typedef void (XxApp::*m1_t93)();
    typedef void (QObject::*om1_t93)();
    typedef void (XxApp::*m1_t94)();
    typedef void (QObject::*om1_t94)();
    typedef void (XxApp::*m1_t95)();
    typedef void (QObject::*om1_t95)();
    typedef void (XxApp::*m1_t96)();
    typedef void (QObject::*om1_t96)();
    typedef void (XxApp::*m1_t97)();
    typedef void (QObject::*om1_t97)();
    typedef void (XxApp::*m1_t98)();
    typedef void (QObject::*om1_t98)();
    typedef void (XxApp::*m1_t99)();
    typedef void (QObject::*om1_t99)();
    m1_t0 v1_0 = &XxApp::updateWidgets;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &XxApp::onNbLinesChanged;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &XxApp::editDone;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    m1_t3 v1_3 = &XxApp::openLeft;
    om1_t3 ov1_3 = (om1_t3)v1_3;
    m1_t4 v1_4 = &XxApp::openMiddle;
    om1_t4 ov1_4 = (om1_t4)v1_4;
    m1_t5 v1_5 = &XxApp::openRight;
    om1_t5 ov1_5 = (om1_t5)v1_5;
    m1_t6 v1_6 = &XxApp::saveAsLeft;
    om1_t6 ov1_6 = (om1_t6)v1_6;
    m1_t7 v1_7 = &XxApp::saveAsMiddle;
    om1_t7 ov1_7 = (om1_t7)v1_7;
    m1_t8 v1_8 = &XxApp::saveAsRight;
    om1_t8 ov1_8 = (om1_t8)v1_8;
    m1_t9 v1_9 = &XxApp::saveAsMerged;
    om1_t9 ov1_9 = (om1_t9)v1_9;
    m1_t10 v1_10 = &XxApp::saveAs;
    om1_t10 ov1_10 = (om1_t10)v1_10;
    m1_t11 v1_11 = &XxApp::saveSelectedOnly;
    om1_t11 ov1_11 = (om1_t11)v1_11;
    m1_t12 v1_12 = &XxApp::editLeft;
    om1_t12 ov1_12 = (om1_t12)v1_12;
    m1_t13 v1_13 = &XxApp::editMiddle;
    om1_t13 ov1_13 = (om1_t13)v1_13;
    m1_t14 v1_14 = &XxApp::editRight;
    om1_t14 ov1_14 = (om1_t14)v1_14;
    m1_t15 v1_15 = &XxApp::quit;
    om1_t15 ov1_15 = (om1_t15)v1_15;
    m1_t16 v1_16 = &XxApp::search;
    om1_t16 ov1_16 = (om1_t16)v1_16;
    m1_t17 v1_17 = &XxApp::searchForward;
    om1_t17 ov1_17 = (om1_t17)v1_17;
    m1_t18 v1_18 = &XxApp::searchBackward;
    om1_t18 ov1_18 = (om1_t18)v1_18;
    m1_t19 v1_19 = &XxApp::redoDiff;
    om1_t19 ov1_19 = (om1_t19)v1_19;
    m1_t20 v1_20 = &XxApp::editDiffOptions;
    om1_t20 ov1_20 = (om1_t20)v1_20;
    m1_t21 v1_21 = &XxApp::editDisplayOptions;
    om1_t21 ov1_21 = (om1_t21)v1_21;
    m1_t22 v1_22 = &XxApp::diffFilesAtCursor;
    om1_t22 ov1_22 = (om1_t22)v1_22;
    m1_t23 v1_23 = &XxApp::nextAndDiffFiles;
    om1_t23 ov1_23 = (om1_t23)v1_23;
    m1_t24 v1_24 = &XxApp::copyFileLeftToRight;
    om1_t24 ov1_24 = (om1_t24)v1_24;
    m1_t25 v1_25 = &XxApp::copyFileRightToLeft;
    om1_t25 ov1_25 = (om1_t25)v1_25;
    m1_t26 v1_26 = &XxApp::removeFileLeft;
    om1_t26 ov1_26 = (om1_t26)v1_26;
    m1_t27 v1_27 = &XxApp::removeFileRight;
    om1_t27 ov1_27 = (om1_t27)v1_27;
    m1_t28 v1_28 = &XxApp::nextDifference;
    om1_t28 ov1_28 = (om1_t28)v1_28;
    m1_t29 v1_29 = &XxApp::previousDifference;
    om1_t29 ov1_29 = (om1_t29)v1_29;
    m1_t30 v1_30 = &XxApp::nextUnselected;
    om1_t30 ov1_30 = (om1_t30)v1_30;
    m1_t31 v1_31 = &XxApp::previousUnselected;
    om1_t31 ov1_31 = (om1_t31)v1_31;
    m1_t32 v1_32 = &XxApp::selectGlobalLeft;
    om1_t32 ov1_32 = (om1_t32)v1_32;
    m1_t33 v1_33 = &XxApp::selectGlobalMiddle;
    om1_t33 ov1_33 = (om1_t33)v1_33;
    m1_t34 v1_34 = &XxApp::selectGlobalRight;
    om1_t34 ov1_34 = (om1_t34)v1_34;
    m1_t35 v1_35 = &XxApp::selectGlobalNeither;
    om1_t35 ov1_35 = (om1_t35)v1_35;
    m1_t36 v1_36 = &XxApp::selectGlobalUnselect;
    om1_t36 ov1_36 = (om1_t36)v1_36;
    m1_t37 v1_37 = &XxApp::selectGlobalUnselectedLeft;
    om1_t37 ov1_37 = (om1_t37)v1_37;
    m1_t38 v1_38 = &XxApp::selectGlobalUnselectedMiddle;
    om1_t38 ov1_38 = (om1_t38)v1_38;
    m1_t39 v1_39 = &XxApp::selectGlobalUnselectedRight;
    om1_t39 ov1_39 = (om1_t39)v1_39;
    m1_t40 v1_40 = &XxApp::selectGlobalUnselectedNeither;
    om1_t40 ov1_40 = (om1_t40)v1_40;
    m1_t41 v1_41 = &XxApp::selectGlobalMerge;
    om1_t41 ov1_41 = (om1_t41)v1_41;
    m1_t42 v1_42 = &XxApp::selectRegionLeft;
    om1_t42 ov1_42 = (om1_t42)v1_42;
    m1_t43 v1_43 = &XxApp::selectRegionMiddle;
    om1_t43 ov1_43 = (om1_t43)v1_43;
    m1_t44 v1_44 = &XxApp::selectRegionRight;
    om1_t44 ov1_44 = (om1_t44)v1_44;
    m1_t45 v1_45 = &XxApp::selectRegionNeither;
    om1_t45 ov1_45 = (om1_t45)v1_45;
    m1_t46 v1_46 = &XxApp::selectRegionUnselect;
    om1_t46 ov1_46 = (om1_t46)v1_46;
    m1_t47 v1_47 = &XxApp::selectRegionLeftAndNext;
    om1_t47 ov1_47 = (om1_t47)v1_47;
    m1_t48 v1_48 = &XxApp::selectRegionMiddleAndNext;
    om1_t48 ov1_48 = (om1_t48)v1_48;
    m1_t49 v1_49 = &XxApp::selectRegionRightAndNext;
    om1_t49 ov1_49 = (om1_t49)v1_49;
    m1_t50 v1_50 = &XxApp::selectRegionNeitherAndNext;
    om1_t50 ov1_50 = (om1_t50)v1_50;
    m1_t51 v1_51 = &XxApp::regionSplitSwapJoin;
    om1_t51 ov1_51 = (om1_t51)v1_51;
    m1_t52 v1_52 = &XxApp::selectLineLeft;
    om1_t52 ov1_52 = (om1_t52)v1_52;
    m1_t53 v1_53 = &XxApp::selectLineMiddle;
    om1_t53 ov1_53 = (om1_t53)v1_53;
    m1_t54 v1_54 = &XxApp::selectLineRight;
    om1_t54 ov1_54 = (om1_t54)v1_54;
    m1_t55 v1_55 = &XxApp::selectLineNeither;
    om1_t55 ov1_55 = (om1_t55)v1_55;
    m1_t56 v1_56 = &XxApp::selectLineUnselect;
    om1_t56 ov1_56 = (om1_t56)v1_56;
    m1_t57 v1_57 = &XxApp::tabsAt3;
    om1_t57 ov1_57 = (om1_t57)v1_57;
    m1_t58 v1_58 = &XxApp::tabsAt4;
    om1_t58 ov1_58 = (om1_t58)v1_58;
    m1_t59 v1_59 = &XxApp::tabsAt8;
    om1_t59 ov1_59 = (om1_t59)v1_59;
    m1_t60 v1_60 = &XxApp::ignoreTrailing;
    om1_t60 ov1_60 = (om1_t60)v1_60;
    m1_t61 v1_61 = &XxApp::ignoreWhitespace;
    om1_t61 ov1_61 = (om1_t61)v1_61;
    m1_t62 v1_62 = &XxApp::ignoreCase;
    om1_t62 ov1_62 = (om1_t62)v1_62;
    m1_t63 v1_63 = &XxApp::ignoreBlankLines;
    om1_t63 ov1_63 = (om1_t63)v1_63;
    m1_t64 v1_64 = &XxApp::hideCarriageReturns;
    om1_t64 ov1_64 = (om1_t64)v1_64;
    m1_t65 v1_65 = &XxApp::dirDiffRecursive;
    om1_t65 ov1_65 = (om1_t65)v1_65;
    m1_t66 v1_66 = &XxApp::ignoreFileChanges;
    om1_t66 ov1_66 = (om1_t66)v1_66;
    m1_t67 v1_67 = &XxApp::qualityNormal;
    om1_t67 ov1_67 = (om1_t67)v1_67;
    m1_t68 v1_68 = &XxApp::qualityFastest;
    om1_t68 ov1_68 = (om1_t68)v1_68;
    m1_t69 v1_69 = &XxApp::qualityHighest;
    om1_t69 ov1_69 = (om1_t69)v1_69;
    m1_t70 v1_70 = &XxApp::toggleShowFilenames;
    om1_t70 ov1_70 = (om1_t70)v1_70;
    m1_t71 v1_71 = &XxApp::togglePaneMergedView;
    om1_t71 ov1_71 = (om1_t71)v1_71;
    m1_t72 v1_72 = &XxApp::togglePopupMergedView;
    om1_t72 ov1_72 = (om1_t72)v1_72;
    m1_t73 v1_73 = &XxApp::toggleToolbar;
    om1_t73 ov1_73 = (om1_t73)v1_73;
    m1_t74 v1_74 = &XxApp::toggleLineNumbers;
    om1_t74 ov1_74 = (om1_t74)v1_74;
    m1_t75 v1_75 = &XxApp::toggleVerticalLine;
    om1_t75 ov1_75 = (om1_t75)v1_75;
    m1_t76 v1_76 = &XxApp::toggleOverview;
    om1_t76 ov1_76 = (om1_t76)v1_76;
    m1_t77 v1_77 = &XxApp::hordiffTypeNone;
    om1_t77 ov1_77 = (om1_t77)v1_77;
    m1_t78 v1_78 = &XxApp::hordiffTypeSingle;
    om1_t78 ov1_78 = (om1_t78)v1_78;
    m1_t79 v1_79 = &XxApp::hordiffTypeMultiple;
    om1_t79 ov1_79 = (om1_t79)v1_79;
    m1_t80 v1_80 = &XxApp::hordiffTypeUpdate;
    om1_t80 ov1_80 = (om1_t80)v1_80;
    m1_t81 v1_81 = &XxApp::toggleIgnoreHorizontalWs;
    om1_t81 ov1_81 = (om1_t81)v1_81;
    m1_t82 v1_82 = &XxApp::toggleIgnorePerHunkWs;
    om1_t82 ov1_82 = (om1_t82)v1_82;
    m1_t83 v1_83 = &XxApp::toggleFormatClipboardText;
    om1_t83 ov1_83 = (om1_t83)v1_83;
    m1_t84 v1_84 = &XxApp::ignoreFileNone;
    om1_t84 ov1_84 = (om1_t84)v1_84;
    m1_t85 v1_85 = &XxApp::ignoreFileLeft;
    om1_t85 ov1_85 = (om1_t85)v1_85;
    m1_t86 v1_86 = &XxApp::ignoreFileMiddle;
    om1_t86 ov1_86 = (om1_t86)v1_86;
    m1_t87 v1_87 = &XxApp::ignoreFileRight;
    om1_t87 ov1_87 = (om1_t87)v1_87;
    m1_t88 v1_88 = &XxApp::helpManPage;
    om1_t88 ov1_88 = (om1_t88)v1_88;
    m1_t89 v1_89 = &XxApp::helpGenInitFile;
    om1_t89 ov1_89 = (om1_t89)v1_89;
    m1_t90 v1_90 = &XxApp::helpAbout;
    om1_t90 ov1_90 = (om1_t90)v1_90;
    m1_t91 v1_91 = &XxApp::pageDown;
    om1_t91 ov1_91 = (om1_t91)v1_91;
    m1_t92 v1_92 = &XxApp::pageUp;
    om1_t92 ov1_92 = (om1_t92)v1_92;
    m1_t93 v1_93 = &XxApp::cursorDown;
    om1_t93 ov1_93 = (om1_t93)v1_93;
    m1_t94 v1_94 = &XxApp::cursorUp;
    om1_t94 ov1_94 = (om1_t94)v1_94;
    m1_t95 v1_95 = &XxApp::cursorTop;
    om1_t95 ov1_95 = (om1_t95)v1_95;
    m1_t96 v1_96 = &XxApp::cursorBottom;
    om1_t96 ov1_96 = (om1_t96)v1_96;
    m1_t97 v1_97 = &XxApp::quitAccept;
    om1_t97 ov1_97 = (om1_t97)v1_97;
    m1_t98 v1_98 = &XxApp::quitReject;
    om1_t98 ov1_98 = (om1_t98)v1_98;
    m1_t99 v1_99 = &XxApp::quitMerged;
    om1_t99 ov1_99 = (om1_t99)v1_99;
    QMetaData *slot_tbl = QMetaObject::new_metadata(100);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(100);
    slot_tbl[0].name = "updateWidgets()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "onNbLinesChanged()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "editDone()";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    slot_tbl[3].name = "openLeft()";
    slot_tbl[3].ptr = (QMember)ov1_3;
    slot_tbl_access[3] = QMetaData::Public;
    slot_tbl[4].name = "openMiddle()";
    slot_tbl[4].ptr = (QMember)ov1_4;
    slot_tbl_access[4] = QMetaData::Public;
    slot_tbl[5].name = "openRight()";
    slot_tbl[5].ptr = (QMember)ov1_5;
    slot_tbl_access[5] = QMetaData::Public;
    slot_tbl[6].name = "saveAsLeft()";
    slot_tbl[6].ptr = (QMember)ov1_6;
    slot_tbl_access[6] = QMetaData::Public;
    slot_tbl[7].name = "saveAsMiddle()";
    slot_tbl[7].ptr = (QMember)ov1_7;
    slot_tbl_access[7] = QMetaData::Public;
    slot_tbl[8].name = "saveAsRight()";
    slot_tbl[8].ptr = (QMember)ov1_8;
    slot_tbl_access[8] = QMetaData::Public;
    slot_tbl[9].name = "saveAsMerged()";
    slot_tbl[9].ptr = (QMember)ov1_9;
    slot_tbl_access[9] = QMetaData::Public;
    slot_tbl[10].name = "saveAs()";
    slot_tbl[10].ptr = (QMember)ov1_10;
    slot_tbl_access[10] = QMetaData::Public;
    slot_tbl[11].name = "saveSelectedOnly()";
    slot_tbl[11].ptr = (QMember)ov1_11;
    slot_tbl_access[11] = QMetaData::Public;
    slot_tbl[12].name = "editLeft()";
    slot_tbl[12].ptr = (QMember)ov1_12;
    slot_tbl_access[12] = QMetaData::Public;
    slot_tbl[13].name = "editMiddle()";
    slot_tbl[13].ptr = (QMember)ov1_13;
    slot_tbl_access[13] = QMetaData::Public;
    slot_tbl[14].name = "editRight()";
    slot_tbl[14].ptr = (QMember)ov1_14;
    slot_tbl_access[14] = QMetaData::Public;
    slot_tbl[15].name = "quit()";
    slot_tbl[15].ptr = (QMember)ov1_15;
    slot_tbl_access[15] = QMetaData::Public;
    slot_tbl[16].name = "search()";
    slot_tbl[16].ptr = (QMember)ov1_16;
    slot_tbl_access[16] = QMetaData::Public;
    slot_tbl[17].name = "searchForward()";
    slot_tbl[17].ptr = (QMember)ov1_17;
    slot_tbl_access[17] = QMetaData::Public;
    slot_tbl[18].name = "searchBackward()";
    slot_tbl[18].ptr = (QMember)ov1_18;
    slot_tbl_access[18] = QMetaData::Public;
    slot_tbl[19].name = "redoDiff()";
    slot_tbl[19].ptr = (QMember)ov1_19;
    slot_tbl_access[19] = QMetaData::Public;
    slot_tbl[20].name = "editDiffOptions()";
    slot_tbl[20].ptr = (QMember)ov1_20;
    slot_tbl_access[20] = QMetaData::Public;
    slot_tbl[21].name = "editDisplayOptions()";
    slot_tbl[21].ptr = (QMember)ov1_21;
    slot_tbl_access[21] = QMetaData::Public;
    slot_tbl[22].name = "diffFilesAtCursor()";
    slot_tbl[22].ptr = (QMember)ov1_22;
    slot_tbl_access[22] = QMetaData::Public;
    slot_tbl[23].name = "nextAndDiffFiles()";
    slot_tbl[23].ptr = (QMember)ov1_23;
    slot_tbl_access[23] = QMetaData::Public;
    slot_tbl[24].name = "copyFileLeftToRight()";
    slot_tbl[24].ptr = (QMember)ov1_24;
    slot_tbl_access[24] = QMetaData::Public;
    slot_tbl[25].name = "copyFileRightToLeft()";
    slot_tbl[25].ptr = (QMember)ov1_25;
    slot_tbl_access[25] = QMetaData::Public;
    slot_tbl[26].name = "removeFileLeft()";
    slot_tbl[26].ptr = (QMember)ov1_26;
    slot_tbl_access[26] = QMetaData::Public;
    slot_tbl[27].name = "removeFileRight()";
    slot_tbl[27].ptr = (QMember)ov1_27;
    slot_tbl_access[27] = QMetaData::Public;
    slot_tbl[28].name = "nextDifference()";
    slot_tbl[28].ptr = (QMember)ov1_28;
    slot_tbl_access[28] = QMetaData::Public;
    slot_tbl[29].name = "previousDifference()";
    slot_tbl[29].ptr = (QMember)ov1_29;
    slot_tbl_access[29] = QMetaData::Public;
    slot_tbl[30].name = "nextUnselected()";
    slot_tbl[30].ptr = (QMember)ov1_30;
    slot_tbl_access[30] = QMetaData::Public;
    slot_tbl[31].name = "previousUnselected()";
    slot_tbl[31].ptr = (QMember)ov1_31;
    slot_tbl_access[31] = QMetaData::Public;
    slot_tbl[32].name = "selectGlobalLeft()";
    slot_tbl[32].ptr = (QMember)ov1_32;
    slot_tbl_access[32] = QMetaData::Public;
    slot_tbl[33].name = "selectGlobalMiddle()";
    slot_tbl[33].ptr = (QMember)ov1_33;
    slot_tbl_access[33] = QMetaData::Public;
    slot_tbl[34].name = "selectGlobalRight()";
    slot_tbl[34].ptr = (QMember)ov1_34;
    slot_tbl_access[34] = QMetaData::Public;
    slot_tbl[35].name = "selectGlobalNeither()";
    slot_tbl[35].ptr = (QMember)ov1_35;
    slot_tbl_access[35] = QMetaData::Public;
    slot_tbl[36].name = "selectGlobalUnselect()";
    slot_tbl[36].ptr = (QMember)ov1_36;
    slot_tbl_access[36] = QMetaData::Public;
    slot_tbl[37].name = "selectGlobalUnselectedLeft()";
    slot_tbl[37].ptr = (QMember)ov1_37;
    slot_tbl_access[37] = QMetaData::Public;
    slot_tbl[38].name = "selectGlobalUnselectedMiddle()";
    slot_tbl[38].ptr = (QMember)ov1_38;
    slot_tbl_access[38] = QMetaData::Public;
    slot_tbl[39].name = "selectGlobalUnselectedRight()";
    slot_tbl[39].ptr = (QMember)ov1_39;
    slot_tbl_access[39] = QMetaData::Public;
    slot_tbl[40].name = "selectGlobalUnselectedNeither()";
    slot_tbl[40].ptr = (QMember)ov1_40;
    slot_tbl_access[40] = QMetaData::Public;
    slot_tbl[41].name = "selectGlobalMerge()";
    slot_tbl[41].ptr = (QMember)ov1_41;
    slot_tbl_access[41] = QMetaData::Public;
    slot_tbl[42].name = "selectRegionLeft()";
    slot_tbl[42].ptr = (QMember)ov1_42;
    slot_tbl_access[42] = QMetaData::Public;
    slot_tbl[43].name = "selectRegionMiddle()";
    slot_tbl[43].ptr = (QMember)ov1_43;
    slot_tbl_access[43] = QMetaData::Public;
    slot_tbl[44].name = "selectRegionRight()";
    slot_tbl[44].ptr = (QMember)ov1_44;
    slot_tbl_access[44] = QMetaData::Public;
    slot_tbl[45].name = "selectRegionNeither()";
    slot_tbl[45].ptr = (QMember)ov1_45;
    slot_tbl_access[45] = QMetaData::Public;
    slot_tbl[46].name = "selectRegionUnselect()";
    slot_tbl[46].ptr = (QMember)ov1_46;
    slot_tbl_access[46] = QMetaData::Public;
    slot_tbl[47].name = "selectRegionLeftAndNext()";
    slot_tbl[47].ptr = (QMember)ov1_47;
    slot_tbl_access[47] = QMetaData::Public;
    slot_tbl[48].name = "selectRegionMiddleAndNext()";
    slot_tbl[48].ptr = (QMember)ov1_48;
    slot_tbl_access[48] = QMetaData::Public;
    slot_tbl[49].name = "selectRegionRightAndNext()";
    slot_tbl[49].ptr = (QMember)ov1_49;
    slot_tbl_access[49] = QMetaData::Public;
    slot_tbl[50].name = "selectRegionNeitherAndNext()";
    slot_tbl[50].ptr = (QMember)ov1_50;
    slot_tbl_access[50] = QMetaData::Public;
    slot_tbl[51].name = "regionSplitSwapJoin()";
    slot_tbl[51].ptr = (QMember)ov1_51;
    slot_tbl_access[51] = QMetaData::Public;
    slot_tbl[52].name = "selectLineLeft()";
    slot_tbl[52].ptr = (QMember)ov1_52;
    slot_tbl_access[52] = QMetaData::Public;
    slot_tbl[53].name = "selectLineMiddle()";
    slot_tbl[53].ptr = (QMember)ov1_53;
    slot_tbl_access[53] = QMetaData::Public;
    slot_tbl[54].name = "selectLineRight()";
    slot_tbl[54].ptr = (QMember)ov1_54;
    slot_tbl_access[54] = QMetaData::Public;
    slot_tbl[55].name = "selectLineNeither()";
    slot_tbl[55].ptr = (QMember)ov1_55;
    slot_tbl_access[55] = QMetaData::Public;
    slot_tbl[56].name = "selectLineUnselect()";
    slot_tbl[56].ptr = (QMember)ov1_56;
    slot_tbl_access[56] = QMetaData::Public;
    slot_tbl[57].name = "tabsAt3()";
    slot_tbl[57].ptr = (QMember)ov1_57;
    slot_tbl_access[57] = QMetaData::Public;
    slot_tbl[58].name = "tabsAt4()";
    slot_tbl[58].ptr = (QMember)ov1_58;
    slot_tbl_access[58] = QMetaData::Public;
    slot_tbl[59].name = "tabsAt8()";
    slot_tbl[59].ptr = (QMember)ov1_59;
    slot_tbl_access[59] = QMetaData::Public;
    slot_tbl[60].name = "ignoreTrailing()";
    slot_tbl[60].ptr = (QMember)ov1_60;
    slot_tbl_access[60] = QMetaData::Public;
    slot_tbl[61].name = "ignoreWhitespace()";
    slot_tbl[61].ptr = (QMember)ov1_61;
    slot_tbl_access[61] = QMetaData::Public;
    slot_tbl[62].name = "ignoreCase()";
    slot_tbl[62].ptr = (QMember)ov1_62;
    slot_tbl_access[62] = QMetaData::Public;
    slot_tbl[63].name = "ignoreBlankLines()";
    slot_tbl[63].ptr = (QMember)ov1_63;
    slot_tbl_access[63] = QMetaData::Public;
    slot_tbl[64].name = "hideCarriageReturns()";
    slot_tbl[64].ptr = (QMember)ov1_64;
    slot_tbl_access[64] = QMetaData::Public;
    slot_tbl[65].name = "dirDiffRecursive()";
    slot_tbl[65].ptr = (QMember)ov1_65;
    slot_tbl_access[65] = QMetaData::Public;
    slot_tbl[66].name = "ignoreFileChanges()";
    slot_tbl[66].ptr = (QMember)ov1_66;
    slot_tbl_access[66] = QMetaData::Public;
    slot_tbl[67].name = "qualityNormal()";
    slot_tbl[67].ptr = (QMember)ov1_67;
    slot_tbl_access[67] = QMetaData::Public;
    slot_tbl[68].name = "qualityFastest()";
    slot_tbl[68].ptr = (QMember)ov1_68;
    slot_tbl_access[68] = QMetaData::Public;
    slot_tbl[69].name = "qualityHighest()";
    slot_tbl[69].ptr = (QMember)ov1_69;
    slot_tbl_access[69] = QMetaData::Public;
    slot_tbl[70].name = "toggleShowFilenames()";
    slot_tbl[70].ptr = (QMember)ov1_70;
    slot_tbl_access[70] = QMetaData::Public;
    slot_tbl[71].name = "togglePaneMergedView()";
    slot_tbl[71].ptr = (QMember)ov1_71;
    slot_tbl_access[71] = QMetaData::Public;
    slot_tbl[72].name = "togglePopupMergedView()";
    slot_tbl[72].ptr = (QMember)ov1_72;
    slot_tbl_access[72] = QMetaData::Public;
    slot_tbl[73].name = "toggleToolbar()";
    slot_tbl[73].ptr = (QMember)ov1_73;
    slot_tbl_access[73] = QMetaData::Public;
    slot_tbl[74].name = "toggleLineNumbers()";
    slot_tbl[74].ptr = (QMember)ov1_74;
    slot_tbl_access[74] = QMetaData::Public;
    slot_tbl[75].name = "toggleVerticalLine()";
    slot_tbl[75].ptr = (QMember)ov1_75;
    slot_tbl_access[75] = QMetaData::Public;
    slot_tbl[76].name = "toggleOverview()";
    slot_tbl[76].ptr = (QMember)ov1_76;
    slot_tbl_access[76] = QMetaData::Public;
    slot_tbl[77].name = "hordiffTypeNone()";
    slot_tbl[77].ptr = (QMember)ov1_77;
    slot_tbl_access[77] = QMetaData::Public;
    slot_tbl[78].name = "hordiffTypeSingle()";
    slot_tbl[78].ptr = (QMember)ov1_78;
    slot_tbl_access[78] = QMetaData::Public;
    slot_tbl[79].name = "hordiffTypeMultiple()";
    slot_tbl[79].ptr = (QMember)ov1_79;
    slot_tbl_access[79] = QMetaData::Public;
    slot_tbl[80].name = "hordiffTypeUpdate()";
    slot_tbl[80].ptr = (QMember)ov1_80;
    slot_tbl_access[80] = QMetaData::Public;
    slot_tbl[81].name = "toggleIgnoreHorizontalWs()";
    slot_tbl[81].ptr = (QMember)ov1_81;
    slot_tbl_access[81] = QMetaData::Public;
    slot_tbl[82].name = "toggleIgnorePerHunkWs()";
    slot_tbl[82].ptr = (QMember)ov1_82;
    slot_tbl_access[82] = QMetaData::Public;
    slot_tbl[83].name = "toggleFormatClipboardText()";
    slot_tbl[83].ptr = (QMember)ov1_83;
    slot_tbl_access[83] = QMetaData::Public;
    slot_tbl[84].name = "ignoreFileNone()";
    slot_tbl[84].ptr = (QMember)ov1_84;
    slot_tbl_access[84] = QMetaData::Public;
    slot_tbl[85].name = "ignoreFileLeft()";
    slot_tbl[85].ptr = (QMember)ov1_85;
    slot_tbl_access[85] = QMetaData::Public;
    slot_tbl[86].name = "ignoreFileMiddle()";
    slot_tbl[86].ptr = (QMember)ov1_86;
    slot_tbl_access[86] = QMetaData::Public;
    slot_tbl[87].name = "ignoreFileRight()";
    slot_tbl[87].ptr = (QMember)ov1_87;
    slot_tbl_access[87] = QMetaData::Public;
    slot_tbl[88].name = "helpManPage()";
    slot_tbl[88].ptr = (QMember)ov1_88;
    slot_tbl_access[88] = QMetaData::Public;
    slot_tbl[89].name = "helpGenInitFile()";
    slot_tbl[89].ptr = (QMember)ov1_89;
    slot_tbl_access[89] = QMetaData::Public;
    slot_tbl[90].name = "helpAbout()";
    slot_tbl[90].ptr = (QMember)ov1_90;
    slot_tbl_access[90] = QMetaData::Public;
    slot_tbl[91].name = "pageDown()";
    slot_tbl[91].ptr = (QMember)ov1_91;
    slot_tbl_access[91] = QMetaData::Public;
    slot_tbl[92].name = "pageUp()";
    slot_tbl[92].ptr = (QMember)ov1_92;
    slot_tbl_access[92] = QMetaData::Public;
    slot_tbl[93].name = "cursorDown()";
    slot_tbl[93].ptr = (QMember)ov1_93;
    slot_tbl_access[93] = QMetaData::Public;
    slot_tbl[94].name = "cursorUp()";
    slot_tbl[94].ptr = (QMember)ov1_94;
    slot_tbl_access[94] = QMetaData::Public;
    slot_tbl[95].name = "cursorTop()";
    slot_tbl[95].ptr = (QMember)ov1_95;
    slot_tbl_access[95] = QMetaData::Public;
    slot_tbl[96].name = "cursorBottom()";
    slot_tbl[96].ptr = (QMember)ov1_96;
    slot_tbl_access[96] = QMetaData::Public;
    slot_tbl[97].name = "quitAccept()";
    slot_tbl[97].ptr = (QMember)ov1_97;
    slot_tbl_access[97] = QMetaData::Public;
    slot_tbl[98].name = "quitReject()";
    slot_tbl[98].ptr = (QMember)ov1_98;
    slot_tbl_access[98] = QMetaData::Public;
    slot_tbl[99].name = "quitMerged()";
    slot_tbl[99].ptr = (QMember)ov1_99;
    slot_tbl_access[99] = QMetaData::Public;
    typedef void (XxApp::*m2_t0)(int);
    typedef void (QObject::*om2_t0)(int);
    typedef void (XxApp::*m2_t1)();
    typedef void (QObject::*om2_t1)();
    m2_t0 v2_0 = &XxApp::cursorChanged;
    om2_t0 ov2_0 = (om2_t0)v2_0;
    m2_t1 v2_1 = &XxApp::textSizeChanged;
    om2_t1 ov2_1 = (om2_t1)v2_1;
    QMetaData *signal_tbl = QMetaObject::new_metadata(2);
    signal_tbl[0].name = "cursorChanged(int)";
    signal_tbl[0].ptr = (QMember)ov2_0;
    signal_tbl[1].name = "textSizeChanged()";
    signal_tbl[1].ptr = (QMember)ov2_1;
    metaObj = QMetaObject::new_metaobject(
	"XxApp", "QApplication",
	slot_tbl, 100,
	signal_tbl, 2,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}

// SIGNAL cursorChanged
void XxApp::cursorChanged( int t0 )
{
    activate_signal( "cursorChanged(int)", t0 );
}

// SIGNAL textSizeChanged
void XxApp::textSizeChanged()
{
    activate_signal( "textSizeChanged()" );
}
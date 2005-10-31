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

#ifndef INCL_XXDIFF_KDESUPPORT
#define INCL_XXDIFF_KDESUPPORT

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifdef XX_KDE

#include <kapplication.h>
#include <kmainwindow.h>
#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include <kcmdlineargs.h>
#include <kaccel.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kaboutapplication.h>
#include <ktextbrowser.h>
#include <kpushbutton.h>
#include <klineedit.h>
#include <klistbox.h>
#include <kfontdialog.h>
#include <kcolordialog.h>

typedef class KApplication     QkApplication;
typedef class KMainWindow      QkMainWindow;
typedef class KPopupMenu       QkPopupMenu;
typedef class KToolBar         QkToolBar;
typedef class KMenuBar         QkMenuBar;
typedef class KDialogBase      QkDialog;
typedef class KFileDialog      QkFileDialog;
typedef class KTextBrowser     QkTextBrowser;
typedef class KPushButton      QkPushButton;
typedef class KLineEdit        QkLineEdit;
typedef class KListBox         QkListBox;

#else

typedef class QApplication     QkApplication;
typedef class QMainWindow      QkMainWindow;
typedef class QPopupMenu       QkPopupMenu;
typedef class QToolBar         QkToolBar;
typedef class QMenuBar         QkMenuBar;
typedef class QDialog          QkDialog;
typedef class QFileDialog      QkFileDialog;
typedef class QTextBrowser     QkTextBrowser;
typedef class QPushButton      QkPushButton;
typedef class QLineEdit        QkLineEdit;
typedef class QListBox         QkListBox;

#endif

#endif

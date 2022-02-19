.. -*- coding: utf-8 -*-

====================================================================
  xxdiff: Graphical File And Directories Comparator And Merge Tool
====================================================================

.. contents:: Table of Contents
..
    1  Description
      1.1  Features
        1.1.1  Unicode and Inline Editing
    2  Documentation
      2.1  Screenshots
    3  Download
      3.1  Tools
      3.2  Packages
    4  Reporting Bugs
    5  Installation
      5.1  Dependencies
      5.2  Portability
    6  External Links
    7  Copyright and License
    8  Author


Description
===========

This is the official home of **xxdiff**, a graphical file and
directories comparator and merge tool.

**xxdiff** is provided under the GNU GPL open source license. It has
reached stable state, and is known to run on many popular unices,
including IRIX, Linux, Solaris, HP/UX, DEC Tru64.  It has been
deployed inside many large organizations and is being actively
maintained by its author (Martin Blais).


Features
--------

- Comparing **two** files, **three** files, or two **directories**
  (shallow and recursive);

- **Horizontal diffs** highlighting;

- Files can be **merged** interactively and resulting output
  visualized and saved;

- Has features to assist in performing **merge reviews/policing**;

- Can **unmerge CVS conflicts** in automatically merged file and
  display them as two files, to help resolve conflicts;

- Uses **external diff program** to compute differences: works with
  GNU diff, SGI diff and ClearCase's cleardiff, and any other diff
  whose output is similar to those;

- Fully customizable with a resource file;

- Look-and-feel similar to Rudy Wortel's/SGI xdiff, it is desktop
  agnostic (i.e. will work equally well with KDE or Gnome);

- Features and output that ease integration with scripts.

**xxdiff** was (and probably still is) tested daily in a merge
reviewing/policing context by more than 50 engineers at `discreet
<http://www.discreet.com>`_, and since this task is a
significant part of their development process, many of the features
are a direct result of suggestions from these people while the author
was working there.

Unicode and Inline Editing
~~~~~~~~~~~~~~~~~~~~~~~~~~

**xxdiff** does not support comparing files encoded with Unicode.
Also, it does not support inline editing of the diffed files.  If you
need these features, you could try emacs or Meld (although both of
these lack some of the most interesting features of xxdiff).


Documentation
=============

- `User's Manual
  <https://htmlpreview.github.io/?https://github.com/blais/xxdiff/blob/master/doc/xxdiff-doc.html>`_
  *(user's manual, all features, FAQ)*
- `The almost secret nifty features of xxdiff <doc/xxdiff-secrets.html>`_
- `Integrating xxdiff with scripts <doc/xxdiff-integration.html>`_
- `Helper Scripts for xxdiff <doc/xxdiff-scripts.html>`_
- `CHANGES <CHANGES>`_ *(news, recent changes, history)*
- `TODO <TODO>`_ *(future features, wishlist)*


Screenshots
-----------

- `Screenshots <doc/screenshots/gallery/index.html>`_


Download
========

* Source code can be found the public Git repository, here:

     http://github.com/blais/xxdiff

  There are no releases. Pick up the source for from Github.


* A complete Qt4 port (which works on the Mac) has been done by
  Alexandre Feblot (alexandre dot feblot at gmail dot com). This forms
  the basis for versions 4.x of xxdiff as of November 2010. Some notes
  about the Mac OS X build:

    On OSX, a Qt bug
    (http://bugreports.qt.nokia.com/browse/QTBUG-9271) affects
    rendering performance. As a workaround you can use the much faster
    "raster" graphics engine by either

    - using the ``--graphicssystem`` raster option
    - defining this environment variable in your terminal::

         QT_GRAPHICSSYSTEM=raster

    This engine has a couple of drawbacks (it will not refresh the
    display during window resizing, and will dump warning lines when
    opening native OSX dialogs like colors chooser or file selection
    dialogs), but it's worth coping with them anyway.


Tools
-----

**xxdiff** comes with a bunch of wrapper Python scripts that invoke xxdiff in
creative ways.  The author has been using these for a long time now and decided
for version 3.0 to clean them up and make them a little more generic so everyone
can use them.  As of version 3.2, the standalone scripts have been refactored a
great deal, to share code together and to make them more stable.  All this code
is now distributed with xxdiff, in an ``xxdiff`` Python package, which you can
leverage to build your own file transformation scripts around xxdiff.

See the document `Helper Scripts for xxdiff <doc/xxdiff-scripts.html>`_ for full
details.

The scripts are available in the xxdiff distribution.  To install them and the
accompanying Python package, use the standard distutils procedure::

   su -  # before root
   cd xxdiff-3.x
   python setup.py install

Other tools can be found in the distribution under the tools/ directory (CVS,
ClearCase, etc.).


Packages
--------

Packages are available for some specific operating systems. Here are links to
the relevant pages.  The author does not publish binaries anymore.

- `Linux / Debian package <http://www.sourcepole.ch/sources/software/xxdiff/>`_
  (provided by Tomas Pospisek)

- `Linux / Mandrake package (1) <http://www.geek.co.il/RPMS/xxdiff>`_
  (maintained by Oded Arbel)

- `Linux / Mandrake package (2) <http://www.roberto.foglietta.name/pub/xxdiff>`_
  (maintained by Roberto A. Foglietta)

- `Linux / Fedore Core (1) <http://timeoff.wsisiz.edu.pl/rpms.html#xxdiff>`_
  (maintained by Marcin Zajączkowski)

- `Linux / Gentoo port <http://packages.gentoo.org/ebuilds/?xxdiff-3.1>`_
  (in the dev-utils category)

- `OpenBSD / ports tree <http://www.openbsd.org/ports.html>`_ (maintained by Dan
  Weeks, available in the ports tree)

- `IRIX / Tardist package <http://freeware.sgi.com>`_ (provided by the
  SGI freeware team (version may be lagging behind))


Reporting Bugs
==============

* `Reporting Bugs <https://github.com/blais/xxdiff/issues>`_
  *(bugs and feature requests)*


Installation
============

Dependencies
------------

Build dependencies are:

- Qt-4.5 or higher
- a C++ compiler, flex and yacc

Runtime dependencies are:

- Qt-4.5 or higher


Portability
-----------

xxdiff is known to build and work on a great majority of UNIX platforms where Qt
works, including Mac OS X.


External Links
==============

- Other file comparison programs at `Google directory
  <http://directory.google.com/Top/Computers/Software/File_Management/File_Comparison/>`_,
  `DMOZ <http://dmoz.org/Computers/Software/File_Management/File_Comparison/>`_

- `Author's Homepage <http://furius.ca>`_


Copyright and License
=====================

Copyright (C) 2001-2004  Martin Blais.  All Rights Reserved.

This code is distributed under the `GNU General Public License <COPYING>`_;


Author
======

Martin Blais <blais@furius.ca>

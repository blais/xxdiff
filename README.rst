.. -*- coding: utf-8 -*-

================================================================================
  xxdiff-qt6: Updated Graphical File And Directories Comparator And Merge Tool
================================================================================

.. contents:: Table of Contents
..
    1  Description
    2  Changes to this version
    3  Reporting Bugs
    4  Installation
    5  Copyright and License
    6  Author


Description
===========

This is a fork of `xxdiff <http://github.com/blais/xxdiff>`_, a graphical file and
directories comparator and merge tool.  See there for full information about this
program, including documentation and more instructions.


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

* Source code can be found in the public GitHub repository, here:

     http://github.com/blais/xxdiff

  There are no releases. Pick up the source for from Github.


* A Qt6 port has been done by Lior Silberman. This is currently in the `qt6`
  branch and is intended to replace the `master` branch eventually.


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


Reporting Bugs
==============

* `Reporting Bugs <https://github.com/l2718/xxdiff-qt6/issues>`_
  *(bugs and feature requests)*


Installation
============

Dependencies
------------

Build dependencies are:

- Qt-6 or higher (possibly Qt-5 suffices, not tested)
- a C++ compiler, flex and yacc

Runtime dependencies are:

- Qt-6 or higher


Portability
-----------

The Qt6 version of xxdiff was only tested on Fedora GNU/Linux and Ubuntu Linux.


Copyright and License
=====================

xxdiff Copyright (C) 2001-2022  Martin Blais.  All Rights Reserved.

This code is distributed under the `GNU General Public License <COPYING>`_;


Authorship
==========

* Martin Blais <blais@furius.ca>
* Port to Qt6 by Lior Silberman <lior3.1415+xxdiff-qt6@gmail.com>

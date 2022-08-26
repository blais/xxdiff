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

**xxdiff-qt6**, like xxdiff, is provided under the GNU GPL free software license.
Unlike its parent it has only been tested on Fedora GNU/Linux.


Changes in this version
=======================

* auto_ptr replaced with unique_ptr and C++ standard switched to c++17
* Code updated to conform with Qt6 with no functionality changes.

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

xxdiff-qt6 was only tested on Fedora GNU/Linux; unmodified xxdiff is highly portable.


Copyright and License
=====================

xxdiff Copyright (C) 2001-2004  Martin Blais.  All Rights Reserved.

This code is distributed under the `GNU General Public License <COPYING>`_;


Authorship
==========

* The original author of the program is Martin Blais <blais@furius.ca>
* Modifications by Lior Silberman <lior3.1415+xxdiff-qt6@gmail.com>

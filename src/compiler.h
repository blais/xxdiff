/* -*- c-file-style: "xxdiff" -*- */
/******************************************************************************\
 * $RCSfile$
 *
 * Copyright (C) 1999-2004  Martin Blais <blais@furius.ca>
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

#ifndef INCL_XXDIFF_COMPILER
#define INCL_XXDIFF_COMPILER

// This file is highly inspired by the select_compiler_config.hpp file from
// Boost (http://boost.org).
//
// Locate which compiler we are using and define COMPILER as needed: 

# if defined __COMO__
//  Comeau C++
#   define COMPILER_COMEAU

#elif defined __DMC__
//  Digital Mars C++
#   define COMPILER_DIGITALMARS

#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)
//  Intel
#   define COMPILER_INTEL

# elif defined __GNUC__
//  GNU C++:
#   define COMPILER_GCC

#elif defined __KCC
//  Kai C++
#   define COMPILER_KAI

#elif defined __sgi
//  SGI MIPSpro C++
#   define COMPILER_MIPSPRO

#elif defined __DECCXX
//  Compaq Tru64 Unix cxx
#   define COMPILER_DECCXX

#elif defined __ghs
//  Greenhills C++
#   define COMPILER_GREENHILLS

#elif defined __BORLANDC__
//  Borland
#   define COMPILER_BORLAND

#elif defined  __MWERKS__
//  Metrowerks CodeWarrior
#   define COMPILER_METROWERKS

#elif defined  __SUNPRO_CC
//  Sun Workshop Compiler C++
#   define COMPILER_SUNWSPRO

#elif defined __HP_aCC
//  HP aCC
#   define COMPILER_ACC

#elif defined(__MRC__) || defined(__SC__)
//  MPW MrCpp or SCpp
#   define COMPILER_MPW

#elif defined(__IBMCPP__)
//  IBM Visual Age
#   define COMPILER_VACPP

#elif defined _MSC_VER
//  Microsoft Visual C++
//
//  Must remain the last #elif since some other vendors (Metrowerks, for
//  example) also #define _MSC_VER
#   define COMPILER_VISUALC

#elif defined _AIX
// IBM AIX xlc
#   define COMPILER_AIXXLC

#elif defined (ASSERT_CONFIG)
// generate an error if we don't recognise the compiler:
#  error "Unknown compiler - please configure in this file"

#endif

#endif


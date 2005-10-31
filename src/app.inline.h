/******************************************************************************\
 * $Id: app.inline.h 291 2001-10-20 22:15:00Z blais $
 * $Date: 2001-10-20 18:15:00 -0400 (Sat, 20 Oct 2001) $
 *
 * Copyright (C) 1999-2001  Martin Blais <blais@iro.umontreal.ca>
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
 *****************************************************************************/

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

#ifndef INCL_XXDIFF_DIFFS
#include <diffs.h>
#define INCL_XXDIFF_DIFFS
#endif

#ifndef INCL_XXDIFF_FILE
#include <buffer.h>
#define INCL_XXDIFF_FILE
#endif

#ifndef INCL_QT_QSCROLLBAR
#include <qscrollbar.h>
#define INCL_QT_QSCROLLBAR
#endif


XX_NAMESPACE_BEGIN


/*==============================================================================
 * CLASS XxApp
 *============================================================================*/

//------------------------------------------------------------------------------
//
inline uint XxApp::getNbFiles() const
{
   return _nbFiles;
}

//------------------------------------------------------------------------------
//
inline XxBuffer* XxApp::getBuffer( const XxFno no ) const
{
   if ( 0 <= no && no < _nbFiles ) {
      return _files[no].get();
   }
   return 0;
}

//------------------------------------------------------------------------------
//
inline const std::auto_ptr<XxBuffer>* XxApp::getBuffers() const
{
   return _files;
}

//------------------------------------------------------------------------------
//
inline uint XxApp::getMaxDigits() const
{
   uint nbDigits = 0;
   for ( XxFno ii = 0; ii < _nbFiles; ++ii ) {
      nbDigits = std::max( nbDigits, _files[ii]->getNbDigits() );
   }
   return nbDigits;
}

//------------------------------------------------------------------------------
//
inline XxDiffs* XxApp::getDiffs() const
{
   return _diffs.get();
}

//------------------------------------------------------------------------------
//
inline XxDln XxApp::getNbLines() const
{
   if ( _diffs.get() == 0 ) {
      return 1;
   }
   return _diffs->getNbLines();
}

//------------------------------------------------------------------------------
//
inline const XxResources& XxApp::getResources() const
{
   return *_resources;
}

//------------------------------------------------------------------------------
//
inline XxResources& XxApp::getResourcesNC()
{
   return *_resources;
}

//------------------------------------------------------------------------------
//
inline XxDln XxApp::getNbDisplayLines() const
{
   return _displayLines;
}

//------------------------------------------------------------------------------
//
inline XxDln XxApp::getTopLine() const
{
   if ( _diffs.get() == 0 ) {
      return 0;
   }
   return _vscroll[0]->value() + 1;
}

//------------------------------------------------------------------------------
//
inline XxDln XxApp::getCenterLine() const
{
   if ( _diffs.get() == 0 ) {
      return 0;
   }
   return std::min( _vscroll[0]->value() + 1 + getNbDisplayLines() / 2, 
                    int(_diffs->getNbLines()) );
}

//------------------------------------------------------------------------------
//
inline XxDln XxApp::getBottomLine() const
{
   if ( _diffs.get() == 0 ) {
      return 0;
   }
   return std::min( 
      getTopLine() /* top line */
      + (getNbDisplayLines() - 1) /* to bottom line */
      - 1, /* don't allow cursor on half-displayed line */
      int(_diffs->getNbLines())
   );
}

//------------------------------------------------------------------------------
//
inline XxDln XxApp::getCursorLine() const
{
   if ( _diffs.get() == 0 ) {
      return 0;
   }
   return _cursorLine;
}

XX_NAMESPACE_END

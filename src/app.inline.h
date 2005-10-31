/******************************************************************************\
 * $Id: app.inline.h 2 2000-09-15 02:19:22Z blais $
 * $Date: 2000-09-14 22:19:22 -0400 (Thu, 14 Sep 2000) $
 *
 * Copyright (C) 1999, 2000  Martin Blais <blais@iro.umontreal.ca>
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
inline XxBuffer* XxApp::getFile( const int no ) const
{
   if ( 0 <= no && no < _nbFiles ) {
      return _files[no].get();
   }
   return 0;
}

//------------------------------------------------------------------------------
//
inline const std::auto_ptr<XxBuffer>* XxApp::getFiles() const
{
   return _files;
}

//------------------------------------------------------------------------------
//
inline uint XxApp::getMaxDigits() const
{
   uint nbDigits = 0;
   for ( int ii = 0; ii < _nbFiles; ++ii ) {
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
inline uint XxApp::getNbLines() const
{
   if ( _diffs.get() == 0 ) {
      return 1;
   }
   return _diffs->getNbLines();
}

//------------------------------------------------------------------------------
//
inline XxResources* XxApp::getResourcesNC()
{
   return _resources;
}

//------------------------------------------------------------------------------
//
inline const QFont& XxApp::getFont() const
{
   return _font;
}

//------------------------------------------------------------------------------
//
inline uint XxApp::getNbDisplayLines() const
{
   return _displayLines;
}

//------------------------------------------------------------------------------
//
inline uint XxApp::getTopLine() const
{
   if ( _diffs.get() == 0 ) {
      return 0;
   }
   return _vscroll[0]->value() + 1;
}

//------------------------------------------------------------------------------
//
inline uint XxApp::getCenterLine() const
{
   if ( _diffs.get() == 0 ) {
      return 0;
   }
   return std::min( _vscroll[0]->value() + 1 + getNbDisplayLines() / 2, 
                    _diffs->getNbLines() );
}

//------------------------------------------------------------------------------
//
inline uint XxApp::getBottomLine() const
{
   if ( _diffs.get() == 0 ) {
      return 0;
   }
   return std::min( 
      getTopLine() /* top line */
      + (getNbDisplayLines() - 1) /* to bottom line */
      - 1, /* don't allow cursor on half-displayed line */
      _diffs->getNbLines() 
   );
}

//------------------------------------------------------------------------------
//
inline uint XxApp::getCursorLine() const
{
   if ( _diffs.get() == 0 ) {
      return 0;
   }
   return _cursorLine;
}

XX_NAMESPACE_END

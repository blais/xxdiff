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

/*==============================================================================
 * EXTERNAL DECLARATIONS
 *============================================================================*/

XX_NAMESPACE_BEGIN

/*==============================================================================
 * CLASS XxResources
 *============================================================================*/

//------------------------------------------------------------------------------
//
inline const QRect& XxResources::getDefaultGeometry()
{
   return _defaultGeometry;
}

//------------------------------------------------------------------------------
//
inline const QRect& XxResources::getPreferredGeometry() const
{
   return _preferredGeometry;
}

//------------------------------------------------------------------------------
//
inline const QString& XxResources::getStyleKey() const
{
   return _styleKey;
}

//------------------------------------------------------------------------------
//
inline bool XxResources::getMaximize() const
{
   return _maximize;
}

//------------------------------------------------------------------------------
//
inline int XxResources::getAccelerator( XxAccel accel ) const
{
   return _accelerators[ int(accel) ];
}

//------------------------------------------------------------------------------
//
inline const QFont& XxResources::getFontApp() const
{
   return _fontApp;
}

//------------------------------------------------------------------------------
//
inline const QFont& XxResources::getFontText() const
{
   return _fontText;
}

//------------------------------------------------------------------------------
//
inline XxIgnoreFile XxResources::getIgnoreFile() const
{
   return _ignoreFile;
}

//------------------------------------------------------------------------------
//
inline const QString& XxResources::getCommand( XxCommand cmdId ) const
{
   return _commands[ int(cmdId) ];
}

//------------------------------------------------------------------------------
//
inline const QString& XxResources::getCommandSwitch(
   XxCommandSwitch cmdId
) const
{
   return _commandSwitch[ int(cmdId) ];
}

//------------------------------------------------------------------------------
//
inline int XxResources::getInitSwitch(
   XxCommandSwitch cmdId
) const
{
   return _initSwitch[ int(cmdId) ];
}

//------------------------------------------------------------------------------
//
inline const QString& XxResources::getTag( XxTag tagId ) const
{
   return _tags[ int(tagId) ];
}

//------------------------------------------------------------------------------
//
inline void XxResources::getRegionColor(
   const XxColor colorType,
   QColor&       background,
   QColor&       foreground
) const
{
   int i = int(colorType);
   background = _backColors[ i ];
   foreground = _foreColors[ i ];
}

//------------------------------------------------------------------------------
//
inline const QColor& XxResources::getColor(
   const XxColor colorType,
   const bool    fore
) const
{
   if ( fore ) {
      return _foreColors[ int(colorType) ];
   }
   else {
      return _backColors[ int(colorType) ];
   }
}

//------------------------------------------------------------------------------
//
inline bool XxResources::getBoolOpt( const XxBoolOpt opt ) const
{
   XX_ASSERT( 0 <= opt && opt < BOOL_LAST );
   return _boolOpts[ int(opt) ];
}

//------------------------------------------------------------------------------
//
inline bool XxResources::getShowOpt( const XxShowOpt opt ) const
{
   return _showOpts[ int(opt) ];
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getTabWidth() const
{
   return _tabWidth;
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getOverviewFileWidth() const
{
   return _overviewFileWidth;
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getOverviewSepWidth() const
{
   return _overviewSepWidth;
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getVerticalLinePos() const
{
   return _verticalLinePos;
}

//------------------------------------------------------------------------------
//
inline const QString& XxResources::getClipboardHeadFormat() const
{
   return _clipboardHeadFormat;
}

//------------------------------------------------------------------------------
//
inline const QString& XxResources::getClipboardLineFormat() const
{
   return _clipboardLineFormat;
}

//------------------------------------------------------------------------------
//
inline XxHordiff XxResources::getHordiffType() const
{
   return _hordiffType;
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getHordiffMax() const
{
   return _hordiffMax;
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getHordiffContext() const
{
   return _hordiffContext;
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getShowPaneMergedViewPercent() const
{
   return _showPaneMergedViewPercent;
}

//------------------------------------------------------------------------------
//
inline const QString& XxResources::getMergedFilename() const
{
   return _mergedFilename;
}

//------------------------------------------------------------------------------
//
inline void XxResources::setFbColors(
   XxColor     color,
   const char* backstr,
   const char* forestr
)
{
   _backColors[ color ].setNamedColor( backstr );
   _foreColors[ color ].setNamedColor( forestr );
   // Note: no need to emit changed(), this is a convenience method for the
   // inside.
}

//------------------------------------------------------------------------------
//
inline int* XxResources::getDynProgTable( const uint htx, const uint hty ) const
{
   if ( ( htx * hty ) > _hdTableSize ) {
      // We won't start messing with large diffs, allow a maximum table size
      // larger than a constant number.
      if ( (htx * hty) > _hordiffMax ) {
         return 0;
      }
      _hdTableSize = htx * hty;
      _hdTable = (int*) realloc( _hdTable, _hdTableSize * sizeof(int) );
   }
   return _hdTable;
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getHordiffBuffers(
   int*&  hbuffer0, 
   int*&  hbuffer1
) const
{
   hbuffer0 = _hdBuffer0;
   hbuffer1 = _hdBuffer1;
   return _hdBufferSize;
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getHordiffLineBuffers(
   char*&  hdlinebuf0, 
   char*&  hdlinebuf1
) const
{
   hdlinebuf0 = _hdLineBuf0;
   hdlinebuf1 = _hdLineBuf1;
   return _hdLineBufSize;
}

XX_NAMESPACE_END


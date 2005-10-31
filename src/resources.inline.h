/******************************************************************************\
 * $Id: resources.inline.h 295 2001-10-21 18:39:14Z blais $
 * $Date: 2001-10-21 14:39:14 -0400 (Sun, 21 Oct 2001) $
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


XX_NAMESPACE_BEGIN


/*==============================================================================
 * CLASS XxResources
 *============================================================================*/

//------------------------------------------------------------------------------
//
inline const QRect& XxResources::getPreferredGeometry() const
{
   return _preferredGeometry;
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
inline const QString& XxResources::getClipboardFormat() const
{
   return _clipboardFormat;
}

//------------------------------------------------------------------------------
//
inline XxIgnoreFile XxResources::getIgnoreFile() const
{
   return _ignoreFile;
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

XX_NAMESPACE_END


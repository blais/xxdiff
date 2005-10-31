/******************************************************************************\
 * $Id: resources.inline.h 161 2001-05-28 14:47:52Z blais $
 * $Date: 2001-05-28 10:47:52 -0400 (Mon, 28 May 2001) $
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
inline int XxResources::getAccelerator( Resource resource ) const
{
   int iaccel = int( resource ) - int( ACCEL_FIRST );
   XX_CHECK( iaccel < int(ACCEL_LAST) - int(ACCEL_FIRST) );
   return _accelerators[ iaccel ];
}

//------------------------------------------------------------------------------
//
inline const QFont& XxResources::getAppFont() const
{
   return _fontApp;
}

//------------------------------------------------------------------------------
//
inline const QFont& XxResources::getTextFont() const
{
   return _fontText;
}

//------------------------------------------------------------------------------
//
inline void XxResources::getRegionColor(
   Resource type,
   QColor&  background,
   QColor&  foreground
) const
{
   int i = int(type) - int(COLOR_FIRST);
   XX_CHECK( i < int(COLOR_LAST) - int(COLOR_FIRST) );
   XX_CHECK( i < int(COLOR_BACKGROUND) );
   // XX_CHECK( ( i - int(COLOR_FIRST) ) % 2 == 0 );
   background = _colors[ i ];
   foreground = _colors[ i + 1 ];
}

//------------------------------------------------------------------------------
//
inline const QColor& XxResources::getColor( const Resource resource ) const
{
   int i = int(resource) - int(COLOR_FIRST);
   XX_CHECK( i < int(COLOR_LAST) - int(COLOR_FIRST) );
   return _colors[ i ];
}

//------------------------------------------------------------------------------
//
inline bool XxResources::getBoolOpt( const Resource resource ) const
{
   int ibool = int( resource ) - int( BOOL_FIRST );
   XX_CHECK( ibool < int(BOOL_LAST) - int(BOOL_FIRST) );
   return _boolOpts[ ibool ];
}

//------------------------------------------------------------------------------
//
inline void XxResources::setBoolOpt(
   const Resource resource,
   const bool option
)
{
   int ibool = int( resource ) - int( BOOL_FIRST );
   XX_CHECK( ibool < int(BOOL_LAST) - int(BOOL_FIRST) );
   _boolOpts[ ibool ] = option;
}

//------------------------------------------------------------------------------
//
inline bool XxResources::toggleBoolOpt( const Resource resource )
{
   int ibool = int( resource ) - int( BOOL_FIRST );
   XX_CHECK( ibool < int(BOOL_LAST) - int(BOOL_FIRST) );
   _boolOpts[ ibool ] = ! _boolOpts[ ibool ];
   return _boolOpts[ ibool ];
}

//------------------------------------------------------------------------------
//
inline uint XxResources::getTabWidth() const
{
   return _tabWidth;
}

//------------------------------------------------------------------------------
//
inline void XxResources::setTabWidth( uint tabWidth )
{
   _tabWidth = tabWidth;
}

//------------------------------------------------------------------------------
//
inline void XxResources::setVerticalLinePos( uint vlinePos )
{
   _verticalLinePos = vlinePos;
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

XX_NAMESPACE_END

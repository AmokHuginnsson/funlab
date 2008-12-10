/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrenderer.cxx - this file is integral part of `funlab' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "hrenderer.hxx"

using namespace yaal;

namespace funlab
{

void HRendererEngineInterface::draw_frame( void )
	{
	do_draw_frame();
	}

double HRendererSurfaceInterface::get_width( void ) const
	{
	return ( do_get_width() );
	}

double HRendererSurfaceInterface::get_height( void ) const
	{
	return ( do_get_height() );
	}

void HRendererSurfaceInterface::clear( u32_t c )
	{
	do_clear( c );
	}

void HRendererSurfaceInterface::do_clear( u32_t c )
	{
	fill_rect( 0, 0, get_width(), get_height(), c );
	}

void HRendererSurfaceInterface::commit( void )
	{
	do_commit();
	}

void HRendererSurfaceInterface::put_pixel( double x, double y, u32_t c )
	{
	do_put_pixel( x, y, c );
	}

void HRendererSurfaceInterface::line( double x1, double y1, double x2, double y2, u32_t c )
	{
	do_line( x1, y1, x2, y2, c );
	}

void HRendererSurfaceInterface::fill_rect( double x, double y, double w, double h, u32_t c )
	{
	do_fill_rect( x, y, w, h, c );
	}

void HRendererSurfaceBase::set_engine( HRendererEngineInterface::ptr_t engine )
	{
	M_PROLOG
	f_oEngine = engine;
	return;
	M_EPILOG
	}

HRendererEngineInterface::ptr_t HRendererSurfaceInterface::get_engine( void )
	{
	M_PROLOG
	return ( do_get_engine() );
	M_EPILOG
	}

HRendererEngineInterface::ptr_t HRendererSurfaceBase::do_get_engine( void )
	{
	M_PROLOG
	return ( f_oEngine );
	M_EPILOG
	}

yaal::u32_t HRendererSurfaceInterface::RGB( int r, int g, int b )
	{
	return ( do_RGB( static_cast<u8_t>( r ), static_cast<u8_t>( g ), static_cast<u8_t>( b ) ) );
	}

}


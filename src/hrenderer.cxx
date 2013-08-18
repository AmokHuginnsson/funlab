/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrenderer.cxx - this file is integral part of `funlab' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <yaal/yaal.hxx>
M_VCSID( "$Id: " __ID__ " $" )
#include "hrenderer.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace funlab {

void HRendererEngineInterface::draw_frame( void ) {
	do_draw_frame();
}

double HRendererSurfaceInterface::get_width( void ) const {
	return ( do_get_width() );
}

double HRendererSurfaceInterface::get_height( void ) const {
	return ( do_get_height() );
}

void HRendererSurfaceInterface::clear( u32_t c ) {
	do_clear( c );
}

void HRendererSurfaceInterface::do_clear( u32_t c ) {
	fill_rect( 0, 0, get_width(), get_height(), c );
}

void HRendererSurfaceInterface::commit( void ) {
	do_commit();
}

void HRendererSurfaceInterface::put_pixel( double x, double y, u32_t c ) {
	do_put_pixel( x, y, c );
}

void HRendererSurfaceInterface::line( double x1, double y1, double x2, double y2, u32_t c ) {
	do_line( x1, y1, x2, y2, c );
}

void HRendererSurfaceInterface::fill_rect( double x, double y, double w, double h, u32_t c ) {
	do_fill_rect( x, y, w, h, c );
}

void HRendererSurfaceBase::set_engine( HRendererEngineInterface::ptr_t engine ) {
	M_PROLOG
	_engine = engine;
	return;
	M_EPILOG
}

HRendererEngineInterface::ptr_t HRendererSurfaceInterface::get_engine( void ) {
	M_PROLOG
	return ( do_get_engine() );
	M_EPILOG
}

HRendererEngineInterface::ptr_t HRendererSurfaceBase::do_get_engine( void ) {
	M_PROLOG
	return ( _engine );
	M_EPILOG
}

yaal::u32_t HRendererSurfaceInterface::RGB( int r, int g, int b ) {
	return ( do_RGB( static_cast<u8_t>( r ), static_cast<u8_t>( g ), static_cast<u8_t>( b ) ) );
}

HRendererSurfaceBase::HRendererSurfaceBase( void )
	: _engine()
	{}

}


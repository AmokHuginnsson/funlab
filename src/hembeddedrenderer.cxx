/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hembeddedrenderer.cxx - this file is integral part of `funlab' project.

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

#include <iostream>
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#include <gtkmm.h>
#pragma GCC diagnostic error "-Wnon-virtual-dtor"
#pragma GCC diagnostic error "-Wredundant-decls"
#pragma GCC diagnostic error "-Weffc++"
#pragma GCC diagnostic error "-Wold-style-cast"

#include <yaal/hcore/base.hxx>
M_VCSID( "$Id: " __ID__ " $" )
#include "hembeddedrenderer.hxx"
#include "setup.hxx"
#include "hfunlab.hxx"

using namespace std;
using namespace yaal;

namespace funlab {

HEmbeddedRenderer::HEmbeddedRenderer( BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& )
	: Gtk::DrawingArea( obj ), _move(), _lineBuffer(), _context() {
}

HEmbeddedRenderer::~HEmbeddedRenderer( void ) {
}

bool HEmbeddedRenderer::on_expose_event( GdkEventExpose* event_ ) {
	Glib::RefPtr<Gdk::Window> window = get_window();
	if ( window ) {
		_context = window->create_cairo_context();
		if ( event_ ) {
			_context->rectangle( event_->area.x, event_->area.y,
					event_->area.width, event_->area.height );
			_context->clip();
		}
		Gtk::Allocation allocation = get_allocation();

		_context->scale( allocation.get_width() / static_cast<double>( setup._resolutionX ),
				allocation.get_height() / static_cast<double>( setup._resolutionY ) );
//		_context->set_antialias( Cairo::ANTIALIAS_NONE );
//		_context->set_line_cap( Cairo::LINE_CAP_ROUND );
		_engine->draw_frame();
	}
	return ( true );
}

double HEmbeddedRenderer::do_get_width( void ) const {
	return ( setup._resolutionX );
}

double HEmbeddedRenderer::do_get_height( void ) const {
	return ( setup._resolutionY );
}

void HEmbeddedRenderer::do_put_pixel( double, double, yaal::u32_t ) {
}

void HEmbeddedRenderer::do_line( double x1, double y1, double x2, double y2, yaal::u32_t c ) {
	if ( ( c != _lineBuffer._lastColor ) && ( ! _lineBuffer._empty ) )
		stroke_line_buffer();
	_context->move_to( x1, y1 );
	_context->line_to( x2, y2 );
	_lineBuffer._empty = false;
	_lineBuffer._lastColor = c;
}

void HEmbeddedRenderer::do_fill_rect( double x, double y, double w, double h, yaal::u32_t c ) {
	if ( ! _lineBuffer._empty )
		stroke_line_buffer();
	_context->set_source_rgba( red( c ), green( c ), blue( c ), alpha( c ) );
	_context->rectangle( x, y, w, h );
	_context->fill();
}

void HEmbeddedRenderer::do_clear( yaal::u32_t c ) {
	if ( ! _lineBuffer._empty )
		stroke_line_buffer();
	_context->set_source_rgba( red( c ), green( c ), blue( c ), alpha( c ) );
	_context->set_operator( Cairo::OPERATOR_SOURCE );
	_context->paint();
	if ( setup._stereo ) {
		_context->set_line_width( 0.8 );
		_context->set_operator( static_cast<Cairo::Operator>( CAIRO_OPERATOR_MULTIPLY ) );
	} else
		_context->set_line_width( 0.5 );
}

void HEmbeddedRenderer::do_commit( void ) {
	if ( ! _lineBuffer._empty )
		stroke_line_buffer();
}

yaal::u32_t HEmbeddedRenderer::do_RGB( u8_t red_, u8_t green_, u8_t blue_ ) {
	yaal::u8_t c[] = { 255, blue_, green_, red_ };
	return ( *reinterpret_cast<yaal::u32_t const*>( c ) );
}

double HEmbeddedRenderer::red( yaal::u32_t color ) {
	double nominator = color >> 24;
	double debominator = 0xff;
	return ( nominator / debominator );
}

double HEmbeddedRenderer::green( yaal::u32_t color ) {
	double nominator = ( color >> 16 ) & 0xff;
	double debominator = 0xff;
	return ( nominator / debominator );
}

double HEmbeddedRenderer::blue( yaal::u32_t color ) {
	double nominator = ( color >> 8 ) & 0xff;
	double debominator = 0xff;
	return ( nominator / debominator );
}

double HEmbeddedRenderer::alpha( yaal::u32_t color ) {
	double nominator = color & 0xff;
	double debominator = 0xff;
	return ( nominator / debominator );
}

bool HEmbeddedRenderer::on_scroll_event( GdkEventScroll* ev ) {
	bool skip = false;
	HMouseEvent e( HMouseEvent::TYPE::PRESS );
	switch ( ev->direction ) {
		case ( GDK_SCROLL_UP ):
			e.set_button( HMouseEvent::BUTTON::B_4 );
		break;
		case ( GDK_SCROLL_DOWN ):
			e.set_button( HMouseEvent::BUTTON::B_5 );
		break;
		default:
			skip = true;
		break;
	}
	if ( ! skip ) {
		_engine->on_event( &e );
		invoke_refresh( false );
	}
	return ( true );
}

bool HEmbeddedRenderer::on_motion_notify_event( GdkEventMotion* ev ) {
	int nx = static_cast<int>( ev->x );
	int ny = static_cast<int>( ev->y );
	int dx = yaal::abs( nx - _move._x );
	int dy = yaal::abs( ny - _move._y );

	if ( ( dx < ( setup._resolutionX >> 1 ) ) && ( dy < ( setup._resolutionY >> 1 ) ) ) {
		HMouseEvent e( HMouseEvent::TYPE::MOVE );
		e.set_pos( nx - _move._x, ny - _move._y );
		bool skip = false;
		switch ( ev->state ) {
			case ( GDK_BUTTON1_MASK ):
				e.set_button( HMouseEvent::BUTTON::B_1 );
			break;
			case ( GDK_BUTTON2_MASK ):
				e.set_button( HMouseEvent::BUTTON::B_2 );
			break;
			case ( GDK_BUTTON3_MASK ):
				e.set_button( HMouseEvent::BUTTON::B_3 );
			break;
			case ( GDK_BUTTON1_MASK | GDK_BUTTON3_MASK ):
				e.set_button( static_cast<HMouseEvent::BUTTON::button_t>( HMouseEvent::BUTTON::B_1 | HMouseEvent::BUTTON::B_3 ) );
			break;
			default:
				skip = true;
			break;
		}
		if ( ! skip ) {
			_engine->on_event( &e );
			invoke_refresh( false );
		}
	}
	_move._x = nx;
	_move._y = ny;
	return ( true );
}

bool HEmbeddedRenderer::on_button_press_event( GdkEventButton* ev ) {
	_move._x = static_cast<int>( ev->x );
	_move._y = static_cast<int>( ev->y );
	return ( false );
}

void HEmbeddedRenderer::invoke_refresh( bool full ) {
	if ( full ) {
		HFunlab* f = dynamic_cast<HFunlab*>( &*_engine );
		M_ASSERT( f );
		f->regen_cache( setup._density );
	}
  Glib::RefPtr<Gdk::Window> win = get_window();
	if ( win ) {
		Gdk::Rectangle r( 0, 0, get_allocation().get_width(),
				get_allocation().get_height() );
		win->invalidate_rect( r, false );
	}
}

void HEmbeddedRenderer::stroke_line_buffer( void ) {
	_context->set_source_rgba(
			red( _lineBuffer._lastColor ),
			green( _lineBuffer._lastColor ),
			blue( _lineBuffer._lastColor ),
			alpha( _lineBuffer._lastColor ) );
	_context->stroke();
	_lineBuffer._empty = true;
}

}


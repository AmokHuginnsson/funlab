/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hembeddedrenderer.hxx - this file is integral part of `funlab' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef FUNLAB_HEMBEDDEDRENDERER_HXX_INCLUDED
#define FUNLAB_HEMBEDDEDRENDERER_HXX_INCLUDED

#include <gtkmm.h>
#include <yaal/hcore/base.hxx>

#include "hrenderer.hxx"

namespace funlab {

class HEmbeddedRenderer : public HRendererSurfaceBase, public Gtk::DrawingArea {
	struct OMove {
		OMove( void ) : _x( 0 ), _y( 0 ) {}
		int _x;
		int _y;
	} _move;
	struct OLineBuffer {
		bool _empty;
		yaal::u32_t _lastColor;
		OLineBuffer( void ) : _empty( true ), _lastColor( 0 ) {}
	};
	OLineBuffer _lineBuffer;
	Cairo::RefPtr<Cairo::Context> _context;
public:
	HEmbeddedRenderer( BaseObjectType*, Glib::RefPtr<Gtk::Builder> const& );
	virtual ~HEmbeddedRenderer( void );
	void invoke_refresh( bool );
protected:
	virtual bool on_expose_event( GdkEventExpose* );
	virtual bool on_button_press_event( GdkEventButton* );
	virtual bool on_scroll_event( GdkEventScroll* );
	virtual bool on_motion_notify_event( GdkEventMotion* );
	virtual double do_get_width( void ) const;
	virtual double do_get_height( void ) const;
	virtual void do_commit( void );
	virtual void do_put_pixel( double, double, yaal::u32_t );
	virtual void do_line( double, double, double, double, yaal::u32_t );
	virtual void do_fill_rect( double, double, double, double, yaal::u32_t );
	virtual void do_clear( yaal::u32_t );
	virtual yaal::u32_t do_RGB( yaal::u8_t, yaal::u8_t, yaal::u8_t );
private:
	void stroke_line_buffer( void );
	double red( yaal::u32_t );
	double green( yaal::u32_t );
	double blue( yaal::u32_t );
	double alpha( yaal::u32_t );
};

}

#endif /* not FUNLAB_HEMBEDDEDRENDERER_HXX_INCLUDED */


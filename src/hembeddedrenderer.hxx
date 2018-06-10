/* Read booker/LICENSE.md file for copyright and licensing information. */

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
	virtual bool on_draw( Cairo::RefPtr<Cairo::Context> const& );
	virtual bool on_button_press_event( GdkEventButton* );
	virtual bool on_scroll_event( GdkEventScroll* );
	virtual bool on_motion_notify_event( GdkEventMotion* );
	virtual double do_get_width( void ) const;
	virtual double do_get_height( void ) const;
	virtual void do_commit( void );
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


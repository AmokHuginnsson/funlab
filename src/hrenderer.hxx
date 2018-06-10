/* Read booker/LICENSE.md file for copyright and licensing information. */

#ifndef FUNLAB_HRENDERER_HXX_INCLUDED
#define FUNLAB_HRENDERER_HXX_INCLUDED

#include <yaal/hcore/hpointer.hxx>

#include "events.hxx"

namespace funlab {

class HRendererEngineInterface : public HKeyboardEventListener, public HMouseEventListener {
public:
	using HKeyboardEventListener::on_event;
	using HMouseEventListener::on_event;
	void draw_frame( void );
	typedef yaal::hcore::HPointer<HRendererEngineInterface> ptr_t;
protected:
	virtual void do_draw_frame( void ) = 0;
};

class HRendererSurfaceInterface {
public:
	typedef yaal::hcore::HPointer<HRendererSurfaceInterface> ptr_t;
	virtual ~HRendererSurfaceInterface( void ) {}
	double get_width( void ) const;
	double get_height( void ) const;
	void clear( yaal::u32_t );
	void commit( void );
	yaal::u32_t RGB( int, int, int );
	void line( double, double, double, double, yaal::u32_t );
	void fill_rect( double, double, double, double, yaal::u32_t );
	HRendererEngineInterface::ptr_t get_engine( void );
protected:
	virtual HRendererEngineInterface::ptr_t do_get_engine( void ) = 0;
	virtual double do_get_width( void ) const = 0;
	virtual double do_get_height( void ) const = 0;
	virtual void do_commit( void ) = 0;
	virtual yaal::u32_t do_RGB( yaal::u8_t, yaal::u8_t, yaal::u8_t ) = 0;
	virtual void do_line( double, double, double, double, yaal::u32_t ) = 0;
	virtual void do_fill_rect( double, double, double, double, yaal::u32_t ) = 0;
	virtual void do_clear( yaal::u32_t );
};

class HRendererSurfaceBase : public HRendererSurfaceInterface {
protected:
	HRendererEngineInterface::ptr_t _engine;
public:
	HRendererSurfaceBase( void );
	virtual ~HRendererSurfaceBase( void ) {}
	void set_engine( HRendererEngineInterface::ptr_t );
protected:
	virtual HRendererEngineInterface::ptr_t do_get_engine( void );
};

}

#endif /* not FUNLAB_HRENDERER_HXX_INCLUDED */


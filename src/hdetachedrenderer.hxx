/* Read booker/LICENSE.md file for copyright and licensing information. */

#ifndef FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED
#define FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED

#include <yaal/hcore/hthread.hxx>

#include "hrenderer.hxx"
#include "events.hxx"

namespace funlab {

class HDetachedRenderer;

class HDetachedRenderer : public HRendererSurfaceBase {
protected:
	/*{*/
	bool _loop;
	void* _handler;
	int	_width;
	int _height;
	int _bPP;
	yaal::hcore::HSemaphore _semaphore;
	yaal::hcore::HThread _thread;
	HKeyboardEventListener* _keyboardEventListener;
	static int _activeSurfaces;
	/*}*/
public:
	/*{*/
	HDetachedRenderer( HKeyboardEventListener* = NULL );
	virtual ~HDetachedRenderer ( void );
	bool render_surface( void );
	void run( void );
	void shutdown( void );
	/*}*/
private:
	int init( int, int, int = 32 );
	void down( void );
	void toggle_fullscreen( void );
protected:
	/*{*/
	virtual double do_get_width( void ) const;
	virtual double do_get_height( void ) const;
	virtual void do_commit( void );
	virtual void do_put_pixel( double, double, yaal::u32_t );
	virtual yaal::u32_t do_get_pixel( double, double );
	virtual void do_line( double, double, double, double, yaal::u32_t );
	virtual void do_fill_rect( double, double, double, double, yaal::u32_t );
	virtual yaal::u32_t do_RGB( yaal::u8_t, yaal::u8_t, yaal::u8_t );
	/*}*/
private:
	HDetachedRenderer( const HDetachedRenderer& );
	HDetachedRenderer& operator = ( const HDetachedRenderer& );
};

}

#endif /* not FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED */


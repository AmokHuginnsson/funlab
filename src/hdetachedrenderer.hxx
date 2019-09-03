/* Read booker/LICENSE.md file for copyright and licensing information. */

#ifndef FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED
#define FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED

#include <yaal/hcore/hthread.hxx>

#include "hrenderer.hxx"
#include "events.hxx"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace funlab {

class HDetachedRenderer;

class HDetachedRenderer : public HRendererSurfaceBase {
public:
	typedef yaal::hcore::HResource<SDL_Window, void (*)(SDL_Window*)> sdl_window_t;
	typedef yaal::hcore::HResource<SDL_Renderer, void (*)(SDL_Renderer*)> sdl_renderer_t;
protected:
	/*{*/
	bool _loop;
	bool _fullscreen;
	sdl_window_t _window;
	sdl_renderer_t _renderer;
	int	_width;
	int _height;
	yaal::hcore::HSemaphore _semaphore;
	yaal::hcore::HThread _thread;
	HKeyboardEventListener* _keyboardEventListener;
	static int _activeSurfaces;
	/*}*/
public:
	/*{*/
	HDetachedRenderer( HKeyboardEventListener* = nullptr );
	virtual ~HDetachedRenderer ( void );
	bool render_surface( void );
	void run( void );
	void shutdown( void );
	/*}*/
private:
	int init( int, int );
	void down( void );
	void toggle_fullscreen( void );
protected:
	/*{*/
	virtual double do_get_width( void ) const;
	virtual double do_get_height( void ) const;
	virtual void do_commit( void );
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


/* Read booker/LICENSE.md file for copyright and licensing information. */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libintl.h>
#include <SDL2/SDL.h>

#include <yaal/hcore/hlog.hxx>
M_VCSID( "$Id: " __ID__ " $" )
#include "hdetachedrenderer.hxx"
#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace funlab {

inline constexpr u8_t red( u32_t color_ ) {
	return ( static_cast<u8_t>( color_ >> 24 ) );
}

inline constexpr u8_t green( u32_t color_ ) {
	return ( static_cast<u8_t>( color_ >> 16 ) );
}

inline constexpr u8_t blue( u32_t color_ ) {
	return ( static_cast<u8_t>( color_ >> 8 ) );
}

int HDetachedRenderer::_activeSurfaces = 0;

HDetachedRenderer::HDetachedRenderer( HKeyboardEventListener* keyboardEventListener_ )
	: _loop( false )
	, _fullscreen( false )
	, _window()
	, _renderer()
	, _width( 0 )
	, _height( 0 )
	, _semaphore()
	, _thread()
	, _keyboardEventListener( keyboardEventListener_ ) {
	M_PROLOG
	int error = 0;
	HString message;
	if ( _activeSurfaces < 1 ) {
		hcore::log << _( "Initializing SDL library " );
		u32_t flag( 0 );
#ifdef SDL_INIT_EVENTTHREAD
		flag = SDL_INIT_EVENTTHREAD;
#endif /* #ifdef SDL_INIT_EVENTTHREAD */
		if ( ( error = SDL_Init( SDL_INIT_VIDEO | flag ) ) < 0 ) {
			message = _( "Couldn't initialize SDL: " );
			message += SDL_GetError();
			M_THROW( message, error );
		}
		hcore::log << _( "ok." ) << endl;
	}
	SDL_EventState( SDL_MOUSEMOTION, SDL_ENABLE );
	SDL_EventState( SDL_MOUSEBUTTONDOWN, SDL_ENABLE );
	SDL_EventState( SDL_MOUSEBUTTONUP, SDL_ENABLE );
	SDL_EventState( SDL_WINDOWEVENT, SDL_ENABLE );
	return;
	M_EPILOG
}

HDetachedRenderer::~HDetachedRenderer( void ) {
	M_PROLOG
	if ( _activeSurfaces ) {
		_semaphore.wait();
	}
	if ( !! _window ) {
		down();
	}
	if ( _activeSurfaces < 1 ) {
		SDL_Quit();
	}
	return;
	M_EPILOG
}

bool HDetachedRenderer::render_surface( void ) {
	M_PROLOG
	init( setup._resolutionX, setup._resolutionY );
	return ( false );
	M_EPILOG
}

int HDetachedRenderer::init( int width_, int height_ ) {
	M_PROLOG
	HString message;

	_width = width_;
	_height = height_;

	_window = sdl_window_t(
		SDL_CreateWindow(
			"",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			_width, _height,
			SDL_WINDOW_BORDERLESS
		),
		&SDL_DestroyWindow
	);

	if ( ! _window ) {
		int error = errno;
		M_THROW( to_string( _( "Couldn't create window: " ) ).append( SDL_GetError() ), error );
	}

	_renderer = sdl_renderer_t(
		SDL_CreateRenderer( _window.get(), -1, SDL_RENDERER_ACCELERATED ),
		&SDL_DestroyRenderer
	);
	if ( ! _renderer ) {
		int error = errno;
		M_THROW( to_string( _( "Couldn't create renderer: " ) ).append( SDL_GetError() ), error );
	}

	Uint32 flags( SDL_GetWindowFlags( _window.get() ) );
	if ( ! ( flags & SDL_WINDOW_BORDERLESS ) ) {
		hcore::log << _( "Requested video settings not satisfied: " ) << flags << endl;
	}
	SDL_RendererInfo info;
	if ( SDL_GetRendererInfo( _renderer.get(), &info ) != 0 ) {
		int error = errno;
		M_THROW( to_string( _( "Couldn't get renderer flags: " ) ).append( SDL_GetError() ), error );
	}
	if ( ! ( info.flags & SDL_RENDERER_ACCELERATED ) ) {
		hcore::log << _( "Requested renderer settings not satisfied: " ) << info.flags << endl;
	}
	hcore::log << "Set " << _width << 'x' << _height << " mode" << endl;
	++ _activeSurfaces;
	SDL_WarpMouseInWindow( _window.get(), static_cast<Uint16>( setup._resolutionX >> 1 ),
		static_cast<Uint16>( setup._resolutionY >> 1 ) );
	_loop = true;
	_thread.spawn( call( &HDetachedRenderer::run, this ) );
	return ( 0 );
	M_EPILOG
}

void HDetachedRenderer::run( void ) {
	M_PROLOG
	SDL_Event l_uEvent;
	while ( _loop && _thread.is_alive() ) {
		if ( SDL_WaitEvent( &l_uEvent ) ) {
			switch ( l_uEvent.type ) {
				case ( SDL_MOUSEMOTION ): {
					int dx = yaal::math::abs( l_uEvent.motion.xrel );
					int dy = yaal::math::abs( l_uEvent.motion.yrel );
					if ( ( dx < ( setup._resolutionX >> 1 ) ) && ( dy < ( setup._resolutionY >> 1 ) ) ) {
						HMouseEvent e( HMouseEvent::TYPE::MOVE );
						e.set_pos( l_uEvent.motion.xrel, l_uEvent.motion.yrel );
						bool skip = false;
						switch ( l_uEvent.motion.state ) {
							case ( SDL_BUTTON( 1 ) ):
								e.set_button( HMouseEvent::BUTTON::B_1 );
							break;
							case ( SDL_BUTTON( 2 ) ):
								e.set_button( HMouseEvent::BUTTON::B_2 );
							break;
							case ( SDL_BUTTON( 3 ) ):
								e.set_button( HMouseEvent::BUTTON::B_3 );
							break;
							default:
								skip = true;
							break;
						}
						if ( ! skip )
							_engine->on_event( &e );
					}
					break;
				}
				case ( SDL_MOUSEWHEEL ): {
					bool skip = false;
					HMouseEvent e( HMouseEvent::TYPE::PRESS );
					if ( l_uEvent.wheel.y > 0 ) {
						e.set_button( HMouseEvent::BUTTON::B_4 );
					} else if ( l_uEvent.wheel.y < 0 ) {
						e.set_button( HMouseEvent::BUTTON::B_5 );
					} else {
						skip = true;
					}
					if ( ! skip ) {
						_engine->on_event( &e );
					}
				} break;
				case ( SDL_KEYUP ): {
					switch ( l_uEvent.key.keysym.sym ) {
						case ( 'q' ): {
							if ( _keyboardEventListener ) {
								HKeyboardEvent e( static_cast<int>( l_uEvent.key.keysym.sym ) );
								_keyboardEventListener->on_event( &e );
							} else {
								_loop = false;
								down();
								_semaphore.signal();
							}
						} break;
						case ( 'f' ):
							toggle_fullscreen();
						break;
						default:
							HKeyboardEvent e( static_cast<int>( l_uEvent.key.keysym.sym ),
								( l_uEvent.key.keysym.mod & ( KMOD_RSHIFT | KMOD_LSHIFT ) )
								? HKeyboardEvent::MOD::SHIFT : HKeyboardEvent::MOD::NONE );
							_engine->on_event( &e );
						break;
					} break;
				} break;
				default:
				break;
			}
			if ( _loop ) {
				_engine->draw_frame();
				while ( SDL_PollEvent( &l_uEvent ) )
					;
			}
		}
	}
	return;
	M_EPILOG
}

void HDetachedRenderer::shutdown( void ) {
	_thread.finish();
	cout << __PRETTY_FUNCTION__ << endl;
	down();
}

double HDetachedRenderer::do_get_width( void ) const {
	return ( _width );
}

double HDetachedRenderer::do_get_height( void ) const {
	return ( _height );
}

void HDetachedRenderer::down( void ) {
	M_PROLOG
	cout << __PRETTY_FUNCTION__ << endl;
	M_ASSERT( _activeSurfaces > 0 );
	if ( ! _window ) {
		M_THROW( _( "window not initialized" ), errno );
	}
	_renderer.reset();
	_window.reset();
	-- _activeSurfaces;
	M_EPILOG
}

void HDetachedRenderer::do_commit( void ) {
	M_PROLOG
	SDL_RenderPresent( _renderer.get() );
	return;
	M_EPILOG
}

void HDetachedRenderer::do_line( double x1, double y1, double x2, double y2, u32_t color ) {
	M_PROLOG
	SDL_SetRenderDrawColor( _renderer.get(), red( color ), green( color ), blue( color ), 0x00 );
	SDL_RenderDrawLine( _renderer.get(), static_cast<int>( x1 ), static_cast<int>( y1 ), static_cast<int>( x2 ), static_cast<int>( y2 ) );
	return;
	M_EPILOG
}

u32_t HDetachedRenderer::do_RGB( u8_t red, u8_t green, u8_t blue ) {
	M_PROLOG
	u32_t color( 0 );
	color |= ( static_cast<u32_t>( red ) << 24 );
	color |= ( static_cast<u32_t>( green ) << 16 );
	color |= ( static_cast<u32_t>( blue ) << 8 );
	return ( color );
	M_EPILOG
}

void HDetachedRenderer::do_fill_rect( double, double, double, double, u32_t color ) {
	M_PROLOG
	SDL_SetRenderDrawColor( _renderer.get(), red( color ), green( color ), blue( color ), 0x00 );
	SDL_RenderClear( _renderer.get() );
	return;
	M_EPILOG
}

void HDetachedRenderer::toggle_fullscreen( void ) {
	M_PROLOG
	_fullscreen = ! _fullscreen;
	SDL_SetWindowFullscreen( _window.get(), _fullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
	return;
	M_EPILOG
}

}


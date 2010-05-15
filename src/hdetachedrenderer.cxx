/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	renderer.cxx - this file is integral part of `funlab' project.

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
 FITNESS FOR _precountA PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libintl.h>
#include <SDL/SDL.h>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "hdetachedrenderer.hxx"
#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab
{

int HDetachedRenderer::_activeSurfaces = 0;

HDetachedRenderer::HDetachedRenderer( HKeyboardEventListener* keyboardEventListener_ )
	: _loop( false ), _handler( NULL ), _width( 0 ), _height( 0 ), _bPP( 0 ),
	_semaphore(), _thread(), _keyboardEventListener( keyboardEventListener_ )
	{
	M_PROLOG
	int error = 0;
	HString message;
	if ( _activeSurfaces < 1 )
		{
		hcore::log << _( "Initializing SDL library " );
		if ( ( error = SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD ) ) < 0 )
			{
			message = _( "Couldn't initialize SDL: " );
			message += SDL_GetError();
			M_THROW( message, error );
			}
		hcore::log << _( "ok." ) << endl;
		}
	SDL_EventState( SDL_MOUSEMOTION, SDL_ENABLE );
	SDL_EventState( SDL_MOUSEBUTTONDOWN, SDL_ENABLE );
	SDL_EventState( SDL_MOUSEBUTTONUP, SDL_ENABLE );
	SDL_EventState( SDL_VIDEOEXPOSE, SDL_ENABLE );
	return;
	M_EPILOG
	}

HDetachedRenderer::~HDetachedRenderer( void )
	{
	M_PROLOG
	if ( _activeSurfaces )
		_semaphore.wait();
	if ( _handler )
		down();
	if ( _activeSurfaces < 1 )
		SDL_Quit();
	return;
	M_EPILOG
	}

bool HDetachedRenderer::render_surface( void )
	{
	M_PROLOG
	init( setup._resolutionX, setup._resolutionY );
	return ( false );
	M_EPILOG
	}

int HDetachedRenderer::init( int width_, int height_, int bpp_ )
	{
	M_PROLOG
	int error = 0;
	HString message;
	SDL_Surface* surface = NULL;

	_width = width_;
	_height = height_;
	_bPP = bpp_;
	surface = SDL_SetVideoMode( _width, _height, _bPP,
			SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_NOFRAME );
	if ( surface == NULL )
		{
		error = errno;
		message = _( "Couldn't set 640x480x8 video mode: " );
		message += SDL_GetError();
		M_THROW( message, error );
		}

	if ( surface->flags != ( SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_NOFRAME ) )
		{
		hcore::log << _( "Requested video settings not satisfied: " ) << surface->flags;
		hcore::log << " (" << SDL_HWSURFACE << '|' << SDL_ANYFORMAT << '|' << SDL_DOUBLEBUF << '|' << SDL_NOFRAME << ")" << endl;
		}
	_handler = surface;
	_bPP = surface->format->BitsPerPixel;
	if ( _bPP != bpp_ )
		hcore::log << "BPP downgrade: " << bpp_ << " -> " << _bPP << endl;
	hcore::log << "Set " << _width << 'x' << _height << " at " << _bPP << " bits-per-pixel mode" << endl;
	if ( SDL_MUSTLOCK( surface ) )
		{
		hcore::log << "Locking surface ";
		if ( SDL_LockSurface( surface ) < 0 )
			hcore::log << "failed: " << SDL_GetError() << endl;
		else
			hcore::log << "ok." << endl;
		}
	_activeSurfaces ++;
	SDL_WarpMouse( static_cast<Uint16>( setup._resolutionX >> 1 ),
			static_cast<Uint16>( setup._resolutionY >> 1 ) );
	_loop = true;
	_thread.spawn( bound_call( &HDetachedRenderer::run, this ) );
	return ( 0 );
	M_EPILOG
	}

void* HDetachedRenderer::run( void )
	{
	M_PROLOG
	SDL_Event l_uEvent;
	while ( _loop && _thread.is_alive() )
		{
		if ( SDL_WaitEvent( &l_uEvent ) )
			{
			switch ( l_uEvent.type )
				{
				case ( SDL_MOUSEMOTION ):
					{
					int dx = yaal::abs( l_uEvent.motion.xrel );
					int dy = yaal::abs( l_uEvent.motion.yrel );
					if ( ( dx < ( setup._resolutionX >> 1 ) ) && ( dy < ( setup._resolutionY >> 1 ) ) )
						{
						HMouseEvent e( HMouseEvent::TYPE::MOVE );
						e.set_pos( l_uEvent.motion.xrel, l_uEvent.motion.yrel );
						bool skip = false;
						switch ( l_uEvent.motion.state )
							{
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
				case ( SDL_MOUSEBUTTONDOWN ):
					{
					bool skip = false;
					HMouseEvent e( HMouseEvent::TYPE::PRESS );
					switch ( l_uEvent.button.button )
						{
						case ( 4 ):
							e.set_button( HMouseEvent::BUTTON::B_4 );
						break;
						case ( 5 ):
							e.set_button( HMouseEvent::BUTTON::B_5 );
						break;
						default:
							skip = true;
						break;
						}
					if ( ! skip )
						_engine->on_event( &e );
					}
				break;
				case ( SDL_KEYUP ):
					{
					switch ( l_uEvent.key.keysym.sym )
						{
						case ( 'q' ):
							{
							if ( _keyboardEventListener )
								{
								HKeyboardEvent e( static_cast<int>( l_uEvent.key.keysym.sym ) );
								_keyboardEventListener->on_event( &e );
								}
							else
								{
								_loop = false;
								down();
								_semaphore.signal();
								}
							}
						break;
						case ( 'f' ):
							toggle_fullscreen();
						break;
						default:
							HKeyboardEvent e( static_cast<int>( l_uEvent.key.keysym.sym ),
									( l_uEvent.key.keysym.mod & ( KMOD_RSHIFT | KMOD_LSHIFT ) )
										? HKeyboardEvent::MOD::SHIFT : HKeyboardEvent::MOD::NONE );
							_engine->on_event( &e );
						break;
						}
					break;
					}
				break;
				default:
				break;
				}
			if ( _loop )
				{
				_engine->draw_frame();
				while ( SDL_PollEvent( &l_uEvent ) )
					;
				}
			}
		}
	return ( 0 );
	M_EPILOG
	}

void HDetachedRenderer::shutdown( void )
	{
	_thread.finish();
	cout << __PRETTY_FUNCTION__ << endl;
	down();
	}

double HDetachedRenderer::do_get_width( void ) const
	{
	return ( _width );
	}

double HDetachedRenderer::do_get_height( void ) const
	{
	return ( _height );
	}

void HDetachedRenderer::down( void )
	{
	M_PROLOG
	cout << __PRETTY_FUNCTION__ << endl;
	SDL_Surface* surface = static_cast<SDL_Surface*>( _handler );
	M_ASSERT( _activeSurfaces > 0 );
	if ( ! _handler )
		M_THROW( _( "surface not initialized" ), errno );
	if ( SDL_MUSTLOCK( surface ) )
		SDL_UnlockSurface( surface );
	SDL_FreeSurface( static_cast<SDL_Surface*>( _handler ) );
	_handler = NULL;
	-- _activeSurfaces;
	M_EPILOG
	}

void HDetachedRenderer::do_commit( void )
	{
	M_PROLOG
	SDL_Surface* surface = static_cast<SDL_Surface*>( _handler );

	if ( SDL_MUSTLOCK( surface ) )
		SDL_UnlockSurface( surface );

	while ( SDL_Flip( surface ) )
		;
	if ( SDL_MUSTLOCK( surface ) )
		{
		if ( SDL_LockSurface( surface ) < 0 )
			hcore::log( LOG_TYPE::ERROR ) << "Can't lock screen: " << SDL_GetError() << endl;
		}
	return;
	M_EPILOG
	}

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
u32_t HDetachedRenderer::do_get_pixel( double x, double y )
	{
	M_PROLOG
	unsigned char* rawMemory = NULL;
	SDL_Surface* surface = static_cast<SDL_Surface*>( _handler );

	if ( ( x < 0 ) || ( y < 0 ) || ( x >= _width ) || ( y >= _height ) )
		return ( static_cast<u32_t>( -1 ) );

	/* Here rawMemory is the address to the pixel we want to retrieve */
	int long ix = static_cast<int long>( x );
	int long iy = static_cast<int long>( y );
	rawMemory = static_cast<unsigned char*>( surface->pixels )
	                + iy * surface->pitch + ix * surface->format->BytesPerPixel;

	switch ( surface->format->BytesPerPixel )
		{
		case ( 1 ):
			{
			return ( *rawMemory );
			}
		case ( 2 ):
			{
			return ( *reinterpret_cast<Uint16*>( rawMemory ) );
			}
		case ( 3 ):
			{
			if ( SDL_BYTEORDER == SDL_BIG_ENDIAN )
				return ( rawMemory[ 0 ] << 16 | rawMemory[ 1 ] << 8 | rawMemory[ 2 ] );
			else
				return ( rawMemory[ 0 ] | rawMemory[ 1 ] << 8 | rawMemory[ 2 ] << 16 );
			}
		case ( 4 ):
			{
			return ( *reinterpret_cast<u32_t*>( rawMemory ) );
			}
		}
	return ( 0 );       /* shouldn't happen, but avoids warnings */
	M_EPILOG
	}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void HDetachedRenderer::do_put_pixel( double x, double y, u32_t pixel )
	{
	M_PROLOG
	unsigned char* rawMemory = NULL;
	SDL_Surface* surface = static_cast<SDL_Surface*>( _handler );

	if ( ( x < 0 ) || ( y < 0 ) || ( x >= _width ) || ( y >= _height ) )
		return;

	/* Here rawMemory is the address to the pixel we want to set */
	int long ix = static_cast<int long>( x );
	int long iy = static_cast<int long>( y );
	rawMemory = static_cast<unsigned char*>( surface->pixels )
	                + iy * surface->pitch + ix * surface->format->BytesPerPixel;

	switch ( surface->format->BytesPerPixel )
		{
		case ( 1 ):
			( *rawMemory ) = static_cast<char>( pixel );
		break;
		case ( 2 ):
			( *reinterpret_cast<Uint16*>( rawMemory ) ) = static_cast<Uint16>( pixel );
		break;
		case ( 3 ):
			{
			if ( SDL_BYTEORDER == SDL_BIG_ENDIAN )
				{
				rawMemory[ 0 ] = static_cast<char>( ( pixel >> 16 ) & 0xff );
				rawMemory[ 1 ] = static_cast<char>( ( pixel >> 8 ) & 0xff );
				rawMemory[ 2 ] = static_cast<char>( pixel & 0xff );
				}
			else
				{
				rawMemory[ 0 ] = static_cast<char>( pixel & 0xff );
				rawMemory[ 1 ] = static_cast<char>( ( pixel >> 8 ) & 0xff );
				rawMemory[ 2 ] = static_cast<char>( ( pixel >> 16 ) & 0xff );
				}
			}
		break;
		case ( 4 ):
			( *reinterpret_cast<u32_t*>( rawMemory ) ) = pixel;
		break;
		default:
			M_ASSERT( ! "color plane not supported" );
		break;
		}
	return;
	M_EPILOG
	}

void HDetachedRenderer::do_line( double x0, double y0, double x1, double y1, u32_t color )
	{
	M_PROLOG
/*
 * Implementation ripped from http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html
 * Line-Drawing Algorithms
 */

/*
 *  1001 | 1000 | 1010
 * ------+------+------
 *  0001 | 0000 | 0010
 * ------+------+------
 *  0101 | 0100 | 0110
 *
 */
	
#define EDGE_LEFT		1
#define EDGE_TOP		8
#define EDGE_RIGHT	2
#define EDGE_BOTTOM	4

	double dx = 0, dy = 0, stepx = 0, stepy = 0, fraction = 0, cx = 0, cy = 0;

	int edges0 = 0, edges1 = 0, edges = 0;

	do
		{
		edges0 = edges1 = edges = 0;
		edges0 |= ( x0 < 0 ) ? EDGE_LEFT : 0;
		edges0 |= ( y0 < 0 ) ? EDGE_TOP : 0;
		edges0 |= ( x0 >= _width ) ? EDGE_RIGHT : 0;
		edges0 |= ( y0 >= _height ) ? EDGE_BOTTOM : 0;
		edges1 |= ( x1 < 0 ) ? EDGE_LEFT : 0;
		edges1 |= ( y1 < 0 ) ? EDGE_TOP : 0;
		edges1 |= ( x1 >= _width ) ? EDGE_RIGHT : 0;
		edges1 |= ( y1 >= _height ) ? EDGE_BOTTOM : 0;
		if ( edges0 & edges1 )
			return;
		if ( edges0 )
			edges = edges0;
		else if ( edges1 )
			edges = edges1;
		else
			break;

		if ( edges & EDGE_LEFT )
			{
			cx = 0;
			cy = y0 + ( y1 - y0 ) * ( 0 - x0 ) / ( x1 - x0 );
			}
		else if ( edges & EDGE_TOP )
			{
			cx = x0 + ( x1 - x0 ) * ( 0 - y0 ) / ( y1 - y0 );
			cy = 0;
			}
		else if ( edges & EDGE_RIGHT )
			{
			cx = _width - 1;
			cy = y0 + ( y1 - y0 ) * ( cx - x0 ) / ( x1 - x0 );
			}
		else if ( edges & EDGE_BOTTOM )
			{
			cy = _height - 1;
			cx = x0 + ( x1 - x0 ) * ( cy - y0 ) / ( y1 - y0 );
			}
		if ( edges0 )
			x0 = cx, y0 = cy;
		else
			x1 = cx, y1 = cy;
		}
	while ( edges0 | edges1 );
	
	dx = x1 - x0;
	dy = y1 - y0;

	if ( dx < 0 )
		dx = -dx, stepx = -1;
	else
		stepx = 1;
	if ( dy < 0 )
		dy = -dy, stepy = -1;
	else
		stepy = 1;

	dx *= 2;
	dy *= 2;

	if ( ( x0 >= 0 ) && ( x0 < _width ) && ( y0 >= 0 ) && ( y0 < _height ) )
		put_pixel( static_cast<int>( x0 ), static_cast<int>( y0 ), color );
	else
		return;
	if ( dx > dy )
		{
		fraction = dy - ( dx / 2 );
		while ( ( ( stepx > 0 ) && ( x0 <= x1 ) ) || ( ( stepx < 0 ) && ( x0 >= x1 ) ) )
			{
			if ( fraction >= 0 )
				{
				y0 += stepy;
				fraction -= dx;
				}
			x0 += stepx;
			fraction += dy;
			if ( ( x0 >= 0 ) && ( x0 < _width ) && ( y0 >= 0 ) && ( y0 < _height ) )
				put_pixel( static_cast<int>( x0 ), static_cast<int>( y0 ), color );
			else
				return;
			}
		}
	else
		{
		fraction = dx - ( dy / 2 );
		while ( ( ( stepy > 0 ) && ( y0 <= y1 ) ) || ( ( stepy < 0 ) && ( y0 >= y1 ) ) )
			{
			if ( fraction >= 0 )
				{
				x0 += stepx;
				fraction -= dy;
				}
			y0 += stepy;
			fraction += dx;
			if ( ( x0 >= 0 ) && ( x0 < _width ) && ( y0 >= 0 ) && ( y0 < _height ) )
				put_pixel( static_cast<int>( x0 ), static_cast<int>( y0 ), color );
			else
				return;
			}
		}
	return;
	M_EPILOG
	}

u32_t HDetachedRenderer::do_RGB( u8_t red, u8_t green, u8_t blue )
	{
	M_PROLOG
	u32_t value = 0;
	SDL_Surface* surface = static_cast<SDL_Surface*>( _handler );
	value = SDL_MapRGB( surface->format, red, green, blue );
	return ( value );
	M_EPILOG
	}

void HDetachedRenderer::do_fill_rect( double, double, double, double, u32_t color )
	{
	M_PROLOG
	SDL_Surface* surface = static_cast<SDL_Surface*>( _handler );
	SDL_FillRect( surface, NULL, color );
	return;
	M_EPILOG
	}

void HDetachedRenderer::toggle_fullscreen( void )
	{
	M_PROLOG
	SDL_WM_ToggleFullScreen( static_cast<SDL_Surface*>( _handler ) );

	return;
	M_EPILOG
	}

}


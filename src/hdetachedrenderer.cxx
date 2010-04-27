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
 FITNESS FOR f_dPrecountA PARTICULAR PURPOSE. Use it at your own risk.
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

int HDetachedRenderer::f_iActiveSurfaces = 0;

HDetachedRenderer::HDetachedRenderer( HKeyboardEventListener* a_poKeyboardEventListener )
	: f_bLoop( false ), f_pvHandler( NULL ), f_iWidth( 0 ), f_iHeight( 0 ), f_iBPP( 0 ),
	f_oSemaphore(), _thread(), f_poKeyboardEventListener( a_poKeyboardEventListener )
	{
	M_PROLOG
	int l_iError = 0;
	HString l_oMessage;
	if ( f_iActiveSurfaces < 1 )
		{
		hcore::log << _( "Initializing SDL library " );
		if ( ( l_iError = SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD ) ) < 0 )
			{
			l_oMessage = _( "Couldn't initialize SDL: " );
			l_oMessage += SDL_GetError();
			M_THROW( l_oMessage, l_iError );
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
	if ( f_iActiveSurfaces )
		f_oSemaphore.wait();
	if ( f_pvHandler )
		down();
	if ( f_iActiveSurfaces < 1 )
		SDL_Quit();
	return;
	M_EPILOG
	}

bool HDetachedRenderer::render_surface( void )
	{
	M_PROLOG
	init( setup.f_iResolutionX, setup.f_iResolutionY );
	return ( false );
	M_EPILOG
	}

int HDetachedRenderer::init( int a_iWidth, int a_iHeight, int a_iBpp )
	{
	M_PROLOG
	int l_iError = 0;
	HString l_oMessage;
	SDL_Surface* l_psSurface = NULL;

	f_iWidth = a_iWidth;
	f_iHeight = a_iHeight;
	f_iBPP = a_iBpp;
	l_psSurface = SDL_SetVideoMode( f_iWidth, f_iHeight, f_iBPP,
			SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_NOFRAME );
	if ( l_psSurface == NULL )
		{
		l_iError = errno;
		l_oMessage = _( "Couldn't set 640x480x8 video mode: " );
		l_oMessage += SDL_GetError();
		M_THROW( l_oMessage, l_iError );
		}

	if ( l_psSurface->flags != ( SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_NOFRAME ) )
		{
		hcore::log << _( "Requested video settings not satisfied: " ) << l_psSurface->flags;
		hcore::log << " (" << SDL_HWSURFACE << '|' << SDL_ANYFORMAT << '|' << SDL_DOUBLEBUF << '|' << SDL_NOFRAME << ")" << endl;
		}
	f_pvHandler = l_psSurface;
	f_iBPP = l_psSurface->format->BitsPerPixel;
	if ( f_iBPP != a_iBpp )
		hcore::log << "BPP downgrade: " << a_iBpp << " -> " << f_iBPP << endl;
	hcore::log << "Set " << f_iWidth << 'x' << f_iHeight << " at " << f_iBPP << " bits-per-pixel mode" << endl;
	if ( SDL_MUSTLOCK( l_psSurface ) )
		{
		hcore::log << "Locking surface ";
		if ( SDL_LockSurface( l_psSurface ) < 0 )
			hcore::log << "failed: " << SDL_GetError() << endl;
		else
			hcore::log << "ok." << endl;
		}
	f_iActiveSurfaces ++;
	SDL_WarpMouse( static_cast<Uint16>( setup.f_iResolutionX >> 1 ),
			static_cast<Uint16>( setup.f_iResolutionY >> 1 ) );
	f_bLoop = true;
	_thread.spawn( bound_call( &HDetachedRenderer::run, this ) );
	return ( 0 );
	M_EPILOG
	}

void* HDetachedRenderer::run( void )
	{
	M_PROLOG
	SDL_Event l_uEvent;
	while ( f_bLoop && _thread.is_alive() )
		{
		if ( SDL_WaitEvent( &l_uEvent ) )
			{
			switch ( l_uEvent.type )
				{
				case ( SDL_MOUSEMOTION ):
					{
					int dx = yaal::abs( l_uEvent.motion.xrel );
					int dy = yaal::abs( l_uEvent.motion.yrel );
					if ( ( dx < ( setup.f_iResolutionX >> 1 ) ) && ( dy < ( setup.f_iResolutionY >> 1 ) ) )
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
							f_oEngine->on_event( &e );
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
						f_oEngine->on_event( &e );
					}
				break;
				case ( SDL_KEYUP ):
					{
					switch ( l_uEvent.key.keysym.sym )
						{
						case ( 'q' ):
							{
							if ( f_poKeyboardEventListener )
								{
								HKeyboardEvent e( static_cast<int>( l_uEvent.key.keysym.sym ) );
								f_poKeyboardEventListener->on_event( &e );
								}
							else
								{
								f_bLoop = false;
								down();
								f_oSemaphore.signal();
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
							f_oEngine->on_event( &e );
						break;
						}
					break;
					}
				break;
				default:
				break;
				}
			if ( f_bLoop )
				{
				f_oEngine->draw_frame();
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
	return ( f_iWidth );
	}

double HDetachedRenderer::do_get_height( void ) const
	{
	return ( f_iHeight );
	}

void HDetachedRenderer::down( void )
	{
	M_PROLOG
	cout << __PRETTY_FUNCTION__ << endl;
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );
	M_ASSERT( f_iActiveSurfaces > 0 );
	if ( ! f_pvHandler )
		M_THROW( _( "surface not initialized" ), errno );
	if ( SDL_MUSTLOCK( l_psSurface ) )
		SDL_UnlockSurface( l_psSurface );
	SDL_FreeSurface( static_cast<SDL_Surface*>( f_pvHandler ) );
	f_pvHandler = NULL;
	-- f_iActiveSurfaces;
	M_EPILOG
	}

void HDetachedRenderer::do_commit( void )
	{
	M_PROLOG
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );

	if ( SDL_MUSTLOCK( l_psSurface ) )
		SDL_UnlockSurface( l_psSurface );

	while ( SDL_Flip( l_psSurface ) )
		;
	if ( SDL_MUSTLOCK( l_psSurface ) )
		{
		if ( SDL_LockSurface( l_psSurface ) < 0 )
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
	unsigned char* l_pcRawMemory = NULL;
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );

	if ( ( x < 0 ) || ( y < 0 ) || ( x >= f_iWidth ) || ( y >= f_iHeight ) )
		return ( static_cast<u32_t>( -1 ) );

	/* Here l_pcRawMemory is the address to the pixel we want to retrieve */
	int long ix = static_cast<int long>( x );
	int long iy = static_cast<int long>( y );
	l_pcRawMemory = static_cast<unsigned char*>( l_psSurface->pixels )
	                + iy * l_psSurface->pitch + ix * l_psSurface->format->BytesPerPixel;

	switch ( l_psSurface->format->BytesPerPixel )
		{
		case ( 1 ):
			{
			return ( *l_pcRawMemory );
			}
		case ( 2 ):
			{
			return ( *reinterpret_cast<Uint16*>( l_pcRawMemory ) );
			}
		case ( 3 ):
			{
			if ( SDL_BYTEORDER == SDL_BIG_ENDIAN )
				return ( l_pcRawMemory[ 0 ] << 16 | l_pcRawMemory[ 1 ] << 8 | l_pcRawMemory[ 2 ] );
			else
				return ( l_pcRawMemory[ 0 ] | l_pcRawMemory[ 1 ] << 8 | l_pcRawMemory[ 2 ] << 16 );
			}
		case ( 4 ):
			{
			return ( *reinterpret_cast<u32_t*>( l_pcRawMemory ) );
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
	unsigned char* l_pcRawMemory = NULL;
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );

	if ( ( x < 0 ) || ( y < 0 ) || ( x >= f_iWidth ) || ( y >= f_iHeight ) )
		return;

	/* Here l_pcRawMemory is the address to the pixel we want to set */
	int long ix = static_cast<int long>( x );
	int long iy = static_cast<int long>( y );
	l_pcRawMemory = static_cast<unsigned char*>( l_psSurface->pixels )
	                + iy * l_psSurface->pitch + ix * l_psSurface->format->BytesPerPixel;

	switch ( l_psSurface->format->BytesPerPixel )
		{
		case ( 1 ):
			( *l_pcRawMemory ) = static_cast<char>( pixel );
		break;
		case ( 2 ):
			( *reinterpret_cast<Uint16*>( l_pcRawMemory ) ) = static_cast<Uint16>( pixel );
		break;
		case ( 3 ):
			{
			if ( SDL_BYTEORDER == SDL_BIG_ENDIAN )
				{
				l_pcRawMemory[ 0 ] = static_cast<char>( ( pixel >> 16 ) & 0xff );
				l_pcRawMemory[ 1 ] = static_cast<char>( ( pixel >> 8 ) & 0xff );
				l_pcRawMemory[ 2 ] = static_cast<char>( pixel & 0xff );
				}
			else
				{
				l_pcRawMemory[ 0 ] = static_cast<char>( pixel & 0xff );
				l_pcRawMemory[ 1 ] = static_cast<char>( ( pixel >> 8 ) & 0xff );
				l_pcRawMemory[ 2 ] = static_cast<char>( ( pixel >> 16 ) & 0xff );
				}
			}
		break;
		case ( 4 ):
			( *reinterpret_cast<u32_t*>( l_pcRawMemory ) ) = pixel;
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

	int l_iEdges0 = 0, l_iEdges1 = 0, l_iEdges = 0;

	do
		{
		l_iEdges0 = l_iEdges1 = l_iEdges = 0;
		l_iEdges0 |= ( x0 < 0 ) ? EDGE_LEFT : 0;
		l_iEdges0 |= ( y0 < 0 ) ? EDGE_TOP : 0;
		l_iEdges0 |= ( x0 >= f_iWidth ) ? EDGE_RIGHT : 0;
		l_iEdges0 |= ( y0 >= f_iHeight ) ? EDGE_BOTTOM : 0;
		l_iEdges1 |= ( x1 < 0 ) ? EDGE_LEFT : 0;
		l_iEdges1 |= ( y1 < 0 ) ? EDGE_TOP : 0;
		l_iEdges1 |= ( x1 >= f_iWidth ) ? EDGE_RIGHT : 0;
		l_iEdges1 |= ( y1 >= f_iHeight ) ? EDGE_BOTTOM : 0;
		if ( l_iEdges0 & l_iEdges1 )
			return;
		if ( l_iEdges0 )
			l_iEdges = l_iEdges0;
		else if ( l_iEdges1 )
			l_iEdges = l_iEdges1;
		else
			break;

		if ( l_iEdges & EDGE_LEFT )
			{
			cx = 0;
			cy = y0 + ( y1 - y0 ) * ( 0 - x0 ) / ( x1 - x0 );
			}
		else if ( l_iEdges & EDGE_TOP )
			{
			cx = x0 + ( x1 - x0 ) * ( 0 - y0 ) / ( y1 - y0 );
			cy = 0;
			}
		else if ( l_iEdges & EDGE_RIGHT )
			{
			cx = f_iWidth - 1;
			cy = y0 + ( y1 - y0 ) * ( cx - x0 ) / ( x1 - x0 );
			}
		else if ( l_iEdges & EDGE_BOTTOM )
			{
			cy = f_iHeight - 1;
			cx = x0 + ( x1 - x0 ) * ( cy - y0 ) / ( y1 - y0 );
			}
		if ( l_iEdges0 )
			x0 = cx, y0 = cy;
		else
			x1 = cx, y1 = cy;
		}
	while ( l_iEdges0 | l_iEdges1 );
	
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

	if ( ( x0 >= 0 ) && ( x0 < f_iWidth ) && ( y0 >= 0 ) && ( y0 < f_iHeight ) )
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
			if ( ( x0 >= 0 ) && ( x0 < f_iWidth ) && ( y0 >= 0 ) && ( y0 < f_iHeight ) )
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
			if ( ( x0 >= 0 ) && ( x0 < f_iWidth ) && ( y0 >= 0 ) && ( y0 < f_iHeight ) )
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
	u32_t l_ulValue = 0;
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );
	l_ulValue = SDL_MapRGB( l_psSurface->format, red, green, blue );
	return ( l_ulValue );
	M_EPILOG
	}

void HDetachedRenderer::do_fill_rect( double, double, double, double, u32_t color )
	{
	M_PROLOG
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );
	SDL_FillRect( l_psSurface, NULL, color );
	return;
	M_EPILOG
	}

void HDetachedRenderer::toggle_fullscreen( void )
	{
	M_PROLOG
	SDL_WM_ToggleFullScreen( static_cast<SDL_Surface*>( f_pvHandler ) );

	return;
	M_EPILOG
	}

}


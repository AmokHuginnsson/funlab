/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	hsurface.cxx - this file is integral part of `funlab' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.
	
Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <libintl.h>
#include <SDL/SDL.h>

#include <yaal/yaal.h>
M_VCSID( "$Id: "__ID__" $" )

#include "hsurface.h"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab
{

int HSurface::f_iActiveSurfaces = 0;

int HSurface::surface_count( void )
	{
	return ( f_iActiveSurfaces );
	}

HSurface::HSurface( void ) : f_iWidth( 0 ), f_iHeight( 0 ), f_iBPP( 0 ), f_pvHandler( NULL )
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
	return;
	M_EPILOG
	}

HSurface::~HSurface( void )
	{
	M_PROLOG
	if ( f_pvHandler )
		down();
	if ( f_iActiveSurfaces < 1 )
		SDL_Quit();
	return;
	M_EPILOG
	}

void HSurface::down( void )
	{
	M_PROLOG
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );
	M_ASSERT( f_iActiveSurfaces > 0 );
	if ( ! f_pvHandler )
		M_THROW( _( "surface not initialized" ), errno );
	if ( SDL_MUSTLOCK( l_psSurface ) )
		SDL_UnlockSurface( l_psSurface );
	SDL_FreeSurface( static_cast<SDL_Surface*>( f_pvHandler ) );
	f_pvHandler = NULL;
	f_iActiveSurfaces --;
	M_EPILOG
	}

int HSurface::init( int a_iWidth, int a_iHeight, int a_iBpp )
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
	return ( 0 );
	M_EPILOG
	}

void HSurface::refresh( void )
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
			hcore::log( LOG_TYPE::D_ERROR ) << "Can't lock screen: " << SDL_GetError() << endl;
		}
	return;
	M_EPILOG
	}

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
int long unsigned HSurface::get_pixel( int x, int y )
	{
	M_PROLOG
	unsigned char* l_pcRawMemory = NULL;
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );

	if ( ( x < 0 ) || ( y < 0 ) || ( x >= f_iWidth ) || ( y >= f_iHeight ) )
		return ( static_cast<int long unsigned>( -1 ) );

	/* Here l_pcRawMemory is the address to the pixel we want to retrieve */
	l_pcRawMemory = static_cast<unsigned char*>( l_psSurface->pixels )
	                + y * l_psSurface->pitch + x * l_psSurface->format->BytesPerPixel;

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
			return ( *reinterpret_cast<int long unsigned*>( l_pcRawMemory ) );
			}
		}
	return ( 0 );       /* shouldn't happen, but avoids warnings */
	M_EPILOG
	}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void HSurface::put_pixel( int x, int y, int long unsigned pixel )
	{
	M_PROLOG
	unsigned char* l_pcRawMemory = NULL;
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );

	if ( ( x < 0 ) || ( y < 0 ) || ( x >= f_iWidth ) || ( y >= f_iHeight ) )
		return;

	/* Here l_pcRawMemory is the address to the pixel we want to set */
	l_pcRawMemory = static_cast<unsigned char*>( l_psSurface->pixels )
	                + y * l_psSurface->pitch + x * l_psSurface->format->BytesPerPixel;

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
			( *reinterpret_cast<int long unsigned*>( l_pcRawMemory ) ) = pixel;
		break;
		default:
			M_ASSERT( ! "color plane not supported" );
		break;
		}
	return;
	M_EPILOG
	}

void HSurface::line ( double x0, double y0, double x1, double y1, int long unsigned color )
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
	
#define D_EDGE_LEFT		1
#define D_EDGE_TOP		8
#define D_EDGE_RIGHT	2
#define D_EDGE_BOTTOM	4

	double dx = 0, dy = 0, stepx = 0, stepy = 0, fraction = 0, cx = 0, cy = 0;

	int l_iEdges0 = 0, l_iEdges1 = 0, l_iEdges = 0;

	do
		{
		l_iEdges0 = l_iEdges1 = l_iEdges = 0;
		l_iEdges0 |= ( x0 < 0 ) ? D_EDGE_LEFT : 0;
		l_iEdges0 |= ( y0 < 0 ) ? D_EDGE_TOP : 0;
		l_iEdges0 |= ( x0 >= f_iWidth ) ? D_EDGE_RIGHT : 0;
		l_iEdges0 |= ( y0 >= f_iHeight ) ? D_EDGE_BOTTOM : 0;
		l_iEdges1 |= ( x1 < 0 ) ? D_EDGE_LEFT : 0;
		l_iEdges1 |= ( y1 < 0 ) ? D_EDGE_TOP : 0;
		l_iEdges1 |= ( x1 >= f_iWidth ) ? D_EDGE_RIGHT : 0;
		l_iEdges1 |= ( y1 >= f_iHeight ) ? D_EDGE_BOTTOM : 0;
		if ( l_iEdges0 & l_iEdges1 )
			return;
		if ( l_iEdges0 )
			l_iEdges = l_iEdges0;
		else if ( l_iEdges1 )
			l_iEdges = l_iEdges1;
		else
			break;

		if ( l_iEdges & D_EDGE_LEFT )
			{
			cx = 0;
			cy = y0 + ( y1 - y0 ) * ( 0 - x0 ) / ( x1 - x0 );
			}
		else if ( l_iEdges & D_EDGE_TOP )
			{
			cx = x0 + ( x1 - x0 ) * ( 0 - y0 ) / ( y1 - y0 );
			cy = 0;
			}
		else if ( l_iEdges & D_EDGE_RIGHT )
			{
			cx = f_iWidth - 1;
			cy = y0 + ( y1 - y0 ) * ( cx - x0 ) / ( x1 - x0 );
			}
		else if ( l_iEdges & D_EDGE_BOTTOM )
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

int long unsigned HSurface::RGB( int a_iRed, int a_iGreen, int a_iBlue )
	{
	M_PROLOG
	int long unsigned l_ulValue = 0;
	SDL_Surface * l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );
	l_ulValue = SDL_MapRGB( l_psSurface->format,
			static_cast<Uint8>( a_iRed ),
			static_cast<Uint8>( a_iGreen ),
			static_cast<Uint8>( a_iBlue ) );
	return ( l_ulValue );
	M_EPILOG
	}

void HSurface::clear ( void )
	{
	M_PROLOG
	SDL_Surface* l_psSurface = static_cast<SDL_Surface*>( f_pvHandler );
	SDL_FillRect( l_psSurface, NULL, static_cast<int>( RGB( 0, 0, 0 ) ) );
	return;
	M_EPILOG
	}

bool HSurface::is_valid( void )
	{
	M_PROLOG
	return ( f_pvHandler ? true : false );
	M_EPILOG
	}

void HSurface::toggle_fullscreen( void )
	{
	M_PROLOG
	SDL_WM_ToggleFullScreen( static_cast<SDL_Surface*>( f_pvHandler ) );

	return;
	M_EPILOG
	}

}


/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	gl.c - this file is integral part of `funlab' project.

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

#include <stdhapi.h>

#include "gl.h"

using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::tools;

HSurface::HSurface ( void )
	: f_iWidth ( 0 ), f_iHeight ( 0 ), f_iBpp ( 0 ), f_pvHandler ( NULL )
	{
	M_PROLOG
	int l_iError = 0;
	HString l_oMessage;
	if( ( l_iError = SDL_Init ( SDL_INIT_VIDEO ) ) < 0 )
		{
		l_oMessage = _ ( "Couldn't initialize SDL: " );
		l_oMessage += SDL_GetError ( );
		M_THROW ( l_oMessage, l_iError );
		}
	return;
	M_EPILOG
	}

HSurface::~HSurface ( void )
	{
	M_PROLOG
	if ( f_pvHandler )
		down ( );
	SDL_Quit ( );
	return;
	M_EPILOG
	}

void HSurface::down ( void )
	{
	M_PROLOG
	if ( ! f_pvHandler )
		M_THROW ( _ ( "surface not initialized" ), g_iErrNo );
	SDL_FreeSurface ( static_cast < SDL_Surface * > ( f_pvHandler ) );
	f_pvHandler = NULL;
	M_EPILOG
	}

int HSurface::init ( int a_iWidth, int a_iHeight, int a_iBpp )
	{
	M_PROLOG
	int l_iError = 0;
	HString l_oMessage;
	SDL_Surface * l_psSurface = NULL;
	f_iWidth = a_iWidth;
	f_iHeight = a_iHeight;
	f_iBpp = a_iBpp;
	l_psSurface = SDL_SetVideoMode ( f_iWidth, f_iHeight, f_iBpp,
			SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_NOFRAME );
	if ( l_psSurface == NULL )
		{
		l_iError = g_iErrNo;
		l_oMessage = _ ( "Couldn't set 640x480x8 video mode: " );
		l_oMessage +=	SDL_GetError ( );
		M_THROW ( l_oMessage, l_iError );
		}
	f_pvHandler = l_psSurface;
	f_iBpp = l_psSurface->format->BitsPerPixel;
	if ( f_iBpp != a_iBpp )
		hcore::log << "BPP downgrade: " << a_iBpp << " -> " << f_iBpp << endl;
	hcore::log << "Set " << f_iWidth << 'x' << f_iHeight << " at " << f_iBpp << " bits-per-pixel mode" << endl;
	return ( 0 );
	M_EPILOG
	}

void HSurface::refresh ( void )
	{
	M_PROLOG
	SDL_Surface * l_psSurface = static_cast < SDL_Surface * > ( f_pvHandler );
	if ( SDL_MUSTLOCK ( l_psSurface ) )
		SDL_UnlockSurface ( l_psSurface );
	SDL_Flip ( l_psSurface );
/*	SDL_UpdateRect ( l_psSurface, 0, 0, f_iWidth, f_iHeight); */
	if ( SDL_MUSTLOCK ( l_psSurface ) )
		{
		if ( SDL_LockSurface ( l_psSurface ) < 0 )
			hcore::log ( D_LOG_ERROR ) << "Can't lock screen: " << SDL_GetError ( ) << endl;
		}
	return;
	M_EPILOG
	}

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
unsigned long int HSurface::get_pixel ( int x, int y )
	{
	SDL_Surface * l_psSurface = static_cast < SDL_Surface * > ( f_pvHandler );
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)l_psSurface->pixels + y * l_psSurface->pitch + x * f_iBpp;

	switch(f_iBpp)
		{
		case 1:
		return *p;

		case 2:
		return *(Uint16 *)p;

		case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;

		case 4:
		return *(unsigned long int *)p;

		default:
		return 0;       /* shouldn't happen, but avoids warnings */
		}
	}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void HSurface::put_pixel ( int x, int y, unsigned long int pixel )
	{
	SDL_Surface * l_psSurface = static_cast < SDL_Surface * > ( f_pvHandler );
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)l_psSurface->pixels + y * l_psSurface->pitch + x * f_iBpp;

	switch(f_iBpp)
		{
		case 1:
		*p = pixel;
		break;

		case 2:
		*(Uint16 *)p = pixel;
		break;

		case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
			} else {
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
		break;

		case 4:
		*(unsigned long int *)p = pixel;
		break;
		}
	}

void HSurface::line ( double x1, double y1, double x2, double y2, unsigned long int color )
	{
	double dx = x2 - x1;
	double dy = y2 - x1;
	double a = 0;
	if ( dx < 0 )
		{
		dx = x1;
		x1 = x2;
		x2 = dx;
		dx = y1;
		y1 = y2;
		y2 = dx;
		}
	if ( dx )
		{
		a = dy / dx;
		while ( x1 <= x2 )
			{
			y1 = a * x1;
			if ( ( x1 >= 0 ) && ( x1 < 640 ) && ( y1 >= 0 ) && ( y1 < 480 ) )
				put_pixel ( static_cast < int > ( x1 ), static_cast < int > ( y1 ), color );
			x1 ++;
			}
		}
	else
		{
		if ( dy < 0 )
			{
			dy = y1;
			y1 = y2;
			y2 = dy;
			}
		while ( y1 <= y2 )
			{
			if ( ( x1 >= 0 ) && ( x1 < 640 ) && ( y1 >= 0 ) && ( y1 < 480 ) )
				put_pixel ( static_cast < int > ( x1 ), static_cast < int > ( y1 ), color );
			y1 ++;
			}
		}
	return;
	}

unsigned long int HSurface::RGB ( int a_iRed, int a_iGreen, int a_iBlue )
	{
	return ( SDL_MapRGB ( static_cast < SDL_Surface * > ( f_pvHandler )->format,
				a_iRed, a_iGreen, a_iBlue ) );
	}


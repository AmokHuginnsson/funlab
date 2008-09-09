/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	renderer.cxx - this file is integral part of `funlab' project.

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
 FITNESS FOR f_dPrecountA PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libintl.h>
#include <SDL/SDL.h>

#include <yaal/yaal.h>
M_VCSID( "$Id: "__ID__" $" )

#include "hdetachedrenderer.h"
#include "setup.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab
{

HDetachedRenderer::HDetachedRenderer( HKeyboardEventListener* a_poKeyboardEventListener )
	: f_iRed( 0 ), f_iGreen( 0 ), f_iBlue( 0 ),
	f_dLowerXEdge( 0 ), f_dLowerYEdge( 0 ), f_dSize( 0 ), f_dResolution( 0 ),
	f_dAngleX( 0 ), f_dAngleY( 0 ), f_dAngleZ( 0 ),
	f_dDX( 0 ), f_dDY( 0 ), f_dDZ( 0 ), f_dFOV( 0 ),
	f_pdXVariable( NULL ), f_pdYVariable( NULL ),
	f_ppdLand( NULL ), f_oAnalyser(), f_oSurface(),
	f_bLoop( false ), f_bBusy( false ),
	f_ulColor( 0 ), f_dCosAlpha( 0 ), f_dSinAlpha( 0 ),
	f_dCosBeta( 0 ), f_dSinBeta( 0 ),
	f_dCosGamma( 0 ), f_dSinGamma( 0 ),
	f_dPrecountA( 0 ), f_dPrecountB( 0 ), f_dPrecountC( 0 ), f_pdTrygo( NULL ),
	f_oMutex(), f_oSemaphore(), f_oThread( *this ), f_poKeyboardEventListener( a_poKeyboardEventListener )
	{
	M_PROLOG
	int l_iCtr = 0;
	f_ppdLand = xcalloc < double * > ( setup.f_iDensity );
	for ( l_iCtr = 0; l_iCtr < setup.f_iDensity; l_iCtr ++ )
		f_ppdLand [ l_iCtr ] = xcalloc < double > ( setup.f_iDensity );
	for ( l_iCtr = 0; l_iCtr < 3; l_iCtr ++ )
		f_ppiNode [ l_iCtr ] = xcalloc < int > ( setup.f_iDensity );
	f_pdTrygo = xcalloc < double > ( 1024 );
	for ( l_iCtr = 0; l_iCtr < 1024; l_iCtr ++ )
		f_pdTrygo [ l_iCtr ] = sin( ( ( double ) l_iCtr * M_PI ) / 2048. );
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
	cout << "waiting ... " << flush;
	if ( HSurface::surface_count() )
		f_oSemaphore.wait();
	cout << "still waiting ... " << flush;
	while ( f_bBusy )
		;
	cout << "done" << endl;
	int l_iCtr = 0;
	if ( f_pdTrygo )
		xfree ( f_pdTrygo );
	for ( l_iCtr = 0; l_iCtr < 3; l_iCtr ++ )
		{
		if ( f_ppiNode [ l_iCtr ] )
			xfree ( f_ppiNode [ l_iCtr ] );
		f_ppiNode [ l_iCtr ] = NULL;
		}
	for ( l_iCtr = 0; l_iCtr < setup.f_iDensity; l_iCtr ++ )
		{
		if ( f_ppdLand [ l_iCtr ] )
			xfree ( f_ppdLand [ l_iCtr ] );
		f_ppdLand [ l_iCtr ] = NULL;
		}
	if ( f_ppdLand )
		xfree ( f_ppdLand );
	f_ppdLand = NULL;
	return;
	M_EPILOG
	}

void HDetachedRenderer::makeland( void )
	{
	M_PROLOG
	int i, j;
	double E, x, y;
	f_dLowerXEdge = - f_dSize;
	f_dLowerYEdge = - f_dSize;
	E = f_dSize;
	f_dResolution = ( E - f_dLowerYEdge ) / ( double ) setup.f_iDensity;
	y = f_dLowerXEdge;
	for ( j = 0; j < setup.f_iDensity; j ++ )
		{
		x = f_dLowerYEdge;
		( *f_pdYVariable ) = y;
		for ( i = 0; i < setup.f_iDensity; i ++ )
			{
			( * f_pdXVariable ) = x;
			f_ppdLand[ i ][ j ] = f_oAnalyser.count();
			x += f_dResolution;
			}
		y += f_dResolution;
		}
	return;
	M_EPILOG
	}

void HDetachedRenderer::precount ( void )
	{
	f_dCosAlpha = cosq( static_cast<unsigned int>( f_dAngleX ) );
	f_dSinAlpha = sinq( static_cast<unsigned int>( f_dAngleX ) );
	f_dCosBeta = cosq( static_cast<unsigned int>( f_dAngleY ) );
	f_dSinBeta = sinq( static_cast<unsigned int>( f_dAngleY ) );
	f_dCosGamma = cosq( static_cast<unsigned int>( f_dAngleZ ) );
	f_dSinGamma = sinq( static_cast<unsigned int>( f_dAngleZ ) );
	f_dPrecountA = f_dCosAlpha * f_dSinGamma;
	f_dPrecountB = f_dSinAlpha * f_dSinBeta;
	f_dPrecountC = f_dCosAlpha * f_dCosGamma;
	f_ulColor = f_oSurface->RGB( f_iRed, f_iGreen, f_iBlue );
	return;
	}

double HDetachedRenderer::sinq( unsigned int a_iAngle )
	{
	a_iAngle &= 4095;
	if ( a_iAngle > 2047 )
		{
		a_iAngle -= 2048;
		if ( a_iAngle > 1023 )
			a_iAngle = 2047 - a_iAngle;
		return ( - f_pdTrygo[ a_iAngle ] );
		}
	if ( a_iAngle > 1023 )
		a_iAngle = 2047 - a_iAngle;
	return ( f_pdTrygo [ a_iAngle ] );
	}

double HDetachedRenderer::cosq( unsigned int a_iAngle )
	{
	return ( sinq( a_iAngle + 1024 ) );
	}

bool HDetachedRenderer::T( double _x, double _y, double _z, int& _c, int& _r )
	{
	M_PROLOG
	double x = 0, y = 0, z = 0;
	x = _x * f_dCosBeta * f_dCosGamma - _y * f_dSinGamma * f_dCosBeta - _z * f_dSinBeta;
	y = _x * ( f_dPrecountA - f_dPrecountB * f_dCosGamma )
		+ _y * ( f_dPrecountC + f_dPrecountB * f_dSinGamma )
		- _z * f_dSinAlpha * f_dCosBeta;
	z = _x * ( f_dSinAlpha * f_dSinGamma + f_dSinBeta * f_dPrecountC )
		+ _y * ( f_dSinAlpha * f_dCosGamma - f_dSinBeta * f_dPrecountA )
		+ _z * f_dCosAlpha * f_dCosBeta;
	x += f_dDX;
	y += f_dDY;
	z += f_dDZ;

	if ( setup.f_bStereo )
		{
		int alpha = ( f_dDX > 0 ) ? 3 : - 3;
		double ox = x;
		x = x * cosq( alpha ) - y * sinq( alpha );
		y = y * cosq( alpha ) + ox * sinq( alpha );
		}
	
	if ( y > 0 )
		return ( false );
	if ( y == 0 )
		return ( false );
	_c = ( int ) ( -( x * f_dFOV ) / y );
	_r = ( int ) ( -( z * f_dFOV ) / y );
	_c = -_c;
	_c += ( setup.f_iResolutionX >> 1 );
	_r = -_r;
	_r += ( setup.f_iResolutionY >> 1 );
	return ( true );
	M_EPILOG
	}

void HDetachedRenderer::draw_frame ( void )
	{
	M_PROLOG
	if ( f_bBusy || ! f_oThread.is_alive() )
		return;
	f_bBusy = true;
	bool valid = false, oldvalid = false;
	int f = 0, i = 0, j = 0, c = 0, r = 0, oldc = 0, oldr = 0;
	double x = 0, y = 0;
	u32_t l_iRed = 0, l_iBlue = 0;
	for ( j = 0; j < 3; j++ )
		for ( i = 0; i < setup.f_iDensity; i++ )
			f_ppiNode[ j ][ i ] = 0;
	f_dFOV = 240.0;
	f_oSurface->clear();
	precount();
	if ( setup.f_bStereo )
		{
		l_iRed = f_oSurface->RGB( 0xff, 0, 0 );
		l_iBlue = f_oSurface->RGB( 0, 0, 0xff );
		}
	for ( f = 0; f < ( setup.f_bStereo ? 2 : 1 ); f ++ )
		{
		f_dDX = setup.f_bStereo ? ( f ? - 4 : 4 ) : 0;
		y = f_dLowerXEdge;
		for ( j = 0; j < setup.f_iDensity; j ++ )
			{
			x = f_dLowerYEdge;
			for ( i = 0; i < setup.f_iDensity; i ++ )
				{
				valid = T( x, y, f_ppdLand [ i ] [ j ], c, r );
				if ( valid && oldvalid && f_ppiNode [ 2 ] [ i ] )
					{
					if ( i > 0 )
						f_oSurface->line( oldc, oldr, c, r,
								setup.f_bStereo ? ( f ? l_iRed : l_iBlue ) : f_ulColor );
					if ( j > 0 )
						f_oSurface->line( c, r, f_ppiNode [ 0 ] [ i ],
								f_ppiNode [ 1 ] [ i ],
								setup.f_bStereo ? ( f ? l_iRed : l_iBlue ) : f_ulColor );
					}
				f_ppiNode [ 0 ] [ i ] = c;
				f_ppiNode [ 1 ] [ i ] = r;
				f_ppiNode [ 2 ] [ i ] = valid;
				oldc = c;
				oldr = r;
				oldvalid = valid;
				x += f_dResolution;
				}
			y += f_dResolution;
			}
		}
	usleep( 1000 );
	f_oSurface->refresh();
	f_bBusy = false;
	M_EPILOG
	}

bool HDetachedRenderer::render_surface( HString const& a_oFormula )
	{
	M_PROLOG
	
		{
	HLock l_oLock ( f_oMutex );
	if ( ! a_oFormula )
		return ( true );

	double* l_pdVariables = NULL;
	l_pdVariables = f_oAnalyser.analyse( a_oFormula );
	if ( ! l_pdVariables )
		return ( true );
	f_dSize = 10;
	f_dDY = - 15.0;
	f_pdXVariable = ( l_pdVariables + 'X' ) - 'A';
	f_pdYVariable = ( l_pdVariables + 'Y' ) - 'A';
	f_iRed = 8;
	f_iGreen = 8;
	f_iBlue = 0xf8;
	makeland();
		}
	if ( ! HSurface::surface_count() )
		{
		if ( ! f_oSurface )
			f_oSurface = HSurface::ptr_t( new HSurface );
		f_oSurface->init( setup.f_iResolutionX, setup.f_iResolutionY );
		SDL_WarpMouse( static_cast<Uint16>( setup.f_iResolutionX >> 1 ),
				static_cast<Uint16>( setup.f_iResolutionY >> 1 ) );
		f_bLoop = true;
		f_oThread.spawn();
		}
	else
		SDL_WarpMouse( static_cast<Uint16>( setup.f_iResolutionX >> 1 ),
				static_cast<Uint16>( setup.f_iResolutionY >> 1 ) );
	return ( false );
	M_EPILOG
	}

int HDetachedRenderer::operator() ( HThread const* const a_poCaller )
	{
	M_PROLOG
	int dx = 0, dy = 0;
	SDL_Event l_uEvent;
	while ( f_bLoop && a_poCaller->is_alive ( ) )
		{
		if ( SDL_WaitEvent( &l_uEvent ) && f_oSurface->is_valid() )
			{
			HLock l_oLock ( f_oMutex );
			HKeyboardEvent e;
			e.set_code( l_uEvent.key.keysym.sym );
			switch ( l_uEvent.type )
				{
				case ( SDL_MOUSEMOTION ):
					{
					dx = l_uEvent.motion.xrel > 0 ? l_uEvent.motion.xrel : - l_uEvent.motion.xrel;
					dy = l_uEvent.motion.yrel > 0 ? l_uEvent.motion.yrel : - l_uEvent.motion.yrel;
					if ( ( dx < ( setup.f_iResolutionX >> 1 ) ) && ( dy < ( setup.f_iResolutionY >> 1 ) ) )
						{
						switch ( l_uEvent.motion.state )
							{
							case ( SDL_BUTTON( 1 ) ):
								f_dAngleZ += l_uEvent.motion.xrel << 2;
								f_dAngleX -= l_uEvent.motion.yrel << 2;
							break;
							case ( SDL_BUTTON( 2 ) ):
								f_dSize += l_uEvent.motion.xrel;
								makeland();
							break;
							case ( SDL_BUTTON( 3 ) ):
								f_dAngleY += l_uEvent.motion.xrel << 2;
							break;
							default:
							break;
							}
						}
					break;
					}
				case ( SDL_MOUSEBUTTONDOWN ):
					{
					switch ( l_uEvent.button.button )
						{
						case ( 4 ):
							f_dDY ++;
						break;
						case ( 5 ):
							f_dDY --;
						break;
						default:
						break;
						}
					break;
					}
				case ( SDL_KEYUP ):
					{
					switch ( l_uEvent.key.keysym.sym )
						{
						case ( 'q' ):
							{
							if ( f_poKeyboardEventListener )
								{
								HKeyboardEvent e;
								e.set_code( l_uEvent.key.keysym.sym );
								f_poKeyboardEventListener->on_event( &e );
								}
							else
								{
								f_bLoop = false;
								f_oSurface->down();
								f_oSemaphore.signal();
								}
							}
						break;
						case ( 'f' ):
							f_oSurface->toggle_fullscreen();
						break;
						case ( 'r' ):
							{
							if ( l_uEvent.key.keysym.mod & ( KMOD_RSHIFT | KMOD_LSHIFT ) )
								{
								f_iRed += 240;
								f_iRed %= 256;
								}
							else
								{
								f_iRed += 16;
								f_iRed %= 256;
								}
							}
						break;
						case ( 'g' ):
							{
							if ( l_uEvent.key.keysym.mod & ( KMOD_RSHIFT | KMOD_LSHIFT ) )
								{
								f_iGreen += 240;
								f_iGreen %= 256;
								}
							else
								{
								f_iGreen += 16;
								f_iGreen %= 256;
								}
							}
						break;
						case ( 'b' ):
							{
							if ( l_uEvent.key.keysym.mod & ( KMOD_RSHIFT | KMOD_LSHIFT ) )
								{
								f_iBlue += 240;
								f_iBlue %= 256;
								}
							else
								{
								f_iBlue += 16;
								f_iBlue %= 256;
								}
							}
						break;
						default:
						break;
						}
					}
				break;
				default:
				break;
				}
			if ( f_bLoop )
				{
				draw_frame();
				while ( SDL_PollEvent( &l_uEvent ) )
					;
				}
			}
		}
	return ( 0 );
	M_EPILOG
	}

char const* HDetachedRenderer::error( void ) const
	{
	M_PROLOG
	return ( f_oAnalyser.get_error() );
	M_EPILOG
	}

int HDetachedRenderer::error_position( void ) const
	{
	M_PROLOG
	return ( f_oAnalyser.get_error_token() );
	M_EPILOG
	}

void HDetachedRenderer::shutdown( void )
	{
	f_oThread.finish();
	cout << __PRETTY_FUNCTION__ << endl;
	f_oSurface->down();
	f_oSurface = HSurface::ptr_t();
	}

}


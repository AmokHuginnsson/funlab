/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	renderer.c - this file is integral part of `funlab' project.

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

#include <stdhapi.h>

#include "renderer.h"
#include "gl.h"

using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::tools;

#define D_X_RES	640
#define D_Y_RES	480
#define D_LAND_SIZE 32

HRenderer::HRenderer ( void )
	: f_dLowerXEdge ( 0 ), f_dLowerYEdge ( 0 ), f_dSize ( 0 ), f_dResolution ( 0 ),
	f_dAngleX ( 0 ), f_dAngleY ( 0 ), f_dAngleZ ( 0 ),
	f_dDX ( 0 ), f_dDY ( 0 ), f_dDZ ( 0 ), f_dFOV ( 0 ),
	f_pdXVariable ( NULL ), f_pdYVariable ( NULL ),
	f_ppdLand ( NULL ), f_poAnalyser ( NULL ), f_poSurface ( NULL ), f_ulColor ( 0 ),
	f_dCosAlpha ( 0 ), f_dSinAlpha ( 0 ), f_dCosBeta ( 0 ), f_dSinBeta ( 0 ),
	f_dCosGamma ( 0 ), f_dSinGamma ( 0 ),
	f_dPrecountA ( 0 ), f_dPrecountB ( 0 ), f_dPrecountC ( 0 ), f_pdTrygo ( NULL )
	{
	M_PROLOG
	int l_iCtr = 0;
	f_ppdLand = xcalloc ( D_LAND_SIZE, double * );
	for ( l_iCtr = 0; l_iCtr < D_LAND_SIZE; l_iCtr ++ )
		f_ppdLand [ l_iCtr ] = xcalloc ( D_LAND_SIZE, double );
	for ( l_iCtr = 0; l_iCtr < 3; l_iCtr ++ )
		f_ppiNode [ l_iCtr ] = xcalloc ( D_LAND_SIZE, int );
	f_pdTrygo = xcalloc ( 1024, double );
	for ( l_iCtr = 0; l_iCtr < 1024; l_iCtr ++ )
		f_pdTrygo [ l_iCtr ] = sin( ( ( double ) l_iCtr * M_PI ) / 2048. );
	f_poAnalyser = new ( std::nothrow ) HAnalyser;
	M_ENSURE ( f_poAnalyser );
	f_poSurface = new ( std::nothrow ) HSurface;
	M_ENSURE ( f_poSurface );
	SDL_EventState ( SDL_MOUSEMOTION, SDL_ENABLE );
	SDL_EventState ( SDL_MOUSEBUTTONDOWN, SDL_ENABLE );
	SDL_EventState ( SDL_MOUSEBUTTONUP, SDL_ENABLE );
	SDL_EventState ( SDL_VIDEOEXPOSE, SDL_ENABLE );
	return;
	M_EPILOG
	}

HRenderer::~HRenderer ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( f_poAnalyser )
		delete f_poAnalyser;
	f_poAnalyser = NULL;
	if ( f_poSurface )
		delete f_poSurface;
	f_poSurface = NULL;
	if ( f_pdTrygo )
		xfree ( f_pdTrygo );
	for ( l_iCtr = 0; l_iCtr < 3; l_iCtr ++ )
		{
		if ( f_ppiNode [ l_iCtr ] )
			xfree ( f_ppiNode [ l_iCtr ] );
		f_ppiNode [ l_iCtr ] = NULL;
		}
	for ( l_iCtr = 0; l_iCtr < D_LAND_SIZE; l_iCtr ++ )
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

void HRenderer::makeland( void )
	{
	M_PROLOG
	int i, j;
	double E, S, x, y;
	f_dLowerXEdge = - f_dSize;
	f_dLowerYEdge = - f_dSize;
	E = f_dSize;
	S = f_dSize;
	f_dResolution = ( E - f_dLowerYEdge ) / ( double ) D_LAND_SIZE;
	y = f_dLowerXEdge;
	for ( j = 0; j < D_LAND_SIZE; j ++ )
		{
		x = f_dLowerYEdge;
		( * f_pdYVariable ) = y;
		for ( i = 0; i < D_LAND_SIZE; i ++ )
			{
			( * f_pdXVariable ) = x;
			f_ppdLand [ i ] [ j ] = f_poAnalyser->count ( );
			x += f_dResolution;
			}
		y += f_dResolution;
		}
	return;
	M_EPILOG
	}

void HRenderer::precount ( void )
	{
	f_dCosAlpha = cosq ( static_cast < unsigned int > ( f_dAngleX ) );
	f_dSinAlpha = sinq ( static_cast < unsigned int > ( f_dAngleX ) );
	f_dCosBeta = cosq ( static_cast < unsigned int > ( f_dAngleY ) );
	f_dSinBeta = sinq ( static_cast < unsigned int > ( f_dAngleY ) );
	f_dCosGamma = cosq ( static_cast < unsigned int > ( f_dAngleZ ) );
	f_dSinGamma = sinq ( static_cast < unsigned int > ( f_dAngleZ ) );
	f_dPrecountA = f_dCosAlpha * f_dSinGamma;
	f_dPrecountB = f_dSinAlpha * f_dSinBeta;
	f_dPrecountC = f_dCosAlpha * f_dCosGamma;
	f_ulColor = f_poSurface->RGB ( 0, 0xff, 0xff );
	return;
	}

double HRenderer::sinq( unsigned int a_iAngle )
	{
	a_iAngle &= 4095;
	if ( a_iAngle > 2047 )
		{
		a_iAngle -= 2048;
		if ( a_iAngle > 1023 ) a_iAngle = 2047-a_iAngle;
		return ( - f_pdTrygo [ a_iAngle ] );
		}
	if ( a_iAngle > 1023 ) a_iAngle = 2047-a_iAngle;
	return ( f_pdTrygo [ a_iAngle ] );
	}

double HRenderer::cosq( unsigned int a_iAngle )
	{
	return ( sinq( a_iAngle + 1024 ) );
	}

bool HRenderer::T( double _x, double _y, double _z, int & _c, int & _r )
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
	if ( y > 0 )
		return ( false );
	if ( y == 0 )
		return ( false );
	_c = ( int ) ( -( x * f_dFOV ) / y );
	_r = ( int ) ( -( z * f_dFOV ) / y );
	_c = -_c;
	_c += 320;
	_r = -_r;
	_r += 240;
	return ( true );
	M_EPILOG
	}

void HRenderer::draw_frame ( void )
	{
	M_PROLOG
	static bool l_bBusy = false;
	if ( l_bBusy )
		return;
	l_bBusy = true;
	bool valid = false, oldvalid = false;
	int i = 0, j = 0, c = 0, r = 0, oldc = 0, oldr = 0;
	double x = 0, y = 0;
	for ( j = 0; j < 3; j++ )
		for ( i = 0; i < D_LAND_SIZE; i++ )
			f_ppiNode [ j ] [ i ] = 0;
	y = f_dLowerXEdge;
	f_dDY = - 15.0;
	f_dFOV = 240.0;
	f_poSurface->clear ( );
	for ( j = 0; j < D_LAND_SIZE; j++ )
		{
		x = f_dLowerYEdge;
		for ( i = 0; i < D_LAND_SIZE; i++ )
			{
			valid = T( x, y, f_ppdLand [ i ] [ j ], c, r );
			if ( valid && oldvalid )
				{
				if ( ( c >= 0 ) && ( c < D_X_RES ) && ( r >= 0 ) && ( r < D_Y_RES )
					&& ( oldc >= 0 ) && ( oldc < D_X_RES ) && ( oldr >= 0 ) && ( oldr < D_Y_RES ) )
					{
					if ( i > 0 )
						f_poSurface->line( oldc, oldr, c, r, f_ulColor );
					}
				if ( f_ppiNode [ 2 ] [ i ]
					&& ( c >= 0 ) && ( c < D_X_RES ) && ( r >= 0 ) && ( r < D_Y_RES )
					&& ( f_ppiNode [ 0 ] [ i ] >= 0 ) && ( f_ppiNode [ 0 ] [ i ] < D_X_RES )
					&& ( f_ppiNode [ 1 ] [ i ] >= 0 ) && ( f_ppiNode [ 1 ] [ i ] < D_Y_RES ) )
					{
					if ( j > 0 )
						f_poSurface->line( c, r, f_ppiNode [ 0 ] [ i ], f_ppiNode [ 1 ] [ i ], f_ulColor );
					}
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
	M_LOG ( "ooo" );
	usleep ( 100000 );
	f_poSurface->refresh ( );
	l_bBusy = false;
	M_EPILOG
	}

void HRenderer::render_surface ( char const * a_pcFormula )
	{
	M_PROLOG
	if ( ! a_pcFormula )
		return;

	double * l_pdVariables = NULL;
	l_pdVariables = f_poAnalyser->analyse ( a_pcFormula );
	if ( ! l_pdVariables )
		return;
	if ( ! HSurface::surface_count ( ) )
		{
		f_poSurface->init ( D_X_RES, D_Y_RES );
		spawn ( );
		}
	f_dSize = 10;
	f_pdXVariable = ( l_pdVariables + 'X' ) - 'A';
	f_pdYVariable = ( l_pdVariables + 'Y' ) - 'A';
	makeland ( );
	precount ( );
//	draw_frame ( );
	return;
	M_EPILOG
	}

int HRenderer::run ( void )
	{
	M_PROLOG
	SDL_Event l_uEvent;
	while ( 1 )
		{
		if ( SDL_PollEvent ( & l_uEvent ) && f_poSurface->is_valid ( ) )
			{
			if ( l_uEvent.type == SDL_MOUSEMOTION )
				{
				f_dAngleZ += l_uEvent.motion.xrel;
				f_dAngleX -= l_uEvent.motion.yrel;
				draw_frame ( );
				}
			}
		listen ( );
		}
	M_EPILOG
	}


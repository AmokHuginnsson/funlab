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
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libintl.h>

#include <stdhapi.h>

#include "gl.h"

using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::tools;

#define D_LAND_SIZE 32

int node[ 3 ][ D_LAND_SIZE ];
float land[ D_LAND_SIZE ][ D_LAND_SIZE ];

float trygo[ 1024 ];

void createtabl()
	{
	int i;
	for ( i = 0; i < 1024; i++ )
		trygo[ i ] = sin( ( ( float ) i * M_PI ) / 2048. );
	return ;
	}

float sinq( unsigned int kat )
	{
	kat &= 4095;
	if ( kat > 2047 )
		{
		kat -= 2048;
		if ( kat > 1023 ) kat = 2047-kat;
		return ( - trygo [ kat ] );
		}
	if ( kat > 1023 ) kat = 2047-kat;
	return ( trygo [ kat ] );
	}

float cosq( unsigned int kat )
	{
	return ( sinq( kat + 1024 ) );
	}

bool T( float _x, float _y, float _z, float _xr, float _yr, float _zr, int _alfa, int _beta, int _gamma, float _fov, int & _c, int & _r )
	{
	float x_, y_, z_, A, B, C, sa, sb, sg, ca, cb, cg;
	sa = sinq( _alfa );
	sb = sinq( _beta );
	sg = sinq( _gamma );
	ca = cosq( _alfa );
	cb = cosq( _beta );
	cg = cosq( _gamma );
	A = ca * sg;
	B = sa * sb;
	C = ca * cg;
	x_ = _x * cb * cg - _y * sg * cb - _z * sb;
	y_ = _x * ( A - B * cg ) + _y * ( C + B * sg ) - _z * sa * cb;
	z_ = _x * ( sa * sg + sb * C ) + _y * ( sa * cg - sb * A ) + _z * ca * cb;
	x_ += _xr;
	y_ += _yr;
	z_ += _zr;
	if ( y_ > 0 )
		return ( false );
	if ( y_ == 0 )
		return ( false );
	_c = ( int ) ( -( x_ * _fov ) / y_ );
	_r = ( int ) ( -( z_ * _fov ) / y_ );
	_c = -_c;
	_c += 320;
	_r = -_r;
	_r += 240;
	return ( true );
	}

HAnalyser g_oRenderer;

void makeland( float H, float & _hx, float & _hy, float & N, float & W )
	{
	int i, j;
	float E, S, x, y;
	N = -H;
	E = H;
	S = H;
	W = -H;
	_hx = ( E - W ) / ( double ) D_LAND_SIZE;
	_hy = ( S - N ) / ( double ) D_LAND_SIZE;
	x = W;
	y = N;
	for ( j = 0; j < D_LAND_SIZE; j ++ )
		{
		x = W;
		g_oRenderer [ 'Y' ] = y;
		for ( i = 0; i < D_LAND_SIZE; i ++ )
			{
			g_oRenderer [ 'X' ] = x;
			land [ i ] [ j ] = g_oRenderer.count ( );
			x += _hx;
			}
		y += _hy;
		}
	return ;
	}

HSurface g_oSurface;
/**/
void show ( const HString & a_roFormula )
	{
	M_PROLOG
	if ( ! a_roFormula )
		return;

	if ( ! g_oRenderer.analyse ( a_roFormula ) )
		return;
	bool valid = false, oldvalid = false;
	int i = 0, j = 0, kx = 0, ky = 0, kz = 0, c = 0, r = 0, oldc = 0, oldr = 0;
	float hx = 0, hy = 0, x = 0, y = 0, N = 0, W = 0;
	unsigned long int blue;
	createtabl();
	makeland( 10, hx, hy, N, W );
	if ( ! HSurface::surface_count ( ) )
		g_oSurface.init ( 640, 480 );
	blue = g_oSurface.RGB ( 0, 0xff, 0xff );
	g_oSurface.clear ( );

	for ( j = 0; j < 3; j++ )
		for ( i = 0; i < D_LAND_SIZE; i++ )
			node [ j ] [ i ] = 0;
	y = N;
	for ( j = 0; j < D_LAND_SIZE; j++ )
		{
		x = W;
		for ( i = 0; i < D_LAND_SIZE; i++ )
			{
			valid = T( x, y, land [ i ] [ j ], 0, - 15.0, 0, kx, ky, kz, 240.0, c, r );
			if ( valid && oldvalid && node [ 2 ] [ i ]
					&& ( ( c < 640 ) && ( c >= 0 ) && ( r < 480 ) && ( r >= 0 ) ) )
				{
				if ( j > 0 )
					g_oSurface.line( c, r, node[ 0 ][ i ], node[ 1 ][ i ], blue );
				if ( i > 0 )
					g_oSurface.line( oldc, oldr, c, r, blue );
				}
			node [ 0 ] [ i ] = c;
			node [ 1 ] [ i ] = r;
			node [ 2 ] [ i ] = valid;
			oldc = c;
			oldr = r;
			oldvalid = valid;
			x += hx;
			}
		y += hy;
		}
	usleep ( 100000 );
	g_oSurface.refresh();
	return;
	M_EPILOG
	}


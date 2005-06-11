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
#include <libintl.h>
#include <SDL/SDL.h>

#include <stdhapi.h>

using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::tools;

#define D_LAND_SIZE 32

float node[ 2 ][ D_LAND_SIZE ];
float land[ D_LAND_SIZE ][ D_LAND_SIZE ];

extern float f( float, float );

float trygo[ 1024 ];
float *ptrtrygo;

void createtabl()
	{
	int i;
	for ( i = 0; i < 1024; i++ ) trygo[ i ] = sin( ( ( float ) i * M_PI ) / 2048. );
	ptrtrygo = &trygo[ 0 ];
	return ;
	}

float sinq( unsigned int kat )
	{
	kat &= 4095;
	if ( kat > 2047 )
		{
		kat -= 2048;
		if ( kat > 1023 ) kat = 2047-kat;
		return ( -*( ptrtrygo + kat ) );
		}
	if ( kat > 1023 ) kat = 2047-kat;
	return ( *( ptrtrygo + kat ) );
	}

float cosq( unsigned int kat )
	{
	return ( sinq( kat + 1024 ) );
	}

void T( float _x, float _y, float _z, float _xr, float _yr, float _zr, int _alfa, int _beta, int _gamma, float _fov, int & _c, int & _r )
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
	if ( y_ == 0 )
		{
		_c = ( int ) ( -( x_ * _fov * 100 ) );
		_r = ( int ) ( -( z_ * _fov * 100 ) );
		}
	else
		{
		_c = ( int ) ( -( x_ * _fov ) / y_ );
		_r = ( int ) ( -( z_ * _fov ) / y_ );
		}
	if ( y_ > 0 )
		{
		_c = 10000;
		_r = 10000;
		}
	_c = -_c;
	_c += 320;
	_r = -_r;
	_r += 240;
	return ;
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
	for ( j = 0; j < D_LAND_SIZE; j++ )
		{
		x = W;
		g_oRenderer [ 'Y' ] = j;
		for ( i = 0; i < D_LAND_SIZE; i++ )
			{
			g_oRenderer [ 'X' ] = i;
			land[ i ][ j ] = g_oRenderer.count ( );
			x += _hx;
			}
		y += _hy;
		}
	return ;
	}

/**/
void show ( const HString & a_roFormula )
	{
	M_PROLOG
		if ( ! a_roFormula )
			return;

	/*************/

	fprintf ( stderr, "%s\n", static_cast < char * > ( a_roFormula ) );
	g_oRenderer.analyse ( a_roFormula );



	int mx, my, kl, zn = 0;
	createtabl();
	//	mouse_setposition_6d( 320, 239, 0, 0, 0, 0, MOUSE_2DIM );
	//	refresh();
	//	gl_clearscreen( 0 );
	int i, j = 0, kx = 0, ky = 0, kz = 0, o = 240, d = -15, c, r, oc = 0, oldr = 0, col = 9, hor = 1, ver = 1;
	float hx, hy, x, y, N, W, dz = 10;
	makeland( 10, hx, hy, N, W );
	goto odswiez;
	//	while ( zn != 'q' )
		{
		//		zn = gl_kbhit();
		if ( zn == ' ' )
			{
			col++;
			col %= 16;
			zn = 200;
			goto odswiez;
			}
		if ( zn == 'h' )
			{
			hor = 1 - hor;
			zn = 200;
			goto odswiez;
			}
		if ( zn == 'v' )
			{
			ver = 1 - ver;
			zn = 200;
			goto odswiez;
			}
		//		mouse_update();
		//		mouse_getposition_6d( &mx, &my, NULL, NULL, NULL, NULL );
		//		kl = mouse_getbutton();
		if ( kl == 0 /* MOUSE_RIGHTBUTTON */ )
			{
			if ( ( mx > 330 ) || ( mx < 310 ) )
				{
				ky += ( mx >> 1 );
				ky -= 160;
				//				mouse_setposition_6d( 320, 240, 0, 0, 0, 0, 0 /* MOUSE_2DIM */ );
				}
			else if ( ( my > 260 ) || ( my < 220 ) )
				{
				o += ( my >> 2 );
				o -= 60;
				//				mouse_setposition_6d( 320, 240, 0, 0, 0, 0, 0 /* MOUSE_2DIM */ );
				}
			else if ( ( mx != 330 ) || ( my != 310 ) )
				{
				//				mouse_setposition_6d( 320, 240, 0, 0, 0, 0, 0 /* MOUSE_2DIM */ );
				//				continue;
				}
			//			else continue;
			}
		else if ( kl == 1 /* MOUSE_LEFTBUTTON */ )
			{
			if ( ( my > 270 ) || ( my < 210 ) )
				{
				if ( my > 240 ) d++;
				else d--;
				//				mouse_setposition_6d( 320, 240, 0, 0, 0, 0, MOUSE_2DIM );
				}
			else if ( ( mx > 350 ) || ( mx < 290 ) )
				{
				if ( mx > 320 ) dz++;
				else if ( dz > 1 ) dz--;
				makeland( dz, hx, hy, N, W );
				//				mouse_setposition_6d( 320, 240, 0, 0, 0, 0, MOUSE_2DIM );
				}
			else if ( ( mx != 320 ) || ( my != 240 ) )
				{
				//				mouse_setposition_6d( 320, 240, 0, 0, 0, 0, MOUSE_2DIM );
				//				continue;
				}
			//			else continue;
			}
		else if ( kl == 2 /* ( MOUSE_LEFTBUTTON | MOUSE_RIGHTBUTTON ) */ )
			{
			kx = ky = kz = 0;
			o = 240;
			d = -15;
			dz = 10;
			col = 9;
			hor = 1;
			ver = 1;
			makeland( dz, hx, hy, N, W );
			}
		else if ( ( mx != 320 ) || ( my != 240 ) )
			{
			kz += ( mx >> 2 );
			kx -= ( my >> 2 );
			kz -= 80;
			kx += 60;
			//			mouse_setposition_6d( 320, 240, 0, 0, 0, 0, MOUSE_2DIM );
			}
		//		else continue;
odswiez: ;
				 x = W; y = N;
				 for ( j = 0; j < D_LAND_SIZE; j++ )
					 {
					 x = W;
					 for ( i = 0; i < D_LAND_SIZE; i++ )
						 {
						 T( x, y, land[ i ][ j ], 0, d, 0, kx, ky, kz, o, c, r );
						 if (
								 ( ( ( c > 0 ) && ( node[ 0 ][ i ] > 0 ) ) && ( ( r > 0 ) && ( node[ 1 ][ i ] > 0 ) ) ) &&
								 ( ( ( c < 640 ) && ( node[ 0 ][ i ] < 640 ) ) && ( ( r < 480 ) && ( node[ 1 ][ i ] < 480 ) ) )
								)
							 {
//							 if ( ( j > 0 ) && ( ver ) )
//									line( screen, c, r, ( int ) node[ 0 ][ i ], ( int ) node[ 1 ][ i ], color );
//							 if ( ( i > 0 ) && ( hor ) )
//								 line( screen, oc, oldr, c, r, color );
							 }
						 node[ 0 ][ i ] = c;
						 node[ 1 ][ i ] = r;
						 oc = c;
						 oldr = r;
						 x += hx;
						 }
					 y += hy;
					 }

				 //for ( i = 0; i < 640; i ++ )
				 //	line( screen, 0, 0, i, 479, color );

				 //		refresh();
				 //		gl_clearscreen( 0 );
		}
	return;
	M_EPILOG
	}


/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfunlab.cxx - this file is integral part of `funlab' project.

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

#include <iostream>

#include <yaal/yaal.h>
M_VCSID( "$Id: "__ID__" $" )
#include "hfunlab.h"

#include "setup.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab
{

#define D_TRYGO_BASE 8192

HFunlab::HMesh::HMesh( void )
	: f_iSize( 0 ), f_iSurfaces( 0 ),
	f_oValues( f_iSize, values_t::D_AUTO_GROW ),
	f_oValuesBackbone( f_iSize, values_backbone_t::D_AUTO_GROW )
	{
	}

void HFunlab::HMesh::set_size( int size, int surfaces )
	{
	if ( ( surfaces > 0 ) && ( size > 0 ) )
		{
		f_iSize = size;
		f_iSurfaces = surfaces;
		f_oValues.pool_realloc( size * size * surfaces );
		f_oValuesBackbone.pool_realloc( size * surfaces );
		OValue* ptr = f_oValues.raw();
		OValue** bone = f_oValuesBackbone.raw();
		for ( int i = 0; i < ( f_iSurfaces * f_iSize ); ++ i, ptr += f_iSize, ++ bone )
			*bone = ptr;
		}
	return;
	}

int HFunlab::HMesh::get_size( void ) const
	{
	return ( f_iSize );
	}

HFunlab::HMesh::OValue** HFunlab::HMesh::fast( int surface )
	{
	return ( f_oValuesBackbone.raw() + f_iSize * surface );
	}

HFunlab::HFunlab( HRendererSurfaceInterface* a_poRenderer )
	: f_iRed( 0 ), f_iGreen( 0 ), f_iBlue( 0 ),
	f_dAngleX( 0 ), f_dAngleY( 0 ), f_dAngleZ( 0 ),
	f_dDX( 0 ), f_dDY( 0 ), f_dDZ( 0 ), f_dFOV( 0 ),
	f_pdXVariable( NULL ), f_pdYVariable( NULL ),
	f_oMesh(), f_oNode( 0, node_t::D_AUTO_GROW ),
	f_ulColor( 0 ), f_dCosAlpha( 0 ), f_dSinAlpha( 0 ),
	f_dCosBeta( 0 ), f_dSinBeta( 0 ),
	f_dCosGamma( 0 ), f_dSinGamma( 0 ),
	f_oCache( 0, 0, 0 ), f_pdTrygo( NULL ),
	f_oExpressions(), f_poRenderer( a_poRenderer ),
	f_oError(), f_iErrorIndex( 0 )
	{
	int i = 0;
	f_pdTrygo = xcalloc<double>( D_TRYGO_BASE );
	for ( i = 0; i < D_TRYGO_BASE; i ++ )
		f_pdTrygo[ i ] = sin( ( ( double ) i * M_PI ) / static_cast<double>( 2 * D_TRYGO_BASE ) );
	}

HFunlab::~HFunlab( void )
	{
	if ( f_pdTrygo )
		xfree( f_pdTrygo );
	}

void HFunlab::generate_surface( void )
	{
	M_PROLOG
	int size = f_oMesh.get_size();
	double long gridSize = ( setup.f_dDomainUpperBound - setup.f_dDomainLowerBound ) / static_cast<double long>( size );
	if ( size && setup.f_b3D )
		{
		int formula = 0;
		for ( expressions_t::iterator it = f_oExpressions.begin(); it != f_oExpressions.end(); ++ it, ++ formula )
			{
			double long* l_pdVariables = it->variables();
			f_pdXVariable = ( l_pdVariables + 'X' ) - 'A';
			f_pdYVariable = ( l_pdVariables + 'Y' ) - 'A';
			( *f_pdYVariable ) = setup.f_dDomainLowerBound;
			HMesh::OValue** values = f_oMesh.fast( formula );
			for ( int j = 0; j < size; ++ j )
				{
				( *f_pdXVariable ) = setup.f_dDomainLowerBound;
				for ( int i = 0; i < size; ++ i )
					{
					try
						{
						values[ i ][ j ]._value = static_cast<double>( it->evaluate() );
						values[ i ][ j ]._valid = true;
						}
					catch ( HExpressionException& )
						{
						values[ i ][ j ]._valid = false;
						}
					( *f_pdXVariable ) += gridSize;
					}
				( *f_pdYVariable ) += gridSize;
				}
			}
		}
	return;
	M_EPILOG
	}

void HFunlab::precalculate( void )
	{
	f_dCosAlpha = cosq( static_cast<int unsigned>( f_dAngleX ) );
	f_dSinAlpha = sinq( static_cast<int unsigned>( f_dAngleX ) );
	f_dCosBeta = cosq( static_cast<int unsigned>( f_dAngleY ) );
	f_dSinBeta = sinq( static_cast<int unsigned>( f_dAngleY ) );
	f_dCosGamma = cosq( static_cast<int unsigned>( f_dAngleZ ) );
	f_dSinGamma = sinq( static_cast<int unsigned>( f_dAngleZ ) );
	f_oCache.f_dPreCalcA = f_dCosAlpha * f_dSinGamma;
	f_oCache.f_dPreCalcB = f_dSinAlpha * f_dSinBeta;
	f_oCache.f_dPreCalcC = f_dCosAlpha * f_dCosGamma;
	f_ulColor = f_poRenderer->RGB( f_iRed, f_iGreen, f_iBlue );
	return;
	}

double HFunlab::sinq( int unsigned a_iAngle )
	{
	a_iAngle &= ( ( 4 * D_TRYGO_BASE ) - 1 );
	if ( a_iAngle > ( ( 2 * D_TRYGO_BASE ) - 1 ) )
		{
		a_iAngle -= ( 2 * D_TRYGO_BASE );
		if ( a_iAngle > ( D_TRYGO_BASE - 1 ) )
			a_iAngle = ( ( 2 * D_TRYGO_BASE ) - 1 ) - a_iAngle;
		return ( - f_pdTrygo[ a_iAngle ] );
		}
	if ( a_iAngle > ( D_TRYGO_BASE - 1 ) )
		a_iAngle = ( ( 2 * D_TRYGO_BASE ) - 1 ) - a_iAngle;
	return ( f_pdTrygo [ a_iAngle ] );
	}

double HFunlab::cosq( int unsigned a_iAngle )
	{
	return ( sinq( a_iAngle + D_TRYGO_BASE ) );
	}

bool HFunlab::T( double long _x, double long _y, double long _z, int& _c, int& _r )
	{
	M_PROLOG
	double long x = 0, y = 0, z = 0;
	x = _x * f_dCosBeta * f_dCosGamma - _y * f_dSinGamma * f_dCosBeta - _z * f_dSinBeta;
	y = _x * ( f_oCache.f_dPreCalcA - f_oCache.f_dPreCalcB * f_dCosGamma )
		+ _y * ( f_oCache.f_dPreCalcC + f_oCache.f_dPreCalcB * f_dSinGamma )
		- _z * f_dSinAlpha * f_dCosBeta;
	z = _x * ( f_dSinAlpha * f_dSinGamma + f_dSinBeta * f_oCache.f_dPreCalcC )
		+ _y * ( f_dSinAlpha * f_dCosGamma - f_dSinBeta * f_oCache.f_dPreCalcA )
		+ _z * f_dCosAlpha * f_dCosBeta;
	x += f_dDX;
	y += f_dDY;
	z += f_dDZ;

	if ( setup.f_bStereo )
		{
		int alpha = ( f_dDX > 0 ) ? 3 : - 3;
		double long ox = x;
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

void HFunlab::do_draw_frame( void )
	{
	M_PROLOG
	int size = f_oMesh.get_size();
	f_dFOV = 240.0;
	f_poRenderer->clear( f_poRenderer->RGB( 0, 0, 0 ) );
	precalculate();
	u32_t l_iRed = 0, l_iBlue = 0;
	if ( setup.f_bStereo )
		{
		l_iRed = f_poRenderer->RGB( 0xff, 0, 0 );
		l_iBlue = f_poRenderer->RGB( 0, 0, 0xff );
		}
	double long gridSize = ( setup.f_dDomainUpperBound - setup.f_dDomainLowerBound ) / static_cast<double long>( size );
	if ( setup.f_b3D )
		{
		if ( size )
			{
			if ( ! setup.f_bStereo && setup.f_bShowAxis )
				draw_axis();
			int sfSize = f_oExpressions.size();
			for ( int sf = 0; sf < sfSize; ++ sf )
				{
				bool valid = false, oldvalid = false;
				int f = 0, i = 0, j = 0, c = 0, r = 0, oldc = 0, oldr = 0;
				double long x = 0, y = 0;
				HMesh::OValue** values = f_oMesh.fast( sf );
				yaal::fill( f_oNode.raw(), f_oNode.raw() + size, ONode() );
				for ( f = 0; f < ( setup.f_bStereo ? 2 : 1 ); f ++ )
					{
					f_dDX = setup.f_bStereo ? ( f ? - 4 : 4 ) : 0;
					y = setup.f_dDomainLowerBound;
					for ( j = 0; j < size; ++ j )
						{
						x = setup.f_dDomainLowerBound;
						for ( i = 0; i < size; ++ i )
							{
							valid = values[ i ][ j ]._valid && T( x, y, values[ i ][ j ]._value, c, r );
							if ( valid && oldvalid && f_oNode[ i ]._valid )
								{
								if ( i > 0 )
									f_poRenderer->line( oldc, oldr, c, r,
											setup.f_bStereo ? ( f ? l_iRed : l_iBlue ) : f_ulColor );
								if ( j > 0 )
									f_poRenderer->line( c, r, f_oNode[ i ]._col,
											f_oNode[ i ]._row,
											setup.f_bStereo ? ( f ? l_iRed : l_iBlue ) : f_ulColor );
								}
							f_oNode[ i ]._col = c;
							f_oNode[ i ]._row = r;
							f_oNode[ i ]._valid = valid;
							oldc = c;
							oldr = r;
							oldvalid = valid;
							x += gridSize;
							}
						y += gridSize;
						}
					}
				}
			}
		}
	else
		{
		for ( expressions_t::iterator it = f_oExpressions.begin(); it != f_oExpressions.end(); ++ it )
			{
			double long* l_pdVariables = it->variables();
			f_pdXVariable = ( l_pdVariables + 'X' ) - 'A';
			( *f_pdXVariable ) = setup.f_dDomainLowerBound;
			double long gridSize = ( setup.f_dDomainUpperBound - setup.f_dDomainLowerBound ) / static_cast<double long>( setup.f_iResolutionX );
			double long oldVal = 0;
			for ( int x = 0; x < setup.f_iResolutionX; ++ x )
				{
				try
					{
					double long val = static_cast<double long>( it->evaluate() ) * setup.f_iResolutionY;
					if ( x )
						f_poRenderer->line( x, - val + setup.f_iResolutionY / 2, x - 1, - oldVal + setup.f_iResolutionY / 2, f_ulColor );
					oldVal = val;
					}
				catch ( HExpressionException& )
					{
					}
				( *f_pdXVariable ) += gridSize;
				}
			}
		}
	usleep( 1000 );
	f_poRenderer->commit();
	return;
	M_EPILOG
	}

void HFunlab::draw_axis( void )
	{
	u32_t D_WHITE = 0xffffffff;
	double long frac = 4.;
	int x1, x2, y1, y2;
	x1 = x2 = y1 = y2 = 0;
	if ( T( 0, 0, 0, x1, y1 ) && T( 0, 0, setup.f_dDomainUpperBound, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( 0, 0, 0, x1, y1 ) && T( 0, setup.f_dDomainUpperBound, 0, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( 0, 0, 0, x1, y1 ) && T( setup.f_dDomainUpperBound, 0, 0, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	int size = f_oMesh.get_size();
	double long gridSize = ( setup.f_dDomainUpperBound - setup.f_dDomainLowerBound ) / static_cast<double long>( size );
	double long x = setup.f_dDomainLowerBound;
	double long a = gridSize / frac;
	for ( int i = 0; i < size; ++ i )
		{
		if ( T( x, - a, 0, x1, y1 ) && T( x, a, 0, x2, y2 ) )
			f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
		if ( T( x, 0, - a, x1, y1 ) && T( x, 0, a, x2, y2 ) )
			f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
		if ( T( - a, x, 0, x1, y1 ) && T( a, x, 0, x2, y2 ) )
			f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
		if ( T( 0, x, - a, x1, y1 ) && T( 0, x, a, x2, y2 ) )
			f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
		if ( T( - a, 0, x, x1, y1 ) && T( a, 0, x, x2, y2 ) )
			f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
		if ( T( 0, - a, x, x1, y1 ) && T( 0, a, x, x2, y2 ) )
			f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
		x += gridSize;
		}
	double long t = setup.f_dDomainUpperBound + a;
	double long d = setup.f_dDomainUpperBound - a;
	if ( T( t, 0, 0, x1, y1 ) && T( d, a, 0, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( t, 0, 0, x1, y1 ) && T( d, - a, 0, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( t, 0, 0, x1, y1 ) && T( d, 0, a, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( t, 0, 0, x1, y1 ) && T( d, 0, - a, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );

	if ( T( 0, t, 0, x1, y1 ) && T( a, d, 0, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( 0, t, 0, x1, y1 ) && T( - a, d, 0, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( 0, t, 0, x1, y1 ) && T( 0, d, a, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( 0, t, 0, x1, y1 ) && T( 0, d, - a, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );

	if ( T( 0, 0, t, x1, y1 ) && T( a, 0, d, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( 0, 0, t, x1, y1 ) && T( - a, 0, d, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( 0, 0, t, x1, y1 ) && T( 0, a, d, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	if ( T( 0, 0, t, x1, y1 ) && T( 0, - a, d, x2, y2 ) )
		f_poRenderer->line( x1, y1, x2, y2, D_WHITE );
	}

void HFunlab::do_on_event( HMouseEvent const* e )
	{
	switch ( e->get_type() )
		{
		case ( HMouseEvent::TYPE::D_PRESS ):
			{
			switch ( e->get_button() )
				{
				case ( HMouseEvent::BUTTON::D_4 ):
					f_dDY ++;
				break;
				case ( HMouseEvent::BUTTON::D_5 ):
					f_dDY --;
				break;
				default:
				break;
				}
			}
		break;
		case ( HMouseEvent::TYPE::D_MOVE ):
			{
			switch ( e->get_button() )
				{
				case ( HMouseEvent::BUTTON::D_1 ):
					f_dAngleZ += e->get_x() << 2;
					f_dAngleX -= e->get_y() << 2;
				break;
				case ( HMouseEvent::BUTTON::D_2 ):
					setup.f_dDomainLowerBound -= e->get_x();
					setup.f_dDomainUpperBound += e->get_x();
					generate_surface();
				break;
				case ( HMouseEvent::BUTTON::D_3 ):
					f_dAngleY += e->get_x() << 2;
				break;
				default:
				break;
				}
			}
		default:
		break;
		}
	return;
	}

void HFunlab::do_on_event( HKeyboardEvent const* e )
	{
	switch ( e->get_code() )
		{
		case ( 'r' ):
			{
			if ( e->get_mod() & HKeyboardEvent::MOD::D_SHIFT )
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
			if ( e->get_mod() & ( HKeyboardEvent::MOD::D_SHIFT ) )
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
			if ( e->get_mod() & ( HKeyboardEvent::MOD::D_SHIFT ) )
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

bool HFunlab::push_formula( HString const& a_oFormula )
	{
	M_PROLOG
	if ( ! a_oFormula )
		return ( true );
	HExpression ex;
	try
		{
		ex.compile( a_oFormula );
		f_oExpressions.push_back( ex );
		f_dDY = - 15.0;
		f_iRed = 8;
		f_iGreen = 8;
		f_iBlue = 0xf8;
		f_iErrorIndex = 0;
		regen_cache( setup.f_iDensity );
		}
	catch ( HExpressionException& e )
		{
		f_oError = ex.get_error();
		f_iErrorIndex = ex.get_error_token();
		}
	return ( f_iErrorIndex ? true : false );
	M_EPILOG
	}

void HFunlab::regen_cache( int size )
	{
	M_PROLOG
	f_oNode.pool_realloc( size );
	f_oMesh.set_size( size, f_oExpressions.size() );
	if ( ! f_oExpressions.empty() )
		generate_surface();
	M_EPILOG
	}

char const* HFunlab::error( void ) const
	{
	M_PROLOG
	return ( f_oError.raw() );
	M_EPILOG
	}

int HFunlab::error_position( void ) const
	{
	M_PROLOG
	return ( f_iErrorIndex );
	M_EPILOG
	}

void HFunlab::clear( void )
	{
	M_PROLOG
	f_oExpressions.clear();
	return;
	M_EPILOG
	}

}


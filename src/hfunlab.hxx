/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfunlab.hxx - this file is integral part of `funlab' project.

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

#ifndef FUNLAB_HFUNLAB_HXX_INCLUDED
#define FUNLAB_HFUNLAB_HXX_INCLUDED

#include <yaal/tools/hexpression.hxx>

#include "hrenderer.hxx"

namespace funlab
{

class HFunlab : public HRendererEngineInterface
	{
	int f_iRed;
	int f_iGreen;
	int f_iBlue;
	double f_dAngleX;
	double f_dAngleY;
	double f_dAngleZ;
	double f_dDX;
	double f_dDY;
	double f_dDZ;
	double f_dFOV;
	double long* f_pdXVariable;
	double long* f_pdYVariable;

	class HMesh
		{
	public:
		struct OValue
			{
			bool _valid;
			double long _value;
			OValue( void ) : _valid( false ), _value( 0 ) {}
			};
	private:
		int f_iSize;
		int f_iSurfaces;
		typedef yaal::hcore::HPool<OValue> values_t;
		values_t f_oValues;
		typedef yaal::hcore::HPool<OValue*> values_backbone_t;
		values_backbone_t f_oValuesBackbone;
	public:
		HMesh( void );
		void set_size( int, int );
		int get_size( void ) const;
		OValue** fast( int );
		};
	HMesh f_oMesh;
	struct ONode
		{
		int _col;
		int _row;
		int _valid;
		ONode( void ) : _col( 0 ), _row( 0 ), _valid( 0 ) {}
		};
	typedef yaal::hcore::HPool<ONode> node_t;
	node_t f_oNode;
	yaal::u32_t f_ulColor;
	double f_dCosAlpha;
	double f_dSinAlpha;
	double f_dCosBeta;
	double f_dSinBeta;
	double f_dCosGamma;
	double f_dSinGamma;
	struct OCache
		{
		OCache( double a, double b, double c ) : f_dPreCalcA( a ), f_dPreCalcB( b ), f_dPreCalcC( c ) {}
		double f_dPreCalcA;
		double f_dPreCalcB;
		double f_dPreCalcC;
		} f_oCache;
	double* f_pdTrygo;
	typedef yaal::hcore::HList<yaal::tools::HExpression> expressions_t;
	expressions_t f_oExpressions;
	HRendererSurfaceInterface* f_poRenderer;
	yaal::hcore::HString f_oError;
	int f_iErrorIndex;
public:
	HFunlab( HRendererSurfaceInterface* );
	virtual ~HFunlab( void );
	bool push_formula( yaal::hcore::HString const& );
	char const* error( void ) const;
	int error_position( void ) const;
	void regen_cache( int );
	void clear( void );
protected:
	virtual void do_on_event( HMouseEvent const* );
	virtual void do_on_event( HKeyboardEvent const* );
private:
	double sinq( int unsigned );
	double cosq( int unsigned );
	bool T( double long, double long, double long, int&, int& );
	void precalculate( void );
	void generate_surface( void );
	void draw_axis( void );
	virtual void do_draw_frame( void );
	};

}

#endif /* not FUNLAB_HFUNLAB_HXX_INCLUDED */


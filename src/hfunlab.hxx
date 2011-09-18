/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfunlab.hxx - this file is integral part of `funlab' project.

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
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef FUNLAB_HFUNLAB_HXX_INCLUDED
#define FUNLAB_HFUNLAB_HXX_INCLUDED

#include <yaal/tools/hexpression.hxx>

#include "hrenderer.hxx"
#include "oplotdesc.hxx"

namespace funlab {

class HFunlab : public HRendererEngineInterface {
	int _red;
	int _green;
	int _blue;
	double _angleX;
	double _angleY;
	double _angleZ;
	double _dX;
	double _dY;
	double _dZ;
	double _fOV;
	double long* _xVariable;
	double long* _yVariable;

	class HMesh {
	public:
		struct OValue {
			bool _valid;
			double long _value;
			OValue( void ) : _valid( false ), _value( 0 ) {}
		};
	private:
		int _size;
		int _surfaces;
		yaal::hcore::HChunk _values;
		yaal::hcore::HChunk _valuesBackbone;
	public:
		HMesh( void );
		void set_size( int, int );
		int get_size( void ) const;
		OValue** fast( int );
	};
	HMesh _mesh;
	struct ONode {
		int _col;
		int _row;
		int _valid;
		ONode( void ) : _col( 0 ), _row( 0 ), _valid( 0 ) {}
	};
	yaal::hcore::HChunk _node;
	yaal::u32_t _color;
	double _cosAlpha;
	double _sinAlpha;
	double _cosBeta;
	double _sinBeta;
	double _cosGamma;
	double _sinGamma;
	struct OCache {
		OCache( double a, double b, double c ) : _preCalcA( a ), _preCalcB( b ), _preCalcC( c ) {}
		double _preCalcA;
		double _preCalcB;
		double _preCalcC;
	} _cache;
	double* _trygo;
	typedef yaal::hcore::HList<OPlotDesc> plots_t;
	plots_t _plots;
	HRendererSurfaceInterface* _renderer;
	yaal::hcore::HString _error;
	int _errorIndex;
public:
	HFunlab( HRendererSurfaceInterface* );
	virtual ~HFunlab( void );
	bool push_formula( OPlotDesc );
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


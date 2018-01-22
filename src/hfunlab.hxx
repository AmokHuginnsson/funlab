/* Read booker/LICENSE.md file for copyright and licensing information. */

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
	int _paralax;
	double _dX;
	double _dY;
	double _dZ;
	double _fov;
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
		bool _valid;
		ONode( void ) : _col( 0 ), _row( 0 ), _valid( true ) {}
	};
	yaal::hcore::HChunk _node;
	yaal::u32_t _color;
	double _cosAlpha;
	double _sinAlpha;
	double _cosBeta;
	double _sinBeta;
	double _cosGamma;
	double _sinGamma;
	double _cosParalax;
	double _sinParalax;
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
	yaal::hcore::HString const& error( void ) const;
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
	void precalculate( int = 0 );
	void generate_surface( void );
	void draw_axis( void );
	virtual void do_draw_frame( void );
	HFunlab( HFunlab const& );
	HFunlab& operator = ( HFunlab const& );
};

}

#endif /* not FUNLAB_HFUNLAB_HXX_INCLUDED */


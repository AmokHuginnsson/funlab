/* Read booker/LICENSE.md file for copyright and licensing information. */

#include <cmath>

#include <yaal/tools/hexpression.hxx>
#include <yaal/hcore/math.hxx>
M_VCSID( "$Id: " __ID__ " $" )
#include "hfunlab.hxx"

#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab {

static int const TRYGO_BASE = 8192;

HFunlab::HMesh::HMesh( void )
	: _size( 0 )
	, _surfaces( 0 )
	, _values()
	, _valuesBackbone() {
}

void HFunlab::HMesh::set_size( int size, int surfaces ) {
	if ( ( surfaces > 0 ) && ( size > 0 ) ) {
		_size = size;
		_surfaces = surfaces;
		_values.realloc( chunk_size<OValue>( size * size * surfaces ) );
		_valuesBackbone.realloc( chunk_size<OValue*>( size * surfaces ) );
		OValue* ptr = _values.get<OValue>();
		OValue** bone = _valuesBackbone.get<OValue*>();
		for ( int i = 0; i < ( _surfaces * _size ); ++ i, ptr += _size, ++ bone )
			*bone = ptr;
	}
	return;
}

int HFunlab::HMesh::get_size( void ) const {
	return ( _size );
}

HFunlab::HMesh::OValue** HFunlab::HMesh::fast( int surface ) {
	return ( _valuesBackbone.get<OValue*>() + _size * surface );
}

HFunlab::HFunlab( HRendererSurfaceInterface* renderer_ )
	: _red( 0 )
	, _green( 0 )
	, _blue( 0 )
	, _angleX( 0 )
	, _angleY( 0 )
	, _angleZ( 0 )
	, _paralax( 160 )
	, _dX( 0 )
	, _dY( 0 )
	, _dZ( 0 )
	, _fov( 0 )
	, _xVariable( nullptr )
	, _yVariable( nullptr )
	, _mesh()
	, _node()
	, _color( 0 )
	, _cosAlpha( 0 )
	, _sinAlpha( 0 )
	, _cosBeta( 0 )
	, _sinBeta( 0 )
	, _cosGamma( 0 )
	, _sinGamma( 0 )
	, _cosParalax( 0 )
	, _sinParalax( 0 )
	, _cache( 0, 0, 0 )
	, _trygo( nullptr )
	, _plots()
	, _renderer( renderer_ )
	, _error()
	, _errorIndex( 0 ) {
	int i = 0;
	_trygo = memory::calloc<double>( TRYGO_BASE );
	for ( i = 0; i < TRYGO_BASE; ++ i ) {
		_trygo[ i ] = sin( ( static_cast<double>( i ) * static_cast<double>( math::PI ) ) / static_cast<double>( 2 * TRYGO_BASE ) );
	}
}

HFunlab::~HFunlab( void ) {
	if ( _trygo )
		memory::free( _trygo );
}

void HFunlab::generate_surface( void ) {
	M_PROLOG
	int size( _mesh.get_size() );
	double long gridSize = ( setup._domainUpperBound - setup._domainLowerBound ) / static_cast<double long>( size );
	if ( size && setup._3D ) {
		int formula( 0 );
		for ( plots_t::iterator it = _plots.begin(); it != _plots.end(); ++ it, ++ formula ) {
			double long* variables = it->_expression.variables();
			_xVariable = ( variables + 'X' ) - 'A';
			_yVariable = ( variables + 'Y' ) - 'A';
			( *_yVariable ) = it->_domainLowerBound;
			HMesh::OValue** values = _mesh.fast( formula );
			for ( int j = 0; j < size; ++ j ) {
				( *_xVariable ) = it->_domainLowerBound;
				for ( int i = 0; i < size; ++ i ) {
					try {
						values[ i ][ j ]._value = static_cast<double>( it->_expression.evaluate() );
						values[ i ][ j ]._valid = true;
					} catch ( HExpressionException& ) {
						values[ i ][ j ]._valid = false;
					}
					( *_xVariable ) += gridSize;
				}
				( *_yVariable ) += gridSize;
			}
		}
	}
	return;
	M_EPILOG
}

void HFunlab::precalculate( int angle_ ) {
	_cosAlpha = cosq( static_cast<int unsigned>( _angleX ) );
	_sinAlpha = sinq( static_cast<int unsigned>( _angleX ) );
	_cosBeta = cosq( static_cast<int unsigned>( _angleY ) );
	_sinBeta = sinq( static_cast<int unsigned>( _angleY ) );
	_cosGamma = cosq( static_cast<int unsigned>( _angleZ ) );
	_sinGamma = sinq( static_cast<int unsigned>( _angleZ ) );
	_cosParalax = cosq( static_cast<int unsigned>( angle_ ) );
	_sinParalax = sinq( static_cast<int unsigned>( angle_ ) );
	_cache._preCalcA = _cosAlpha * _sinGamma;
	_cache._preCalcB = _sinAlpha * _sinBeta;
	_cache._preCalcC = _cosAlpha * _cosGamma;
	_color = _renderer->RGB( _red, _green, _blue );
	return;
}

double HFunlab::sinq( int unsigned angle_ ) {
	angle_ &= ( ( 4 * TRYGO_BASE ) - 1 );
	if ( angle_ > ( ( 2 * TRYGO_BASE ) - 1 ) ) {
		angle_ -= ( 2 * TRYGO_BASE );
		if ( angle_ > ( TRYGO_BASE - 1 ) )
			angle_ = ( ( 2 * TRYGO_BASE ) - 1 ) - angle_;
		return ( - _trygo[ angle_ ] );
	}
	if ( angle_ > ( TRYGO_BASE - 1 ) )
		angle_ = ( ( 2 * TRYGO_BASE ) - 1 ) - angle_;
	return ( _trygo [ angle_ ] );
}

double HFunlab::cosq( int unsigned angle_ ) {
	return ( sinq( angle_ + TRYGO_BASE ) );
}

bool HFunlab::T( double long _x, double long _y, double long _z, int& _c, int& _r ) {
	M_PROLOG
	double long x( 0 );
	double long y( 0 );
	double long z( 0 );
	x = _x * _cosBeta * _cosGamma - _y * _sinGamma * _cosBeta - _z * _sinBeta;
	y = _x * ( _cache._preCalcA - _cache._preCalcB * _cosGamma )
		+ _y * ( _cache._preCalcC + _cache._preCalcB * _sinGamma )
		- _z * _sinAlpha * _cosBeta;
	z = _x * ( _sinAlpha * _sinGamma + _sinBeta * _cache._preCalcC )
		+ _y * ( _sinAlpha * _cosGamma - _sinBeta * _cache._preCalcA )
		+ _z * _cosAlpha * _cosBeta;

	if ( setup._stereo ) {
		double long ox( x );
		x = x * _cosParalax - y * _sinParalax;
		y = y * _cosParalax + ox * _sinParalax;
	}

	x += _dX;
	y += _dY;
	z += _dZ;

	if ( y > 0 )
		return ( false );
	if ( y == 0 )
		return ( false );
	_c = static_cast<int>( -( x * _fov ) / y );
	_r = static_cast<int>( -( z * _fov ) / y );
	_c = -_c;
	_c += ( setup._resolutionX >> 1 );
	_r = -_r;
	_r += ( setup._resolutionY >> 1 );
	return ( true );
	M_EPILOG
}

void HFunlab::do_draw_frame( void ) {
	M_PROLOG
	int size = _mesh.get_size();
	_fov = setup._stereo ? 600.0 : 240.0;
	if ( setup._stereo )
		_renderer->clear( _renderer->RGB( 0xff, 0xff, 0xff ) );
	else
		_renderer->clear( _renderer->RGB( 0, 0, 0 ) );
	precalculate();
	u32_t red = 0, blue = 0;
	if ( setup._stereo ) {
		red = _renderer->RGB( 0xff, 0xb0, 0xb0 );
		blue = _renderer->RGB( 0x50, 0xff, 0xff );
	}
	double long gridSize = ( setup._domainUpperBound - setup._domainLowerBound ) / static_cast<double long>( size );
	if ( ! setup._stereo && setup._showAxis ) {
		draw_axis();
	}
	if ( setup._3D ) {
		if ( size ) {
			int sfSize = static_cast<int>( _plots.size() );
			for ( int sf = 0; sf < sfSize; ++ sf ) {
				bool valid = false, oldvalid = false;
				int f = 0, i = 0, j = 0, c = 0, r = 0, oldc = 0, oldr = 0;
				double long x = 0, y = 0;
				HMesh::OValue** values = _mesh.fast( sf );
				ONode* nodes = _node.get<ONode>();
				for ( f = 0; f < ( setup._stereo ? 2 : 1 ); f ++ ) {
					yaal::fill( nodes, nodes + size, ONode() );
					precalculate( setup._stereo ? ( f ? -_paralax : _paralax ) : 0 );
					y = setup._domainLowerBound;
					for ( j = 0; j < size; ++ j ) {
						x = setup._domainLowerBound;
						for ( i = 0; i < size; ++ i ) {
							valid = values[ i ][ j ]._valid && T( x, y, values[ i ][ j ]._value, c, r );
							if ( valid && oldvalid && nodes[ i ]._valid ) {
								if ( i > 0 )
									_renderer->line( oldc, oldr, c, r,
											setup._stereo ? ( f ? red : blue ) : _color );
								if ( j > 0 )
									_renderer->line( c, r, nodes[ i ]._col,
											nodes[ i ]._row,
											setup._stereo ? ( f ? red : blue ) : _color );
							}
							nodes[ i ]._col = c;
							nodes[ i ]._row = r;
							nodes[ i ]._valid = valid;
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
	} else {
		for ( plots_t::iterator it = _plots.begin(); it != _plots.end(); ++ it ) {
			double long* variables = it->_expression.variables();
			_xVariable = ( variables + 'X' ) - 'A';
			( *_xVariable ) = it->_domainLowerBound;
			gridSize = ( it->_domainUpperBound - it->_domainLowerBound ) / static_cast<double long>( setup._resolutionX );
			double long oldVal( 0 );
			for ( int x( 0 ); x < setup._resolutionX; ++ x ) {
				try {
					double long val( static_cast<double long>( it->_expression.evaluate() ) * setup._resolutionY / ( it->_rangeUpperBound - it->_rangeLowerBound ) );
					if ( x ) {
						_renderer->line( x, - static_cast<double>( val ) + setup._resolutionY / 2, x - 1, - static_cast<double>( oldVal ) + setup._resolutionY / 2, _color );
					}
					oldVal = val;
				} catch ( HExpressionException& ) {
				}
				( *_xVariable ) += gridSize;
			}
		}
	}
	_renderer->commit();
	return;
	M_EPILOG
}

void HFunlab::draw_axis( void ) {
	if ( setup._3D ) {
		u32_t WHITE = 0xffffffff;
		double long frac( 4. );
		int x1( 0 );
		int x2( 0 );
		int y1( 0 );
		int y2( 0 );
		if ( T( 0, 0, 0, x1, y1 ) && T( 0, 0, setup._domainUpperBound, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( 0, 0, 0, x1, y1 ) && T( 0, setup._domainUpperBound, 0, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( 0, 0, 0, x1, y1 ) && T( setup._domainUpperBound, 0, 0, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		int size = _mesh.get_size();
		double long gridSize = ( setup._domainUpperBound - setup._domainLowerBound ) / static_cast<double long>( size );
		double long x = setup._domainLowerBound;
		double long a = gridSize / frac;
		for ( int i = 0; i < size; ++ i ) {
			if ( T( x, - a, 0, x1, y1 ) && T( x, a, 0, x2, y2 ) )
				_renderer->line( x1, y1, x2, y2, WHITE );
			if ( T( x, 0, - a, x1, y1 ) && T( x, 0, a, x2, y2 ) )
				_renderer->line( x1, y1, x2, y2, WHITE );
			if ( T( - a, x, 0, x1, y1 ) && T( a, x, 0, x2, y2 ) )
				_renderer->line( x1, y1, x2, y2, WHITE );
			if ( T( 0, x, - a, x1, y1 ) && T( 0, x, a, x2, y2 ) )
				_renderer->line( x1, y1, x2, y2, WHITE );
			if ( T( - a, 0, x, x1, y1 ) && T( a, 0, x, x2, y2 ) )
				_renderer->line( x1, y1, x2, y2, WHITE );
			if ( T( 0, - a, x, x1, y1 ) && T( 0, a, x, x2, y2 ) )
				_renderer->line( x1, y1, x2, y2, WHITE );
			x += gridSize;
		}
		double long t = setup._domainUpperBound + a;
		double long d = setup._domainUpperBound - a;
		if ( T( t, 0, 0, x1, y1 ) && T( d, a, 0, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( t, 0, 0, x1, y1 ) && T( d, - a, 0, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( t, 0, 0, x1, y1 ) && T( d, 0, a, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( t, 0, 0, x1, y1 ) && T( d, 0, - a, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );

		if ( T( 0, t, 0, x1, y1 ) && T( a, d, 0, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( 0, t, 0, x1, y1 ) && T( - a, d, 0, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( 0, t, 0, x1, y1 ) && T( 0, d, a, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( 0, t, 0, x1, y1 ) && T( 0, d, - a, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );

		if ( T( 0, 0, t, x1, y1 ) && T( a, 0, d, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( 0, 0, t, x1, y1 ) && T( - a, 0, d, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( 0, 0, t, x1, y1 ) && T( 0, a, d, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
		if ( T( 0, 0, t, x1, y1 ) && T( 0, - a, d, x2, y2 ) )
			_renderer->line( x1, y1, x2, y2, WHITE );
	} else {
		u32_t AXIS_COLOR( 0xdddddddd );
		u32_t GRID_COLOR( 0x88888888 );
		int stepX( static_cast<int>( setup._resolutionX / ( setup._domainUpperBound - setup._domainLowerBound ) ) );
		for ( int i( 0 ); i < setup._resolutionX; i += stepX ) {
			_renderer->line( i, 0, i, setup._resolutionY, GRID_COLOR );
		}
		int stepY( static_cast<int>( setup._resolutionY / ( setup._rangeUpperBound - setup._rangeLowerBound ) ) );
		for ( int i( 0 ); i < setup._resolutionY; i += stepY ) {
			_renderer->line( 0, i, setup._resolutionX, i, GRID_COLOR );
		}
		_renderer->line( 0, setup._resolutionY / 2, setup._resolutionX, setup._resolutionY / 2, AXIS_COLOR );
		_renderer->line( setup._resolutionX / 2, 0, setup._resolutionX / 2, setup._resolutionY, AXIS_COLOR );
	}
}

void HFunlab::do_on_event( HMouseEvent const* e ) {
	switch ( e->get_type() ) {
		case ( HMouseEvent::TYPE::PRESS ): {
			switch ( e->get_button() ) {
				case ( HMouseEvent::BUTTON::B_4 ):
					_dY ++;
				break;
				case ( HMouseEvent::BUTTON::B_5 ):
					_dY --;
				break;
				default:
				break;
			}
		}
		break;
		case ( HMouseEvent::TYPE::MOVE ): {
			switch ( static_cast<int>( e->get_button() ) ) {
				case ( HMouseEvent::BUTTON::B_1 ):
					_angleZ += e->get_x() << 2;
					_angleX -= e->get_y() << 2;
				break;
				case ( HMouseEvent::BUTTON::B_2 ):
					setup._domainLowerBound -= e->get_x();
					setup._domainUpperBound += e->get_x();
					generate_surface();
				break;
				case ( HMouseEvent::BUTTON::B_3 ):
					_angleY += e->get_x() << 2;
				break;
				case ( HMouseEvent::BUTTON::B_1 | HMouseEvent::BUTTON::B_3 ):
					_paralax += e->get_y() << 2;
					if ( _paralax < 0 )
						_paralax = 0;
					else if ( _paralax > 1024 )
						_paralax = 1024;
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

void HFunlab::do_on_event( HKeyboardEvent const* e ) {
	switch ( e->get_code() ) {
		case ( 'r' ): {
			if ( e->get_mod() & HKeyboardEvent::MOD::SHIFT ) {
				_red += 240;
				_red %= 256;
			} else {
				_red += 16;
				_red %= 256;
			}
		}
		break;
		case ( 'g' ): {
			if ( e->get_mod() & ( HKeyboardEvent::MOD::SHIFT ) ) {
				_green += 240;
				_green %= 256;
			} else {
				_green += 16;
				_green %= 256;
			}
		}
		break;
		case ( 'b' ): {
			if ( e->get_mod() & ( HKeyboardEvent::MOD::SHIFT ) ) {
				_blue += 240;
				_blue %= 256;
			} else {
				_blue += 16;
				_blue %= 256;
			}
		}
		break;
		default:
		break;
	}
}

bool HFunlab::push_formula( OPlotDesc formula_ ) {
	M_PROLOG
	if ( ! formula_._formula )
		return ( true );
	try {
		formula_._expression.compile( formula_._formula );
		_plots.push_back( formula_ );
		_dY = - 15.0;
		_red = 8;
		_green = 8;
		_blue = 0xf8;
		_errorIndex = 0;
		regen_cache( setup._density );
	} catch ( HExpressionException& e ) {
		_error = formula_._expression.get_error();
		_errorIndex = formula_._expression.get_error_token();
	}
	return ( _errorIndex ? true : false );
	M_EPILOG
}

void HFunlab::regen_cache( int size ) {
	M_PROLOG
	_node.realloc( chunk_size<ONode>( size ) );
	_mesh.set_size( size, static_cast<int>( _plots.size() ) );
	if ( ! _plots.empty() )
		generate_surface();
	M_EPILOG
}

HString const& HFunlab::error( void ) const {
	M_PROLOG
	return ( _error );
	M_EPILOG
}

int HFunlab::error_position( void ) const {
	M_PROLOG
	return ( _errorIndex );
	M_EPILOG
}

void HFunlab::clear( void ) {
	M_PROLOG
	_plots.clear();
	return;
	M_EPILOG
}

}


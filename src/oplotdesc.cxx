/* Read booker/LICENSE.md file for copyright and licensing information. */

#include <yaal/hcore/htokenizer.hxx>
M_VCSID( "$Id: " __ID__ " $" )
#include "oplotdesc.hxx"

#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace funlab {

OPlotDesc::OPlotDesc( yaal::hcore::HString const& formula )
	: _domainLowerBound( setup._domainLowerBound )
	, _domainUpperBound( setup._domainUpperBound )
	, _rangeLowerBound( setup._rangeLowerBound )
	, _rangeUpperBound( setup._domainUpperBound )
	, _formula( formula ), _expression() {
}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& stream, OPlotDesc const& plot ) {
	M_PROLOG
	stream << plot._domainLowerBound << ";"
		<< plot._domainUpperBound << ";"
		<< plot._rangeLowerBound << ";"
		<< plot._rangeUpperBound << ";"
		<< plot._formula;
	return ( stream );
	M_EPILOG
}

OPlotDesc plot_desc_from_string( yaal::hcore::HString const& line ) {
	M_PROLOG
	OPlotDesc plot;
	HTokenizer t( line, ";" );
	plot._domainLowerBound = lexical_cast<double long>( t[ 0 ] );
	plot._domainUpperBound = lexical_cast<double long>( t[ 1 ] );
	plot._rangeLowerBound = lexical_cast<double long>( t[ 2 ] );
	plot._rangeUpperBound = lexical_cast<double long>( t[ 3 ] );
	plot._formula = t[ 4 ];
	return ( plot );
	M_EPILOG
}

}


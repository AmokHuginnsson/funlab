/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	oplotdesc.cxx - this file is integral part of `funlab' project.

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

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "oplotdesc.hxx"

#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace funlab
{

OPlotDesc::OPlotDesc( yaal::hcore::HString const& formula )
	: _3d( setup.f_b3D ),
	_domainLowerBound( setup.f_dDomainLowerBound ),
	_domainUpperBound( setup.f_dDomainUpperBound ),
	_rangeLowerBound( setup.f_dRangeLowerBound ),
	_rangeUpperBound( setup.f_dDomainUpperBound ),
	_formula( formula )
	{
	}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& stream, OPlotDesc const& plot )
	{
	M_PROLOG
	stream << ( plot._3d ? "true" : "false" ) << ";"
		<< plot._domainLowerBound << ";"
		<< plot._domainUpperBound << ";"
		<< plot._rangeLowerBound << ";"
		<< plot._rangeUpperBound << ";"
		<< plot._formula;
	return ( stream );
	M_EPILOG
	}

OPlotDesc plot_desc_from_string( yaal::hcore::HString const& line )
	{
	M_PROLOG
	OPlotDesc plot;
	HTokenizer t( line, ";" );
	plot._3d = lexical_cast<bool>( t[ 0 ] );
	plot._domainLowerBound = lexical_cast<double long>( t[ 1 ] );
	plot._domainUpperBound = lexical_cast<double long>( t[ 2 ] );
	plot._rangeLowerBound = lexical_cast<double long>( t[ 3 ] );
	plot._rangeUpperBound = lexical_cast<double long>( t[ 4 ] );
	plot._formula = t[ 5 ];
	return ( plot );
	M_EPILOG
	}

}


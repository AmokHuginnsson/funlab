/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	setup.cxx - this file is integral part of `funlab' project.

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

#include <libintl.h>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "setup.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab
{
OSetup::OSetup( void )
	: _quiet( false ), _verbose( false ),
		_stereo( false ),
		f_b3D( true ), _showAxis( false ),
		_multiFormula( false ),
		_density( 32 ), _resolutionX( 640 ),
		_resolutionY( 480 ), _aspect( 1 ),
		_domainLowerBound( -1 ), _domainUpperBound( 1 ),
		_rangeLowerBound( -1 ), _rangeUpperBound( 1 ),
		_programName( NULL ), _logPath(),
		_formula(), _resourcePath(),
		_iconPath()
	{}

void OSetup::test_setup( void )
	{
	M_PROLOG
	if ( _quiet && _verbose )
		yaal::tools::util::failure( 1,
				_( "quiet and verbose options are exclusive\n" ) );
	return;
	M_EPILOG
	}

}


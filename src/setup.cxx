/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	setup.cxx - this file is integral part of `funlab' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <libintl.h>
#include <cstdio>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "setup.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab {
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

void OSetup::test_setup( void ) {
	M_PROLOG
	if ( _quiet && _verbose )
		yaal::tools::util::failure( 1,
				_( "quiet and verbose options are exclusive\n" ) );
	if ( _verbose )
		clog.reset( make_pointer<HFile>( stdout ) );
	if ( _quiet )
		cout.reset();
	return;
	M_EPILOG
}

}


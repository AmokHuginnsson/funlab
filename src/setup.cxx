/* Read booker/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>
#include <cstdio>

#include "setup.hxx"
M_VCSID( "$Id: " __ID__ " $" )
#include <yaal/tools/util.hxx>

using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab {
OSetup::OSetup( void )
	: _quiet( false ), _verbose( false ),
		_stereo( false ),
		_3D( true ), _showAxis( false ),
		_multiFormula( false ),
		_density( 32 ), _resolutionX( 640 ),
		_resolutionY( 480 ), _aspect( 1 ),
		_domainLowerBound( -1 ), _domainUpperBound( 1 ),
		_rangeLowerBound( -1 ), _rangeUpperBound( 1 ),
		_programName( NULL ), _logPath(),
		_formula(), _resourcePath(),
		_iconPath() {
}

void OSetup::test_setup( void ) {
	M_PROLOG
	if ( _quiet && _verbose ) {
		yaal::tools::util::failure( 1,
				_( "quiet and verbose options are exclusive\n" ) );
	}
	if ( _verbose ) {
		clog.reset( make_pointer<HFile>( stdout, HFile::OWNERSHIP::EXTERNAL ) );
	}
	if ( _quiet ) {
		cout.reset();
	}
	return;
	M_EPILOG
}

}


/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	setup.cxx - this file is integral part of `funlab' project.

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

#include <libintl.h>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "setup.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab
{
OSetup::OSetup( void )
	: f_bQuiet( false ), f_bVerbose( false ),
		f_bHelp( false ), f_bStereo( false ),
		f_b3D( true ), f_bShowAxis( false ),
		f_bMultiFormula( false ),
		f_iDensity( 32 ), f_iResolutionX( 640 ),
		f_iResolutionY( 480 ), f_dAspect( 1 ),
		f_dDomainLowerBound( -1 ), f_dDomainUpperBound( 1 ),
		f_dRangeLowerBound( -1 ), f_dRangeUpperBound( 1 ),
		f_pcProgramName( NULL ), f_oLogPath(),
		f_oFormula(), f_oResourcePath(),
		f_oIconPath()
	{}

void OSetup::test_setup( void )
	{
	M_PROLOG
	if ( f_bQuiet && f_bVerbose )
		yaal::tools::util::failure( 1,
				_( "quiet and verbose options are exclusive\n" ) );
	return;
	M_EPILOG
	}

}


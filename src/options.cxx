/*
---       `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski         ---

	rc_options.cxx - this file is integral part of `funlab' project.

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

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )

#include "options.hxx"
#include "version.hxx"
#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace funlab
{

bool set_variables( HString& a_roOption, HString& a_roValue )
	{
	::fprintf( stdout, "option: [%s], value: [%s]\n",
			a_roOption.raw(), a_roValue.raw() );
	return ( false );
	}

void version( void* ) __attribute__ ( ( __noreturn__ ) );
void version( void* )
	{
	printf ( "`funlab' %s\n", VER );
	throw ( 0 );
	}

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */
int handle_program_options( int a_iArgc, char** a_ppcArgv )
	{
	M_PROLOG
	HProgramOptionsHandler po;
	OOptionInfo info( po, setup.f_pcProgramName, "renders three dimensional function surfaces", NULL );
	bool stop = false;
	po( "log_path", program_options_helper::option_value( setup.f_oLogPath ), NULL, HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path", "path pointing to file for application logs" )
		( "resource_path", program_options_helper::option_value( setup.f_oResourcePath ), NULL, HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path", "path to XUL resources" )
		( "icon_path", program_options_helper::option_value( setup.f_oIconPath ), NULL, HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path", "path to icon resources" )
		( "resolution-x", program_options_helper::option_value( setup.f_iResolutionX ), "X", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "val", "set x resolution to val value" )
		( "resolution-y", program_options_helper::option_value( setup.f_iResolutionY ), "Y", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "val", "set y resolution to val value" )
		( "aspect", program_options_helper::option_value( setup.f_dAspect ), "a", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "expr", "set aspect of drawing to value of expr" )
		( "domain-lower-bound", program_options_helper::option_value( setup.f_dDomainLowerBound ), "[", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "val", "set lower bound for domain interval to val" )
		( "domain-upper-bound", program_options_helper::option_value( setup.f_dDomainUpperBound ), "]", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "val", "set upper bound for domain interval to val" )
		( "range-lower-bound", program_options_helper::option_value( setup.f_dRangeLowerBound ), "{", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "val", "set lower bound for range interval to val" )
		( "range-upper-bound", program_options_helper::option_value( setup.f_dRangeUpperBound ), "}", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "val", "set upper bound for range interval to val" )
		( "density", program_options_helper::option_value( setup.f_iDensity ), "D", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "val", "set graph density to val" )
		( "stereo", program_options_helper::option_value( setup.f_bStereo ), "S", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "generate stereo picture" )
		( "3D", program_options_helper::option_value( setup.f_b3D ), "3", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "draw 3D function surfaces" )
		( "show-axis", program_options_helper::option_value( setup.f_bShowAxis ), "A", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "draw multiple functions at once" )
		( "multi-formula", program_options_helper::option_value( setup.f_bMultiFormula ), "M", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "draw axes" )
		( "formula", program_options_helper::option_value( setup.f_oFormula ), "F", HProgramOptionsHandler::OOption::TYPE::REQUIRED, "eq", "render specified formula" )
		( "quiet", program_options_helper::option_value( setup.f_bQuiet ), "q", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "inhibit usual output" )
		( "silent", program_options_helper::option_value( setup.f_bQuiet ), "q", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "inhibit usual output" )
		( "verbose", program_options_helper::option_value( setup.f_bVerbose ), "v", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "print more information" )
		( "help", program_options_helper::option_value( stop ), "h", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "display this help and stop", program_options_helper::callback( util::show_help, &info ) )
		( "dump-configuration", program_options_helper::option_value( stop ), "W", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "dump current configuration", program_options_helper::callback( util::dump_configuration, &info ) )
		( "version", program_options_helper::no_value, "V", HProgramOptionsHandler::OOption::TYPE::NONE, NULL, "output version information and stop", program_options_helper::callback( version, NULL ) );
	po.process_rc_file( "funlab", "", NULL );
	if ( setup.f_oLogPath.is_empty() )
		setup.f_oLogPath = "funlab.log";
	int l_iUnknown = 0, l_iNonOption = 0;
	l_iNonOption = po.process_command_line( a_iArgc, a_ppcArgv, &l_iUnknown );
	if ( l_iUnknown > 0 )
		{
		util::show_help( &info );
		throw l_iUnknown;
		}
	if ( stop )
		throw 0;
	return ( l_iNonOption );
	M_EPILOG
	}

}


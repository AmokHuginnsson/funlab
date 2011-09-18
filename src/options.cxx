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
#include "config.hxx"
#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace funlab {

bool set_variables( HString& option_, HString& value_ ) {
	::fprintf( stdout, "option: [%s], value: [%s]\n",
			option_.raw(), value_.raw() );
	return ( false );
}

void version( void* ) __attribute__ ( ( __noreturn__ ) );
void version( void* ) {
	cout << PACKAGE_STRING << endl;
	throw ( 0 );
}

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */
int handle_program_options( int argc_, char** argv_ ) {
	M_PROLOG
	HProgramOptionsHandler po;
	OOptionInfo info( po, setup._programName, "renders three dimensional function surfaces", NULL );
	bool stop = false;
	po( "log_path", program_options_helper::option_value( setup._logPath ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path pointing to file for application logs", "path" )
		( "resource_path", program_options_helper::option_value( setup._resourcePath ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path to XUL resources", "path" )
		( "icon_path", program_options_helper::option_value( setup._iconPath ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "path to icon resources", "path" )
		( "resolution-x", program_options_helper::option_value( setup._resolutionX ), 'X', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set x resolution to val value", "val" )
		( "resolution-y", program_options_helper::option_value( setup._resolutionY ), 'Y', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set y resolution to val value", "val" )
		( "aspect", program_options_helper::option_value( setup._aspect ), 'a', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set aspect of drawing to value of expr", "expr" )
		( "domain-lower-bound", program_options_helper::option_value( setup._domainLowerBound ), '[', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set lower bound for domain interval to val", "val" )
		( "domain-upper-bound", program_options_helper::option_value( setup._domainUpperBound ), ']', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set upper bound for domain interval to val", "val" )
		( "range-lower-bound", program_options_helper::option_value( setup._rangeLowerBound ), '{', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set lower bound for range interval to val", "val" )
		( "range-upper-bound", program_options_helper::option_value( setup._rangeUpperBound ), '}', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set upper bound for range interval to val", "val" )
		( "density", program_options_helper::option_value( setup._density ), 'D', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "set graph density to val", "val" )
		( "stereo", program_options_helper::option_value( setup._stereo ), 'S', HProgramOptionsHandler::OOption::TYPE::NONE, "generate stereo picture" )
		( "3D", program_options_helper::option_value( setup.f_b3D ), '3', HProgramOptionsHandler::OOption::TYPE::NONE, "draw 3D function surfaces" )
		( "show-axis", program_options_helper::option_value( setup._showAxis ), 'A', HProgramOptionsHandler::OOption::TYPE::NONE, "draw multiple functions at once" )
		( "multi-formula", program_options_helper::option_value( setup._multiFormula ), 'M', HProgramOptionsHandler::OOption::TYPE::NONE, "draw axes" )
		( "formula", program_options_helper::option_value( setup._formula ), 'F', HProgramOptionsHandler::OOption::TYPE::REQUIRED, "render specified formula", "eq" )
		( "quiet", program_options_helper::option_value( setup._quiet ), 'q', HProgramOptionsHandler::OOption::TYPE::NONE, "inhibit usual output" )
		( "silent", program_options_helper::option_value( setup._quiet ), 'q', HProgramOptionsHandler::OOption::TYPE::NONE, "inhibit usual output" )
		( "verbose", program_options_helper::option_value( setup._verbose ), 'v', HProgramOptionsHandler::OOption::TYPE::NONE, "print more information" )
		( "help", program_options_helper::option_value( stop ), 'h', HProgramOptionsHandler::OOption::TYPE::NONE, "display this help and stop", program_options_helper::callback( util::show_help, &info ) )
		( "dump-configuration", program_options_helper::option_value( stop ), 'W', HProgramOptionsHandler::OOption::TYPE::NONE, "dump current configuration", program_options_helper::callback( util::dump_configuration, &info ) )
		( "version", program_options_helper::no_value, 'V', HProgramOptionsHandler::OOption::TYPE::NONE, "output version information and stop", program_options_helper::callback( version, NULL ) );
	po.process_rc_file( "funlab", "", NULL );
	if ( setup._logPath.is_empty() )
		setup._logPath = "funlab.log";
	int unknown = 0, nonOption = 0;
	nonOption = po.process_command_line( argc_, argv_, &unknown );
	if ( unknown > 0 ) {
		util::show_help( &info );
		throw unknown;
	}
	if ( stop )
		throw 0;
	return ( nonOption );
	M_EPILOG
}

}


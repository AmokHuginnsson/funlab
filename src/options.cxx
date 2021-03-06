/* Read booker/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "config.hxx"

#include <yaal/hcore/hprogramoptionshandler.hxx>
#include <yaal/hcore/hlog.hxx>
#include <yaal/tools/util.hxx>
M_VCSID( "$Id: " __ID__ " $" )

#include "options.hxx"
#include "setup.hxx"
#include "commit_id.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace funlab {

namespace {

bool set_variables( HString& option_, HString& value_ ) {
	static bool const FUNLAB_RC_DEBUG( !! ::getenv( "FUNLAB_RC_DEBUG" ) );
	if ( FUNLAB_RC_DEBUG ) {
		cout << "option: [" << option_ << "], value: [" << value_ << "]" << endl;
	}
	return ( true );
}

void version( void ) {
	cout << PACKAGE_STRING << ( setup._verbose ? " " COMMIT_ID : "" ) << endl;
}

}

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */
int handle_program_options( int argc_, char** argv_ ) {
	M_PROLOG
	HProgramOptionsHandler po;
	HOptionInfo info( po );
	info.name( setup._programName ).intro( "renders three dimensional function surfaces" );
	bool help( false );
	bool conf( false );
	bool vers( false );
	po(
		HProgramOptionsHandler::HOption()
		.long_form( "log-path" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "path pointing to file for application logs" )
		.recipient(	setup._logPath )
		.argument_name( "path" )
		.default_value( "funlab.log" )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "resource-path" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "path to XUL resources" )
		.recipient( setup._resourcePath )
		.argument_name( "path" )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "icon-path" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "path to icon resources" )
		.recipient( setup._iconPath )
		.argument_name( "path" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'X' )
		.long_form( "resolution-x" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set x resolution to val value" )
		.recipient( setup._resolutionX )
		.argument_name( "val" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'Y' )
		.long_form( "resolution-y" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set y resolution to val value" )
		.recipient( setup._resolutionY )
		.argument_name( "val" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'a' )
		.long_form( "aspect" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set aspect of drawing to value of expr" )
		.recipient( setup._aspect )
		.argument_name( "expr" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( '[' )
		.long_form( "domain-lower-bound" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set lower bound for domain interval to val" )
		.recipient( setup._domainLowerBound )
		.argument_name( "val" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( ']' )
		.long_form( "domain-upper-bound" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set upper bound for domain interval to val" )
		.recipient( setup._domainUpperBound )
		.argument_name( "val" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( '{' )
		.long_form( "range-lower-bound" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set lower bound for range interval to val" )
		.recipient( setup._rangeLowerBound )
		.argument_name( "val" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( '}' )
		.long_form( "range-upper-bound" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set upper bound for range interval to val" )
		.recipient( setup._rangeUpperBound )
		.argument_name( "val" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'D' )
		.long_form( "density" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "set graph density to val" )
		.recipient( setup._density )
		.argument_name( "val" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'S' )
		.long_form( "stereo" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "generate stereo picture" )
		.recipient( setup._stereo )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( '3' )
		.long_form( "3D" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "draw 3D function surfaces" )
		.recipient( setup._3D )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'A' )
		.long_form( "show-axis" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "draw multiple functions at once" )
		.recipient( setup._showAxis )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'M' )
		.long_form( "multi-formula" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "draw axes" )
		.recipient( setup._multiFormula )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'F' )
		.long_form( "formula" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "render specified formula" )
		.recipient( setup._formula )
		.argument_name( "eq" )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'q' )
		.long_form( "quiet" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "inhibit usual output" )
		.recipient( setup._quiet )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'q' )
		.long_form( "silent" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "inhibit usual output" )
		.recipient( setup._quiet )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'v' )
		.long_form( "verbose" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "print more information" )
		.recipient( setup._verbose )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'h' )
		.long_form( "help" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "display this help and stop" )
		.recipient( help )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'W' )
		.long_form( "dump-configuration" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "dump current configuration" )
		.recipient( conf )
	)(
		HProgramOptionsHandler::HOption()
		.short_form( 'V' )
		.long_form( "version" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::NONE )
		.description( "output version information and stop" )
		.recipient( vers )
	);
	po.process_rc_file( "", set_variables );
	int unknown( 0 );
	int nonOption( po.process_command_line( argc_, argv_, &unknown ) );
	if ( help || conf || vers || ( unknown > 0 ) ) {
		if ( help || ( unknown > 0 ) ) {
			util::show_help( info );
		} else if ( conf ) {
			util::dump_configuration( info );
		} else if ( vers ) {
			version();
		}
		HLog::disable_auto_rehash();
		throw unknown;
	}
	return nonOption;
	M_EPILOG
}

}


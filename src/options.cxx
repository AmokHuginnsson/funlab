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

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */

void usage( void* ) __attribute__((__noreturn__));
void usage( void* arg )
	{
	OOptionInfo* info = static_cast<OOptionInfo*>( arg );
	info->PROC( info->_opt, info->_size, setup.f_pcProgramName, "renders three dimensional function surfaces", NULL );
	throw ( setup.f_bHelp ? 0 : 1 );
	}

void version( void* ) __attribute__ ( ( __noreturn__ ) );
void version( void* )
	{
	printf ( "`funlab' %s\n", VER );
	throw ( 0 );
	}

namespace
{

simple_callback_t help( usage, NULL );
simple_callback_t dump( usage, NULL );
simple_callback_t version_call( version, NULL );

OOption n_psOptions[] =
	{
		{ "log_path", D_HSTRING, &setup.f_oLogPath, NULL, OOption::D_REQUIRED, "path", "path pointing to file for application logs", NULL },
		{ "resource_path", D_HSTRING, &setup.f_oResourcePath, NULL, OOption::D_REQUIRED, "path", "path to XUL resources", NULL },
		{ "icon_path", D_HSTRING, &setup.f_oIconPath, NULL, OOption::D_REQUIRED, "path", "path to icon resources", NULL },
		{ "resolution-x", D_INT, &setup.f_iResolutionX, "X", OOption::D_REQUIRED, "val", "set x resolution to val value", NULL },
		{ "resolution-y", D_INT, &setup.f_iResolutionY, "Y", OOption::D_REQUIRED, "val", "set y resolution to val value", NULL },
		{ "aspect", D_DOUBLE, &setup.f_dAspect, "a", OOption::D_REQUIRED, "expr", "set aspect of drawing to value of expr", NULL },
		{ "domain-lower-bound", D_DOUBLE_LONG, &setup.f_dDomainLowerBound, "[", OOption::D_REQUIRED, "val", "set lower bound for domain interval to val", NULL },
		{ "domain-upper-bound", D_DOUBLE_LONG, &setup.f_dDomainUpperBound, "]", OOption::D_REQUIRED, "val", "set upper bound for domain interval to val", NULL },
		{ "range-lower-bound", D_DOUBLE_LONG, &setup.f_dRangeLowerBound, "{", OOption::D_REQUIRED, "val", "set lower bound for range interval to val", NULL },
		{ "range-upper-bound", D_DOUBLE_LONG, &setup.f_dRangeUpperBound, "}", OOption::D_REQUIRED, "val", "set upper bound for range interval to val", NULL },
		{ "density", D_INT, &setup.f_iDensity, "D", OOption::D_REQUIRED, "val", "set graph density to val", NULL },
		{ "stereo", D_BOOL, &setup.f_bStereo, "S", OOption::D_NONE, NULL, "generate stereo picture", NULL },
		{ "3D", D_BOOL, &setup.f_b3D, "3", OOption::D_NONE, NULL, "draw 3D function surfaces", NULL },
		{ "show-axis", D_BOOL, &setup.f_bShowAxis, "A", OOption::D_NONE, NULL, "draw multiple functions at once", NULL },
		{ "multi-formula", D_BOOL, &setup.f_bMultiFormula, "M", OOption::D_NONE, NULL, "draw axes", NULL },
		{ "formula", D_HSTRING, &setup.f_oFormula, "F", OOption::D_REQUIRED, "eq", "render specified formula", NULL },
		{ "quiet", D_BOOL, &setup.f_bQuiet, "q", OOption::D_NONE, NULL, "inhibit usual output", NULL },
		{ "silent", D_BOOL, &setup.f_bQuiet, "q", OOption::D_NONE, NULL, "inhibit usual output", NULL },
		{ "verbose", D_BOOL, &setup.f_bVerbose, "v", OOption::D_NONE, NULL, "print more information", NULL },
		{ "help", D_BOOL, &setup.f_bHelp, "h", OOption::D_NONE, NULL, "display this help and exit", &help },
		{ "dump-configuration", D_VOID, NULL, "W", OOption::D_NONE, NULL, "dump current configuration", &dump },
		{ "version", D_VOID, NULL, "V", OOption::D_NONE, NULL, "output version information and exit", &version_call },
		{ NULL, D_VOID, NULL, NULL, OOption::D_NONE, NULL, NULL, NULL }
	};

}

int process_funlabrc_file( void )
	{
	rc_file::process_rc_file( "funlab", HString(), n_psOptions, NULL );
	if ( ! setup.f_oLogPath )
		setup.f_oLogPath = "funlab.log";
	return ( 0 );
	}

int decode_switches( int a_iArgc, char** a_ppcArgv )
	{
	M_PROLOG
	int l_iUnknown = 0, l_iNonOption = 0;
	OOptionInfo info( n_psOptions, sizeof ( n_psOptions ) / sizeof ( OOption ), util::show_help );
	OOptionInfo infoConf( n_psOptions, sizeof ( n_psOptions ) / sizeof ( OOption ), util::dump_configuration );
	help.second = &info;
	dump.second = &infoConf;
	l_iNonOption = cl_switch::decode_switches( a_iArgc, a_ppcArgv, n_psOptions,
			info._size, &l_iUnknown );
	if ( l_iUnknown > 0 )
		usage( &info );
	return ( l_iNonOption );
	M_EPILOG
	}

}


/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	cli_options.cxx - this file is integral part of `funlab' project.

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

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <yaal/yaal.h>
M_VCSID ( "$Id$" )

#include "cli_options.h"
#include "version.h"
#include "setup.h"

using namespace yaal;
using namespace yaal::hcore;

#define D_QUIET_SHORT				"q"
#define D_QUIET_LONG				"quiet"
#define D_SILENT_LONG				"silent"
#define D_VERBOSE_SHORT			"v"
#define D_VERBOSE_LONG			"verbose"
#define D_HELP_SHORT				"h"
#define D_HELP_LONG					"help"
#define D_VERSION_SHORT			"V"
#define D_VERSION_LONG			"version"
#define D_RESOLUTION_X_SHORT	"X"
#define D_RESOLUTION_X_LONG		"resolution-x"
#define D_RESOLUTION_Y_SHORT	"Y"
#define D_RESOLUTION_Y_LONG		"resolution-y"
#define D_DENSITY_SHORT	"D"
#define D_DENSITY_LONG	"density"
#define D_STEREO_SHORT	"S"
#define D_STEREO_LONG		"stereo"
#define D_FORMULA_SHORT	"F"
#define D_FORMULA_LONG	"formula"

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */
void usage ( void ) __attribute__ ( ( __noreturn__ ) );
void usage ( void )
	{
	printf ( "%s - "
" renders three dimensional function surfaces.\n", setup.f_pcProgramName );
	printf ( "Usage: %s [OPTION]... [FILE]...\n", setup.f_pcProgramName );
	printf (
"Options:\n"
"  -"D_RESOLUTION_X_SHORT", --"D_RESOLUTION_X_LONG"         set x resolution\n"
"  -"D_RESOLUTION_Y_SHORT", --"D_RESOLUTION_Y_LONG"         set y resolution\n"
"  -"D_DENSITY_SHORT", --"D_DENSITY_LONG"              set graph density\n"
"  -"D_STEREO_SHORT", --"D_STEREO_LONG"               generate stereo picture\n"
"  -"D_FORMULA_SHORT", --"D_FORMULA_LONG"              render specified formula\n"
"  -"D_QUIET_SHORT			", --"D_QUIET_LONG", --"D_SILENT_LONG"      inhibit usual output\n"
"  -"D_VERBOSE_SHORT		", --"D_VERBOSE_LONG		""    "              print more information\n"
"  -"D_HELP_SHORT				", --"D_HELP_LONG				"" "                 display this help and exit\n"
"  -"D_VERSION_SHORT		", --"D_VERSION_LONG		""    "              output version information and exit\n" );
	throw ( setup.f_bHelp ? 0 : 1 );
	}

void version ( void ) __attribute__ ( ( __noreturn__ ) );
void version ( void )
	{
	printf ( "`funlab' %s\n", VER );
	throw ( 0 );
	}

int decode_switches ( int a_iArgc, char ** a_ppcArgv )
	{
	M_PROLOG
	int l_iUnknown = 0, l_iNonOption = 0;
	OOption l_psOptions [ ] =
		{
			{ D_RESOLUTION_X_LONG,	D_RESOLUTION_X_SHORT, OOption::D_REQUIRED, D_INT,			& setup.f_iResolutionX,	NULL },
			{ D_RESOLUTION_Y_LONG,	D_RESOLUTION_Y_SHORT, OOption::D_REQUIRED, D_INT,			& setup.f_iResolutionY,	NULL },
			{ D_DENSITY_LONG,			D_DENSITY_SHORT,		OOption::D_REQUIRED,	D_INT,			& setup.f_iDensity,			NULL },
			{ D_STEREO_LONG,			D_STEREO_SHORT,			OOption::D_NONE,			D_BOOL,			& setup.f_bStereo,			NULL },
			{ D_FORMULA_LONG,			D_FORMULA_SHORT,		OOption::D_REQUIRED,	D_HSTRING,	& setup.f_oFormula,			NULL },
			{ D_QUIET_LONG	,			D_QUIET_SHORT,			OOption::D_NONE,	D_BOOL,	&setup.f_bQuiet,		NULL },
			{ D_SILENT_LONG,			D_QUIET_SHORT,			OOption::D_NONE,	D_BOOL,	&setup.f_bQuiet,		NULL },
			{ D_VERBOSE_LONG,			D_VERBOSE_SHORT,		OOption::D_NONE,	D_BOOL,	&setup.f_bVerbose,	NULL },
			{ D_HELP_LONG,				D_HELP_SHORT,				OOption::D_NONE,	D_BOOL,	&setup.f_bHelp,		usage },
			{ D_VERSION_LONG,			D_VERSION_SHORT,		OOption::D_NONE,	D_VOID,	NULL,								version }
		};
	l_iNonOption = cl_switch::decode_switches ( a_iArgc, a_ppcArgv, l_psOptions,
			sizeof ( l_psOptions ) / sizeof ( OOption ), & l_iUnknown );
	if ( l_iUnknown > 0 )
		usage ( );
	return ( l_iNonOption );
	M_EPILOG
	}


/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	cli_options.c - this file is integral part of `funlab' project.

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

#include <yaal.h>
M_CVSID ( "$CVSHeader: funlab/src/cli_options.c,v 1.8 2006/01/24 11:51:46 amok Exp $" )

#include "cli_options.h"
#include "version.h"
#include "setup.h"

using namespace yaal;
using namespace yaal::hcore;
  
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
"  -X, --resolution-x         set x resolution\n"
"  -Y, --resolution-y         set y resolution\n"
"  -D, --density              set graph density\n"
"  -S, --stereo               generate stereo picture\n"
"  -F, --formula              render specified formula\n"
"  -q, --quiet, --silent      inhibit usual output\n"
"  --verbose                  print more information\n"
"  -h, --help                 display this help and exit\n"
"  -V, --version              output version information and exit\n" );
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
			{ "resolution-x",	'X', OOption::D_REQUIRED, D_INT,			& setup.f_iResolutionX,	NULL },
			{ "resolution-y",	'Y', OOption::D_REQUIRED, D_INT,			& setup.f_iResolutionY,	NULL },
			{ "density",			'D', OOption::D_REQUIRED, D_INT,			& setup.f_iDensity,			NULL },
			{ "stereo",				'S', OOption::D_NONE,			D_BOOL,			& setup.f_bStereo,			NULL },
			{ "formula",			'F', OOption::D_REQUIRED, D_HSTRING,	& setup.f_oFormula,			NULL },
			{ "quiet",				'q', OOption::D_NONE,			D_BOOL,			& setup.f_bQuiet,				NULL },
			{ "silent",				'q', OOption::D_NONE,			D_BOOL,			& setup.f_bQuiet,				NULL },
			{ "verbose",			'v', OOption::D_NONE,			D_BOOL,			& setup.f_bVerbose,			NULL },
			{ "help",					'h', OOption::D_NONE,			D_BOOL,			& setup.f_bHelp,				usage },
			{ "version",			'V', OOption::D_NONE,			D_VOID,			NULL,										version }
		};
	l_iNonOption = cl_switch::decode_switches ( a_iArgc, a_ppcArgv, l_psOptions,
			sizeof ( l_psOptions ) / sizeof ( OOption ), & l_iUnknown );
	if ( l_iUnknown > 0 )
		usage ( );
	return ( l_iNonOption );
	M_EPILOG
	}


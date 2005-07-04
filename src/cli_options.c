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

#include <stdhapi.h>

#include "define.h"
#include "version.h"
#include "variables.h"
#include "cli_options.h"

using namespace stdhapi;
using namespace stdhapi::hcore;
  
/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.                    */

void usage ( int a_iStatus )
	{
	printf ( "%s - renders three dimensional function surfaces.\n", g_pcProgramName );
	printf ( "Usage: %s [OPTION]...\n", g_pcProgramName );
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
	exit ( a_iStatus );
	}

int decode_switches ( int a_iArgc, char ** a_ppcArgv )
	{
	M_PROLOG
	int l_iChar = 0;
	hcore::log << "Decoding switches ... ";
	while ( ( l_iChar = getopt_long ( a_iArgc, a_ppcArgv, 
					"q"	 /* quiet or silent */
					"v"	 /* verbose */
					"h"	 /* help    */
					"V"	 /* version */
					"X:" /* resolution x */
					"Y:" /* resolution y */
					"D:" /* density */
					"S"  /* stereo */
					"F:", /* formula */ 
					g_sLongOptions, ( int * ) 0 ) ) != EOF )
		{
		switch ( l_iChar )
			{
			case ( 'X' ):
				{
				if ( optarg )
					g_iResolutionX = strtol ( optarg, NULL, 10 );
				break;
				}
			case ( 'Y' ):
				{
				if ( optarg )
					g_iResolutionY = strtol ( optarg, NULL, 10 );
				break;
				}
			case ( 'D' ):
				{
				if ( optarg )
					g_iDensity = strtol ( optarg, NULL, 10 );
				break;
				}
			case ( 'S' ):
				{
				g_bStereo = true;
				break;
				}
			case ( 'F' ):
				{
				g_oFormula = optarg;
				break;
				}
			case ( 'q' ):	 /* --quiet, --silent                                     */
				{
				g_iWantQuiet = 1;
			  break;
				}
			case ( 'v' ):
				{
				g_iWantVerbose = 1;
				break;
				}
			case ( 'V' ):
				{
				printf ( "`funlab' %s\n", VER );
				exit ( 0 );
				}
			case ( 'h' ):
				{
				usage ( 0 );
				break;
				}
			default:
				{
				usage (EXIT_FAILURE);
				}
			}
		}
	hcore::log << "done" << endl;
	return ( optind );
	M_EPILOG
	}


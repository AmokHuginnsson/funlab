/*
---       `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski         ---

	rc_options.c - this file is integral part of `funlab' project.

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
#include <string.h>
#include <stdio.h>

#include <stdhapi.h>
M_CVSID ( "$CVSHeader$" );

#include "variables.h"

using namespace stdhapi::hcore;

OVariable g_psVars [ ] =
	{
		{ D_HSTRING, "log_path", & g_oLogPath },
		{ D_HSTRING, "icon_path", & g_oIconPath },
		{ D_HSTRING, "resource_path", & g_oResourcePath },
		{ D_INT, "resolution_x", & g_iResolutionX },
		{ D_INT, "resolution_y", & g_iResolutionY },
		{ D_INT, "density", & g_iDensity },
		{ D_BOOL, "stereo", & g_bStereo },
		{ 0, NULL, NULL }
	};

bool set_variables ( HString & a_roOption, HString & a_roValue )
	{
	printf ( "option: [%s], value: [%s]\n", ( char * ) a_roOption,
			( char * ) a_roValue );
	return ( false );
	}

int process_funlabrc_file ( void )
	{
	rc_file::process_rc_file ( "funlab", NULL, g_psVars, NULL );
	if ( ! g_oLogPath )
		g_oLogPath = "funlab.log";
	return ( 0 );
	}

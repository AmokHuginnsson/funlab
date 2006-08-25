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

#include <yaal.h>
M_VCSID ( "$CVSHeader: funlab/src/rc_options.c,v 1.9 2006/01/24 11:51:46 amok Exp $" )

#include "setup.h"

using namespace yaal;
using namespace yaal::hcore;

bool set_variables ( HString & a_roOption, HString & a_roValue )
	{
	fprintf ( stdout, "option: [%s], value: [%s]\n",
			static_cast < char const * const > ( a_roOption ),
			static_cast < char const * const > ( a_roValue ) );
	return ( false );
	}

int process_funlabrc_file ( void )
	{
	OVariable l_psVars [ ] =
		{
			{ D_HSTRING,	"log_path",				& setup.f_oLogPath },
			{ D_HSTRING,	"icon_path",			& setup.f_oIconPath },
			{ D_HSTRING,	"resource_path",	& setup.f_oResourcePath },
			{ D_INT,			"resolution_x",		& setup.f_iResolutionX },
			{ D_INT,			"resolution_y",		& setup.f_iResolutionY },
			{ D_INT,			"density",				& setup.f_iDensity },
			{ D_BOOL,			"stereo",					& setup.f_bStereo },
			{ D_VOID,			NULL,							NULL }
		};
	rc_file::process_rc_file ( "funlab", NULL, l_psVars, NULL );
	if ( ! setup.f_oLogPath )
		setup.f_oLogPath = "funlab.log";
	return ( 0 );
	}

/*
---       `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski         ---

	rc_options.cxx - this file is integral part of `funlab' project.

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

#include <yaal/yaal.h>
M_VCSID ( "$Id$" )

#include "setup.h"

using namespace yaal;
using namespace yaal::hcore;

bool set_variables( HString& a_roOption, HString& a_roValue )
	{
	::fprintf( stdout, "option: [%s], value: [%s]\n",
			static_cast<char const* const>( a_roOption ),
			static_cast<char const* const>( a_roValue ) );
	return ( false );
	}

int process_funlabrc_file( void )
	{
	OOption l_psVars[] =
		{
			{ "icon_path", D_HSTRING, &setup.f_oIconPath, NULL, OOption::D_REQUIRED, "path", "path to icon resources", NULL },
			{ "resource_path", D_HSTRING, &setup.f_oResourcePath, NULL, OOption::D_REQUIRED, "path", "path to XUL resources", NULL },
			{ "resolution_x", D_INT, &setup.f_iResolutionX, NULL, OOption::D_REQUIRED, "val", "default x resolution", NULL },
			{ "resolution_y", D_INT, &setup.f_iResolutionY, NULL, OOption::D_REQUIRED, "val", "default y resolution", NULL },
			{ "density", D_INT, &setup.f_iDensity, NULL, OOption::D_REQUIRED, "val", "default density", NULL },
			{ "stereo", D_BOOL, &setup.f_bStereo, NULL, OOption::D_REQUIRED, "enabled", "enable/disable stereo vision", NULL },
			{ "log_path", D_HSTRING, &setup.f_oLogPath, NULL, OOption::D_REQUIRED, "path", "path pointing to file for application logs", NULL },
			{ NULL, D_VOID, NULL, NULL, OOption::D_NONE, NULL, NULL, NULL }
		};
	rc_file::process_rc_file ( "funlab", NULL, l_psVars, NULL );
	if ( ! setup.f_oLogPath )
		setup.f_oLogPath = "funlab.log";
	return ( 0 );
	}


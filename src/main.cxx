/*
---            `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

	main.cxx - this file is integral part of `funlab' project.

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

#include <libintl.h>
#include <unistd.h>
#include <yaal/yaal.h> /* all hAPI headers */
M_VCSID ( "$Id$" )

#include "version.h"
#include "setup.h"
#include "cli_options.h"
#include "rc_options.h"
#include "gui.h"
#include "renderer.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;

OSetup setup;

int main ( int a_iArgc, char * a_ppcArgv [ ] )
	{
	M_PROLOG
/* variables declarations for main loop: */
	int l_iOpt = 0;
	int l_iCtr = 0, l_iPosition = 0;
	HString l_oArrow;
/* end. */
	try
		{
/* TO-DO: enter main loop code here */
		signals::set_handlers ( );
		setup.f_pcProgramName = a_ppcArgv [ 0 ];
		process_funlabrc_file ( );
		l_iOpt = decode_switches ( a_iArgc, a_ppcArgv );
		hcore::log.rehash ( setup.f_oLogPath, setup.f_pcProgramName );
/*		if ( ! console::is_enabled ( ) )enter_curses (); / * enabling ncurses ablilities*/
/* *BOOM* */
		if ( setup.f_oFormula )
			{
			HRenderer l_oRenderer;
			l_iOpt = l_oRenderer.render_surface ( setup.f_oFormula ) ? 1 : 0;
			if ( l_iOpt )
				{
				l_iPosition = l_oRenderer.error_position ( );
				for ( l_iCtr = 0; l_iCtr < l_iPosition; l_iCtr ++ )
					l_oArrow += '-';
				l_oArrow += 'v';
				fprintf ( stderr, _ ( "Formula syntax error ...\n%s at this place:\n%s\n%s\n" ),
						static_cast < char const * > ( l_oRenderer.error ( ) ),
						static_cast < char const * > ( l_oArrow ),
						static_cast < char const * > ( setup.f_oFormula ) );
				}
			}
		else
			l_iOpt = gui_start ( a_iArgc, a_ppcArgv );
		if ( hconsole::is_enabled ( ) )leave_curses (); /* ending ncurses sesion */
/* ... there is the place main loop ends. :OD-OT */
		}
	catch ( ... )
		{
		if ( hconsole::is_enabled ( ) )leave_curses (); /* ending ncurses sesion */
		throw;
		}
	fprintf ( stderr, "Done.\n" );
	return ( l_iOpt );
	M_FINAL
	}


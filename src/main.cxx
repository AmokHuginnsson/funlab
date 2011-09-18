/*
---            `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

	main.cxx - this file is integral part of `funlab' project.

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
#include <libintl.h>
#include <unistd.h>
#include <yaal/yaal.hxx> /* all hAPI headers */
M_VCSID( "$Id: "__ID__" $" )

#include "setup.hxx"
#include "options.hxx"
#include "gui.hxx"
#include "hdetachedrenderer.hxx"
#include "hfunlab.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace funlab;

namespace funlab {

OSetup setup;

}

int main ( int argc_, char * argv_ [ ] ) {
	M_AT_END_OF_SCOPE( HSignalService::get_instance().stop(); );
	M_PROLOG
/* variables declarations for main loop: */
	int opt = 0;
	int ctr = 0, position = 0;
	HString arrow;
/* end. */
	try {
/* TO-DO: enter main loop code here */
		HSignalService::get_instance();
		setup._programName = argv_ [ 0 ];
		opt = handle_program_options( argc_, argv_ );
		hcore::log.rehash( setup._logPath, setup._programName );
		setup.test_setup();
/*		if ( ! console::is_enabled() )enter_curses (); / * enabling ncurses ablilities*/
/* *BOOM* */
		if ( !! setup._formula ) {
			HDetachedRenderer renderer;
			HFunlab::ptr_t f( make_pointer<HFunlab>( &renderer ) );
			HFunlab* pf( static_cast<HFunlab*>( f.get() ) );
			opt = pf->push_formula( setup._formula ) ? 1 : 0;
			if ( opt ) {
				position = pf->error_position();
				for ( ctr = 0; ctr < position; ctr ++ )
					arrow += '-';
				arrow += 'v';
				cerr << _( "Formula syntax error ...\n" ) << pf->error() << _( " at this place:\n" ) << arrow << "\n" << setup._formula << endl;
			} else {
				renderer.set_engine( f );
				renderer.render_surface();
			}
		} else
			opt = gui_start ( argc_, argv_ );
		HConsole& cons = HConsole::get_instance();
		if ( cons.is_enabled() )
			cons.leave_curses (); /* ending ncurses sesion */
/* ... there is the place main loop ends. :OD-OT */
	} catch ( ... ) {
		HConsole& cons = HConsole::get_instance();
		if ( cons.is_enabled() )
			cons.leave_curses (); /* ending ncurses sesion */
		throw;
	}
	cerr << _( "Done" ) << endl;
	return ( opt );
	M_FINAL
}


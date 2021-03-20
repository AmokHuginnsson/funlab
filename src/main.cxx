/* Read booker/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>
#include <libintl.h>
#include <unistd.h>

#include <yaal/hcore/hlog.hxx>
#include <yaal/tools/signals.hxx>
#include <yaal/tools/hthreadpool.hxx>
#include <yaal/tools/util.hxx>
M_VCSID( "$Id: " __ID__ " $" )

#include "setup.hxx"
#include "options.hxx"
#include "gui.hxx"
#include "hdetachedrenderer.hxx"
#include "hfunlab.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace funlab;

namespace funlab {

OSetup setup;

}

int main ( int argc_, char * argv_ [ ] ) {
	HScopeExitCall secTP( call( &HThreadPool::stop, &HThreadPool::get_instance() ) );
	HScopeExitCall sec( call( &HSignalService::stop, &HSignalService::get_instance() ) );
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
		} else {
			opt = gui_start ( argc_, argv_ );
		}
		cerr << _( "Done" ) << endl;
/* ... there is the place main loop ends. :OD-OT */
	} catch ( int err ) {
		opt = err;
	}
	return opt;
	M_FINAL
}


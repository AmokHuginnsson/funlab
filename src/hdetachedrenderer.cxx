/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	renderer.cxx - this file is integral part of `funlab' project.

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
 FITNESS FOR f_dPrecountA PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libintl.h>
#include <SDL/SDL.h>

#include <yaal/yaal.h>
M_VCSID( "$Id: "__ID__" $" )

#include "hdetachedrenderer.h"
#include "setup.h"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab
{

HDetachedRenderer::HDetachedRenderer( HKeyboardEventListener* a_poKeyboardEventListener )
	:
	f_dResolution( 0 ), f_bLoop( false ), f_bBusy( false ),
	f_oMutex(), f_oSemaphore(), f_oThread( *this ), f_poKeyboardEventListener( a_poKeyboardEventListener )
	{
	M_PROLOG
	SDL_EventState( SDL_MOUSEMOTION, SDL_ENABLE );
	SDL_EventState( SDL_MOUSEBUTTONDOWN, SDL_ENABLE );
	SDL_EventState( SDL_MOUSEBUTTONUP, SDL_ENABLE );
	SDL_EventState( SDL_VIDEOEXPOSE, SDL_ENABLE );
	return;
	M_EPILOG
	}

HDetachedRenderer::~HDetachedRenderer( void )
	{
	M_PROLOG
	cout << "waiting ... " << flush;
	if ( HSurface::surface_count() )
		f_oSemaphore.wait();
	cout << "still waiting ... " << flush;
	while ( f_bBusy )
		;
	cout << "done" << endl;
	return;
	M_EPILOG
	}

bool HDetachedRenderer::render_surface( void )
	{
	M_PROLOG
	if ( ! HSurface::surface_count() )
		{
		if ( ! f_oSurface )
			f_oSurface = HSurface::ptr_t( new HSurface );
		f_oSurface->init( setup.f_iResolutionX, setup.f_iResolutionY );
		SDL_WarpMouse( static_cast<Uint16>( setup.f_iResolutionX >> 1 ),
				static_cast<Uint16>( setup.f_iResolutionY >> 1 ) );
		f_bLoop = true;
		f_oThread.spawn();
		}
	else
		SDL_WarpMouse( static_cast<Uint16>( setup.f_iResolutionX >> 1 ),
				static_cast<Uint16>( setup.f_iResolutionY >> 1 ) );
	return ( false );
	M_EPILOG
	}

int HDetachedRenderer::operator() ( HThread const* const a_poCaller )
	{
	M_PROLOG
	int dx = 0, dy = 0;
	SDL_Event l_uEvent;
	while ( f_bLoop && a_poCaller->is_alive() )
		{
		if ( SDL_WaitEvent( &l_uEvent ) && f_oSurface->is_valid() )
			{
			HLock l_oLock( f_oMutex );
			switch ( l_uEvent.type )
				{
				case ( SDL_MOUSEMOTION ):
					{
					HMouseEvent e( HMouseEvent::TYPE::D_MOVE );
					dx = yaal::abs( l_uEvent.motion.xrel );
					dy = yaal::abs( l_uEvent.motion.yrel );
					e.set_pos( dx, dy );
					if ( ( dx < ( setup.f_iResolutionX >> 1 ) ) && ( dy < ( setup.f_iResolutionY >> 1 ) ) )
						{
						switch ( l_uEvent.motion.state )
							{
							case ( SDL_BUTTON( 1 ) ):
								e.set_button( HMouseEvent::BUTTON::D_1 );
							break;
							case ( SDL_BUTTON( 2 ) ):
								e.set_button( HMouseEvent::BUTTON::D_2 );
							break;
							case ( SDL_BUTTON( 3 ) ):
								e.set_button( HMouseEvent::BUTTON::D_3 );
							break;
							default:
							break;
							}
						f_oEngine->on_event( &e );
						}
					break;
					}
				case ( SDL_MOUSEBUTTONDOWN ):
					{
					HMouseEvent e( HMouseEvent::TYPE::D_PRESS );
					switch ( l_uEvent.button.button )
						{
						case ( SDL_BUTTON( 4 ) ):
							e.set_button( HMouseEvent::BUTTON::D_4 );
						break;
						case ( SDL_BUTTON( 5 ) ):
							e.set_button( HMouseEvent::BUTTON::D_5 );
						break;
						default:
						break;
						}
					f_oEngine->on_event( &e );
					}
				break;
				case ( SDL_KEYUP ):
					{
					HKeyboardEvent e( l_uEvent.key.keysym.sym, l_uEvent.key.keysym.mod );
					f_oEngine->on_event( &e );
					}
				break;
				default:
				break;
				}
			if ( f_bLoop )
				{
				draw_frame();
				while ( SDL_PollEvent( &l_uEvent ) )
					;
				}
			}
		}
	return ( 0 );
	M_EPILOG
	}

char const* HDetachedRenderer::error( void ) const
	{
	M_PROLOG
	return ( f_oAnalyser.get_error() );
	M_EPILOG
	}

int HDetachedRenderer::error_position( void ) const
	{
	M_PROLOG
	return ( f_oAnalyser.get_error_token() );
	M_EPILOG
	}

void HDetachedRenderer::shutdown( void )
	{
	f_oThread.finish();
	cout << __PRETTY_FUNCTION__ << endl;
	f_oSurface->down();
	f_oSurface = HSurface::ptr_t();
	}

}


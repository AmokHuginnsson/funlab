/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	renderer.hxx - this file is integral part of `funlab' project.

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

#ifndef FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED
#define FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED

#include <yaal/hcore/hthread.hxx>

#include "hrenderer.hxx"
#include "events.hxx"

namespace funlab
{

class HDetachedRenderer;

class HDetachedRenderer : public HRendererSurfaceBase
	{
protected:
	/*{*/
	bool f_bLoop;
	void* f_pvHandler;
	int	f_iWidth;
	int f_iHeight;
	int f_iBPP;
	yaal::hcore::HSemaphore f_oSemaphore;
	yaal::hcore::HThread _thread;
	HKeyboardEventListener* f_poKeyboardEventListener;
	static int f_iActiveSurfaces;
	/*}*/
public:
	/*{*/
	HDetachedRenderer( HKeyboardEventListener* = NULL );
	virtual ~HDetachedRenderer ( void );
	bool render_surface( void );
	int operator() ( yaal::hcore::HThread const* const );
	void shutdown( void );
	/*}*/
private:
	int init( int, int, int = 32 );
	void down( void );
	void toggle_fullscreen( void );
protected:
	/*{*/
	virtual double do_get_width( void ) const;
	virtual double do_get_height( void ) const;
	virtual void do_commit( void );
	virtual void do_put_pixel( double, double, yaal::u32_t );
	virtual yaal::u32_t do_get_pixel( double, double );
	virtual void do_line( double, double, double, double, yaal::u32_t );
	virtual void do_fill_rect( double, double, double, double, yaal::u32_t );
	virtual yaal::u32_t do_RGB( yaal::u8_t, yaal::u8_t, yaal::u8_t );
	/*}*/
private:
	HDetachedRenderer( const HDetachedRenderer& );
	HDetachedRenderer& operator = ( const HDetachedRenderer& );
	};

}

#endif /* not FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED */


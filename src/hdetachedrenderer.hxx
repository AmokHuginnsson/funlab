/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	renderer.hxx - this file is integral part of `funlab' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.
	
Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED
#define FUNLAB_HDETACHEDRENDERER_HXX_INCLUDED

#include <yaal/hcore/hthread.hxx>

#include "hrenderer.hxx"
#include "events.hxx"

namespace funlab {

class HDetachedRenderer;

class HDetachedRenderer : public HRendererSurfaceBase {
protected:
	/*{*/
	bool _loop;
	void* _handler;
	int	_width;
	int _height;
	int _bPP;
	yaal::hcore::HSemaphore _semaphore;
	yaal::hcore::HThread _thread;
	HKeyboardEventListener* _keyboardEventListener;
	static int _activeSurfaces;
	/*}*/
public:
	/*{*/
	HDetachedRenderer( HKeyboardEventListener* = NULL );
	virtual ~HDetachedRenderer ( void );
	bool render_surface( void );
	void* run( void );
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


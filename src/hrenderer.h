/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrenderer.h - this file is integral part of `funlab' project.

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

#ifndef __FUNLAB_HRENDERER_H
#define __FUNLAB_HRENDERER_H

#include <yaal/hcore/hpointer.h>

#include "events.h"

namespace funlab
{

class HRendererEngineInterface : public HKeyboardEventListener, public HMouseEventListener
	{
public:
	using HKeyboardEventListener::on_event;
	using HMouseEventListener::on_event;
	void draw_frame( void );
	typedef yaal::hcore::HPointer<HRendererEngineInterface> ptr_t;
protected:
	virtual void do_draw_frame( void ) = 0;
	};

class HRendererSurfaceInterface
	{
public:
	virtual ~HRendererSurfaceInterface( void ) {}
	typedef yaal::hcore::HPointer<HRendererSurfaceInterface> ptr_t;
	double get_width( void ) const;
	double get_height( void ) const;
	void clear( yaal::u32_t );
	void commit( void );
	yaal::u32_t RGB( int, int, int );
	void put_pixel( double, double, yaal::u32_t );
	void line( double, double, double, double, yaal::u32_t );
	void fill_rect( double, double, double, double, yaal::u32_t );
	HRendererEngineInterface::ptr_t get_engine( void );
protected:
	virtual HRendererEngineInterface::ptr_t do_get_engine( void ) = 0;
	virtual double do_get_width( void ) const = 0;
	virtual double do_get_height( void ) const = 0;
	virtual void do_commit( void ) = 0;
	virtual yaal::u32_t do_RGB( yaal::u8_t, yaal::u8_t, yaal::u8_t ) = 0;
	virtual void do_put_pixel( double, double, yaal::u32_t ) = 0;
	virtual void do_line( double, double, double, double, yaal::u32_t ) = 0;
	virtual void do_fill_rect( double, double, double, double, yaal::u32_t ) = 0;
	};

class HRendererSurfaceBase : public HRendererSurfaceInterface
	{
protected:
	HRendererEngineInterface::ptr_t f_oEngine;
public:
	virtual ~HRendererSurfaceBase( void ) {}
	void set_engine( HRendererEngineInterface::ptr_t );
protected:
	virtual HRendererEngineInterface::ptr_t do_get_engine( void );
	};

}

#endif /* not __FUNLAB_HRENDERER_H */


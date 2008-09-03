/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hembeddedrenderer.h - this file is integral part of `funlab' project.

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

#ifndef __FUNLAB_HEMBEDDEDRENDERER_H
#define __FUNLAB_HEMBEDDEDRENDERER_H

#include <gtkmm.h>
#include <libglademm/xml.h>
#include <yaal/hcore/base.h>

#include "hrenderer.h"

namespace funlab
{

class HEmbeddedRenderer : public HRendererInterface, public Gtk::DrawingArea
	{
public:
	HEmbeddedRenderer( BaseObjectType*, Glib::RefPtr<Gnome::Glade::Xml> const& );
	virtual ~HEmbeddedRenderer( void );
protected:
	virtual bool on_expose_event( GdkEventExpose* );
	virtual double do_get_width( void ) const;
	virtual double do_get_height( void ) const;
	virtual void do_put_pixel( double, double, yaal::u32_t );
	virtual void do_line( double, double, double, double, yaal::u32_t );
	virtual void do_fill_rect( double, double, double, double, yaal::u32_t );
	virtual void do_on_event( HKeyboardEvent* );
	virtual void do_on_event( HMouseEvent* );
	};

}

#endif /* not __FUNLAB_HEMBEDDEDRENDERER_H */


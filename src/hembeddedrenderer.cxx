/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hembeddedrenderer.cxx - this file is integral part of `funlab' project.

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

#include <yaal/yaal.h>
M_VCSID( "$Id: "__ID__" $" )
#include "hembeddedrenderer.h"

using namespace yaal;

namespace funlab
{

HEmbeddedRenderer::HEmbeddedRenderer( BaseObjectType* obj, Glib::RefPtr<Gnome::Glade::Xml> const& )
	: Gtk::DrawingArea( obj )
	{
	}

HEmbeddedRenderer::~HEmbeddedRenderer( void )
	{
	}

bool HEmbeddedRenderer::on_expose_event( GdkEventExpose* event )
	{
	Glib::RefPtr<Gdk::Window> window = get_window();
	if ( window )
		{
		Gtk::Allocation allocation = get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();
		Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
		if ( event )
			{
			// clip to the area indicated by the expose event so that we only
			// redraw the portion of the window that needs to be redrawn
			cr->rectangle(event->area.x, event->area.y,
					event->area.width, event->area.height);
			cr->clip();
			}
		double m_line_width = 0.05;
		double m_radius = 0.42;
		// scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
		// the center of the window
		cr->scale(width, height);
		cr->translate(0.5, 0.5);
		cr->set_line_width(m_line_width);

		cr->save();
		cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
		cr->paint();
		cr->restore();
		cr->arc(0, 0, m_radius, 0, 2 * M_PI);
		cr->save();
		cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
		cr->fill_preserve();
		cr->restore();
		cr->stroke_preserve();
		cr->clip();

		cr->save();
		cr->set_line_cap(Cairo::LINE_CAP_ROUND);

		// draw the hours hand
		cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
		cr->move_to(0, 0);
		cr->line_to(sin(0) * (m_radius * 0.5),
				-cos(0) * (m_radius * 0.5));
		cr->stroke();
		cr->restore();
		}
	return ( false );
	}

double HEmbeddedRenderer::do_get_width( void ) const
	{
	return ( 0 );
	}

double HEmbeddedRenderer::do_get_height( void ) const
	{
	return ( 0 );
	}

void HEmbeddedRenderer::do_put_pixel( double, double, yaal::u32_t )
	{
	}

void HEmbeddedRenderer::do_line( double, double, double, double, yaal::u32_t )
	{
	}

void HEmbeddedRenderer::do_fill_rect( double, double, double, double, yaal::u32_t )
	{
	}

void HEmbeddedRenderer::do_commit( void )
	{
	}

yaal::u32_t HEmbeddedRenderer::do_RGB( u8_t red, u8_t green, u8_t blue )
	{
	yaal::u8_t c[] = { red, green, blue, 0 };
	yaal::u8_t const* pc = c;
	return ( *reinterpret_cast<yaal::u32_t const*>( pc ) );
	}

}


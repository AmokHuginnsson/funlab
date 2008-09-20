/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	events.cxx - this file is integral part of `funlab' project.

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
#include "events.h"

using namespace yaal;
using namespace yaal::hcore;

namespace funlab
{

HMouseEvent::HMouseEvent( TYPE::type_t const& type )
	: f_eType( type ), f_eButton( BUTTON::D_NONE ), f_iX( 0 ), f_iY( 0 )
	{
	}

int HMouseEvent::get_x( void ) const
	{
	return ( f_iX );
	}

int HMouseEvent::get_y( void ) const
	{
	return ( f_iY );
	}

HMouseEvent::TYPE::type_t HMouseEvent::get_type( void ) const
	{
	return ( f_eType );
	}

HMouseEvent::BUTTON::button_t HMouseEvent::get_button( void ) const
	{
	return ( f_eButton );
	}

void HMouseEvent::set_pos( int const& x, int const& y )
	{
	f_iX = x;
	f_iY = y;
	}

void HMouseEvent::set_button( BUTTON::button_t const& b )
	{
	f_eButton = b;
	}

}

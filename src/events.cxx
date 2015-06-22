/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  events.cxx - this file is integral part of `funlab' project.

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

#include "events.hxx"
#include <yaal/hcore/pod.hxx>
M_VCSID( "$Id: " __ID__ " $" )

using namespace yaal;
using namespace yaal::hcore;

namespace funlab {

HMouseEvent::HMouseEvent( TYPE::type_t const& type )
	: _type( type ), _button( BUTTON::NONE ), f_iX( 0 ), f_iY( 0 ) {
}

int HMouseEvent::get_x( void ) const {
	return ( f_iX );
}

int HMouseEvent::get_y( void ) const {
	return ( f_iY );
}

HMouseEvent::TYPE::type_t HMouseEvent::get_type( void ) const {
	return ( _type );
}

HMouseEvent::BUTTON::button_t HMouseEvent::get_button( void ) const {
	return ( _button );
}

void HMouseEvent::set_pos( int x, int y ) {
	f_iX = x;
	f_iY = y;
}

void HMouseEvent::set_button( BUTTON::button_t const& b ) {
	_button = b;
}

}


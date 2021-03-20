/* Read booker/LICENSE.md file for copyright and licensing information. */

#include "events.hxx"
#include <yaal/hcore/macro.hxx>
#include <yaal/hcore/pod.hxx>
M_VCSID( "$Id: " __ID__ " $" )

using namespace yaal;
using namespace yaal::hcore;

namespace funlab {

HMouseEvent::HMouseEvent( TYPE::type_t const& type )
	: _type( type ), _button( BUTTON::NONE ), f_iX( 0 ), f_iY( 0 ) {
}

int HMouseEvent::get_x( void ) const {
	return f_iX;
}

int HMouseEvent::get_y( void ) const {
	return f_iY;
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


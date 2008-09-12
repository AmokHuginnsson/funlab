/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	events.h - this file is integral part of `funlab' project.

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

#ifndef __FUNLAB_EVENTS_H
#define __FUNLAB_EVENTS_H

#include "heventlistener.h"

namespace funlab
{

class HKeyboardEvent
	{
	struct MOD
		{
		typedef enum
			{
			D_SHIFT = 0x01,
			D_CONTROL = 0x02,
			D_ALT = 0x04
			} mod_t;
		};
	int f_iCode;
	MOD::mod_t f_eMod;
public:
	HKeyboardEvent( int const& code, MOD::mod_t const& a_eMod ) : f_iCode( code ), f_eMod( a_eMod ) { }
	int get_code( void ) const { return ( f_iCode ); }
	MOD::mod_t get_mod( void ) const { return ( f_eMod ); }
	};

class HMouseEvent
	{
	struct TYPE
		{
		typedef enum
			{
			D_CLICK,
			D_PRESS,
			D_RELEASE,
			D_MOVE
			} type_t;
		};
	struct BUTTON
		{
		typedef enum
			{
			D_NONE = 0x00,
			D_1 = 0x01,
			D_2 = 0x02,
			D_3 = 0x04,
			D_4 = 0x08,
			D_5 = 0x10,
			D_6 = 0x20,
			D_7 = 0x40,
			D_8 = 0x80
			} button_t;
		};
	TYPE::type_t f_eType;
	BUTTON::button_t f_eButton;
	int f_iX;
	int f_iY;
public:
	HMouseEvent( TYPE::type_t const& );
	void set_pos( int const&, int const& );
	void set_button( BUTTON::button_t const& );
	int get_x( void );
	int get_y( void );
	BUTTON::button_t get_button( void );
	};

typedef HEventListener<HKeyboardEvent*> HKeyboardEventListener;
typedef HEventListener<HMouseEvent*> HMouseEventListener;

}

#endif /* not __FUNLAB_EVENTS_H */


/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	events.hxx - this file is integral part of `funlab' project.

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

#ifndef FUNLAB_EVENTS_HXX_INCLUDED
#define FUNLAB_EVENTS_HXX_INCLUDED

#include "heventlistener.hxx"

namespace funlab
{

class HKeyboardEvent
	{
public:
	struct MOD
		{
		typedef enum
			{
			NONE = 0x0000,
			LSHIFT = 0x0001,
			RSHIFT = 0x0002,
			SHIFT = LSHIFT | RSHIFT,
			LCONTROL = 0x0004,
			RCONTROL = 0x0008,
			CONTROL = LCONTROL | RCONTROL,
			LALT = 0x0010,
			RALT = 0x0020,
			ALT = LALT | RALT
			} mod_t;
		};
private:
	int f_iCode;
	MOD::mod_t f_eMod;
public:
	HKeyboardEvent( int const& code, MOD::mod_t const& a_eMod = MOD::NONE ) : f_iCode( code ), f_eMod( a_eMod ) { }
	int get_code( void ) const { return ( f_iCode ); }
	MOD::mod_t get_mod( void ) const { return ( f_eMod ); }
	};

class HMouseEvent
	{
public:
	struct TYPE
		{
		typedef enum
			{
			CLICK,
			PRESS,
			RELEASE,
			MOVE
			} type_t;
		};
	struct BUTTON
		{
		typedef enum
			{
			NONE = 0x00,
			B_1 = 0x01,
			B_2 = 0x02,
			B_3 = 0x04,
			B_4 = 0x08,
			B_5 = 0x10,
			B_6 = 0x20,
			B_7 = 0x40,
			B_8 = 0x80
			} button_t;
		};
private:
	TYPE::type_t f_eType;
	BUTTON::button_t f_eButton;
	int f_iX;
	int f_iY;
public:
	HMouseEvent( TYPE::type_t const& );
	void set_pos( int const&, int const& );
	void set_button( BUTTON::button_t const& );
	TYPE::type_t get_type( void ) const;
	int get_x( void ) const;
	int get_y( void ) const;
	BUTTON::button_t get_button( void ) const;
	};

typedef HEventListener<HKeyboardEvent const*> HKeyboardEventListener;
typedef HEventListener<HMouseEvent const*> HMouseEventListener;

}

#endif /* not FUNLAB_EVENTS_HXX_INCLUDED */


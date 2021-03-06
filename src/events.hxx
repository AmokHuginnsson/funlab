/* Read booker/LICENSE.md file for copyright and licensing information. */

#ifndef FUNLAB_EVENTS_HXX_INCLUDED
#define FUNLAB_EVENTS_HXX_INCLUDED

#include "heventlistener.hxx"

namespace funlab {

class HKeyboardEvent {
public:
	struct MOD {
		typedef enum {
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
	int _code;
	MOD::mod_t _mod;
public:
	HKeyboardEvent( int code, MOD::mod_t const& mod_ = MOD::NONE ) : _code( code ), _mod( mod_ ) { }
	int get_code( void ) const { return ( _code ); }
	MOD::mod_t get_mod( void ) const { return ( _mod ); }
};

class HMouseEvent {
public:
	struct TYPE {
		typedef enum {
			CLICK,
			PRESS,
			RELEASE,
			MOVE
		} type_t;
	};
	struct BUTTON {
		typedef enum {
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
	TYPE::type_t _type;
	BUTTON::button_t _button;
	int f_iX;
	int f_iY;
public:
	HMouseEvent( TYPE::type_t const& );
	void set_pos( int, int );
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


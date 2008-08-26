/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	hsurface.h - this file is integral part of `funlab' project.

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

#ifndef __HSURFACE_H
#define __HSURFACE_H

#include <yaal/hcore/hpointer.h>

namespace funlab
{

class HSurface
	{
public:
	typedef yaal::hcore::HPointer<HSurface> ptr_t;
protected:
	/*{*/
	int	f_iWidth;
	int f_iHeight;
	int f_iBPP;
	void* f_pvHandler;
	/*}*/
public:
	/*{*/
	HSurface( void );
	virtual ~HSurface( void );
	int init( int, int, int = 32 );
	void down( void );
	void refresh( void );
	void clear( void );
	yaal::u32_t get_pixel( int, int );
	void put_pixel( int, int, yaal::u32_t );
	void line( double, double, double, double, yaal::u32_t );
	yaal::u32_t RGB( int, int, int );
	bool is_valid( void );
	void toggle_fullscreen( void );
	static int surface_count( void );
	/*}*/
protected:
	/*{*/
	/*}*/
private:
	/*{*/
	static int f_iActiveSurfaces;
	HSurface( HSurface const& );
	HSurface& operator = ( HSurface const& );
	/*}*/
	};

}

#endif /* not __HSURFACE_H */

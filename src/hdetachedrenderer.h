/*
---             `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski              ---

	renderer.h - this file is integral part of `funlab' project.

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

#ifndef __RENDERER_H
#define __RENDERER_H

#include <yaal/hcore/hthread.h>
#include <yaal/tools/hanalyser.h>
#include "hsurface.h"
#include "events.h"

namespace funlab
{

class HDetachedRenderer;

typedef yaal::hcore::HThreadT<HDetachedRenderer> renderer_t;

class HDetachedRenderer
	{
protected:
	/*{*/
	int f_iRed;
	int f_iGreen;
	int f_iBlue;
	int* f_ppiNode[ 3 ];
	double f_dLowerXEdge;
	double f_dLowerYEdge;
	double f_dSize;
	double f_dResolution;
	double f_dAngleX;
	double f_dAngleY;
	double f_dAngleZ;
	double f_dDX;
	double f_dDY;
	double f_dDZ;
	double f_dFOV;
	double* f_pdXVariable;
	double* f_pdYVariable;
	double**	f_ppdLand;
	yaal::tools::HAnalyser f_oAnalyser;
	HSurface::ptr_t f_oSurface;
	/*}*/
public:
	/*{*/
	HDetachedRenderer( HKeyboardEventListener* = NULL );
	virtual ~HDetachedRenderer ( void );
	bool render_surface( yaal::hcore::HString const& );
	void draw_frame ( void );
	char const * error ( void ) const;
	int error_position ( void ) const;
	int operator() ( yaal::hcore::HThread const* const );
	void shutdown( void );
	/*}*/
protected:
	/*{*/
	void makeland ( void );
	void precount ( void );
	double sinq( unsigned int );
	double cosq( unsigned int );
	bool T( double, double, double, int& _c, int& _r );
	/*}*/
private:
	/*{*/
	bool f_bLoop;
	bool f_bBusy;
	yaal::u32_t f_ulColor;
	double f_dCosAlpha;
	double f_dSinAlpha;
	double f_dCosBeta;
	double f_dSinBeta;
	double f_dCosGamma;
	double f_dSinGamma;
	double f_dPrecountA;
	double f_dPrecountB;
	double f_dPrecountC;
	double* f_pdTrygo;
	yaal::hcore::HMutex f_oMutex;
	yaal::hcore::HSemaphore f_oSemaphore;
	renderer_t f_oThread;
	HKeyboardEventListener* f_poKeyboardEventListener;
	HDetachedRenderer( const HDetachedRenderer& );
	HDetachedRenderer& operator = ( const HDetachedRenderer& );
	/*}*/
	};

}

#endif /* not __RENDERER_H */

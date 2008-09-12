/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfunlab.h - this file is integral part of `funlab' project.

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

#ifndef __FUNLAB_HFUNLAB_H
#define __FUNLAB_HFUNLAB_H

#include "hrenderer.h"

namespace funlab
{

class HFunlab : public HRendererEngineInterface
	{
	int f_iRed;
	int f_iGreen;
	int f_iBlue;
	int* f_ppiNode[ 3 ];
	double f_dLowerXEdge;
	double f_dLowerYEdge;
	double f_dSize;
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
	yaal::tools::HAnalyser f_oAnalyser;
	HRendererSurfaceInterface* f_poRenderer;
public:
	HFunlab( HRendererSurfaceInterface* );
	virtual ~HFunlab( void );
	bool push_formula( yaal::hcore::HString const& );
protected:
	virtual void do_on_event( HMouseEvent* );
	virtual void do_on_event( HKeyboardEvent* );
	};

}

#endif /* not __FUNLAB_HFUNLAB_H */


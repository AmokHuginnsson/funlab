/*
---           `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	oplotdesc.hxx - this file is integral part of `funlab' project.

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

#ifndef FUNLAB_OPLOTDESC_HXX_INCLUDED
#define FUNLAB_OPLOTDESC_HXX_INCLUDED

#include <yaal/hcore/hstring.hxx>
#include <yaal/tools/hexpression.hxx>

namespace funlab
{

struct OPlotDesc
	{
	bool _3d;
	double long _domainLowerBound;
	double long _domainUpperBound;
	double long _rangeLowerBound;
	double long _rangeUpperBound;
	yaal::hcore::HString _formula;
	yaal::tools::HExpression _expression;
	OPlotDesc( yaal::hcore::HString const& = yaal::hcore::HString() );
	};

}

#endif /* not FUNLAB_OPLOTDESC_HXX_INCLUDED */


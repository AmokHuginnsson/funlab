/* Read booker/LICENSE.md file for copyright and licensing information. */

#ifndef FUNLAB_OPLOTDESC_HXX_INCLUDED
#define FUNLAB_OPLOTDESC_HXX_INCLUDED

#include <yaal/hcore/hstring.hxx>
#include <yaal/tools/hexpression.hxx>
#include <yaal/hcore/hstreaminterface.hxx>

namespace funlab {

struct OPlotDesc {
	double long _domainLowerBound;
	double long _domainUpperBound;
	double long _rangeLowerBound;
	double long _rangeUpperBound;
	yaal::hcore::HString _formula;
	yaal::tools::HExpression _expression;
	OPlotDesc( yaal::hcore::HString const& = yaal::hcore::HString() );
};

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, OPlotDesc const& );
OPlotDesc plot_desc_from_string( yaal::hcore::HString const& );

}

#endif /* not FUNLAB_OPLOTDESC_HXX_INCLUDED */


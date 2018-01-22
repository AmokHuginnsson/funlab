/* Read booker/LICENSE.md file for copyright and licensing information. */

#ifndef SETUP_HXX_INCLUDED
#define SETUP_HXX_INCLUDED

#include <iostream>
#include <yaal/hcore/hfile.hxx>
#include <yaal/hcore/hfile.hxx>

#include "config.hxx"

#define out ( yaal::hcore::cout << __FILE__ + OSetup::PATH_OFFSET << ":" << __LINE__ << ": " )

namespace funlab {

struct OSetup {
	bool _quiet;   /* --quiet, --silent */
	bool _verbose; /* --verbose */
	bool _stereo;
	bool _3D;
	bool _showAxis;
	bool _multiFormula;
	int _density;
	int _resolutionX;
	int _resolutionY;
	double _aspect;
	double long _domainLowerBound;
	double long _domainUpperBound;
	double long _rangeLowerBound;
	double long _rangeUpperBound;
	char * _programName;
	yaal::hcore::HString _logPath;
	yaal::hcore::HString _formula;
	yaal::hcore::HString _resourcePath;
	yaal::hcore::HString _iconPath;
	/* self-sufficient */
	OSetup( void );
	void test_setup( void );
private:
	OSetup( OSetup const& );
	OSetup& operator = ( OSetup const& );
};

extern OSetup setup;

}

#endif /* SETUP_HXX_INCLUDED */

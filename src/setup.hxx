/*
---            `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

	setup.hxx - this file is integral part of `funlab' project.

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

#ifndef SETUP_HXX_INCLUDED
#define SETUP_HXX_INCLUDED

#include <iostream>
#include <yaal/yaal.hxx>

#include "config.hxx"

#define out ( cout << __FILE__ + OSetup::PATH_OFFSET << ":" << __LINE__ << ": " )

namespace funlab
{

inline std::ostream& operator << ( std::ostream& o, yaal::hcore::HString const& s )
	{ return ( o << s.raw() ); }

struct OSetup
	{
	bool _quiet;			/* --quiet, --silent */
	bool _verbose;		/* --verbose */
	bool _stereo;
	bool f_b3D;
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

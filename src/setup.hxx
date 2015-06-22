/*
---            `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

  setup.hxx - this file is integral part of `funlab' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

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

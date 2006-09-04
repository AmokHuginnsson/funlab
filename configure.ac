dnl configure.ac is integral part of funlab project.

dnl Initialization data.
AC_INIT([[funlab]],[[0.0.0]],[[amok@frihet.ath.cx]])
AC_ARG_WITH([gtkmm],[AC_HELP_STRING([--with-gtkmm],[Compile with gtkmm gui, default is `yes' if apriopriate library can be found.])],[GUI_GTKMM=$withval],[GUI_GTKMM="auto"])
dnl `
AC_ARG_WITH([wxwidgets],[AC_HELP_STRING([--with-wxwidgets],[Compile with wxwidgets gui, default is `yes' if gtkmm is not found and apriopriate wxwidgets library can be found.])],[GUI_WXWIDGETS=$withval],[GUI_WXWIDGETS="auto"])
dnl `

AC_MSG_RESULT([--- Welcome to funlab configurator ---                   [[0%]]])
AC_MSG_RESULT([--- Initialization ... ---                               [[6%]]])

dnl Unique file in project: funlabrc is sample configuration file.
AC_MSG_RESULT([--- Finding base directory ... ---                      [[12%]]])
AC_CONFIG_SRCDIR([[funlabrc]])
AC_CONFIG_AUX_DIR([../yaal/aux])

dnl funlab is not Free Software.
AC_COPYRIGHT([[funlab (c) 2003 All rights reserved.]])

dnl funlab is still in alpha stage.
AC_REVISION([[$ Revision: 0.0.0 $]])

dnl We select default language.
AC_LANG([C++])
CXXFLAGS=["-L~/lib -L/usr/local/lib -I~/usr/include -I/usr/local/include"]

dnl Set up how configure will handle header checks.
AC_CONFIG_HEADERS([config.h])

if test ["$GUI_GTKMM"] = ["yes"] -a ["$GUI_WXWIDGETS"] = ["yes"] ; then
	AC_MSG_ERROR([Options --with-gtkmm and --with-wxwidgets are mutualy exclusive.])
fi
if test ["$GUI_GTKMM"] = ["no"] -a ["$GUI_WXWIDGETS"] = ["no"] ; then
	AC_MSG_ERROR([This software requires gui for full usefulness. ])
fi
if test ["$GUI_GTKMM"] = ["no"] ; then
	GUI_WXWIDGETS="yes"
fi
if test ["$GUI_WXWIDGETS"] = ["yes"] ; then
	GUI_GTKMM="no"
fi
if test ["$GUI_GTKMM"] = ["auto"] -a ["$GUI_WXWIDGETS"] = ["auto"] ; then
	GUI_GTKMM="yes"
	GUI_WXWIDGETS="no"
fi

dnl Cheching for machine type.
AC_MSG_RESULT([--- Checking for machine type ... ---                   [[14%]]])
AC_CANONICAL_HOST()

dnl Testing presence of c compiler
if test ["$CXX"] = ["colorgcc"] -o ["$CXX"] = ["colorg++"] ; then
	CXX=["g++"]
fi
if test ["$CC"] = ["colorgcc"] ; then
	CC=["gcc"]
fi
AC_MSG_RESULT([--- Testing presence of c compiler ... ---              [[19%]]])
AC_PROG_CC
dnl and for c++ compiler
AC_MSG_RESULT([--- Testing presence of c++ compiler ... ---            [[25%]]])
AC_PROG_CXX
dnl and for c preprocesor
AC_MSG_RESULT([--- Looking standrad development software ... ---       [[33%]]])
AC_PROG_CPP
AC_PROG_CXXCPP
AC_CHECK_PROG([CTAGS],[exctags],[exctags],[ctags])

dnl Looking for main libc headers
AC_MSG_RESULT([--- Looking for main libc headers ... ---               [[41%]]])
AC_HEADER_MAJOR

dnl We need to know what operating system funlab will be compiled on.
AC_MSG_RESULT([--- Guessing host operating system ... ---              [[64%]]])
AC_CHECK_FILE(
	[[/etc/rc]],
	[AC_DEFINE([[__HOST_OS_TYPE_FREEBSD__]]) _HOST________=['FreeBSD. ---  ']],
	[AC_CHECK_FILE(
		[[/etc/debconf.conf]],
		[AC_DEFINE([[__HOST_OS_TYPE_DEBIAN__]]) _HOST________=['Debian. ---   ']],
		[AC_CHECK_FILE(
			[[/etc/poldek/poldek.conf]],
			[AC_DEFINE([[__HOST_OS_TYPE_PLD__]]) _HOST________=['PLD. ---      ']],
			[AC_CHECK_FILE(
				[[/etc/random-seed]],
				[AC_DEFINE([[__HOST_OS_TYPE_SLACKWARE__]]) _HOST________=['Slackware. ---']],
				[AC_MSG_ERROR([Can not recognize host operating system type!])]
			)]
		)]
	)]
)
AC_MSG_RESULT([--- Guessed host operating system is $_HOST________     [[70%]]])

dnl What special compiler flags we can set?
AC_MSG_RESULT([--- Guessing specific compiler/linker features ... ---  [[71%]]])
_FLAGS=''
CXXFLAGS_ORIG=$CXXFLAGS;
CXXFLAGS=["-Wextra"]
AC_MSG_CHECKING([does gcc support -Wextra])
RESULT=["no"]
AC_TRY_COMPILE([],[],[AC_SUBST([EXTRA_WARNINGS],[-Wextra])] [RESULT=["yes"]],
							[AC_SUBST([EXTRA_WARNINGS],[-W])])
AC_MSG_RESULT([$RESULT])
CXXFLAGS=["-Wshadow -Werror"]
AC_TRY_COMPILE([#include <pthread.h>],[],
							[AC_SUBST([WARN_SHADOW],[-Wshadow])
							_FLAGS=["-Wshadow"]],
							[AC_MSG_WARN([[Can not use -Wshadow!]])])
CXXFLAGS=["-pedantic-errors"]
AC_TRY_COMPILE([#include <stdlib.h>],[],
							[AC_SUBST([PEDANTIC_ERRORS],	[-pedantic-errors])
							_FLAGS=["$_FLAGS -pedantic-errors"]],
							[AC_MSG_WARN([[Can not use -pedantic-errors!]])])
CXXFLAGS=["$CXXFLAGS -Wl,--no-undefined -Wl,--fatal-warnings"]
AC_LINK_IFELSE([[extern int my_undefined_symbol;int main(void){return(my_undefined_symbol);}]],
				[CXXFLAGS=$CXXFLAGS_ORIG],
				[AC_SUBST([SPECIAL_LINKER_FLAGS],
					[[-Wl,--allow-shlib-undefined]])]
					_FLAGS=[['-Wl,--allow-shlib-undefined. ---']]
					CXXFLAGS=["$CXXFLAGS_ORIG -Wl,--allow-shlib-undefined"]
					[AC_MSG_WARN([[We must use -Wl,--allow-shlib-undefined!]])])
if test ["$_FLAGS"] = [""] ; then
	_FLAGS='none. ---                      [[76%]]'
fi
AC_MSG_RESULT([--- Guessed fetures are: $_FLAGS])

dnl Now we can look for all needed libraries.
CPPFLAGS=$CXXFLAGS;
AC_MSG_RESULT([--- Looking for needed libraries ... ---                [[77%]]])
AC_CHECK_LIB([ncurses],[initscr],
							[],[AC_MSG_ERROR([Can not continue without ncurses.])])

if test ["$GUI_GTKMM"] = ["yes"] ; then
	AC_SUBST([GUI_TYPE],[GTKMM])
	AC_SUBST([GUI_LIBS],[["\$(shell pkg-config --libs-only-l \`pkg-config --list-all|awk '/glademm/{print \$\$1}'\`)"]])
	AC_SUBST([GUI_COMPILER_FLAGS],[["\$(shell pkg-config --cflags --libs-only-other \`pkg-config --list-all|awk '/glademm/{print \$\$1}'\`)"]])
	AC_SUBST([GUI_LINKER_FLAGS],[["\$(shell pkg-config --libs-only-other --libs-only-L \`pkg-config --list-all|awk '/glademm/{print \$\$1}'\`)"]])
	ln -sf gui.c.gtkmm src/gui.c
fi
if test ["$GUI_WXWIDGETS"] = ["yes"] ; then
	AC_SUBST([GUI_TYPE],[WXWIDGETS])
	AC_SUBST([GUI_LIBS],[["\$(shell wx-config --libs|sed -e 's/-pthread//') wx_gtk_xrc-2.4"]])
	AC_SUBST([GUI_COMPILER_FLAGS],[["\$(shell wx-config --cflags)"]])
	ln -sf gui.c.wxwidgets src/gui.c
fi
dnl WARNING!!! Names in c++ objects are _mangled_ so we need to use
dnl WARNING!!! this cryptic names here (I hope it is portable).
dnl UPDATE!: Now I know it is not portable across different versions of
dnl UPDATE!: compiller, so I used tricky hack with 'main'.
AC_CHECK_LIB([hcore],[hcore_init],
							[],[AC_MSG_ERROR([Can not continue without hcore.])])
AC_CHECK_LIB([hconsole],[hconsole_init],
							[],[AC_MSG_ERROR([Can not continue without hconsole.])])
AC_CHECK_LIB([tools],[tools_init],
							[],[AC_MSG_ERROR([Can not continue without tools.])])
AC_CHECK_LIB([dbwrapper],[dbwrapper_init],
							[],[AC_MSG_ERROR([Can not continue without dbwrapper.])])
AC_CHECK_LIB([hdata],[main],
							[],[AC_MSG_ERROR([Can not continue without hdata.])])
AC_MSG_RESULT([--- All needed libraries found. ---                     [[83%]]])

AC_MSG_RESULT([--- Looking for needed headers ... ---                  [[84%]]])
AC_CHECK_HEADERS([[ncurses/ncurses.h]],_NCURSES_HEADER=['ncurses/ncurses.h'],
	[AC_CHECK_HEADERS([[ncurses.h]],_NCURSES_HEADER=['ncurses.h'],
			[AC_MSG_ERROR([Can not continue without ncurses devel!])])])
AC_CHECK_HEADERS([[yaal.h]],,
			[AC_MSG_ERROR([[Can not continue without yaal devel!]])])
AC_MSG_RESULT([--- Done looking for needed header, all found. ---      [[92%]]])

dnl We need to check if out ncurses devel is not brain-damaged.
AC_MSG_RESULT([--- Libraries/environment sanity check ... ---          [[93%]]])
AC_MSG_CHECKING([if your ncurses-devel is brain-damaged])
CXXFLAGS=["-W -Wall -Werror -Wcast-align -Wconversion -Wwrite-strings -pedantic-errors -Wcast-qual"]
AC_COMPILE_IFELSE([#include <$_NCURSES_HEADER>
void foo(){printw("x");}],[AC_MSG_RESULT([[no :)]])],
	[AC_MSG_RESULT([[yes :(]])
		AC_MSG_ERROR([Can not continue with brain-damaged ncurses.])])

dnl We have to set up proper compiler/linker flags.

AC_OUTPUT([Makefile])
AC_MSG_RESULT([--- All ok, good luck! ---                     	       [[100%]]])
AC_MSG_RESULT([[Now you can type \`make'.]])


pushdef([MY_PACKAGE_VERSION],[0.0.0-esyscmd(date +%Y%m%d|tr -d '\n')])
AC_INIT([[funlab]],[MY_PACKAGE_VERSION],[[amok@codestation.org]])
dnl It is expected that client applications have link to yaal's _aux.
dnl This macro has to be here due to primitivism of libtoolize.
AC_CONFIG_AUX_DIR([_aux])
m4_include([_aux/client-configure.m4])
YAAL_CONFIGURE_CLIENT_INIT(funlab,MY_PACKAGE_VERSION)

m4_ifndef([PKG_PROG_PKG_CONFIG], [m4_fatal([Cannot self-test for pkg-config, macro PKG_PROG_PKG_CONFIG is not available.])])
PKG_PROG_PKG_CONFIG()
if test ["x${PKG_CONFIG}"] = ["x"] ; then
	AC_MSG_ERROR([Cannot continue without pkg-config.])
fi
PKG_CHECK_MODULES(GTKMM, [gtkmm-3.0],,[AC_MSG_ERROR([Cannot continue without gtkmm library.])])
PKG_CHECK_MODULES(SDL2, [sdl2],,[AC_MSG_ERROR([Cannot continue without SDL2 library.])])

YAAL_CONFIGURE_CLIENT_FINALIZE


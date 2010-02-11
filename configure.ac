pushdef([MY_PACKAGE_VERSION],[0.0.0-esyscmd(date +%Y%m%d|tr -d '\n')])
AC_INIT([[funlab]],[MY_PACKAGE_VERSION],[[amok@codestation.org]])
dnl It is expected that client applications have link to yaal's _aux.
dnl This macro has to be here due to primitivism of libtoolize.
AC_CONFIG_AUX_DIR([_aux])
m4_include([_aux/client-configure.m4])
YAAL_CONFIGURE_CLIENT_INIT(funlab,MY_PACKAGE_VERSION)
GUI_LIBS=$(pkg-config --list-all | awk '/glademm/{print $1}' | xargs pkg-config --libs-only-l)
GUI_COMPILER_FLAGS=$(pkg-config --list-all | awk '/glademm/{print $1}' | xargs pkg-config --cflags --libs-only-other)
GUI_LINKER_FLAGS=$(pkg-config --list-all | awk '/glademm/{print $1}' | xargs pkg-config --libs-only-other --libs-only-L)
AC_SUBST([GUI_LIBS],$GUI_LIBS)
AC_SUBST([GUI_COMPILER_FLAGS],$GUI_COMPILER_FLAGS)
AC_SUBST([GUI_LINKER_FLAGS],$GUI_LINKER_FLAGS)
YAAL_CONFIGURE_CLIENT_FINALIZE

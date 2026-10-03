AC_PREREQ(2.64)
AC_INIT([IP2Location-Varnish], [1.0.4], [support@ip2location.com], [IP2Location-Varnish])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/vmod_ip2location.vcc)
AM_CONFIG_HEADER(config.h)

AC_CANONICAL_SYSTEM
AC_LANG(C)

AM_INIT_AUTOMAKE([foreign])

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CC_STDC
if test "x$ac_cv_prog_cc_c99" = xno; then
	AC_MSG_ERROR([Could not find a C99 compatible compiler])
fi
AC_PROG_CPP

AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET

# Check for rst utilities
AC_CHECK_PROGS(RST2MAN, [rst2man rst2man.py], "no")
if test "x$RST2MAN" = "xno"; then
	AC_MSG_WARN([rst2man not found - not building man pages])
fi
AM_CONDITIONAL(HAVE_RST2MAN, [test "x$RST2MAN" != "xno"])

# Check for pkg-config
PKG_PROG_PKG_CONFIG
# PKG_CHECK_MODULES([libIP2Location], [IP2Location])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/stdlib.h])

# Check for python
AC_CHECK_PROGS(PYTHON, [python3 python2.7 python2.6], "no")
if test "x$PYTHON" = "xno"; then
	AC_MSG_ERROR([Python > 2.5 is needed to build this vmod, please install Python 2.6 or greater.])
fi

# Vinyl Cache 9.1 (and other VCache flavors that adopt its build macros)
# ship VCACHE_REQUIRE in their m4 files. Older Varnish Cache releases only
# provide the VARNISH_* macros, so fall back to those when it is missing.
# Varnish Cache 6.6 is the minimum because of the vmod_priv_methods API.
m4_ifdef([VCACHE_REQUIRE], [
	VCACHE_REQUIRE(
		[[vinyl], [9.1.0]],
		[[varnish], [6.6.0]],
	)
	VCACHE_VMODS([ip2location])
	VMOD_CFLAGS="$VCACHEAPI_CFLAGS"
	VMOD_DATAROOTDIR="$VCACHEAPI_DATAROOTDIR"
], [
	VARNISH_PREREQ([6.6.0])
	VARNISH_VMODS([ip2location])
	VMOD_CFLAGS="$VARNISHAPI_CFLAGS"
	VMOD_DATAROOTDIR="$VARNISHAPI_DATAROOTDIR"
])
AC_SUBST([VMOD_CFLAGS])
AC_SUBST([VMOD_DATAROOTDIR])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT

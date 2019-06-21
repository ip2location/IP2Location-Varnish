AM_CPPFLAGS = @VMOD_INCLUDES@
AM_CFLAGS = -Wall -Wextra -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_ip2location.la

libvmod_ip2location_la_LDFLAGS = -module -export-dynamic -avoid-version -shared \
	-lIP2Location

libvmod_ip2location_la_SOURCES = \
	vcc_if.c \
	vcc_if.h \
	vmod_ip2location.c

vcc_if.c vcc_if.h: @VMODTOOL@ $(top_srcdir)/src/vmod_ip2location.vcc
	@VMODTOOL@ $(top_srcdir)/src/vmod_ip2location.vcc

EXTRA_DIST = vmod_ip2location.vcc

CLEANFILES = $(builddir)/vcc_if.c $(builddir)/vcc_if.h

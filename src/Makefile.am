AM_CPPFLAGS = $(VMOD_CFLAGS)
AM_CFLAGS = -Wall -Wextra -Werror

vmod_LTLIBRARIES = libvmod_ip2location.la

libvmod_ip2location_la_LDFLAGS = $(VMOD_LDFLAGS)
libvmod_ip2location_la_LIBADD = -lIP2Location

libvmod_ip2location_la_SOURCES = \
	vmod_ip2location.c

nodist_libvmod_ip2location_la_SOURCES = \
	vcc_ip2location_if.c \
	vcc_ip2location_if.h

@BUILD_VMOD_IP2LOCATION@

EXTRA_DIST = vmod_ip2location.vcc

DISTCLEANFILES = vmod_vcs_version.txt

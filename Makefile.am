ACLOCAL_AMFLAGS = -I m4 -I ${VMOD_DATAROOTDIR}/aclocal

SUBDIRS = src

doc_DATA = README.rst LICENSE
dist_man_MANS = vmod_ip2location.3
EXTRA_DIST = README.rst LICENSE CHANGES.rst

MAINTAINERCLEANFILES = $(dist_man_MANS)

vmod_ip2location.3: README.rst
if HAVE_RST2MAN
	${RST2MAN} README.rst $@
else
	@echo "========================================"
	@echo "You need rst2man installed to make dist"
	@echo "========================================"
	@false
endif

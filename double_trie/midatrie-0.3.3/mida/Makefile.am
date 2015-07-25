SUBDIRS = utils vmem imitrie midatrie atrie .

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

#until they're stable
EXTRA_DIST = twotrie.h

lib_LTLIBRARIES = libmidatrie.la

libmidatrieincludedir = $(includedir)/mida
libmidatrieinclude_HEADERS = \
	atrie.h \
	imitrie.h \
	midatrie.h

libmidatrie_la_SOURCES = blank.cxx
libmidatrie_la_LIBADD = \
	atrie/libatrie.la \
	midatrie/libmidatrie.la \
	imitrie/libimitrie.la \
	vmem/libvmem.la \
	utils/libutils.la


MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

INCLUDES = -I$(srcdir) -I$(top_srcdir)

noinst_LTLIBRARIES = libmidatrie.la

# until they're stable
EXTRA_DIST = twotrie.h twotrie.cxx

midatrieincludedir = $(includedir)/mida/midatrie
midatrieinclude_HEADERS = \
	midadefs.h \
	midatrie.h \
	branches.h \
	tails.h

libmidatrie_la_SOURCES = \
	midatrie.cxx \
	branches.cxx \
	tails.cxx \
	midadefs.h \
	midatrie.h \
	branches.h \
	tails.h


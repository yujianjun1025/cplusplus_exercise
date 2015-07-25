MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

INCLUDES = -I$(srcdir) -I$(top_srcdir)

bin_PROGRAMS = mida

mida_SOURCES = \
	mida.cxx \
	getopt1.c \
	getopt.c \
	getopt.h

mida_LDADD = \
	$(top_builddir)/mida/libmidatrie.la


MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(srcdir) -I$(top_srcdir)

#until they're stable
EXTRA_DIST = test-twotrie.cxx

TESTS = \
	test-midatrie \
	test-atrie

check_PROGRAMS = \
	test-midatrie \
	test-atrie

test_midatrie_SOURCES = \
	test-midatrie.cxx \
	idx1.cxx \
	idx2.cxx \
	idx3.cxx \
	idx4.cxx \
	idx1.h \
	idx2.h \
	idx3.h \
	idx4.h
test_midatrie_LDADD = \
	$(top_builddir)/mida/libmidatrie.la

#test_twotrie_SOURCES = \
#	test-twotrie.cxx \
#	idx1.cxx \
#	idx2.cxx \
#	idx3.cxx \
#	idx4.cxx \
#	idx1.h \
#	idx2.h \
#	idx3.h \
#	idx4.h
#test_twotrie_LDADD = \
#	$(top_builddir)/mida/libmidatrie.la

test_atrie_SOURCES = \
	test-atrie.cxx
test_atrie_LDADD = \
	$(top_builddir)/mida/libmidatrie.la


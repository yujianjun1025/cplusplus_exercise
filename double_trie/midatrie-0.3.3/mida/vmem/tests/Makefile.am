MAINTAINERCLEANFILES = $(srcdir)Makefile.in

INCLUDES = -I$(top_srcdir)

TESTS = \
	test-vmem \
	test-vheap

check_PROGRAMS = \
	test-vmem \
	test-vheap

test_vmem_SOURCES = \
	test-vmem.cxx

test_vmem_LDADD = \
	$(top_builddir)/mida/vmem/libvmem.la \
	$(top_builddir)/mida/utils/libutils.la

test_vheap_SOURCES = \
	test-vheap.cxx

test_vheap_LDADD = \
	$(top_builddir)/mida/vmem/libvmem.la \
	$(top_builddir)/mida/utils/libutils.la


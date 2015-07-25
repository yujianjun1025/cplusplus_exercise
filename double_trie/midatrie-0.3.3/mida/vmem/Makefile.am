SUBDIRS = . tests

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

INCLUDES = -I$(srcdir) -I$(top_srcdir)

noinst_LTLIBRARIES = libvmem.la

vmemincludedir = $(includedir)/mida/vmem
vmeminclude_HEADERS = \
	dataheap.h \
	heaplist.h \
	vheap.h \
	vmem.h

libvmem_la_SOURCES = \
	dataheap.cxx \
	heaplist.cxx \
	vheap.cxx \
	vmem.cxx \
	pagemap.cxx \
	dataheap.h \
	heaplist.h \
	vheap.h \
	vmem.h \
	pagemap.h




LIBS = -shared 

#FIXME (srcdir??)
AM_CPPFLAGS=-I../src

lib_LTLIBRARIES = libd_rcd_sym.la
libd_rcd_sym_la_SOURCES = d_rcd_sym.cc
libd_rcd_sym_la_INCLUDE = ../src

#libd_rcd_sym_ladir = .
include_HEADERS = d_rcd_sym.h

#%.so: %.cc
#	g++ -DHAVE_CONFIG_H -I. -I../src -I.. -DTRACE_UNTESTED  -g -O2 -MD -MP -MF -c -o $@ $< -fPIC -shared

#clean:
#	rm *.so *~

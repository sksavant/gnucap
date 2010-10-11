

LIBS = -shared 

#FIXME (srcdir??)
AM_CPPFLAGS=-I../src

lib_LTLIBRARIES = libd_rcd_sym.la libd_bti_inf.la

libd_rcd_sym_la_SOURCES = d_rcd_sym.cc
libd_bti_inf_la_SOURCES = d_bti_inf.cc

include_HEADERS = d_rcd_sym.h d_bti_inf.h


# obsolete stuff:

#%.so: %.cc
#	g++ -DHAVE_CONFIG_H -I. -I../src -I.. -DTRACE_UNTESTED  -g -O2 -MD -MP -MF -c -o $@ $< -fPIC -shared

#clean:
#	rm *.so *~

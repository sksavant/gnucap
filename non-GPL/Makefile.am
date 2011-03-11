
LIBS = -shared 

#FIXME (srcdir??)
AM_CPPFLAGS=-I../src

lib_LTLIBRARIES = libd_rcd_sym.la libd_bti_inf.la
                  #libd_rcd_sym_v2.la \
						#libd_rcd_sym_v3.la

libd_rcd_sym_la_SOURCES = d_rcd_sym.cc\
                          d_rcd_sym_v2.cc\
                          d_rcd_sym_v3.cc

libd_bti_inf_la_SOURCES = d_bti_inf.cc

include_HEADERS = d_rcd_sym.h d_bti_inf.h d_rcd_sym_v2.h


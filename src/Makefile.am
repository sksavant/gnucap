## -*- Makefile -*-
##
## $Id: Makefile.am,v 1.8 2010-07-29 12:37:18 felix Exp $
##
##                            COPYRIGHT
##
## This file is part of "Gnucap", the Gnu Circuit Analysis Package
##
##  Copyright (C) 2005, 2006 Dan McMahill
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 3 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA
##  02110-1301, USA.



#------------------------------------------------------------------------
#------------------------------------------------------------------------
# These source files can be omitted, and supplied as plugins.
# device models defined in high level .model files
MODELS = \
d_mos1.model d_mos2.model d_mos3.model \
d_mos4.model d_mos5.model d_mos6.model d_mos7.model d_mos8.model \
d_mos123.model d_mos_base.model d_mos.model \
d_bjt.model d_diode.model d_diode2.model
#d_rcd.model d_bti.model 

# device models hand coded in C++
D_SRCS = \
d_admit.cc d_cap.cc d_cccs.cc d_ccvs.cc d_coil.cc \
d_cs.cc d_poly_g.cc d_poly_cap.cc d_res.cc d_switch.cc \
d_trln.cc d_vcg.cc d_vcr.cc d_vcvs.cc d_vs.cc  \
d_rcd.cc d_bti.cc  d_branchvs.cc

#d_vcvs2.cc 


RAW_HDRS=
gnucap_LDADD=
lib_LTLIBRARIES=

# matrix_example_LDADD = libgnucap.la

#old/unused
#if IVERILOG_VVP
#D_SRCS += d_vvp.cc extlib.cc
#endif

D_IVL_DEV =  d_invert.cc d_clock.cc  d_latch.cc d_lfsr.cc  

D_IVL = d_ivl_base.cc d_ivl_ports.cc \
			 e_ivl_compile.cc $(D_IVL_DEV)


D_IVL_O = $(D_IVL:%.cc=%.o)
libgnucap_ivl_la_CPPFLAGS=
libgnucap_ivl_la_LDFLAGS=

if IVERILOG
# D_SRCS += D_IVL

lib_LTLIBRARIES += libgnucap_ivl.la
libgnucap_ivl_la_SOURCES = ${D_IVL}

#	libvvpg_la_SOURCES = vvp_main.cc
#  bin_PROGRAMS += libvvpg
gnucap_LDADD += libgnucap_ivl.la
gnucap_LDADD += @IVERILOG@

RAW_HDRS += d_vvp.h d_ivl.h e_ivl_compile.h d_ivl_ports.h
# doesnt work (?)
#dvvp_lo_CFLAGS = -I$(IVERILOG_PATH)/include
libgnucap_ivl_la_CPPFLAGS += -I$(IVERILOG_PATH)
libgnucap_ivl_la_LDFLAGS += FOO
endif



# behavioral modeling functions
BM_SRCS = \
bm_complex.cc bm_cond.cc bm_exp.cc bm_fit.cc bm_generator.cc \
bm_model.cc bm_poly.cc bm_posy.cc bm_pulse.cc bm_pwl.cc bm_sffm.cc \
bm_sin.cc bm_tanh.cc bmm_table.cc bmm_semi.cc

# utility commands, usually one file per command
# some have multiple commands in a file
C_SRCS = \
c_clear.cc c_comand.cc c_delete.cc c_exp.cc c_list.cc c_save.cc \
c_measure.cc c_modify.cc c_param.cc c_prbcmd.cc c_status.cc \
c_sweep.cc c_sim.cc c_system.cc c_for.cc

# simulation commands, usually multiple files for one command
S_SRCS = \
s_ac.cc s_dc.cc s_fo.cc \
s_ddc.cc s_ttt.cc s_tt_set.cc \
s_sock.cc \
s_tr.cc s_tr_set.cc s_tr_swp.cc

# netlist languages
LANG_SRCS = \
lang_spice.cc lang_spectre.cc lang_verilog.cc

# parameter functions
FUNC_SRCS = \
func_core.cc measure_eval.cc \
measure_max.cc measure_min.cc measure_slewrate.cc measure_cross.cc \
measure_integral.cc measure_average.cc measure_rms.cc measure_at.cc
#------------------------------------------------------------------------
RAW_SRCS = \
$(BM_SRCS) \
$(D_SRCS) \
$(C_SRCS) \
$(S_SRCS) \
$(LANG_SRCS) \
$(FUNC_SRCS) \
m_expression_dump.cc m_expression_in.cc m_expression_reduce.cc \
m_base_in.cc m_base_math.cc m_fft.cc  m_spline.cc \
ap_construct.cc ap_convert.cc ap_error.cc ap_get.cc ap_match.cc ap_skip.cc \
l_ftos.cc l_pmatch.cc l_timer.cc l_trim.cc l_wmatch.cc \
io.cc io_contr.cc io_error.cc io_findf.cc io_out.cc io_xopen.cc \
u_function.cc u_lang.cc u_nodemap.cc u_opt1.cc u_opt2.cc u_parameter.cc \
u_prblst.cc u_probe.cc u_sim_data.cc u_xprobe.cc \
s__init.cc s__out.cc s__solve.cc \
d_subckt.cc d_logic.cc d_logicmod.cc \
e_base.cc e_card.cc e_node.cc e_model.cc e_compon.cc \
e_elemnt.cc e_ccsrc.cc e_storag.cc e_cardlist.cc e_adplist.cc\
bm_value.cc bm.cc \
c__cmd.cc c_attach.cc c_file.cc c_genrat.cc c_debug.cc \
c_nodeset.cc \
findbr.cc plot.cc globals.cc e_adp.cc e_adp_mos.cc \
startup.cc
#------------------------------------------------------------------------
RAW_HDRS += \
md.h ap.h mode.h constant.h declare.h patchlev.h \
l_compar.h l_dispatcher.h l_denoise.h \
l_lib.h l_stlextra.h l_timer.h \
m_cpoly.h m_divdiff.h m_interp.h m_matrix.h m_spline.h m_wave.h \
m_base.h m_expression.h m_phase.h \
io_.h io_error.h io_trace.h io_matrix.h io_misc.h \
u_cardst.h u_function.h u_lang.h u_limit.h u_nodemap.h u_opt.h u_parameter.h \
u_prblst.h u_probe.h u_sim_data.h u_sdp.h u_status.h u_time_pair.h u_xprobe.h \
s__.h s_tr.h e_adp.h s_tt.h\
e_base.h e_card.h e_node.h e_aux.h e_model.h e_compon.h e_subckt.h \
e_adplist.h \
e_elemnt.h e_ccsrc.h e_storag.h e_cardlist.h \
d_coment.h d_dot.h d_logic.h d_subckt.h \
bm.h \
c_comand.h globals.h extlib.h d_cap.h


if ADMS
ADMS_SRCS = e_adms.cc d_hicum0.cc d_ota_simpl.cc d_ota_medium.cc  d_ctest.cc \
				d_rtest.cc
#not yet d_opamp.cc
RAW_SRCS += $(ADMS_SRCS)
endif
#------------------------------------------------------------------------ 
## The modelgen generated files
MODELSRCS= ${MODELS:.model=.cc} ${MODELS:.model=.h}

# extlib_o_SOURCES = extlib.h 

# henjo:
MODELCODE= ${MODELS:.model=.cc}
MODELHDR= ${MODELS:.model=.h}

MODELOBJS= ${MODELS:.model=.${OBJEXT}}

# nodist_gnucap_SOURCES= ${SRCS1} ${MODELSRCS} ${SRCS2}
# henjo:
# nodist_gnucap_SOURCES= main.cc
# nodist_matrix_example_SOURCES= m_matrix_example.cc


# gibsnich
# LDFLAGS += FOOOO
# tutnix
# LDADD = FOOOO


SRCS1= 
SRCS2= ${RAW_SRCS} 
#${RAW_HDRS}

# make all of the model object files depend on all of the model sources
# this is a bit overkill, but for example the bjt model makes use of the
# diode model and we want to be sure and capture that dependency
#
${MODELOBJS}: ${MODELSRCS}

## Include the .model files in the distfile as well as the rest of the
## non-generated sources

EXTRA_DIST= ${MODELS} ${SRCS1} ${SRCS2} \
configure.old Make1 test_readline.cc spice-wrapper.cc \
Make3 Makefile.template \
Make2.g++ Make2.Debug Make2.mingw32 \
Makefile.am



#matrix_example
bin_PROGRAMS = gnucap 
gnucap_SOURCES = main.cc ${SRCS1} ${MODELSRCS} ${SRCS2}
#gnucap_SOURCES += ${SRCS1} ${MODELSRCS} ${SRCS2}
gnucap_CPPFLAGS = $(AM_CPPFLAGS)

if SHARED_LIB

lib_LTLIBRARIES += libgnucap.la
libgnucap_la_SOURCES = ${SRCS1} ${MODELSRCS} ${SRCS2}
#gnucap_LDADD += libgnucap.la
#libgnucap_la_CFLAGS = $(AM_CFLAGS)

#else


# kills dispatcher (FIXME?!)
# gnucap_LDFLAGS = -all-static
#
# use ./configure --enable-static instead
# gnucap_LDFLAGS = -static

#ifdef static_binaries
#gnucap_LDFLAGS = -static-libtool-libs 
#endif


endif



libgnucap_includedir = $(includedir)/gnucap
libgnucap_include_HEADERS =   ${RAW_HDRS}
 
#gnucap_include_HEADERS = ${RAW_HDRS}

gnucap_includedir=$(includedir)/gnucap 


### Clean out the modelgen generated files
# CLEANFILES= ${MODELSRCS}

## Suffix rules for modelgen.
##

SUFFIXES= .model
MODELGEN= @MODELGEN@

MAKEDEPEND = gcc -M $(CPPFLAGS) -o $*.d $<

IVL=`which iverilog` -v

if IVERILOG 
# FIXME: compile_wrap not there...
iverilog-info:
	@echo $(IVERILOG_PATH)

# $(IVERILOG_PREFIX)/lib/ivl/gc.tgt

$(D_IVL_DEV): %.cc : %.v
	$(IVL) -tgc $< -o $@

endif

%.cc : %.model %.h ${MODELGEN}${EXEEXT}
	${MODELGEN} -cc $<

# if adms ...
ADMS_GC_XML=../adms/gnucap.xml
ADMS_HTM_XML=../adms/admst_htm.xml

%.cc : %.va $(ADMS_GC_XML)
	admsXml $< -e $(ADMS_GC_XML) -o $*

#move to ../doc?
%.htm : %.va $(ADMS_HTM_XML)
	admsXml $< -e $(ADMS_HTM_XML) -o $@

%.h : %.model ${MODELGEN}${EXEEXT}
	${MODELGEN} -h $<

if ADMS
$(ADMS_SRCS): $(ADMS_GC_XML)
libgnucap_include_HEADERS += e_adms.h

$(ADMS_GC_XML):
	make -C ../adms

endif

d_vvp.lo extlib.lo: CPPFLAGS+= @IVERILOG_INCLUDE@

$(D_IVL_O): CPPFLAGS+= IVL_CPPFLAGS

# FIXME: -I iverilog only for .v stuff

.DELETE_ON_ERROR: *.cc

# include $(srcdir)/depend
#
# %.o: .deps/%.


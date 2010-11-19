NAMES=opamp1 diode tp tpcoil inv_amp inv_amp_bug

CIRQUITS= $(NAMES:%=%.ckt)
GNUCAP_PS=$(NAMES:%=%.gc.ps)
NGSPICE_PS=$(NAMES:%=%.ng.ps)

# STYLE="data lines"
STYLE="data dots"

#GNUCAP=/usr/bin/gnucap
GNUCAP=gnucap 2>/dev/null
COLOR=color
LW=2
LOGSCALE=""
#LOGSCALE="set logscale x;"

short_test: rcd_tw.1.2.ps \
            rcd_tr.1.2.ps \
            rcd_cons_tw.1.3.ps \
            rcd_cons_tw_more.1.3.ps \
            bti_tw.1.2.ps


compare: mos_level1_tw.1.2-3.ps  \
         mos_level1_tr.1.2-3.ps \
         mos_level1_tw.1.6.ps  \
         mos_level1_tr.1.4.ps

%.ckt: %.sch models.mod
	gnetlist -g spice-sdb -o $@ $<

%.ckt:
	@echo creating dummy circuit
	touch $@

%.out: %.sim %.ckt
	$(GNUCAP) -b $< | ./gc2gp > $@

%.gc.out: %.ckt %.sim Makefile
	$(GNUCAP) -b $< | perl -wnl -e '$$_=~s/u/e-6/g; $$_=~s/f/e-15/g; $$_=~s/n/e-9/g; $$_=~s/p/e-12/g;   print $$_ unless /^[0-9a-zA-Z\#\*\.]/' |grep -v parameter >  $@
#	             |  perl -wnl -e 'print $$_ unless /f/'

%.sp.out: %.ckt %.sim
	ngspice -b $< | tee $@

%.out.raw: %.ckt %.sim
	ngspice -b $< -r $@

%.ng.ps: %.out.raw
	echo -e "set hcopydevtype=postscript\n hardcopy $@ V(vout) V(vin)" | ngnutmeg $<

%.2.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:2" |gnuplot

# nicht so einfach
%.auto.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:3" |gnuplot


%.1.2.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:2" |gnuplot

%.1.2-3.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:2, '$<' using 1:3" |gnuplot

%.1.3.ps: %.out
	echo "set style ${STYLE}; ${EXTRA_CMD} \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:3" |gnuplot

%.1.4.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced color dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:4" |gnuplot

%.1.5.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced color dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:5" |gnuplot

%.1.6.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced color dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:6" |gnuplot

%.1.7.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced color dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:7" |gnuplot

%.7.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced color dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:7" |gnuplot

%.8.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced color dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:8" |gnuplot

%.1.9.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced color dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:9" |gnuplot


%.1.3-11.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced color dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:3, '$<' using 1:4, \
		     '$<' using 1:5, '$<' using 1:6, \
		     '$<' using 1:7, '$<' using 1:8, \
		     '$<' using 1:9, \
		     '$<' using 1:10, '$<' using 1:11" |gnuplot

%.1.3-8.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:3, '$<' using 1:4, \
		     '$<' using 1:5, '$<' using 1:6, \
		     '$<' using 1:7, '$<' using 1:8" |gnuplot

%.1.3-4.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:3, '$<' using 1:4" |gnuplot

%.1.4-5.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:4, '$<' using 1:5" |gnuplot

%.2.3.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced color dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:3" |gnuplot

%.2.5.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:5" |gnuplot

%.2.3-4.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:3,\
		'$<' using 2:4" |gnuplot

%.2.3-5.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:3,\
		'$<' using 2:4, '$<' using 2:5" |gnuplot

%.2.3-6.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:3,\
		'$<' using 2:4, '$<' using 2:5,  '$<' using 2:6" |gnuplot

%.2.3-19.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced mono dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:3, '$<' using 2:4, '$<' using 2:5,  '$<' using 2:6 , '$<' using 2:7,\
		     '$<' using 2:8, '$<' using 2:9,  '$<' using 2:10, '$<' using 2:11,\
		     '$<' using 2:12, '$<' using 2:13,  '$<' using 2:14, '$<' using 2:15,\
		     '$<' using 2:16, '$<' using 2:17,  '$<' using 2:18, '$<' using 2:19\
		     " |gnuplot


%.2.4.ps: %.out
	echo "set style ${STYLE}; ${EXTRA_CMD} \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:4" |gnuplot

%.2.4-5.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:4, '$<' using 2:5" |gnuplot

%.2.4-6.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:4, '$<' using 2:5,  '$<' using 2:6" |gnuplot

%.2.4-7.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced mono dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:4, '$<' using 2:5,  '$<' using 2:6 , '$<' using 2:7" |gnuplot

%.2.4-11.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced mono dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:4, '$<' using 2:5,  '$<' using 2:6 , '$<' using 2:7,\
		     '$<' using 2:8, '$<' using 2:9,  '$<' using 2:10, '$<' using 2:11" |gnuplot

%.2.4-20.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced mono dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:4, '$<' using 2:5,  '$<' using 2:6 , '$<' using 2:7,\
		     '$<' using 2:8, '$<' using 2:9,  '$<' using 2:10, '$<' using 2:11,\
		     '$<' using 2:12, '$<' using 2:13,  '$<' using 2:14, '$<' using 2:15,\
		     '$<' using 2:16, '$<' using 2:17,  '$<' using 2:18, '$<' using 2:19,\
		     '$<' using 2:20" |gnuplot

%.2.5-6.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced mono dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:5, '$<' using 2:6" |gnuplot

%.2.6.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 2:6" |gnuplot

%.3.1.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 3:1" |gnuplot

%.3.2.ps: %.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced ${COLOR} dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 3:2" |gnuplot

%.gc.ps: %.gc.out
	echo "set style ${STYLE}; \
		set terminal postscript portrait enhanced mono dashed lw ${LW} 'Helvetica' 14; \
		set output '$@'; \
		plot '$<' using 1:2, '$<' using 1:3" |gnuplot

%.drc.out: %.sch
	gnetlist -g drc2 $< -o $@


realclean: clean
	rm -f $(GNUCAP_PS) $(NGSPICE_PS)


clean:
	rm -f *~ $(CIRQUITS)
	rm -f *.raw *.out 
	rm -f *.ps

%.tr: %.sim %.ckt
	echo "plot '<gnucap  -b $< 2>/dev/null | ./gc2gp ' using 1:3 with lines; pause 10" | gnuplot
%.tw: %.sim %.ckt
	echo "splot '< gnucap  -b $< 2>/dev/null | ./gc2gp ' using 2:1:4 with lines; pause 10" | gnuplot

%.tol: %.sim
	echo "#!/bin/bash" > $@
	echo "line=line" >> $@	
	echo '[ "x" = "x$$2" ] || line=$$2' >> $@
	echo '[ "x" = "x$$1" ] && echo no arg && exit 1' >> $@
	echo -n "echo \"$(LOGSCALE)" >>$@
	echo -n "plot " >>$@
	for i in `seq 8`; do \
		echo ".options adpreltol $$i" > batch; \
		gnucap -b $< 2>/dev/null  | ./gc2gp > $*out$$i & \
		sleep 0.1; \
		echo -n "'$*out$$i' using 1:\$$1 with \$$line," >> $@; \
	done 
	echo " 'dummy' using 1:2 with line; pause 300 \" | gnuplot " >>$@
	chmod +x $@

plots: inv_amp_p_1ks.2.3.ps 

1ksplot.ps: inv_amp_p_1ks.out
	echo "set style ${STYLE}; \
	set logscale y; \
	set terminal postscript portrait enhanced mono dashed lw ${LW} 'Helvetica' 14; \
	set output '$@'; \
	plot '$<' using 2:3" |gnuplot


all:

install:

.PRECIOUS: %.out %.ckt

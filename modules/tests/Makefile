
NTESTS=bti_sweep_tw.1.3.ps \
       bti_single_tw.1.3.ps \
       bti_sweep_tol.1.3.ps \
       rcd_tw.1.2.ps \
       rcd_cons_tw.1.3.ps \
       bti_single_compare.1.2-3.ps 
       #bti_tw.1.2-3.ps

       # bti_sin_tw.1.2.ps \
LONG_TESTS=bti_ac_single.1.2.ps \
           bti_ac_5_7.1.2.ps \
           bti_ac.1.2.ps \
			  mos_level1_tw.1.2.ps \
			  mos_level1_tw.1.4.ps \
			  mos_level1_tr.1.2.ps

MOS_TESTS=


all-here: nongpl-tests longertests twice.diff
clean: clean-here

twice1.out twice2.out: rcd_tw_twice.sim
	gnucap -b $< 2>/dev/null

twice.diff: twice1.out twice2.out
	diff twice2.out twice1.out |tee > $@
	

include ../../tests/Makefile

nongpl-tests: ${NTESTS} 

longertests: ${LONG_TESTS}

clean-here:
	rm -rf *.ps


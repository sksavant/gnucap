
NTESTS=bti_sweep_tw.1.3.ps \
       bti_single_tw.1.3.ps \
       bti_sweep_tol.1.3.ps \
		 rcd_tw.1.2.ps \
		 rcd_cons_tw.1.3.ps \
		 bti_single_compare.1.2-3.ps \
		 bti_sin_tw.1.2-3.ps


all-here: gc2gp nongpl-tests 
clean: clean-here

include ../../tests/Makefile

nongpl-tests: ${NTESTS} 

clean-here:
	rm -rf *.ps

gc2gp:
	ln ../../tests/gc2gp . -sf

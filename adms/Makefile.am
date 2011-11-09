COMMONS = hicum0 rtest

DIR_INCLUDE_GNUCAP = gnucap_src
GNUCAP = gnucap_src/gnucap

all: gnucap.xml

# -Wall -Wextra -fPIC -g -c
CPP_FLAGS = -DTRACE_UNTESTED -DNGSPICE_17 -DPREDICTOR -DHAS_STDLIB -fPIC -g -shared 

%.so: %_common.cc
	g++ -I. -I$* -I$(DIR_INCLUDE_GNUCAP) $(CPP_FLAGS) $< -o $@
                
%_common.cc: gnucap.xml
	admsXml $*/$*.va -e $< -D $*_common

%.xml: %.xml.in convert_symb/convert some_analog_functions.xml 
	convert_symb/convert $< -o $@ -addxml
	cat some_analog_functions.xml >> $@

convert_symb/convert:
	cd convert_symb/ && make

clean:
	rm -f *.cc *.h *.o *~ *.c *.xml *.hxx *.so

%.out: %.sp %.so
	$(GNUCAP) -b $< | tee  $@



IVL_HOME=.
IVERILOG=../icarus/verilog/driver/iverilog
IVERILOG_VPI=../icarus/verilog/iverilog-vpi
GNUCAP=../../src/gnucap

all: links netlist-dkc.vvp bindsigs.vpi

links:
	ln -sf  ../icarus/verilog/vpi/bindsigs.cc bindsigs.cc 
	ln -sf  ../icarus/verilog/vpi/v2005_math.vpi v2005_math.vpi 
	ln -sf  ../icarus/verilog/vpi/va_math.vpi va_math.vpi 
	ln -sf  ../icarus/verilog/vpi/system.vpi system.vpi 
       
%.vvp: %.v
	$(IVERILOG) -I. -o $@ $<

bindsigs.vpi: bindsigs.cc
	$(IVERILOG_VPI) -I../icarus/verilog/vvp $<

run: bindsigs.vpi netlist-dkc.vvp
	../../src/gnucap -c verilog netlist-dkc.sp </dev/null

%.sim: %.sp %.vvp bindsigs.vpi
	$(GNUCAP) -c verilog $< </dev/null

kreuz: bindsigs.vpi kreuz.vvp
	../../src/gnucap -c verilog kreuz.sp </dev/null

#netlist-dkc.vvp: netlist-dkc.v
#	$(IVERILOG) -I/usr/include/iverilog -o $@ $<

#bindsigs.vpi: bindsigs.cc
#	$(IVERILOG_VPI) -I../icarus/verilog/vvp $<

#run: bindsigs.vpi
#	../../src/gnucap -c verilog netlist-dkc.sp </dev/null

clean:
	rm -f *.o
	rm -f bindsigs.cc
	rm -f *.vpi
	rm -f *.vvp

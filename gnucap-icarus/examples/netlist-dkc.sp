
load ../gnucap-icarus.so
//* /home/jp/arails/arails0809/demo/g65/simulation/testLib/ringosc/20090526122839/hspice/netlist.sp
spice
vgnd 0 0 dc=0

.subckt inv__d (gnd vdd out in)
iin  in  gnd extpwl
+external="../icarus/verilog/vvp/libvvp.so:-M. -mbindsigs -v netlist-dkc.vvp:%m.in"
vout out gnd extpwl
+external="../icarus/verilog/vvp/libvvp.so:-M. -mbindsigs -v netlist-dkc.vvp:%m.out"
.ends

.include ./mos.gc

.verilog

module inv__a (gnd vdd out in);
pmos4  #(.w(1u),.l(240n),.m(1),.ad(2.05e-13),.as(2.05e-13),.pd(1.41e-06),.ps(1.41e-06)) Mmp1(out,in,vdd,vdd);
nmos4 #(.w(750n),.l(240n),.m(1),.ad(2.05e-13),.as(2.05e-13),.pd(1.41e-06),.ps(1.41e-06)) Mmn1(out,in,gnd,gnd);
endmodule

inv__a x1 (0,ndd,n2,n1);
inv__a x2 (0,ndd,n3,n2);
inv__a x3 (0,ndd,n4,n3);
inv__a x4 (0,ndd,n5,n4);
inv__d x5 (0,ndd,n1,n5);

vsource #(.dc(3)) Vv1 (.p(ndd), .n(0))

spice
.print tran v(nodes)
.transientt 0.0 1n 10p
.end


//* /home/jp/arails/arails0809/demo/g65/simulation/testLib/ringosc/20090526122839/hspice/netlist.sp
spice
.include ./mos.gc
vgnd gnd! 0 dc=0
.verilog
pmos4  #(.w(1u),.l(240n),.m(1),.ad(2.05e-13),.as(2.05e-13),.pd(1.41e-06),.ps(1.41e-06)) Mmp1(n8,n6,vdd!,vdd!);
nmos4 #(.w(750n),.l(240n),.m(1),.ad(2.05e-13),.as(2.05e-13),.pd(1.41e-06),.ps(1.41e-06)) Mmn1(n8,n6,gnd!,gnd!);
pmos4 #(.w(1u),.l(240n),.m(1),.ad(2.05e-13),.as(2.05e-13),.pd(1.41e-06),.ps(1.41e-06)) Mmp2(n14,n8,vdd!,vdd!);
nmos4 #(.w(750n),.l(240n),.m(1),.ad(2.05e-13),.as(2.05e-13),.pd(1.41e-06),.ps(1.41e-06)) Mmn2(n14,n8,gnd!,gnd!);
pmos4 #(.w(1u),.l(240n),.m(1),.ad(2.05e-13),.as(2.05e-13),.pd(1.41e-06),.ps(1.41e-06)) Mmp3(n6,n14,vdd!,vdd!);
nmos4 #(.w(750n),.l(240n),.m(1),.ad(2.05e-13),.as(2.05e-13),.pd(1.41e-06),.ps(1.41e-06)) Mmn3(n6,n14,gnd!,gnd!);
vsource #(.dc(3)) Vv1 (.p(vdd!), .n(gnd!))
spice
.print tran v(n14) v(nodes)
.tran 0.0 10n 10p

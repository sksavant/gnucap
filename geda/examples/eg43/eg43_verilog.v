'
//v 20081231 1 
place #(.x(47500),.y(45600))  (.port(node8853));
place #(.x(47500),.y(46900))  (.port(node7523));
net #() net8854 (.p(node8853),.n(node7523));
place #(.x(48200),.y(46900))  (.port(node8912));
place #(.x(48400),.y(46900))  (.port(node4436));
net #() net8265 (.p(node8912),.n(node4436));
place #(.x(48100),.y(47500))  (.port(node3009));
place #(.x(48100),.y(47400))  (.port(node1857));
net #() net4010 (.p(node3009),.n(node1857));
//T 51800 40900 9 16 1 0 0 0 1 
//Inverter Gate 
//T 50100 40400 9 10 1 0 0 0 1 
//inverter.sch 
//T 50500 40100 9 10 1 0 0 0 1 
//1 
//T 52200 40100 9 10 1 0 0 0 1 
//1 
//T 54200 40400 9 10 1 0 0 0 1 
//1 
//T 54300 40100 9 10 1 0 0 0 1 
//Facundo J Ferrer 
//T 43300 48400 5 10 0 1 0 0 1 
//device=spice-subcircuit-LL 
//T 43300 48500 5 10 1 1 0 0 1 
//refdes=A1 
//T 43300 48200 5 10 1 1 0 0 1 
//model-name=cmos_inverter 
//} 
place #(.x(49100),.y(46200))  (.port(node_4325));
spice-IO #(.basename(spice-subcircuit-IO-1.sym)) P1 (.1_io(node_4325));
place #(.x(47500),.y(46300))  (.port(node_6660));
spice-IO #(.basename(spice-subcircuit-IO-1.sym)) P4 (.1_io(node_6660));
spice-IO #(.basename(spice-subcircuit-IO-1.sym)) P2 (.1_io(node3009));
place #(.x(48100),.y(44700))  (.port(node_1379));
spice-IO #(.basename(spice-subcircuit-IO-1.sym)) P3 (.1_io(node_1379));
//T 43400 47600 5 10 0 1 0 0 1 
//device=model 
//T 43400 47500 5 10 1 1 0 0 1 
//refdes=A2 
//T 44600 47200 5 10 1 1 0 0 1 
//model-name=nmos4 
//T 43800 47000 5 10 1 1 0 0 1 
//file=../model/nmos4.model 
//} 
//T 43400 46400 5 10 0 1 0 0 1 
//device=model 
//T 43400 46300 5 10 1 1 0 0 1 
//refdes=A3 
//T 44600 46000 5 10 1 1 0 0 1 
//model-name=pmos4 
//T 43800 45800 5 10 1 1 0 0 1 
//file=../model/pmos4.model 
//} 
place #(.x(48400),.y(47400))  (.port(node2186));
net #() net6273 (.p(node4436),.n(node2186));
net #() net5377 (.p(node2186),.n(node1857));
place #(.x(48400),.y(44700))  (.port(node4727));
net #() net2602 (.p(node4727),.n(node_1379));
place #(.x(48100),.y(46200))  (.port(node3561));
net #() net5695 (.p(node3561),.n(node_4325));
place #(.x(48100),.y(46100))  (.port(node_7815));
place #(.x(48200),.y(45600))  (.port(node_3331));
place #(.x(48100),.y(45100))  (.port(node_2124));
NMOS_TRANSISTOR #(.basename(asic-nmos-1.sym),.l(3u),.model-name(nmos4),.w(1u)) M2 (.D_pas(node_7815),.B_pas(node_3331),.S_pas(node_2124),.G_pas(node8853));
place #(.x(48100),.y(46400))  (.port(node_369));
PMOS_TRANSISTOR #(.basename(asic-pmos-1.sym),.l(1u),.model-name(pmos4),.w(10u)) M1 (.S_pas(node1857),.B_pas(node8912),.D_pas(node_369),.G_pas(node7523));
net #() net3342 (.p(node_7815),.n(node_369));
place #(.x(48400),.y(45600))  (.port(node7441));
net #() net2101 (.p(node4727),.n(node7441));
net #() net8669 (.p(node7441),.n(node_3331));
net #() net4396 (.p(node_1379),.n(node_2124));

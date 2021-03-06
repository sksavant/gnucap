'
//v 20040111 1 
place #(.x(67000),.y(52900))  (.port(node_482));
place #(.x(67000),.y(53600))  (.port(node_8855));
place #(.x(67000),.y(54000))  (.port(node_2913));
place #(.x(67000),.y(54400))  (.port(node_9467));
place #(.x(66300),.y(55300))  (.port(node_7483));
place #(.x(65500),.y(55300))  (.port(node_5358));
place #(.x(64700),.y(54000))  (.port(node_2989));
place #(.x(64700),.y(52900))  (.port(node_6676));
555timer #(.basename(lm555-1.sym),.footprint(DIL)) U1 (._(node_482),._(node_8855),._(node_2913),._(node_9467),._(node_7483),._(node_5358),._(node_2989),._(node_6676));
place #(.x(64200),.y(52900))  (.port(node_8377));
!_gnd-1.sym #(.basename(gnd-1.sym))  (.1_pwr(node_8377));
place #(.x(67900),.y(53600))  (.port(node_6118));
!_switcap-capacitor-1.sym #(.basename(switcap-capacitor-1.sym),.footprint(RADIAL_CAN),.value(1.0)) C1 (.A_io(node_8855),.B_io(node_6118));
place #(.x(68100),.y(55200))  (.port(node_4383));
place #(.x(67200),.y(55200))  (.port(node_7007));
resistor #(.basename(resistor-1.sym),.footprint(R025),.value(4K7)) R1 (.2_pas(node_4383),.1_pas(node_7007));
place #(.x(68600),.y(55200))  (.port(node_2860));
!_vcc-1.sym #(.basename(vcc-1.sym))  (.1_pwr(node_2860));
place #(.x(70000),.y(51400))  (.port(node_7106));
place #(.x(69100),.y(51400))  (.port(node_6040));
LED #(.basename(led-2.sym),.footprint(LED)) D2 (.1_pas(node_7106),.2_pas(node_6040));
place #(.x(69800),.y(53400))  (.port(node_1654));
place #(.x(69800),.y(52400))  (.port(node_1783));
place #(.x(69200),.y(52900))  (.port(node_9389));
2N3053 #(.basename(2N3055-1.sym),.footprint(TO92)) Q1 (.3_pas(node_1654),.2_pas(node_1783),.1_pas(node_9389));
place #(.x(70800),.y(51900))  (.port(node_5798));
place #(.x(70800),.y(50900))  (.port(node_6784));
place #(.x(70200),.y(51400))  (.port(node_1299));
2N2905 #(.basename(2N3904-1.sym),.footprint(TO92)) Q2 (.3_pas(node_5798),.1_pas(node_6784),.2_pas(node_1299));
place #(.x(64200),.y(54000))  (.port(node_5769));
place #(.x(64200),.y(53100))  (.port(node_6513));
!_capacitor-3.sym #(.basename(capacitor-3.sym),.footprint(RADIAL_CAN)) C2 (.+_pas(node_5769),.-_pas(node_6513));
place #(.x(68100),.y(54400))  (.port(node_8912));
place #(.x(67200),.y(54400))  (.port(node_774));
resistor #(.basename(resistor-1.sym),.footprint(R025),.value(100K)) R2 (.2_pas(node_8912),.1_pas(node_774));
place #(.x(68900),.y(52900))  (.port(node_7452));
place #(.x(68000),.y(52900))  (.port(node_7111));
resistor #(.basename(resistor-1.sym),.footprint(R025),.value(220)) R3 (.2_pas(node_7452),.1_pas(node_7111));
place #(.x(68800),.y(51400))  (.port(node_7769));
place #(.x(67900),.y(51400))  (.port(node_5841));
resistor #(.basename(resistor-1.sym),.footprint(R025),.value(100)) R4 (.2_pas(node_7769),.1_pas(node_5841));
net #() gnd (.p(node_6676),.n(node_8377));
place #(.x(68400),.y(53600))  (.port(node6323));
net #() gnd (.p(node_6118),.n(node6323));
net #() net9604 (.p(node_2989),.n(node_5769));
!_gnd-1.sym #(.basename(gnd-1.sym))  (.1_pwr(node6323));
net #() gnd (.p(node_6513),.n(node_8377));
place #(.x(65800),.y(54000))  (.port(node3439));
place #(.x(65800),.y(54400))  (.port(node2562));
net #() net2142 (.p(node3439),.n(node2562));
place #(.x(64200),.y(54400))  (.port(node116));
net #() net5132 (.p(node2562),.n(node116));
net #() net7291 (.p(node116),.n(node_5769));
place #(.x(68100),.y(54000))  (.port(node4499));
net #() net1250 (.p(node3439),.n(node4499));
net #() net4298 (.p(node4499),.n(node_8912));
place #(.x(67000),.y(55200))  (.port(node7957));
net #() net462 (.p(node7957),.n(node_9467));
net #() net6691 (.p(node7957),.n(node_7007));
net #() net2116 (.p(node_774),.n(node_9467));
!_vcc-1.sym #(.basename(vcc-1.sym))  (.1_pwr(node_5358));
net #() vcc (.p(node_5358),.n(node_7483));
net #() net2432 (.p(node_482),.n(node_7111));
place #(.x(67500),.y(52900))  (.port(node9229));
place #(.x(67500),.y(51400))  (.port(node3731));
net #() net4267 (.p(node9229),.n(node3731));
net #() net36 (.p(node3731),.n(node_5841));
net #() net5743 (.p(node_7452),.n(node_9389));
place #(.x(69800),.y(52200))  (.port(node_2643));
!_gnd-1.sym #(.basename(gnd-1.sym))  (.1_pwr(node_2643));
place #(.x(70800),.y(49600))  (.port(node_7162));
!_gnd-1.sym #(.basename(gnd-1.sym))  (.1_pwr(node_7162));
net #() net9547 (.p(node_7106),.n(node_1299));
net #() net9754 (.p(node_7769),.n(node_6040));
net #() gnd (.p(node_1783),.n(node_2643));
place #(.x(69800),.y(54500))  (.port(node_5389));
place #(.x(69800),.y(53600))  (.port(node_503));
LEDseries #(.basename(led-2.sym),.footprint(LED)) D1 (.1_pas(node_5389),.2_pas(node_503));
place #(.x(70800),.y(50800))  (.port(node_5574));
place #(.x(70800),.y(49900))  (.port(node_8064));
LEDseries #(.basename(led-2.sym),.footprint(LED)) D? (.1_pas(node_5574),.2_pas(node_8064));
net #() net6459 (.p(node_503),.n(node_1654));
place #(.x(70800),.y(55200))  (.port(node6559));
net #() vcc (.p(node_4383),.n(node6559));
place #(.x(69800),.y(55200))  (.port(node1693));
net #() vcc (.p(node_5389),.n(node1693));
net #() gnd (.p(node_7162),.n(node_8064));
net #() vcc (.p(node_5798),.n(node6559));
net #() net8985 (.p(node_6784),.n(node_5574));

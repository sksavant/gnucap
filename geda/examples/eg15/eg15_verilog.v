'
//v 20030525 
place #(.x(79500),.y(59500))  (.port(node_1279));
place #(.x(79500),.y(58500))  (.port(node_5593));
place #(.x(78900),.y(59000))  (.port(node_1499));
!_npn-3.sym #(.basename(npn-3.sym)) Q1 (.C_pas(node_1279),.E_pas(node_5593),.B_pas(node_1499));
place #(.x(78000),.y(59000))  (.port(node_3869));
!_resistor-2.sym #(.basename(resistor-2.sym)) R1 (.2_pas(node_1499),.1_pas(node_3869));
place #(.x(79500),.y(57600))  (.port(node_7483));
!_resistor-2.sym #(.basename(resistor-2.sym)) R3 (.2_pas(node_5593),.1_pas(node_7483));
place #(.x(78900),.y(58500))  (.port(node_2521));
place #(.x(78900),.y(57600))  (.port(node_2640));
!_resistor-2.sym #(.basename(resistor-2.sym)) R2 (.2_pas(node_2521),.1_pas(node_2640));
place #(.x(80400),.y(58500))  (.port(node_4116));
!_resistor-2.sym #(.basename(resistor-2.sym)) R4 (.2_pas(node_5593),.1_pas(node_4116));
net #() net1508 (.p(node_2521),.n(node_1499));
place #(.x(77600),.y(57600))  (.port(node2818));
place #(.x(80800),.y(57600))  (.port(node5689));
net #() net9592 (.p(node2818),.n(node5689));
place #(.x(77600),.y(59500))  (.port(node8730));
net #() net9718 (.p(node_1279),.n(node8730));
!_input-1.sym #(.basename(input-1.sym))  (._(node8730));
!_input-1.sym #(.basename(input-1.sym))  (._(node8730));
!_input-1.sym #(.basename(input-1.sym))  (._(node8730));
place #(.x(77600),.y(59000))  (.port(node_4629));
!_input-1.sym #(.basename(input-1.sym))  (._(node_4629));
!_input-1.sym #(.basename(input-1.sym))  (._(node2818));
place #(.x(80800),.y(58500))  (.port(node_1624));
!_output-1.sym #(.basename(output-1.sym))  (._(node_1624));
!_output-1.sym #(.basename(output-1.sym))  (._(node5689));
//T 76200 59400 9 10 1 0 0 0 
//VSync 
//T 76200 58900 9 10 1 0 0 0 
//HSync 
//T 76300 57500 9 10 1 0 0 0 
//GND 
//T 81700 57500 9 10 1 0 0 0 
//GND 
//T 81700 58400 9 10 1 0 0 0 
//CSync 
net #() net447 (.p(node_4629),.n(node_3869));
net #() net4146 (.p(node_4116),.n(node_1624));

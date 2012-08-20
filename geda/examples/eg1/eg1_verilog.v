'
v 20100214 2
place #(.x(43800),.y(48100))  (.port(node_6376));
place #(.x(43800),.y(49000))  (.port(node_8170));
VOLTAGE_SOURCE #(.basename(voltage-3.sym),.value(10)) Vin (.2_pwr(node_6376),.1_pwr(node_8170));
place #(.x(45000),.y(49000))  (.port(node_1331));
place #(.x(44100),.y(49000))  (.port(node_54));
RESISTOR #(.basename(resistor-1.sym),.value(2k)) R1 (.2_pas(node_1331),.1_pas(node_54));
place #(.x(45200),.y(49000))  (.port(node_3990));
place #(.x(45200),.y(48100))  (.port(node_100));
RESISTOR #(.basename(resistor-1.sym),.value(1k)) R2 (.2_pas(node_3990),.1_pas(node_100));
net #() net1532 (.p(node_6376),.n(node_100));
place #(.x(44300),.y(48100))  (.port(node_494));
net #() net8643 (.p(node_8170),.n(node_54));
!_gnd-1.sym #(.basename(gnd-1.sym))  (.1_pwr(node_494));
net #() net4207 (.p(node_1331),.n(node_3990));

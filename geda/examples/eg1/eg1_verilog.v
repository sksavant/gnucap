'
//v 20100214 2 
place #(.x(43800),.y(48100))  (.port(node_2953));
place #(.x(43800),.y(49000))  (.port(node_2808));
VOLTAGE_SOURCE #(.basename(voltage-3.sym),.value(10)) Vin (.2_pwr(node_2953),.1_pwr(node_2808));
place #(.x(45000),.y(49000))  (.port(node_3617));
place #(.x(44100),.y(49000))  (.port(node_3082));
RESISTOR #(.basename(resistor-1.sym),.value(2k)) R1 (.2_pas(node_3617),.1_pas(node_3082));
place #(.x(45200),.y(49000))  (.port(node_8083));
place #(.x(45200),.y(48100))  (.port(node_708));
RESISTOR #(.basename(resistor-1.sym),.value(1k)) R2 (.2_pas(node_8083),.1_pas(node_708));
net #() net5325 (.p(node_2953),.n(node_708));
place #(.x(44300),.y(48100))  (.port(node_9474));
!_gnd-1.sym #(.basename(gnd-1.sym))  (.1_pwr(node_9474));
net #() net3530 (.p(node_2808),.n(node_3082));
net #() net3276 (.p(node_3617),.n(node_8083));

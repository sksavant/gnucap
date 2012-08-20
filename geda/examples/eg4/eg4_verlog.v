'
v 20031231 1
place #(.x(30300),.y(22700))  (.port(node_5887));
place #(.x(30500),.y(22300))  (.port(node_6589));
place #(.x(30300),.y(21900))  (.port(node_5918));
place #(.x(29800),.y(22300))  (.port(node_7063));
place #(.x(30300),.y(19900))  (.port(node2137));
place #(.x(30300),.y(19700))  (.port(node8859));
net #() net4487 (.p(node2137),.n(node8859));
!_pmos-2.sym #(.basename(pmos-2.sym))  (.D_pas(node_5887),.B_pas(node_6589),.S_pas(node_5918),.G_pas(node_7063));
place #(.x(29800),.y(20300))  (.port(node_1906));
place #(.x(30300),.y(20700))  (.port(node_3077));
place #(.x(30500),.y(20300))  (.port(node_3011));
place #(.x(31100),.y(20300))  (.port(node3583));
net #() net8073 (.p(node_3011),.n(node3583));
!_nmos-2.sym #(.basename(nmos-2.sym))  (.G_pas(node_1906),.D_pas(node_3077),.B_pas(node_3011),.S_pas(node2137));
place #(.x(31100),.y(19700))  (.port(node7884));
net #() net1996 (.p(node3583),.n(node7884));
place #(.x(30900),.y(22300))  (.port(node9826));
net #() net967 (.p(node_6589),.n(node9826));
place #(.x(31000),.y(22900))  (.port(node_536));
place #(.x(31400),.y(19700))  (.port(node_8292));
place #(.x(28600),.y(21300))  (.port(node_5394));
place #(.x(30900),.y(21300))  (.port(node_7970));
place #(.x(29600),.y(20300))  (.port(node9067));
net #() net9985 (.p(node_1906),.n(node9067));
!_out-1.sym #(.basename(out-1.sym))  (._(node_7970));
!_in-1.sym #(.basename(in-1.sym))  (._(node_5394));
!_gnd-1.sym #(.basename(gnd-1.sym))  (.1_pwr(node_8292));
!_vdd-1.sym #(.basename(vdd-1.sym))  (.1_pwr(node_536));
net #() net3369 (.p(node_5918),.n(node_3077));
place #(.x(29600),.y(22300))  (.port(node1057));
net #() net4682 (.p(node9067),.n(node1057));
net #() net9890 (.p(node1057),.n(node_7063));
place #(.x(29600),.y(21300))  (.port(node7025));
net #() net2851 (.p(node_5394),.n(node7025));
place #(.x(30300),.y(22900))  (.port(node4077));
net #() net7559 (.p(node4077),.n(node_536));
net #() net3514 (.p(node8859),.n(node_8292));
net #() net9799 (.p(node_5887),.n(node4077));
place #(.x(30900),.y(22900))  (.port(node5784));
net #() net7019 (.p(node9826),.n(node5784));
place #(.x(30300),.y(21300))  (.port(node7858));
net #() net6862 (.p(node7858),.n(node_7970));
T 28900 18900 9 10 1 0 0 0 1
DRAWN BY ARNO PETERS

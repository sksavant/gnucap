'
//v 20031231 1
place #(.x(28100),.y(28500))  (.port(node_4089));
place #(.x(26800),.y(28700))  (.port(node_457));
place #(.x(26800),.y(28300))  (.port(node_4498));
!_7408-1.sym #(.basename(7408-1.sym)) U101 (.Y_out(node_4089),.A_in(node_457),.B_in(node_4498));
place #(.x(26800),.y(30300))  (.port(node_268));
place #(.x(26800),.y(29900))  (.port(node_1054));
place #(.x(28100),.y(30100))  (.port(node_6152));
!_7486-1.sym #(.basename(7486-1.sym),.slot(1)) U100 (.A_in(node_268),.B_in(node_1054),.Y_out(node_6152));
place #(.x(26300),.y(28700))  (.port(node6054));
place #(.x(26300),.y(30300))  (.port(node7859));
place #(.x(25900),.y(28300))  (.port(node5240));
place #(.x(25900),.y(29900))  (.port(node4700));
place #(.x(29600),.y(30100))  (.port(node5626));
net #() S_H (.p(node5626),.n(node_6152));
net #() net340 (.p(node4700),.n(node5240));
net #() net5776 (.p(node_4498),.n(node5240));
net #() net3993 (.p(node6054),.n(node7859));
net #() net407 (.p(node_457),.n(node6054));
place #(.x(29600),.y(28500))  (.port(node1201));
net #() C_H (.p(node1201),.n(node_4089));
//T 26400 27300 9 20 1 0 0 0 1
//Half Adder
place #(.x(23600),.y(25900))  (.port(node_7219));
place #(.x(23600),.y(25500))  (.port(node_3284));
place #(.x(24900),.y(25700))  (.port(node_5517));
!_7486-1.sym #(.basename(7486-1.sym),.slot(2)) U100 (.A_in(node_7219),.B_in(node_3284),.Y_out(node_5517));
place #(.x(29800),.y(24000))  (.port(node_2782));
place #(.x(28500),.y(24200))  (.port(node_4499));
place #(.x(28500),.y(23800))  (.port(node_2048));
!_7408-1.sym #(.basename(7408-1.sym),.slot(3)) U101 (.Y_out(node_2782),.A_in(node_4499),.B_in(node_2048));
place #(.x(28500),.y(25700))  (.port(node_3594));
place #(.x(28500),.y(25300))  (.port(node_4087));
place #(.x(29800),.y(25500))  (.port(node_5208));
!_7486-1.sym #(.basename(7486-1.sym),.slot(3)) U100 (.A_in(node_3594),.B_in(node_4087),.Y_out(node_5208));
place #(.x(27000),.y(25300))  (.port(node7536));
place #(.x(27000),.y(23800))  (.port(node662));
place #(.x(27000),.y(22900))  (.port(node5419));
place #(.x(28000),.y(24200))  (.port(node8245));
place #(.x(28000),.y(25700))  (.port(node7665));
place #(.x(23600),.y(24200))  (.port(node8880));
place #(.x(23100),.y(24200))  (.port(node1658));
place #(.x(22800),.y(23800))  (.port(node1008));
place #(.x(23600),.y(23800))  (.port(node3251));
place #(.x(25800),.y(24000))  (.port(node5708));
place #(.x(24900),.y(24000))  (.port(node5767));
place #(.x(25800),.y(23300))  (.port(node8300));
place #(.x(23100),.y(25900))  (.port(node6050));
place #(.x(22800),.y(25500))  (.port(node2486));
place #(.x(33900),.y(25500))  (.port(node2435));
net #() S_H (.p(node_5208),.n(node2435));
net #() net1584 (.p(node1008),.n(node2486));
net #() net6969 (.p(node1658),.n(node6050));
net #() net8831 (.p(node5708),.n(node8300));
net #() net3205 (.p(node5708),.n(node5767));
net #() net2864 (.p(node1008),.n(node3251));
net #() net8653 (.p(node8880),.n(node1658));
net #() net2826 (.p(node8245),.n(node7665));
net #() net5160 (.p(node_4499),.n(node8245));
net #() net7977 (.p(node7536),.n(node5419));
net #() net4962 (.p(node662),.n(node_2048));
net #() net3021 (.p(node_4087),.n(node7536));
place #(.x(32700),.y(23800))  (.port(node8778));
place #(.x(33900),.y(23800))  (.port(node6523));
net #() C_H (.p(node8778),.n(node6523));
//T 26500 21900 9 20 1 0 0 0 1
//Full Adder
place #(.x(31000),.y(23300))  (.port(node8151));
place #(.x(31000),.y(23600))  (.port(node1057));
!_7408-1.sym #(.basename(7408-1.sym),.slot(2)) U101 (.Y_out(node5767),.A_in(node8880),.B_in(node3251));
net #() net411 (.p(node8151),.n(node1057));
net #() net6094 (.p(node8300),.n(node8151));
//T 33100 17700 9 20 1 0 0 0 1
net #() net8813 (.p(node_3594),.n(node_5517));
//Half and Full Adders
//T 32000 17100 9 10 1 0 0 0 1
//adders.sch
//T 32500 16800 9 10 1 0 0 0 1
//1
//T 34100 16800 9 10 1 0 0 0 1
//1
//T 36100 17100 9 10 1 0 0 0 1
//1
//T 36100 16800 9 10 1 0 0 0 1
//Ales Hvezda
place #(.x(31400),.y(24000))  (.port(node_4741));
place #(.x(31400),.y(23600))  (.port(node_2828));
!_7432-1.sym #(.basename(7432-1.sym),.slot(1)) U102 (.A_in(node_4741),.B_in(node_2828),.Y_out(node8778));
place #(.x(21500),.y(25500))  (.port(node7991));
net #() Y_H (.p(node_3284),.n(node7991));
net #() net2986 (.p(node_2782),.n(node_4741));
net #() net326 (.p(node_2828),.n(node1057));
place #(.x(21500),.y(25900))  (.port(node7238));
net #() X_H (.p(node_7219),.n(node7238));
place #(.x(21500),.y(22900))  (.port(node856));
net #() Z_H (.p(node5419),.n(node856));
place #(.x(24700),.y(29900))  (.port(node5604));
net #() Y_H (.p(node_1054),.n(node5604));
place #(.x(24700),.y(30300))  (.port(node307));
net #() X_H (.p(node_268),.n(node307));

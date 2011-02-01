`timescale 100ps/1ps

module inv__a (out,in);
   input in;
   output out;
endmodule

module inv__d (out,in);
   input in;
   output out;
   reg 	  out;   

   parameter mode        = 0;
   parameter gate_delay  = 0;
   parameter init_v      = 0.0;
   parameter rise        = 1e-12;
   parameter vdd         = 3.0;
   parameter vss         = 0.0;

   initial begin
      $bindsigs(in,out); 
   end

   always @(out) begin
      $display($realtime," OUT->",out," IN=",in); 
      $sync_out(out);
   end
 
   always @(in) begin
      $display($realtime," IN44 ->",in); 
      out <= #10 !in;
   end
   
endmodule

module _;

   inv__a x1 (n2,n1);
//    inv__a x2 (n3,n2);
//    inv__d x3 (n4,n3);
//    inv__a x4 (n5,n4);

    inv__d x5 (n1,n5);


endmodule // top

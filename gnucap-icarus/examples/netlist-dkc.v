`timescale 10ps/1ps

module inv__d (out,in);
   input in;
   output out;
   reg 	  out;   

   parameter mode        = 0;
   parameter gate_delay  = 0;
   parameter init_v      = 0.0;
   parameter rise        = 2e-11;
   parameter fall        = 2e-11;
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
      $display($realtime," IN ->",in); 
      out <= #0 !in;
   end
   
endmodule

module main;
//    inv__d x4 (n1,n2);
    inv__d x5 (n3,n4);
endmodule // top

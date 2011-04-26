`timescale 10ps/1ps

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
   parameter init_v      = 1;
   parameter fall        = 1.1111e-9;
   parameter rise        = 1e-9;
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
      out <= #30 !in;
   end
   
endmodule

module _;

	inv__a x1 (n2,n1);
	inv__d x5 (n1,n5);


endmodule // top

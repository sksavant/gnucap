`timescale 10ps/1ps 

module invert (out,in);
   input in;
   output reg out;

 	parameter inv_delay = 260; // inverter delay in ps
	// parameters for AD and DA conversion come from LOGIC_MODEL

   always @(in) begin
      out <= #inv_delay !in;
		// not implemented.
		//$display($realtime, " inverter sees ", in);
   end
endmodule

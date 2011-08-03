`timescale 10ps/1ps 

module invert (out,in);
   input in;
   output reg out;

 	parameter inv_delay = 260; // inverter delay in ps
	// parameters for AD and DA conversion. 
	// parameter vhigh=3;   
	// parameter vlow=0;   
	// parameter risetime=13;
	// parameter output_high=13;
	// parameter output_low=13;

   always @(in) begin
      //out <= #inv_delay !in;
      out <= #inv_delay !in;
		// not implemented.
		//$display($realtime, " inverter sees ", in);
   end
endmodule

`timescale 10ps/1ps 

module latch (out,in);
   input in;
   output reg out;

 	parameter inv_delay = 260; // inverter delay in ps
	parameter bar =1;
	parameter baz =3;
	// parameters for AD and DA conversion. 
	// parameter vhigh=3;   
	// parameter vlow=0;   
	// parameter risetime=13;
	// parameter output_high=13;
	// parameter output_low=13;

   always @(in) begin
      out <= #inv_delay !in;
		// not implemented.
		//$display($realtime, " inverter sees ", in);
   end
endmodule

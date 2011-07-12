module invert (out,in);
   input in;
   output reg out;

 	parameter delay = 1900; // inverter delay.
	// parameters for AD and DA conversion. 
	// parameter vhigh=3;   
	// parameter vlow=0;   
	// parameter risetime=13;
	// parameter output_high=13;
	// parameter output_low=13;

   always @(in) begin
      out <= #delay !in;
		// not implemented.
		//$display($realtime, " inverter sees ", in);
   end
endmodule

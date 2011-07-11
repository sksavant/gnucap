

/*
module sub (subout,subin);
   // not allowed here (yet?)
   input subin;
   output reg subout;
   always @(subin) begin
      subout <= #1 subin;
   end
endmodule
*/

//paramset othersub sub;
//.testdelay=5;
//end

module invert (out,in);
   output reg out;
   input in;
//	reg intern;

 	parameter delay = 1900; // inverter delay.
 	parameter pdelay = 1900; // inverter delay.
	// parameters involving AD and DA conversion. are there standard names?
	// parameter vhigh=3;   
	// parameter vlow=0;   
	// parameter risetime=13;
	// parameter output_high=13;
	// parameter output_low=13;

   always @(in) begin
      out <= !in;
		$display($realtime, " inverter sees ", in);
   end

	
//	sub s1 (out,i0);
endmodule


//module main ();
//invert i1 (a,b);
//invert i2 (b,a);
//endmodule

`timescale 10ps/1ps  // <- not implemented yet.

/*
nature Voltage;
units      = "V";
access     = V;
idt_nature = Flux;
`ifdef VOLTAGE_ABSTOL                                                                                            
	abstol     = `VOLTAGE_ABSTOL;                                                                                  
`else                                                                                                            
	abstol     = 1e-6;                                                                                             
`endif                                                                                                           
endnature    

nature Current;
units      = "A";
access     = I;
idt_nature = Charge;
`ifdef CURRENT_ABSTOL
	abstol     = `CURRENT_ABSTOL
`else
	abstol     = 1e-12;
`endif
endnature


discipline electrical;
	potential Voltage;
	flow Current;
enddiscipline
*/

module clock (out);
	output out;
	reg out;
   // wire out; ?
	//	electrical out;

 	parameter width = 500; 
	parameter period = 1000;
	parameter iv = 0;
	parameter delay = 0;


	initial begin
		out = iv;
		out <= #(delay+width) !iv;
		#(delay+width);
		forever begin
			out <= #width	iv;
			#width;
			out <= #(period-width)  !iv;
			#(period-width);
		end
	end
	/*
	always @( posedge out ) begin
		#(width-notify);
	  	out <= #notify	!out;
   end
	always @( negedge out ) begin
	end
	*/

//	analog begin
//		V(out) <+ transition( vout_val, tdelay, trise, tfall);
//	end

endmodule

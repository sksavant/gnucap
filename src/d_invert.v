

module sub (subout,subin);
   input subin;
   output reg subout;
   always @(subin) begin
      subout <= #4 !subin;
   end
endmodule

module invert (out,in);
   input in;
   output reg out;

   always @(in) begin
      out <= #4 !in;
		$display($realtime, " inverter sees ", in);
   end
	sub s1 (a,in);
   always @(a) begin

		$display($realtime, " suba ", a);
   end
endmodule


//module main ();
//invert i1 (a,b);
//invert i2 (b,a);
//endmodule

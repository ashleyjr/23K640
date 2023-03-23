module x_23K640_sck(
   input    logic          i_clk,
   input    logic          i_rst, 
   output   logic          o_advance,
   output   logic          o_sck
);

   assign o_advance = 1'b1;
    
   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)   o_sck <= 'd0;
      else        o_sck <= ~o_sck;
   end   

endmodule

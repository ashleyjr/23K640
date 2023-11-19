module x_23K640_sck(
   input    logic          i_clk,
   input    logic          i_rst, 
   input    logic [7:0]    i_period,
   output   logic          o_advance,
   output   logic          o_sck
);
   logic       top;
   logic [7:0] cnt_d;
   logic [7:0] cnt_q;
   logic       sck_n;

   // Count the period
   assign top = (cnt_q == i_period);

   assign cnt_d = (top) ? 'd0 : (cnt_q + 'd1);

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)   cnt_q <= 'd0;
      else        cnt_q <= cnt_d;
   end      
  
   // Drive advance pulse
   // - Advance non clock signals on the falling edge
   assign o_advance = top & o_sck;

   // Drive clock output
   assign sck_n = ~o_sck;

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)      o_sck <= 'd0;
      else if(top)   o_sck <= sck_n;
   end   

endmodule

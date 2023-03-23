module x_23K640(
   input    logic          i_clk,
   input    logic          i_rst,
   // Application side - Requests 
   input    logic          i_valid,
   output   logic          o_accept,
   input    logic          i_rd_n_wr,
   input    logic [15:0]   i_addr,
   input    logic [7:0]    i_wdata,
   // Application side - Completions
   output   logic          o_ready,
   output   logic [7:0]    o_rdata,
   // SPI SRAM side
   output   logic          o_sck,
   output   logic          o_cs,
   output   logic          o_so,
   input    logic          i_si
);

   logic advance;

   x_23K640_sck u_s(
      .i_clk      (i_clk      ),
      .i_rst      (i_rst      ),     
      .o_advance  (advance    ),
      .o_sck      (o_sck      )
   );
   x_23K640_data u_d(
      .i_clk      (i_clk      ),
      .i_rst      (i_rst      ),     
      .i_advance  (advance    ),
      .i_sck      (o_sck      ),
      .i_valid    (i_valid    ),
      .o_accept   (o_accept   ),
      .i_rd_n_wr  (i_rd_n_wr  ),
      .i_addr     (i_addr     ),
      .i_wdata    (i_wdata    ),
      .o_ready    (o_ready    ),
      .o_rdata    (o_rdata    ),   
      .o_cs       (o_cs       ),
      .o_so       (o_so       ),
      .i_si       (i_si       )
   );

endmodule

module x_23K640(
   input    logic          i_clk,
   input    logic          i_rst,
   // Application side - Configuration 
   input    logic [7:0]    i_div,
   // Application side - Mission Mode
   input    logic          i_en,
   input    logic          i_rd_n_wr,
   input    logic [15:0]   i_addr,
   input    logic [7:0]    i_data,
   output   logic [7:0]    o_data,
   output   logic          o_ready,
   // SPI SRAM side
   output   logic          o_sck,
   output   logic          o_cs,
   output   logic          o_so,
   input    logic          i_si
);
   localparam IDLE_COLD    = 0; 
   localparam CONFIG_0     = 1;
   localparam CONFIG_1     = 2;
   localparam CONFIG_2     = 3;
   localparam CONFIG_3     = 4;
   localparam CONFIG_4     = 5;
   localparam CONFIG_5     = 6;
   localparam CONFIG_6     = 7;
   localparam CONFIG_7     = 8;
   localparam CONFIG_8     = 9;
   localparam CONFIG_9     = 10;
   localparam CONFIG_10    = 11;
   localparam CONFIG_11    = 12;
   localparam CONFIG_12    = 13;
   localparam CONFIG_13    = 14;
   localparam CONFIG_14    = 15;
   localparam CONFIG_15    = 16;
   localparam IDLE_WARM    = 17;
   localparam READ_WARM_0  = 18;
   localparam READ_WARM_1  = 19;
   localparam READ_WARM_2  = 20;
   localparam READ_WARM_3  = 21;
   localparam READ_WARM_4  = 22;
   localparam READ_WARM_5  = 23;
   localparam READ_WARM_6  = 24;
   localparam READ_WARM_7  = 25;
   localparam READ_WARM_8  = 26;
   localparam READ_WARM_9  = 27;
   localparam READ_WARM_10 = 28; 
   localparam READ_WARM_11 = 29;
   localparam READ_WARM_12 = 30;
   localparam READ_WARM_13 = 31;
   localparam READ_WARM_14 = 32;
   localparam READ_WARM_15 = 33;
   localparam READ_WARM_16 = 34;
   localparam READ_WARM_17 = 35;
   localparam READ_WARM_18 = 36;
   localparam READ_WARM_19 = 37;
   localparam READ_WARM_20 = 38;
   localparam READ_WARM_21 = 39;
   localparam READ_WARM_22 = 40;
   localparam WRITE_WARM_0  = 18;
   localparam WRITE_WARM_1  = 19;
   localparam WRITE_WARM_2  = 20;
   localparam WRITE_WARM_3  = 21;
   localparam WRITE_WARM_4  = 22;
   localparam WRITE_WARM_5  = 23;
   localparam WRITE_WARM_6  = 24;
   localparam WRITE_WARM_7  = 25;
   localparam WRITE_WARM_8  = 26;
   localparam WRITE_WARM_9  = 27;
   localparam WRITE_WARM_10 = 28; 
   localparam WRITE_WARM_11 = 29;
   localparam WRITE_WARM_12 = 30;
   localparam WRITE_WARM_13 = 31;
   localparam WRITE_WARM_14 = 32;
   localparam WRITE_WARM_15 = 33;
   localparam WRITE_WARM_16 = 34;
   localparam WRITE_WARM_17 = 35;
   localparam WRITE_WARM_18 = 36;
   localparam WRITE_WARM_19 = 37;
   localparam WRITE_WARM_20 = 38;
   localparam WRITE_WARM_21 = 39;
   localparam WRITE_WARM_22 = 40;

   logic        sm_en;
   logic [41:0] sm_d;
   logic [41:0] sm_q;

   logic        div_en;
   logic [7:0]  div_d;
   logic [7:0]  div_q;

   // Divider 
   assign div_en = (div_q == i_div);
   
   assign div_d = (div_en) ? 'd0 : (div_q + 'd1);

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)   div_q <= 'd0;
      else        div_q <= div_d;
   end   
 
   // Advance state machine on falling edge
   assign sm_en = o_sck & div_en; 

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)      sm_q <= 'd0;
      else if(sm_en) sm_q <= sm_d;
   end

   // Ouput the clock
   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)         o_sck <= 'd0;
      else if(div_en)   o_sck <= ~o_sck;
   end   
 
   always_comb begin
      sm_d = 'd0;
      case(1'b1) 
         sm_q[IDLE_WARM]: 
            begin
               sm_d[IDLE_WARM]    = ~i_en;
               sm_d[READ_WARM_0]  =  i_en &  i_rd_n_wr;
               sm_d[WRITE_WARM_0] =  i_en & ~i_rd_n_wr;
            end
         default: 
            sm_d = sm_q << 1;
      endcase
   end

   assign o_cs = sm_q[IDLE_COLD] | sm_q[IDLE_WARM];
      


endmodule

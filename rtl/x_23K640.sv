module x_23K640(
   input    logic          i_clk,
   input    logic          i_rst,
   // Master Control 
   input    logic          i_advance,
   // Application side - Requests 
   input    logic          i_valid,
   output   logic          o_accept,
   input    logic          i_rd_n_wr,
   input    logic [15:0]   i_addr,
   input    logic [7:0]    i_data,
   // Application side - Completions
   output   logic          o_ready,
   output   logic [7:0]    o_data,
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
   localparam READ_WARM_23 = 41;
   localparam READ_WARM_24 = 42;
   localparam READ_WARM_25 = 43;
   localparam READ_WARM_26 = 43;
   localparam READ_WARM_27 = 44;
   localparam READ_WARM_28 = 45;
   localparam READ_WARM_29 = 46;
   localparam READ_WARM_30 = 47;
   localparam READ_WARM_31 = 48;
   localparam WRITE_WARM_0  = 49;
   localparam WRITE_WARM_1  = 50;
   localparam WRITE_WARM_2  = 51;
   localparam WRITE_WARM_3  = 52;
   localparam WRITE_WARM_4  = 53;
   localparam WRITE_WARM_5  = 54;
   localparam WRITE_WARM_6  = 55;
   localparam WRITE_WARM_7  = 56;
   localparam WRITE_WARM_8  = 57;
   localparam WRITE_WARM_9  = 58;
   localparam WRITE_WARM_10 = 59; 
   localparam WRITE_WARM_11 = 60;
   localparam WRITE_WARM_12 = 61;
   localparam WRITE_WARM_13 = 62;
   localparam WRITE_WARM_14 = 63;
   localparam WRITE_WARM_15 = 64;
   localparam WRITE_WARM_16 = 65;
   localparam WRITE_WARM_17 = 66;
   localparam WRITE_WARM_18 = 67;
   localparam WRITE_WARM_19 = 68;
   localparam WRITE_WARM_20 = 69;
   localparam WRITE_WARM_21 = 70;
   localparam WRITE_WARM_22 = 71;
   localparam WRITE_WARM_23 = 72;
   localparam WRITE_WARM_24 = 73;
   localparam WRITE_WARM_25 = 74;
   localparam WRITE_WARM_26 = 75;
   localparam WRITE_WARM_27 = 76;
   localparam WRITE_WARM_28 = 77;
   localparam WRITE_WARM_29 = 78;
   localparam WRITE_WARM_30 = 79;
   localparam WRITE_WARM_31 = 80;

   logic        sm_en;
   logic [81:0] sm_d;
   logic [81:0] sm_q;

   // Advance state machine on falling edge
   assign sm_en = o_sck & i_advance; 

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)      sm_q <= 'd1;
      else if(sm_en) sm_q <= sm_d;
   end

   // Ouput the clock
   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)            o_sck <= 'd0;
      else if(i_advance)   o_sck <= ~o_sck;
   end   
 
   always_comb begin
      sm_d = 'd0;
      case(1'b1) 
         sm_q[IDLE_COLD]:
            begin
               sm_d[IDLE_COLD] = ~i_valid;
               sm_d[CONFIG_0]  =  i_valid;
            end
         sm_q[IDLE_WARM]: 
            begin
               sm_d[IDLE_WARM]    = ~i_valid;
               sm_d[READ_WARM_0]  =  i_valid &  i_rd_n_wr;
               sm_d[WRITE_WARM_0] =  i_valid & ~i_rd_n_wr;
            end
         sm_q[READ_WARM_31]: 
            begin
               sm_d[IDLE_WARM] = 1'b1;
            end
         sm_q[WRITE_WARM_31]: 
            begin
               sm_d[IDLE_WARM] = 1'b1;
            end
         default: 
            sm_d = sm_q << 1;
      endcase
   end

   // App Output: Ready
   assign o_ready = sm_q[WRITE_WARM_31];

   // SPI Output: Chip Select
   assign o_cs = sm_q[IDLE_COLD]| 
                 sm_q[IDLE_WARM];
   
   // SPI Output: Serial Out
   always_comb begin
      o_so = 1'b0;
      case(1'b1)
         sm_q[CONFIG_7],
         sm_q[CONFIG_9],
         sm_q[CONFIG_15],
         sm_q[WRITE_WARM_6],
         sm_q[READ_WARM_6],
         sm_q[READ_WARM_7]:     o_so = 1'b1;
         sm_q[READ_WARM_8],
         sm_q[WRITE_WARM_8]:     o_so = i_addr[15];
         sm_q[READ_WARM_9],
         sm_q[WRITE_WARM_9]:     o_so = i_addr[14];
         sm_q[READ_WARM_10],
         sm_q[WRITE_WARM_10]:    o_so = i_addr[13];
         sm_q[READ_WARM_11],
         sm_q[WRITE_WARM_11]:    o_so = i_addr[12];
         sm_q[READ_WARM_12],
         sm_q[WRITE_WARM_12]:    o_so = i_addr[11];
         sm_q[READ_WARM_13],
         sm_q[WRITE_WARM_13]:    o_so = i_addr[10];
         sm_q[READ_WARM_14],
         sm_q[WRITE_WARM_14]:    o_so = i_addr[9];
         sm_q[READ_WARM_15],
         sm_q[WRITE_WARM_15]:    o_so = i_addr[8];
         sm_q[READ_WARM_16],
         sm_q[WRITE_WARM_16]:    o_so = i_addr[7];
         sm_q[READ_WARM_17],
         sm_q[WRITE_WARM_17]:    o_so = i_addr[6];
         sm_q[READ_WARM_18],
         sm_q[WRITE_WARM_18]:    o_so = i_addr[5];
         sm_q[READ_WARM_19],
         sm_q[WRITE_WARM_19]:    o_so = i_addr[4];
         sm_q[READ_WARM_20],
         sm_q[WRITE_WARM_20]:    o_so = i_addr[3];
         sm_q[READ_WARM_21],
         sm_q[WRITE_WARM_21]:    o_so = i_addr[2];
         sm_q[READ_WARM_22],
         sm_q[WRITE_WARM_22]:    o_so = i_addr[1];
         sm_q[READ_WARM_23],
         sm_q[WRITE_WARM_23]:    o_so = i_addr[0];
         sm_q[WRITE_WARM_24]:    o_so = i_data[7]; 
         sm_q[WRITE_WARM_25]:    o_so = i_data[6]; 
         sm_q[WRITE_WARM_26]:    o_so = i_data[5]; 
         sm_q[WRITE_WARM_27]:    o_so = i_data[4]; 
         sm_q[WRITE_WARM_28]:    o_so = i_data[3]; 
         sm_q[WRITE_WARM_29]:    o_so = i_data[2]; 
         sm_q[WRITE_WARM_30]:    o_so = i_data[1]; 
         sm_q[WRITE_WARM_31]:    o_so = i_data[0]; 
      endcase
   end

endmodule

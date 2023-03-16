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

   typedef enum logic [6:0] {
      IDLE_COLD, 
      CONFIG_0,  
      CONFIG_1,  
      CONFIG_2,  
      CONFIG_3,  
      CONFIG_4,  
      CONFIG_5,  
      CONFIG_6,  
      CONFIG_7,  
      CONFIG_8,  
      CONFIG_9,  
      CONFIG_10,  
      CONFIG_11, 
      CONFIG_12, 
      CONFIG_13, 
      CONFIG_14, 
      CONFIG_15, 
      IDLE_WARM, 
      READ_WARM_0, 
      READ_WARM_1,
      READ_WARM_2,
      READ_WARM_3,
      READ_WARM_4,
      READ_WARM_5,
      READ_WARM_6,
      READ_WARM_7,
      READ_WARM_8,
      READ_WARM_9,
      READ_WARM_10, 
      READ_WARM_11,
      READ_WARM_12,
      READ_WARM_13,
      READ_WARM_14,
      READ_WARM_15,
      READ_WARM_16,
      READ_WARM_17,
      READ_WARM_18,
      READ_WARM_19,
      READ_WARM_20,
      READ_WARM_21,
      READ_WARM_22,
      READ_WARM_23,
      READ_WARM_24,
      READ_WARM_25,
      READ_WARM_26,
      READ_WARM_27,
      READ_WARM_28,
      READ_WARM_29,
      READ_WARM_30,
      READ_WARM_31,
      WRITE_WARM_0,
      WRITE_WARM_1,
      WRITE_WARM_2,
      WRITE_WARM_3,
      WRITE_WARM_4,
      WRITE_WARM_5,
      WRITE_WARM_6,
      WRITE_WARM_7,
      WRITE_WARM_8,
      WRITE_WARM_9,
      WRITE_WARM_10, 
      WRITE_WARM_11,
      WRITE_WARM_12,
      WRITE_WARM_13,
      WRITE_WARM_14,
      WRITE_WARM_15,
      WRITE_WARM_16,
      WRITE_WARM_17,
      WRITE_WARM_18,
      WRITE_WARM_19,
      WRITE_WARM_20,
      WRITE_WARM_21,
      WRITE_WARM_22,
      WRITE_WARM_23,
      WRITE_WARM_24,
      WRITE_WARM_25,
      WRITE_WARM_26,
      WRITE_WARM_27,
      WRITE_WARM_28,
      WRITE_WARM_29,
      WRITE_WARM_30,
      WRITE_WARM_31
   } sm_t;

   logic          sm_en;
   sm_t           sm_d;
   sm_t           sm_q;

   logic          rdata_en;
   logic [7:0]    rdata_d;
   logic [7:0]    rdata_q;
   
   logic          wdata_en;
   logic [7:0]    wdata_d;
   logic [7:0]    wdata_q;

   logic          addr_en;
   logic [15:0]   addr_d;
   logic [15:0]   addr_q;

   logic          rd_n_wr_q;
   logic          seq;
   logic          hop;
   logic          same;

   // Advance state machine on falling edge
   assign sm_en = o_sck & i_advance; 

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)      sm_q <= IDLE_COLD;
      else if(sm_en) sm_q <= sm_d;
   end

   // Output the clock
   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)            o_sck <= 'd0;
      else if(i_advance)   o_sck <= ~o_sck;
   end   

   // Back-to-back ops to sequential addresses
   assign seq  = (i_addr == (addr_q + 'd1));
   assign same = (rd_n_wr_q == i_rd_n_wr);
   assign hop  = i_valid & same & seq;


   // State transistors
   always_comb begin
      sm_d = sm_q + 'd1;
      case(sm_q) 
         IDLE_COLD:
            begin
               if(i_valid) sm_d = CONFIG_0;
               else        sm_d = IDLE_COLD;
            end
         IDLE_WARM: 
            begin
               if(i_valid)
                  if(i_rd_n_wr)  sm_d = READ_WARM_0;
                  else           sm_d = WRITE_WARM_0;
               else              sm_d = IDLE_WARM;
            end
         READ_WARM_31: 
            begin
               if(hop)  sm_d = READ_WARM_24;
               else     sm_d = IDLE_WARM;
            end
         WRITE_WARM_31: 
            begin
               if(hop)  sm_d = WRITE_WARM_24;
               else     sm_d = IDLE_WARM;
            end   
         default:;
      endcase
   end
   
   // Latch the write data
   assign wdata_en = i_valid & ~i_rd_n_wr & o_accept;

   assign wdata_d = i_wdata;

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)         wdata_q <= 'd0;
      else if(wdata_en) wdata_q <= wdata_d;
   end   

   // Latch the addr
   assign addr_en = (sm_q == READ_WARM_24) | 
                    (sm_q == WRITE_WARM_24);

   assign addr_d = i_addr;

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)         addr_q <= 'd0;
      else if(addr_en)  addr_q <= addr_d;
   end 
   
   // App Output: Read data
   assign o_rdata = rdata_q;

   assign rdata_d = {rdata_q[6:0],i_si};

   assign rdata_en = i_advance & ~o_sck &
                     ((sm_q >= READ_WARM_24) & 
                      (sm_q <= READ_WARM_31));

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)         rdata_q <= 'd0;
      else if(rdata_en) rdata_q <= rdata_d;
   end

   // App Output: Ready
   assign o_accept = sm_en & ((sm_q == READ_WARM_24)|
                              (sm_q == WRITE_WARM_23));

   always_ff@(posedge i_clk or posedge i_rst) begin
      if(i_rst)         rd_n_wr_q <= 'd0;
      else if(o_accept) rd_n_wr_q <= (sm_q == READ_WARM_24);
   end   

   // App Output: Ready
   assign o_ready = sm_en & (sm_q == READ_WARM_31);

   // SPI Output: Chip Select
   assign o_cs = (sm_q == IDLE_COLD) | (sm_q == IDLE_WARM);
   
   // SPI Output: Serial Out
   always_comb begin
      o_so = 1'b0;
      case(sm_q)
         CONFIG_7,
         CONFIG_9,
         CONFIG_15,
         WRITE_WARM_6,
         READ_WARM_6,
         READ_WARM_7:     o_so = 1'b1;
         READ_WARM_8,
         WRITE_WARM_8:     o_so = i_addr[15];
         READ_WARM_9,
         WRITE_WARM_9:     o_so = i_addr[14];
         READ_WARM_10,
         WRITE_WARM_10:    o_so = i_addr[13];
         READ_WARM_11,
         WRITE_WARM_11:    o_so = i_addr[12];
         READ_WARM_12,
         WRITE_WARM_12:    o_so = i_addr[11];
         READ_WARM_13,
         WRITE_WARM_13:    o_so = i_addr[10];
         READ_WARM_14,
         WRITE_WARM_14:    o_so = i_addr[9];
         READ_WARM_15,
         WRITE_WARM_15:    o_so = i_addr[8];
         READ_WARM_16,
         WRITE_WARM_16:    o_so = i_addr[7];
         READ_WARM_17,
         WRITE_WARM_17:    o_so = i_addr[6];
         READ_WARM_18,
         WRITE_WARM_18:    o_so = i_addr[5];
         READ_WARM_19,
         WRITE_WARM_19:    o_so = i_addr[4];
         READ_WARM_20,
         WRITE_WARM_20:    o_so = i_addr[3];
         READ_WARM_21,
         WRITE_WARM_21:    o_so = i_addr[2];
         READ_WARM_22,
         WRITE_WARM_22:    o_so = i_addr[1];
         READ_WARM_23,
         WRITE_WARM_23:    o_so = i_addr[0];
         WRITE_WARM_24:    o_so = wdata_q[7]; 
         WRITE_WARM_25:    o_so = wdata_q[6]; 
         WRITE_WARM_26:    o_so = wdata_q[5]; 
         WRITE_WARM_27:    o_so = wdata_q[4]; 
         WRITE_WARM_28:    o_so = wdata_q[3]; 
         WRITE_WARM_29:    o_so = wdata_q[2]; 
         WRITE_WARM_30:    o_so = wdata_q[1]; 
         WRITE_WARM_31:    o_so = wdata_q[0];
         default:;
      endcase
   end

endmodule

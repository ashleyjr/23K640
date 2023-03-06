#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vx_23K640.h"
#include <stdio.h>

#define CYCLES 200

vluint64_t sim_time = 0;
uint64_t cycle;

enum class MemState { 
   IDLE,
   CMD_0, 
   CMD_1, 
   CMD_2, 
   CMD_3, 
   CMD_4, 
   CMD_5, 
   CMD_6, 
   CMD_7, 
   WRSR_0, 
   WRSR_1, 
   WRSR_2, 
   WRSR_3, 
   WRSR_4, 
   WRSR_5, 
   WRSR_6, 
   WRSR_7, 
   READ_0,
   READ_1,
   READ_2,
   READ_3,
   READ_4,
   READ_5,
   READ_6,
   READ_7,
   READ_8,
   READ_9, 
   READ_10,
   READ_11,
   READ_12,
   READ_13,
   READ_14,
   READ_15,
   READ_16,
   READ_17,
   READ_18,
   READ_19,
   READ_20,
   READ_21,
   READ_22,
   READ_23
};

class Mem {
   public:
      
      Mem(bool d){
         debug = d;
         state = MemState::IDLE;
         cmd_data = 0;
      }
     
      void set_sck(int v){
         if((v == 1) && (sck_last == 0)){
            advance();
         }
         sck_last = v;
      }

      void set_cs(int v) {  
         cs = v;  
      }

      void set_si(int v) {  
         si = v;  
      }

      int get_so() {  
         return so; 
      }

   private:
      bool debug;
      MemState state;
      int cs;
      int si;
      int so;
      int addr;
      int cmd_data;
      int sck_last;

      void advance() {  
        if(cs == 0){
            switch(state){
               case MemState::IDLE:       cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::CMD_0;
                                          break;
               case MemState::CMD_0:      cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::CMD_1;
                                          break;
               case MemState::CMD_1:      cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::CMD_2;
                                          break;
               case MemState::CMD_2:      cmd_data <<= 1;
                                          cmd_data |= si;
                                          state =  MemState::CMD_3;
                                          break;
               case MemState::CMD_3:      cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::CMD_4;
                                          break;
               case MemState::CMD_4:      cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::CMD_5;
                                          break;
               case MemState::CMD_5:      cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::CMD_6;
                                          break;
               case MemState::CMD_6:      cmd_data <<= 1;
                                          cmd_data |= si; 
                                          printf("CMD:\t0x%x\n", cmd_data);
                                          switch(cmd_data){
                                             case 3:  state = MemState::READ_0;
                                                      break;
                                             case 2:  state = MemState::IDLE;
                                                      break;
                                             case 5:  state = MemState::IDLE;
                                                      break;
                                             case 1:  state = MemState::WRSR_0;
                                                      break;
                                             default: printf("INVALID CMD\n");
                                                      exit(0);
                                                      break; 
                                          }
                                          cmd_data = 0;
                                          break;            
               case MemState::WRSR_0:     cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::WRSR_1;
                                          break;
               case MemState::WRSR_1:     cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::WRSR_2;
                                          break;
               case MemState::WRSR_2:     cmd_data <<= 1;
                                          cmd_data |= si;
                                          state =  MemState::WRSR_3;
                                          break;
               case MemState::WRSR_3:     cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::WRSR_4;
                                          break;
               case MemState::WRSR_4:     cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::WRSR_5;
                                          break;
               case MemState::WRSR_5:     cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::WRSR_6;
                                          break;
               case MemState::WRSR_6:     cmd_data <<= 1;
                                          cmd_data |= si;
                                          state = MemState::WRSR_7;
                                          break;
               case MemState::WRSR_7:     cmd_data <<= 1;
                                          cmd_data |= si; 
                                          printf("WRSR:\t0x%x\n", cmd_data);
                                          if(cmd_data != 0x41){
                                             printf("INVALID WRSR\n");
                                             exit(0);
                                          }
                                          cmd_data = 0;
                                          break;          
               case MemState::READ_0:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_1;
                                          break;
               case MemState::READ_1:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_2;
                                          break;
               case MemState::READ_2:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_3;
                                          break;
               case MemState::READ_3:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_4;
                                          break;
               case MemState::READ_4:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_5;
                                          break;
               case MemState::READ_5:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_6;
                                          break;
               case MemState::READ_6:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_7;
                                          break;
               case MemState::READ_7:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_8;
                                          break;
               case MemState::READ_8:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_9;
                                          break;
               case MemState::READ_9:     addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_10;
                                          break;
               case MemState::READ_10:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_11;
                                          break;
               case MemState::READ_11:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_12;
                                          break;
               case MemState::READ_12:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_13;
                                          break;
               case MemState::READ_13:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_14;
                                          break;
               case MemState::READ_14:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_15;
                                          break;
               case MemState::READ_15:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_16;
                                          printf("READ:\t0x%x\n", addr);
                                          break;
               case MemState::READ_16:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_17;
                                          break;
               case MemState::READ_17:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_18;
                                          break;
               case MemState::READ_18:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_19;
                                          break;
               case MemState::READ_19:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_20;
                                          break;
               case MemState::READ_20:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_21;
                                          break;
               case MemState::READ_21:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_22;
                                          break;
               case MemState::READ_22:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::READ_23;
                                          break;
               case MemState::READ_23:    addr <<= 1;
                                          addr |= si;
                                          state = MemState::IDLE;
                                          break;
            }
         } else {
            state =  MemState::IDLE;
         }
      
      }

         
};

int main(int argc, char** argv, char** env) {
   Vx_23K640 *dut = new Vx_23K640; 
   Verilated::traceEverOn(true);
   VerilatedVcdC *m_trace = new VerilatedVcdC;
  
   Mem m(true);

   dut->trace(m_trace, 5);
   m_trace->open("waveform.vcd");

   dut->i_rst     = 1;
   dut->i_advance = 1;
   dut->i_en      = 0;
   dut->i_rd_n_wr = 0;
   dut->i_addr    = 0;
   dut->i_data    = 0;
   dut->i_si      = 0;

   while (cycle < CYCLES) {
      
      // Falling Edge
      dut->i_clk = 0;
    
      // Test Vectors
      switch(cycle){
         
         // Reset
         case 1:     dut->i_rst = 0;
                     break;
         
         // Bring Up
         case 10:    dut->i_en = 1;
      }

      
      m.set_sck(dut->o_sck);
      m.set_cs(dut->o_cs);
      m.set_si(dut->o_so);
      dut->i_si=m.get_so();

      
      //printf ("cycle=%8ld\ti_rst=%d\to_cs=%d\to_so=%d\n",
      //   cycle,
      //   dut->i_rst,
      //   dut->o_cs, 
      //   dut->o_so);
          
      // Tick
      dut->eval();
      m_trace->dump(sim_time);
      sim_time++;
 
      // Rising Edge
      dut->i_clk = 1;
      
      // Tick
      dut->eval();
      m_trace->dump(sim_time); 
      sim_time++;

      // Event
      cycle = sim_time >> 1;
   }
   
   m_trace->close();
   delete dut;
   exit(EXIT_SUCCESS);
}


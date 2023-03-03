#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vx_23K640.h"

#define CYCLES 10000

vluint64_t sim_time = 0;
uint64_t cycle;

int main(int argc, char** argv, char** env) {
   Vx_23K640 *dut = new Vx_23K640; 
   Verilated::traceEverOn(true);
   VerilatedVcdC *m_trace = new VerilatedVcdC;
   
   dut->trace(m_trace, 5);
   m_trace->open("waveform.vcd");

   dut->i_rst     = 0;
   dut->i_div     = 0;
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
         case 13:    dut->i_rst = 1;
                     break;
         case 21:    dut->i_rst = 0;
                     break;
         
         // Bring Up
         case 100:    dut->i_en = 1;
      }

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


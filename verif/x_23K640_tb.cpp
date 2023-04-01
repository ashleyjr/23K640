#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vx_23K640.h"
#include "include/logger.h"
#include "include/app_driver.h"
#include "include/sram_model.h"
#include <stdio.h>

#define CYCLES 100000

int main(int argc, char** argv, char** env) {
   
   vluint64_t sim_time = 0;
   uint64_t cycle;
   Vx_23K640 *dut = new Vx_23K640; 
   Verilated::traceEverOn(true);
   VerilatedVcdC *m_trace = new VerilatedVcdC; 
   uint32_t stop = std::stoi(argv[1]);
   uint16_t addrs = std::stoi(argv[2]);
   uint16_t backpressure = std::stoi(argv[3]);
   uint16_t all_reads = std::stoi(argv[4]);
   uint16_t all_writes = std::stoi(argv[5]);
   
   Logger logger("logfile.txt", &sim_time);
   AppDriver d(logger, addrs, backpressure, all_reads, all_writes);
   SramModel m(logger); 

   dut->trace(m_trace, 5);
   m_trace->open("waveform.vcd");

   dut->i_rst     = 1;
   dut->i_valid   = 0;
   dut->i_rd_n_wr = 0;
   dut->i_addr    = 0;
   dut->i_wdata   = 0;
   dut->i_si      = 0;

   while (cycle < stop) {
      
      // Falling Edge
      dut->i_clk = 0;
    
      // Test Vectors
      switch(cycle){   
         // Reset
         case 10:    dut->i_rst = 0;
                     break;
      }


      // AppDriver
      d.set_inputs(
         dut->o_accept,
         dut->o_rdata,
         dut->o_ready
      );
      d.advance();
      dut->i_valid = d.get_valid();
      dut->i_rd_n_wr = d.get_rd_n_wr();
      dut->i_addr = d.get_addr(); 
      dut->i_wdata = d.get_wdata();
      
      // SRAM Model
      m.set_inputs(
         dut->o_cs,
         dut->o_so,      
         dut->o_sck
      );
      dut->i_si = m.get_so(); 
          
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
   
   d.report_bw();

   m_trace->close();
   delete dut;
   exit(EXIT_SUCCESS);
}


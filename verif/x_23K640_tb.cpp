#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vx_23K640.h"
#include <stdio.h>

#define CYCLES 100000

vluint64_t sim_time = 0;
uint64_t cycle;

enum class Verbosity { 
   ERROR,
   INFO,
   DEBUG
};

class Logger {
   public: 
      Logger(char const * filename){  
         fail = false;
         f = fopen(filename, "w+");
      } 
      ~Logger(){
         printf("====\n");
         if(fail){
            printf("FAIL\n");
         }else{
            printf("PASS\n");
         }
         printf("====\n");
         fclose(f);
      }

      void log(Verbosity v, const char *format, ...){
         char buffer0[256];
         char buffer1[512];
         char const * str;
         va_list args;
         va_start(args, format); 
         vsprintf(buffer0,format,args);
         switch(v){
            case Verbosity::ERROR:  str = "ERROR";  break;
            case Verbosity::INFO:   str = "INFO ";  break;
            case Verbosity::DEBUG:  str = "DEBUG";  break;
         }
         sprintf(buffer1,"%10ld [%s]: %s\n",sim_time,str,buffer0);
         switch(v){
            case Verbosity::ERROR:  fail = true;
            case Verbosity::INFO:   printf("%s",buffer1);
            case Verbosity::DEBUG:  fprintf(f,"%s",buffer1);
         }
         va_end(args);
      } 
   
   private:
      FILE *f; 
      bool fail;
};

Logger l("logfile.txt");

enum class MemState { 
   IDLE,
   CMD_0, 
   CMD_1, 
   CMD_2, 
   CMD_3, 
   CMD_4, 
   CMD_5, 
   CMD_6, 
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
   READ_23,
   WRITE_0,
   WRITE_1,
   WRITE_2,
   WRITE_3,
   WRITE_4,
   WRITE_5,
   WRITE_6,
   WRITE_7,
   WRITE_8,
   WRITE_9, 
   WRITE_10,
   WRITE_11,
   WRITE_12,
   WRITE_13,
   WRITE_14,
   WRITE_15,
   WRITE_16,
   WRITE_17,
   WRITE_18,
   WRITE_19,
   WRITE_20,
   WRITE_21,
   WRITE_22,
   WRITE_23

};

class SramModel {
   public:
      
      SramModel(){  
         state = MemState::IDLE;
         cmd_data = 0;
         so = 0;
         for(int i=0;i<65536;i++){
            mem[i] = 0xAA;   
         }
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

      uint8_t get_so() {  
         return so; 
      }

   private:
      bool     debug;
      MemState state;
      uint8_t  cs;
      uint8_t  si;
      uint8_t  so;
      uint16_t addr;
      uint8_t  cmd_data;
      uint8_t  sck_last;
      uint8_t  mem[65536]; 

      void advance() {   
         // CMD Data Shift In 
         switch(state){
            case MemState::IDLE:    
            case MemState::CMD_0:    
            case MemState::CMD_1:    
            case MemState::CMD_2:   
            case MemState::CMD_3:  
            case MemState::CMD_4:   
            case MemState::CMD_5:   
            case MemState::CMD_6:   
            case MemState::WRSR_0:  
            case MemState::WRSR_1:  
            case MemState::WRSR_2:  
            case MemState::WRSR_3:  
            case MemState::WRSR_4:  
            case MemState::WRSR_5:  
            case MemState::WRSR_6:  
            case MemState::WRSR_7:   
            case MemState::WRITE_16:  
            case MemState::WRITE_17: 
            case MemState::WRITE_18:  
            case MemState::WRITE_19: 
            case MemState::WRITE_20:  
            case MemState::WRITE_21: 
            case MemState::WRITE_22:  
            case MemState::WRITE_23:   if(cs == 0){
                                          cmd_data <<= 1;
                                          cmd_data |= si;
                                          l.log(Verbosity::DEBUG,"[SRAM] CMD/DATA:\t0x%02X", cmd_data);
                                       }
                                       break;
         }
         // ADDR Shift In 
         switch(state){
            case MemState::READ_0:     
            case MemState::READ_1:   
            case MemState::READ_2:   
            case MemState::READ_3:   
            case MemState::READ_4:   
            case MemState::READ_5:   
            case MemState::READ_6:   
            case MemState::READ_7:   
            case MemState::READ_8:   
            case MemState::READ_9:   
            case MemState::READ_10:  
            case MemState::READ_11:  
            case MemState::READ_12:  
            case MemState::READ_13:  
            case MemState::READ_14:  
            case MemState::READ_15:  
            case MemState::WRITE_0: 
            case MemState::WRITE_1: 
            case MemState::WRITE_2: 
            case MemState::WRITE_3:
            case MemState::WRITE_4: 
            case MemState::WRITE_5:
            case MemState::WRITE_6: 
            case MemState::WRITE_7: 
            case MemState::WRITE_8: 
            case MemState::WRITE_9:
            case MemState::WRITE_10:
            case MemState::WRITE_11:
            case MemState::WRITE_12:
            case MemState::WRITE_13:
            case MemState::WRITE_14:
            case MemState::WRITE_15:   addr <<= 1;
                                       addr |= si;
                                       l.log(Verbosity::DEBUG,"[SRAM] ADDR:\t\t0x%04X", addr);
                                       break;
         } 
         // State Transistion Only  
         switch(state){
            case MemState::IDLE:       if(cs == 0){
                                          state = MemState::CMD_0;
                                       }
                                       break;
            case MemState::CMD_0:      state = MemState::CMD_1;   break;
            case MemState::CMD_1:      state = MemState::CMD_2;   break;
            case MemState::CMD_2:      state = MemState::CMD_3;   break;
            case MemState::CMD_3:      state = MemState::CMD_4;   break;
            case MemState::CMD_4:      state = MemState::CMD_5;   break;
            case MemState::CMD_5:      state = MemState::CMD_6;   break;
            case MemState::CMD_6:      switch(cmd_data){
                                          case 3:  state = MemState::READ_0;  break;
                                          case 2:  state = MemState::WRITE_0; break;
                                          case 5:  state = MemState::IDLE;    break;
                                          case 1:  state = MemState::WRSR_0;  break;
                                          default: l.log(Verbosity::ERROR,"[SRAM] INVALID CMD");
                                                   break; 
                                       }
                                       break;            
            case MemState::WRSR_0:     state = MemState::WRSR_1;  break;
            case MemState::WRSR_1:     state = MemState::WRSR_2;  break;
            case MemState::WRSR_2:     state = MemState::WRSR_3;  break;
            case MemState::WRSR_3:     state = MemState::WRSR_4;  break;
            case MemState::WRSR_4:     state = MemState::WRSR_5;  break;
            case MemState::WRSR_5:     state = MemState::WRSR_6;  break;
            case MemState::WRSR_6:     state = MemState::WRSR_7;  break;
            case MemState::WRSR_7:     l.log(Verbosity::DEBUG,"[SRAM] WRSR:\t\t0x%02X", cmd_data);
                                       if(cmd_data != 0x41){
                                          l.log(Verbosity::ERROR,"[SRAM] INVALID WRSR");
                                       }
                                       cmd_data = 0; 
                                       state = MemState::IDLE;
                                       break;          
            case MemState::READ_0:     state = MemState::READ_1;  break;
            case MemState::READ_1:     state = MemState::READ_2;  break;
            case MemState::READ_2:     state = MemState::READ_3;  break;
            case MemState::READ_3:     state = MemState::READ_4;  break;
            case MemState::READ_4:     state = MemState::READ_5;  break;
            case MemState::READ_5:     state = MemState::READ_6;  break;
            case MemState::READ_6:     state = MemState::READ_7;  break;
            case MemState::READ_7:     state = MemState::READ_8;  break;
            case MemState::READ_8:     state = MemState::READ_9;  break;
            case MemState::READ_9:     state = MemState::READ_10; break;
            case MemState::READ_10:    state = MemState::READ_11; break;
            case MemState::READ_11:    state = MemState::READ_12; break;
            case MemState::READ_12:    state = MemState::READ_13; break;
            case MemState::READ_13:    state = MemState::READ_14; break; 
            case MemState::READ_14:    state = MemState::READ_15; break; 
            case MemState::READ_15:    cmd_data = mem[addr];
                                       state = MemState::READ_16;
                                       l.log(Verbosity::DEBUG,"[SRAM] READ:\t\tmem[0x%04X] -> 0x%02X", addr, cmd_data);
                                       break; 
            case MemState::READ_16:    state = MemState::READ_17;    break;
            case MemState::READ_17:    state = MemState::READ_18;    break;
            case MemState::READ_18:    state = MemState::READ_19;    break;
            case MemState::READ_19:    state = MemState::READ_20;    break;
            case MemState::READ_20:    state = MemState::READ_21;    break;
            case MemState::READ_21:    state = MemState::READ_22;    break;
            case MemState::READ_22:    state = MemState::READ_23;    break;
            case MemState::READ_23:    state = MemState::IDLE;       break;
            case MemState::WRITE_0:    state = MemState::WRITE_1;    break;
            case MemState::WRITE_1:    state = MemState::WRITE_2;    break;
            case MemState::WRITE_2:    state = MemState::WRITE_3;    break;
            case MemState::WRITE_3:    state = MemState::WRITE_4;    break;
            case MemState::WRITE_4:    state = MemState::WRITE_5;    break;
            case MemState::WRITE_5:    state = MemState::WRITE_6;    break;
            case MemState::WRITE_6:    state = MemState::WRITE_7;    break;
            case MemState::WRITE_7:    state = MemState::WRITE_8;    break;
            case MemState::WRITE_8:    state = MemState::WRITE_9;    break;
            case MemState::WRITE_9:    state = MemState::WRITE_10;   break;
            case MemState::WRITE_10:   state = MemState::WRITE_11;   break;
            case MemState::WRITE_11:   state = MemState::WRITE_12;   break;
            case MemState::WRITE_12:   state = MemState::WRITE_13;   break;
            case MemState::WRITE_13:   state = MemState::WRITE_14;   break;
            case MemState::WRITE_14:   state = MemState::WRITE_15;   break;
            case MemState::WRITE_15:   state = MemState::WRITE_16;   break;
            case MemState::WRITE_16:   state = MemState::WRITE_17;   break;
            case MemState::WRITE_17:   state = MemState::WRITE_18;   break;
            case MemState::WRITE_18:   state = MemState::WRITE_19;   break;
            case MemState::WRITE_19:   state = MemState::WRITE_20;   break;
            case MemState::WRITE_20:   state = MemState::WRITE_21;   break;
            case MemState::WRITE_21:   state = MemState::WRITE_22;   break;
            case MemState::WRITE_22:   state = MemState::WRITE_23;   break; 
            case MemState::WRITE_23:   state = MemState::IDLE;
                                       mem[addr] = cmd_data;
                                       l.log(Verbosity::DEBUG,"[SRAM] WRITE:\t\tmem[0x%04X] <- 0x%02X", addr, cmd_data);
                                       cmd_data = 0;
                                       break;
         }
         // Data Shift Out 
         switch(state){
            case MemState::READ_15:     
            case MemState::READ_16:   
            case MemState::READ_17:   
            case MemState::READ_18:   
            case MemState::READ_19:   
            case MemState::READ_20:   
            case MemState::READ_21:   
            case MemState::READ_22:
            case MemState::READ_23:    so = 1 & (cmd_data >> 7);  
                                       cmd_data <<= 1;
                                       break;
         } 
      }
};

enum class DriverState { 
   IDLE,
   WR,
   RD
};
 
class AppDriver {
   public:
      
      AppDriver(uint16_t n){ 
         state = DriverState::IDLE;
         valid = 0;
         rd_n_wr = 0;
         addr = 0;
         wdata = 0;
         rdata = 0;
         ready = 0;
         accept = 0;
         coverage = 0;
         // Set number of addresses 
         if(n < 1){
            num_addrs = 1;
         }else{
            num_addrs = n;
         }
         for(int i=0;i<65536;i++){
            mem[i] = 0xAA;   
         }
         // Initialise coverage
         for(int i=0;i<65536;i++){
            cov[i] = false;;   
         }
         // Always take top and bottom
         addrs[0] = 0;
         addrs[1] = 0xFFFF;
        
         // Fill the rest with random but unique addresses
         for(uint32_t i=1;i<0xFFFF;i++){
            addrs[i+1] = i;
         }
         std::random_shuffle(&addrs[2],&addrs[0xFFFF]);
      }
     
      uint8_t get_valid(){
         return valid;
      }
   
      uint8_t get_rd_n_wr(){
         return rd_n_wr;
      }

      uint16_t get_addr(){
         return addr;
      }

      uint8_t get_wdata(){
         return wdata;
      }

      void set_accept(uint8_t v){
         accept = v;
      }

      void set_rdata(uint8_t v){
         rdata = v;
      }

      void set_ready(uint8_t v){
         ready = v;
      } 
      
      void advance() {
         check_coverage();

         // Ready could be asserted in any state
         if(ready == 1){
            if(rdata != check){
               l.log(Verbosity::ERROR,"[App ] Mismatch mem[0x%04X]", addr); 
               l.log(Verbosity::ERROR,"[App ] \tGot:      0x%02X", rdata); 
               l.log(Verbosity::ERROR,"[App ] \tExpected: 0x%02X", check); 
            }
         }
        
         // State transistons
         switch(state){        
            case DriverState::IDLE:
               // Start a transactions 
               request_profile(); 
               if(valid == 1){
                  if(rd_n_wr){
                     state = DriverState::RD;
                  } else {
                     state = DriverState::WR;
                  }
               }
               break;
            case DriverState::RD:
               if(accept == 1){
                  check = mem[addr];
                  l.log(Verbosity::DEBUG,"[App ] mem[0x%04X] -> 0x%02X", addr, check);
                  state = DriverState::IDLE; 
               }
               break;
            case DriverState::WR:
               if(accept == 1){ 
                  l.log(Verbosity::DEBUG,"[App ] mem[0x%04X] <- 0x%02X", addr, wdata);
                  state = DriverState::IDLE;
                  mem[addr] = wdata;
               }
               break;
         }
      }

   private:
      uint8_t valid;
      uint8_t rd_n_wr;
      uint16_t addr;
      uint16_t num_addrs;
      uint16_t addrs[65536];
      bool cov[65536];
      uint16_t coverage;
      uint8_t wdata;
      uint8_t rdata;
      uint8_t ready;
      uint8_t mem[65536];
      uint8_t accept;
      uint8_t check;
      DriverState state;

      void request_profile(){
         uint16_t index;
         if(0 == (std::rand() % 10)){  
            index = std::rand() % num_addrs;
            addr = addrs[index];
            cov[index] = true; 
            wdata = std::rand();
            valid = 1;
            if(0 == (std::rand() % 2)){
               rd_n_wr = 1;
            } else {
               rd_n_wr = 0;
            }
         }else{
            valid = 0;      
         }
      }
      void check_coverage(){
         uint16_t this_cov = 0; 
         for(uint16_t i=0;i<num_addrs;i++){
            if(cov[i]){
               this_cov++;
            }
         }     
         if(this_cov > coverage){
            l.log(Verbosity::DEBUG,"[App ] Coverage: %04X/%04X", this_cov, num_addrs);
            coverage = this_cov;
         }
      
      }
};

int main(int argc, char** argv, char** env) {
   Vx_23K640 *dut = new Vx_23K640; 
   Verilated::traceEverOn(true);
   VerilatedVcdC *m_trace = new VerilatedVcdC;
 
   AppDriver d(1);
   SramModel m; 

   dut->trace(m_trace, 5);
   m_trace->open("waveform.vcd");

   dut->i_rst     = 1;
   dut->i_advance = 1;
   dut->i_valid   = 0;
   dut->i_rd_n_wr = 0;
   dut->i_addr    = 0;
   dut->i_wdata   = 0;
   dut->i_si      = 0;

   while (cycle < CYCLES) {
      
      // Falling Edge
      dut->i_clk = 0;
    
      // Test Vectors
      switch(cycle){
         
         // Reset
         case 10:    dut->i_rst = 0;
                     break;
         
      }


      d.set_accept(dut->o_accept);
      d.set_rdata(dut->o_rdata);
      d.set_ready(dut->o_ready);
      d.advance();
      dut->i_valid = d.get_valid();
      dut->i_rd_n_wr = d.get_rd_n_wr();
      dut->i_addr = d.get_addr(); 
      dut->i_wdata = d.get_wdata();
      
      m.set_cs(dut->o_cs);
      m.set_si(dut->o_so);      
      m.set_sck(dut->o_sck);
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
   
   m_trace->close();
   delete dut;
   exit(EXIT_SUCCESS);
}


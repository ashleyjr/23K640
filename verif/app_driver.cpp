 #include <verilated.h>

#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>

#include "include/app_driver.h"

AppDriver::AppDriver(Logger & logger, uint16_t n, uint16_t b, uint16_t r, uint16_t w):
   l(logger)
{ 
   state = DriverState::IDLE;
   valid = 0;
   rd_n_wr = 0;
   addr = 0;
   wdata = 0;
   rdata = 0;
   ready = 0;
   accept = 0;
   coverage = 0;
   cycles = 0;
   wbw = 0;
   rbw = 0;
   backpressure = b;
   all_reads = r;
   all_writes = w;
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
   //std::random_shuffle(&addrs[2],&addrs[0xFFFF]);
}
      
void AppDriver::report_bw(){ 
   float wbits_per_cycle = (float)wbw / (float)cycles;
   float rbits_per_cycle = (float)rbw / (float)cycles;

   l.log(Verbosity::INFO,"[App ] Cycles:       %08d", cycles);
   l.log(Verbosity::INFO,"[App ] Bits written: %08d", wbw);
   l.log(Verbosity::INFO,"[App ] Bits read:    %08d", rbw);
   l.log(Verbosity::INFO,"[App ] Write:        %08f bits/cycle", wbits_per_cycle);
   l.log(Verbosity::INFO,"[App ] Read:         %08f bits/cycle", rbits_per_cycle);

}

uint8_t AppDriver::get_valid(){
   return valid;
}

uint8_t AppDriver::get_rd_n_wr(){
   return rd_n_wr;
}

uint16_t AppDriver::get_addr(){
   return addr;
}

uint8_t AppDriver::get_wdata(){
   return wdata;
}

void AppDriver::set_accept(uint8_t v){
   accept = v;
}

void AppDriver::set_rdata(uint8_t v){
   rdata = v;
}

void AppDriver::set_ready(uint8_t v){
   ready = v;
} 

void AppDriver::advance() {
   cycles++;
   check_coverage();

   // Ready could be asserted in any state
   if(ready == 1){
      rbw += 8;
      if(rdata != check){
         l.log(Verbosity::ERROR,"[App ] Mismatch mem[0x%04X]", check_addr); 
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
            check_addr = addr;
            l.log(Verbosity::DEBUG,"[App ] mem[0x%04X] -> 0x%02X", check_addr, check);
            state = DriverState::IDLE; 
         }
         break;
      case DriverState::WR:
         if(accept == 1){ 
            wbw += 8;
            l.log(Verbosity::DEBUG,"[App ] mem[0x%04X] <- 0x%02X", addr, wdata);
            state = DriverState::IDLE;
            mem[addr] = wdata;
         }
         break;
   }
}

void AppDriver::request_profile(){
   uint16_t index;
   if(0 == (std::rand() % backpressure)){  
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
      if(1 == all_reads){
         rd_n_wr = 1; 
      }
      if(1 == all_writes){
         rd_n_wr = 0; 
      }
   }else{
      valid = 0;      
   }
}
void AppDriver::check_coverage(){
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

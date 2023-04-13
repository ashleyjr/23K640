#include <verilated.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "include/app_driver.h"

AppDriver::AppDriver(Logger & logger, uint8_t p):
   l(logger)
{ 
   state = DriverState::IDLE;
   profile_sel = p;
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
   for(int i=0;i<APP_DRV_SIZE;i++){
      mem[i] = 0x00;   
   }
   // Initialise coverage
   for(int i=0;i<APP_DRV_SIZE;i++){
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

void AppDriver::set_inputs(uint8_t a, uint8_t d, uint8_t r){
   accept = a;
   rdata = d;
   ready = r;
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
   switch(profile_sel){
      case 0:  profile_wr_some_then_read_some(); 
               break;
      case 1:  profile_wr_all_then_read_all(); 
               break;
      case 2:  profile_wr_all_then_read_all_even(); 
               break;
      case 3:  profile_rand_full_rate();
               break;
      case 4:  profile_rand_low_rate();
               break;
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

void AppDriver::profile_wr_some_then_read_some(){
   valid = 1;
   wdata = std::rand() % 256;  
   if(16 == addr){
      addr = 0;
      rd_n_wr = ~rd_n_wr;
      rd_n_wr &= 0x1;
   }
   addr++;
}


void AppDriver::profile_wr_all_then_read_all(){
   valid = 1;
   wdata = std::rand() % 256;  
   if((APP_DRV_SIZE-1) == addr){
      addr = 0;
      rd_n_wr = ~rd_n_wr;
      rd_n_wr &= 0x1;
   }
   addr++;
}

void AppDriver::profile_wr_all_then_read_all_even(){
   valid = 1;
   wdata = std::rand() % 256;  
   if((APP_DRV_SIZE-2) == addr){
      addr = 0;
      rd_n_wr = ~rd_n_wr;
      rd_n_wr &= 0x1;
   }
   addr+=2;
}

void AppDriver::profile_rand_full_rate(){
   valid = 1;
   wdata = std::rand() % 256; 
   addr = std::rand() % APP_DRV_SIZE;
   rd_n_wr = std::rand() % 2; 
}

void AppDriver::profile_rand_low_rate(){
   valid = ((std::rand() % 100) == 0);
   wdata = std::rand() % 256; 
   addr = std::rand() % APP_DRV_SIZE;
   rd_n_wr = std::rand() % 2; 
}

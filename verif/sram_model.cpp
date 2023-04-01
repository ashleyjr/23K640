#include <verilated.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "include/sram_model.h"
#include <stdarg.h> 

SramModel::SramModel(Logger & logger):
   l(logger)
{  
   state = MemState::IDLE;
   cmd_data = 0;
   so = 0;
   for(int i=0;i<65536;i++){
      mem[i] = 0xAA;   
   }
}

void SramModel::set_inputs(uint8_t c, uint8_t s, uint8_t ck) {  
   cs = c;
   si = s;
   if((ck == 1) && (sck_last == 0)){
      advance();
   }
   sck_last = ck;
}

uint8_t SramModel::get_so() {  
   return so; 
}

void SramModel::advance() {   
   l.log(Verbosity::DEBUG,"[SRAM] ~~~~~~~~~~");
   
   // Pin states
   l.log(Verbosity::DEBUG,"[SRAM] CS:\t\t\t0x%01X", cs);
   l.log(Verbosity::DEBUG,"[SRAM] SI:\t\t\t0x%01X", si);
   
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
      case MemState::READ_15:    if(cs == 0){
                                    cmd_data = mem[addr];
                                    state = MemState::READ_16;
                                    l.log(Verbosity::DEBUG,"[SRAM] READ:\t\tmem[0x%04X] -> 0x%02X", addr, cmd_data);
                                 }else{
                                    state = MemState::IDLE;
                                 }
                                 break; 
      case MemState::READ_16:    if(cs == 0){
                                    state = MemState::READ_17; 
                                 }else{
                                    state = MemState::IDLE; 
                                 }
                                 break;
      case MemState::READ_17:    state = MemState::READ_18;    break;
      case MemState::READ_18:    state = MemState::READ_19;    break;
      case MemState::READ_19:    state = MemState::READ_20;    break;
      case MemState::READ_20:    state = MemState::READ_21;    break;
      case MemState::READ_21:    state = MemState::READ_22;    break;
      case MemState::READ_22:    state = MemState::READ_23;    break;
      case MemState::READ_23:    addr++;
                                 cmd_data = mem[addr];
                                 state = MemState::READ_16;    break;
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
      case MemState::WRITE_16:   
                                 l.log(Verbosity::DEBUG,"write 16");

                                 if(cs == 0){
                                    state = MemState::WRITE_17;   
                                 }else{
                                    state = MemState::IDLE;   
                                 }
                                 break;
      case MemState::WRITE_17:   if(cs == 0){
                                    state = MemState::WRITE_18;   
                                 }else{
                                    state = MemState::IDLE;   
                                 }
                                 break;

                                 //state = MemState::WRITE_18;   break;
      case MemState::WRITE_18:   state = MemState::WRITE_19;   break;
      case MemState::WRITE_19:   state = MemState::WRITE_20;   break;
      case MemState::WRITE_20:   state = MemState::WRITE_21;   break;
      case MemState::WRITE_21:   state = MemState::WRITE_22;   break;
      case MemState::WRITE_22:   state = MemState::WRITE_23;   break; 
      case MemState::WRITE_23:   state = MemState::WRITE_16;
                                 mem[addr] = cmd_data;
                                 addr++;
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


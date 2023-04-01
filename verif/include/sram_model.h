#ifndef SRAM_MODEL_H
#define SRAM_MODEL_H

#include "logger.h"

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
      SramModel(Logger & logger);
      void set_sck(int v);
      void set_cs(int v);
      void set_si(int v);
      uint8_t get_so();
   private:
      Logger   l;
      bool     debug;
      MemState state;
      uint8_t  cs;
      uint8_t  si;
      uint8_t  so;
      uint16_t addr;
      uint8_t  cmd_data;
      uint8_t  sck_last;
      uint8_t  mem[65536]; 
      void advance(); 
};
#endif

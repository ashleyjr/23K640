#ifndef APP_DRIVER_H
#define APP_DRIVER_H

#include "logger.h"

enum class DriverState { 
   IDLE,
   WR,
   RD
};
 
class AppDriver {
   public:
      AppDriver(Logger & logger, uint16_t n, uint16_t b, uint16_t r, uint16_t w); 
      void report_bw();
      uint8_t get_valid();
      uint8_t get_rd_n_wr();
      uint16_t get_addr();
      uint8_t get_wdata();
      void set_inputs(uint8_t a, uint8_t d, uint8_t r); 
      void advance() ;
   private:
      Logger   l;
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
      uint32_t cycles;
      uint32_t wbw;
      uint32_t rbw;
      uint16_t backpressure;
      uint16_t all_reads;
      uint16_t all_writes;
      uint16_t check_addr;
      
      void request_profile();      
      void check_coverage();
};
#endif


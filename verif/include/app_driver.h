#ifndef APP_DRIVER_H
#define APP_DRIVER_H

#include "logger.h"

#define APP_DRV_SIZE 65536 

enum class DriverState { 
   IDLE,
   WR,
   RD
};
 
class AppDriver {
   public:
      AppDriver(Logger & logger, uint8_t p); 
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
      uint16_t addrs[APP_DRV_SIZE];
      bool cov[APP_DRV_SIZE];
      uint16_t coverage;
      uint8_t wdata;
      uint8_t rdata;
      uint8_t ready;
      uint8_t mem[APP_DRV_SIZE];
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
      uint8_t profile_sel;
      void request_profile();      
      void check_coverage();
      void profile_wr_some_then_read_some();
      void profile_wr_all_then_read_all();
      void profile_wr_all_then_read_all_even();
      void profile_rand_full_rate();
      void profile_rand_low_rate();
};
#endif


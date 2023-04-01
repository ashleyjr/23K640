#include <verilated.h>

#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "include/logger.h"
#include <stdarg.h> 

Logger::Logger(char const * filename, vluint64_t const * sim_time){  
   s = sim_time;
   fail = false;
   f = fopen(filename, "w+");
} 

Logger::~Logger(){
   printf("====\n");
   if(fail){
      printf("FAIL\n");
   }else{
      printf("PASS\n");
   }
   printf("====\n");
   fclose(f);
}

void Logger::log(Verbosity v, const char *format, ...){
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
   sprintf(buffer1,"%10ld [%s]: %s\n",*s,str,buffer0);
   switch(v){
      case Verbosity::ERROR:  fail = true;
      case Verbosity::INFO:   printf("%s",buffer1);
      case Verbosity::DEBUG:  fprintf(f,"%s",buffer1);
   }
   va_end(args);
} 
   



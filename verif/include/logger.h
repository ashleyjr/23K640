#ifndef LOGGER_H
#define LOGGER_H
enum class Verbosity { 
   ERROR,
   INFO,
   DEBUG
};

class Logger{
   public:
      Logger(char const * filename, vluint64_t const * sim_time);
      ~Logger();
      void log(Verbosity v, const char *format, ...);  
   private:
      FILE *f;
      vluint64_t const * s;
      bool fail;

};
#endif

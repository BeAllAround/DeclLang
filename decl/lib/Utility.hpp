// Also see Terminal::Color




#ifndef DECL_UTILITY_H
#define DECL_UTILITY_H

#include <string>


#define assertm(msg, exp) assert((void(msg), exp))

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CURRENT_LINE_STR TOSTRING(__LINE__)

int print(const std::string& block_name) {
    #ifdef DECL_COMPILER_VERBOSE
    std::cout << "Testing " << block_name << " Block" << std::endl;
    #endif

    return 0;
}

int _defer_block_print(const std::string& block_name) {
    #ifdef DECL_COMPILER_VERBOSE
    std::cout << "Finished " << block_name << " Block" << std::endl;
    #endif

    return 0;
}


class Logger {
  public:


  template<class T>
  inline static void _info( T&& arg ) {
    std::cout << arg << '\n';
  }

  template<class T, class ... Args>
  inline static void _info( T&& arg,  Args ... args) {
    std::cout << arg << ' ';
    return _info(args...);
  }

  template<class ... Args>
  inline static void info( Args ... args) {
    #ifdef DECL_LOG_INFO
    _info(args...);
    #endif
  }

};

#define TEST_BLOCK(block_name) for(int i = print(block_name); i < 1; i++, _defer_block_print(block_name))

// NOTE: ALSO WORKS WITH EMPTY ARRAYS
#define get_arr_size(arr) (sizeof(arr) / (sizeof(decltype(*(arr)))))



void log_section_cpu_time(const char* block_name, double _start_time) {
  // NOTE: ifdef for measuring the time. This MUST BE OPTIONAL
  #ifdef TEST_SECTION_CPU_TIME_USED
  printf("CPU TIME USED for %s section: %f\n", block_name, (static_cast<double>(clock() - _start_time) / CLOCKS_PER_SEC) );
  #endif
}

#define TEST_SECTION(block_name) for(double i = 0, _start_time = static_cast<double>(clock()); i < 1; i++, log_section_cpu_time(block_name, _start_time))


void __pause() {
  char s[2];
  printf("PAUSED\n");
  if(fgets(s, sizeof s, stdin)) {}
}


std::string strip_white_space(const std::string& str) {
  std::string result;
  for(size_t i = 0; i < str.length(); i++) {
    char c = str[i];
    if(c == ' ' || c == '\n' || c == '\t' || c == '\r') {
      continue;
    }

    result += c;
  }
  return result;
}


#endif


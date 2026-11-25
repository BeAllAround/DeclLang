
#include <iostream>

#include <vector>


#include <cassert>
#include <cstring>

#include <cstdint>



// g++ Allocator.cpp -o out.out -O0 --std=c++20 -Wall && valgrind -s --leak-check=full --show-leak-kinds=all ./out.out
// g++ Allocator.cpp -o out.out -O1 --std=c++20 -Wall && valgrind -s --leak-check=full --show-leak-kinds=all ./out.out
// g++ Allocator.cpp -o out.out -O3 --std=c++20 -Wall && valgrind -s --leak-check=full --show-leak-kinds=all ./out.out


#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", static_cast < double > (clock() - s_t_a_r_t) / CLOCKS_PER_SEC);


#define _STRICT_MEMORY_TESTING

class CustomMemoryStorage {
  public:

  unsigned char* buffer {nullptr};
  unsigned char* buffer_start {nullptr};
  unsigned char* buffer_end {nullptr};

  size_t bytes_destroyed {0};
  size_t CAPACITY {0};


  explicit CustomMemoryStorage(const size_t Size)
  {

    buffer = reinterpret_cast<unsigned char*>(
      ::operator new(Size)
    );

    buffer_start = buffer;

    // NOTE: +1 to avoid the zero index ("invalid/illegal" address READ) edge case for the next_block_available if_range
    // This is OK since we never touch (READ/WRITE) this address, it is simply used for address range as uintptr_t because that's all pointers essentially are.
    buffer_end = buffer + Size + 1; // TODO: Would this cause StackOverflow on the uintptr_t if we are one byte away from that after the successfull allocation of buffer on heap? This is rare.

    CAPACITY = Size;
  }


  // NOTE: THIS IS DELETED AS IT DEMANDS MORE FUNCTIONALITY AND RUNTIME OVERHEAD WITH THAT
  CustomMemoryStorage(const CustomMemoryStorage&) = delete;
  CustomMemoryStorage(CustomMemoryStorage&&) = delete;

  CustomMemoryStorage& operator=(const CustomMemoryStorage&) = delete;
  CustomMemoryStorage& operator=(CustomMemoryStorage&&) = delete;


  inline void* next_block_available(const size_t n) {

    unsigned char* tmp_buffer = buffer + n; // NOTE: MAKE SURE THERE IS ONLY ONE ADD ASSEMBLY INSTRUCTION

    if( tmp_buffer >= buffer_end ) {
      throw std::runtime_error("next_block Available cannot be allocated");
      // TODO: Just proceed with the heap (operator ::new)

    }

    // register void* rawMemory = ...
    void* rawMemory = reinterpret_cast<void*>(buffer);


    buffer = tmp_buffer; // MOV

    return rawMemory;
  }

  template<class T>
  inline void destroy(T* block) {
    block->~T();

    bytes_destroyed += sizeof(T);

  }

  void attach_buffer(void* buffer) {

    assert(this->buffer == nullptr); // ONLY ATTACH THE BUFFER IS PREVIOUS IS NULL

    this->buffer = reinterpret_cast<unsigned char*>(buffer);
  }

  // template<class T, class... Args> inline T* custom_new(Args&&...) __attribute__((always_inline));

  template<class T, class... Args>
  inline T* custom_new(Args&&... args) {
    void* rawMemory = next_block_available(sizeof(T));

    return new (rawMemory) T(std::forward<Args>(args)...);
  }

  inline ~CustomMemoryStorage() noexcept(false) {

    if(bytes_destroyed == CAPACITY) {
      ::operator delete (buffer_start);

    } 
    
#ifdef _STRICT_MEMORY_TESTING
    
    else {
      // TODO: Should we actually throw here? It is the programmer's responsibility to destroy all the blocks (used or hanging)
      // However this block should only be present for the unit testing coverage
      std::cout << "bytes_destroyed: " << bytes_destroyed << std::endl;
      std::cout << "CAPACITY: " << CAPACITY << std::endl;

      throw std::runtime_error("Not all blocks destroyed and released!");
    }

#endif


  }


};

class S {
  public:
  int* i_ptr { nullptr };

  S() {
    std::cout << "S()" << std::endl;
  }

  S(int i) {
    std::cout << "S(int)" << std::endl;
    i_ptr = new int(i);
  }

  S(const S& other) {
    std::cout << "S(const S&)" << std::endl;
    i_ptr = new int(*other.i_ptr);
  }

  S(S&& other) {
    std::cout << "S(S&&)" << std::endl;
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
  }


  S& operator=(const S& other) {
    std::cout << "S&operator=(const S&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = new int(*other.i_ptr);
    return *this;
  }


  S& operator=(S&& other) {
    std::cout << "S&operator=(S&&)" << std::endl;
    if(i_ptr != nullptr) {
      delete i_ptr;
    }
    i_ptr = other.i_ptr;
    other.i_ptr = nullptr;
    return *this;
  }

  ~S() {
    std::cout << "~S()" << std::endl;

    if(i_ptr != nullptr) { // The delete below already does this check, effectively
      delete i_ptr;
      i_ptr = nullptr;
    }

  }
};

struct S1 {

  int i {100};
  // int i1 {0};

  S1() = default;

};


static CustomMemoryStorage fuzz_allocator (sizeof(S) * 3); // NOTE: This allocator can only be used as a static variable due to its destructor unless it is scoped somewhere at the upper level where its memory can be safely used such as READ/WRITE, etc.

std::vector<S*> fuzz_test_vector() {
  // static CustomMemoryStorage allocator (1000); // NOTE: This allocator can only be used as a static variable due to its destructor

  std::vector<S*> v;

  // NOTE: Confirming that using a primitive pointer fully escapes the variable (primitive or struct) restoration and the C++ RAII. 
  // This is due to the fact that, just like the memory on heap - this memory has to point to somewhere else - where it is left untouched by the current scope - global/top level, for example.
  S* s3 = fuzz_allocator.custom_new<S>(30);

  v.push_back(
    fuzz_allocator.custom_new<S>(10)
  );

  v.push_back(
    fuzz_allocator.custom_new<S>(20)
  );


  v.push_back(s3);


  return v;
}


#define _LOGS_VERBOSE

int _block_print(const std::string& block_name) {
    #ifdef _LOGS_VERBOSE
    std::cout << "Testing " << block_name << " Block" << std::endl;
    #endif

    return 0;
}

int _defer_block_print(const std::string& block_name) {
    #ifdef _LOGS_VERBOSE
    std::cout << "Finished " << block_name << " Block" << std::endl;
    #endif

    return 0;
}

#define assertm(msg, exp) assert((void(msg), exp))

#define TEST_BLOCK(block_name) for(int i = _block_print(block_name); i < 1; i++, _defer_block_print(block_name))

#define DO_NOT_OPTIMIZE_AWAY(var) asm volatile("" : : "g"(var) : "memory")


int main() {

  TEST_BLOCK("Fuzz Test Vector") {
    __label__ __defer;
    std::vector<S*> v = fuzz_test_vector();

    int _i = 1;

    for(auto start = v.begin(); start != v.end(); start++) {
      assertm("Sanity Check | OK", (*((*start)->i_ptr)) == _i*10);

      _i++;
    }



    __defer: {

      for(auto start = v.begin(); start != v.end(); start++) {
        fuzz_allocator.destroy(*start);
      }

    }

  }


  TEST_BLOCK("Basic Tests | Zero Index ('invalid/illegal' address READ) edge case") {

    CustomMemoryStorage allocator (sizeof(S));


    // This allocation OK
    S* s = allocator.custom_new<S>(10);


    assertm("Sanity Check | OK", *(s->i_ptr) == 10);


    allocator.destroy(s);


  }

  TEST_BLOCK("Basic Tests | Zero Index ('invalid/illegal' address READ) edge case THROWS") {

    CustomMemoryStorage allocator (sizeof(S));


    // This allocation OK
    S* s = allocator.custom_new<S>(10);


    assertm("Sanity Check | OK", *(s->i_ptr) == 10);


    bool thrown = false;

    try{
      S* s = allocator.custom_new<S>(11);
      DO_NOT_OPTIMIZE_AWAY(s);

    } catch(const std::runtime_error& error) {
      assert(
        strcmp(
          error.what(),
          "next_block Available cannot be allocated"
        ) == 0
      );

      thrown = true;
    }

    allocator.destroy(s);


    assertm("Thrown TRUE", thrown);

  } 

  TEST_BLOCK("Basic Tests | Zero Index ('invalid/illegal' address READ) edge case + u_char THROWS") {

    CustomMemoryStorage allocator (sizeof(S));


    // This allocation OK
    S* s = allocator.custom_new<S>(10);


    assertm("Sanity Check | OK", *(s->i_ptr) == 10);


    bool thrown = false;

    try{
      char* c = allocator.custom_new<char>('u');
      DO_NOT_OPTIMIZE_AWAY(c);

    } catch(const std::runtime_error& error) {
      assert(
        strcmp(
          error.what(),
          "next_block Available cannot be allocated"
        ) == 0
      );

      thrown = true;
    }

    allocator.destroy(s);


    assertm("Thrown TRUE", thrown);

  } 

  TEST_BLOCK("Basic Tests | Multiple Allocations OK") {

    CustomMemoryStorage allocator (sizeof(S) * 10);

    uintptr_t _address = 0;

    for(size_t i = 0; i < 10; i++) {
      // This allocation OK
      S* s = allocator.custom_new<S>(10);

      assertm("Should be different point in memory", 
        _address != reinterpret_cast<uintptr_t>(s)
      );

      std::cout << s << std::endl;

      assertm("Sanity Check | OK", *(s->i_ptr) == 10);

      allocator.destroy(s);

      _address = reinterpret_cast<uintptr_t>(s);

    }

  } 

  TEST_BLOCK("Basic Tests | Multiple Allocations THROWS") {

    CustomMemoryStorage allocator (sizeof(S) * 9);

    bool thrown = false;

    for(size_t i = 0; i < 10; i++) {
      // This allocation OK
      if(i < 9) {

        S* s = allocator.custom_new<S>(10);
        std::cout << s << std::endl;
        assertm("Sanity Check | OK", *(s->i_ptr) == 10);

        allocator.destroy(s);

      } else {
        try{
          S* s = allocator.custom_new<S>(11);
          DO_NOT_OPTIMIZE_AWAY(s);
        } catch(const std::runtime_error& error) {
          assert(
            strcmp(
              error.what(),
              "next_block Available cannot be allocated"
            ) == 0
          );

          thrown = true;
        }

      }

    }

    assertm("Thrown TRUE", thrown);


  } 

  TEST_BLOCK("Basic Tests | Structs | Various Size") {

    CustomMemoryStorage allocator (sizeof(S) + sizeof(S1));


    S* s = allocator.custom_new<S>(10);

    assertm("Sanity Check | OK", *(s->i_ptr) == 10);


    S1* s1 = allocator.custom_new<S1>();


    allocator.destroy(s);
    allocator.destroy(s1);

  } 

  TEST_BLOCK("Basic Tests | Primitive and Structs | One Byte Allocation at the end of buffer pointer") {

    typedef unsigned char u_char;

    assert(sizeof(u_char) == 1);

    CustomMemoryStorage allocator (sizeof(S) + sizeof(u_char));


    S* s = allocator.custom_new<S>(10);

    assertm("Sanity Check | OK", *(s->i_ptr) == 10);


    bool thrown = false;
    try{
      S1* s1 = allocator.custom_new<S1>();
      DO_NOT_OPTIMIZE_AWAY(s1);
    } catch(const std::runtime_error& error) {

      assert(
        strcmp(
          error.what(),
          "next_block Available cannot be allocated"
        ) == 0
      );

      thrown = true;
    }

    assertm("Thrown TRUE", thrown);


    char* c = allocator.custom_new<char>('u');
    assertm("Sanity Check | OK", *c == 'u');


    allocator.destroy(s);

    // TODO: FIXME: Destroy should not be NEEDED FOR PRIMITIVE TYPES/STRUCTS AND TRIVIAL DESTRUCTORS.
    allocator.destroy(c);

  } 

  return 0;

}

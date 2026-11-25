#include <flat_hash_map.hpp>
// #include <bytell_hash_map.hpp> // NOTE: Only for STD17 Standard


#include <emhash/hash_table7.hpp>



#include <debug.h>


#include <iostream>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <string>



template<class K, class V>
// using hash_table = std::vector<K>;
using hash_table = emhash7::HashMap<K, V>;
// using hash_table = ska::flat_hash_map<K, V>;
// using hash_table = ska::bytell_hash_map<K, V>; // STD17 Standard
// using hash_table = std::unordered_map<K, V>;

// TODO: RUN THESE TESTS ON VERY LOW-END MACHINE. SET IT UP AS VM AND RUN
//
/*
 * NOTE:
 * Based on the hash table implementation but generally, the lookup performance sorely depends on the number of entries in a hash table. For example, we only need dozens of entries but usually even less than that - so the performance is determined by the use case of the hash table itself. ( IN WHICH CASE WE ARE GOING TO PRODUCE VERY EFFICIENT RESULTS/BENCHMARKS: SOMETIMES BEING NEARLY IDENTICAL TO THE IN-MEMORY LOOKUP BELOW). OF COURSE, THE PERFORMANCE WILL VARY DEPENDING IF THE ENTRY IS ACTUALLY FOUND COMPARED TO WHEN IT IS NOT FOUND. FOR EXAMPLE, WORST CASE, BEST CASE OF THE LINEAR ARRAY SEARCH WHERE THE BEST CASE IS 1 is the element is at 0 index. THIS IS SIMILARLY THE CASE WITH HASH TABLES DEPENDING ON THE IMPLEMENTATION AS WE NEED TO COMPARE THE KEYS FROM THE SAID BUCKET OR DISTANCE ANYWAYS.
 * See the hash table benchmark diagram in this folder.
 */

int main() {

  const size_t LOAD = 1000000;

  {
    hash_table<size_t, size_t> h01;

    h01.reserve(10);
    // h01.reserve(LOAD);

    // Insertion
    // LOOKUP BENCHMARK FOR SMALLER HASH TABLES
    for(volatile size_t i = 0; i < 10; i++) {
      h01[(size_t)i] = i;
    }
    /*
    // LOOKUP BENCHMARK FOR VERY LARGE HASH TABLES
    for(size_t i = 0; i < LOAD; i++) {
      h01[i] = i;
    }
    */


    // std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    // std::chrono::duration<double> seconds = now.time_since_epoch();

    volatile size_t _key = 100;


    // auto start = std::chrono::steady_clock::now();
    for(volatile size_t i = 0; i < 10; i++) {
      start_time;
      for(volatile size_t i = 0; i < LOAD; i++) {
        // Ensuring "clean" lookup by looking with a primitive key and returning primitive value
        // auto value = h01[i]; // NOTE that this is not an actual lookup since operator[]() does more such as emplace, etc.

        // auto value = h01.pre_find((size_t)i, (size_t)i); // COMPARE TO h01.find((size_t)0); (static key), h01.pre_find((size_t)i, (size_t)i);
        
        auto value = h01.find((size_t)i); // COMPARE TO h01.find((size_t)_key) (non-existent key); h01.find((size_t)0); (static existing key entry), h01.pre_find((size_t)i, (size_t)i);

        // assert(value != h01.end());
        if(value != h01.end()) {
          volatile auto v = value->second;
          // std::cout << value->second << std::endl;
          // assert(v == i); // Collision (sanity) check if we are doing weird skarupke modifications
        }

      }
      end_time;
    }

    // auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    // std::cout << "Milliseconds since epoch: " << milliseconds << std::endl;
    // std::cout << "Seconds since epoch (chrono double): " << seconds.count() << std::endl;
    /*
       auto end = std::chrono::steady_clock::now();
       std::chrono::duration<double, std::milli> duration = end - start;
       std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;
       */


  }

  #ifdef SKA_SPECIFIC // Skarupke flat_hash_map specific tests
  {
    std::cout << "pre_find: hash policy benchmark start" << std::endl;
    for(volatile size_t i = 0; i < 10; i++) {
      ska::flat_hash_map<size_t, size_t, ska::power_of_two_std_hash<size_t>> h02;
      // hash_table<size_t, size_t> h02;
      h02.reserve(2);

      // Insertion
      for(volatile size_t i = 0; i < 2; i++) {
        h02[(size_t)i] = i;
      }

      const char* c = "ABBB";
      // size_t _key = 0;
      volatile size_t _key = reinterpret_cast<size_t>(c);

      start_time;
      for(volatile size_t i = 0; i < LOAD; i++) {
        auto value = h02.find((size_t)_key); // ALSO, COMPARE TO h02.find(i)
        // auto value = h02.pre_find(_key, _key);
        if(value != h02.end()) {
          volatile auto v = value->second;
          // std::cout << value->second << std::endl;
        }

      }
      end_time;
    }
    std::cout << "pre_find: hash policy benchmark end" << std::endl;

  }

  {
    std::cout << "isolate index_of policy pre_find: hash policy benchmark start" << std::endl;
    for(size_t i = 0; i < 10; i++) {
      ska::flat_hash_map<size_t, size_t, ska::power_of_two_std_hash<size_t>> h02;
      // hash_table<size_t, size_t> h02;
      h02.reserve(2);

      auto hash_policy = decltype(h02)::hash_policy_selector{};


      // Insertion
      for(size_t i = 0; i < 2; i++) {
        h02[i] = i;
      }

      const char* c = "ABBB";
      // size_t _key = 0;
      volatile size_t _key = reinterpret_cast<size_t>(c); // TODO: Verify if this is correct according to hash_object(key)

      size_t num_slots_minus_one = h02.get_num_slots_minus_one();
      size_t _index = hash_policy.index_for_hash(_key, num_slots_minus_one);
      _index = ptrdiff_t(_index);

      start_time;
      for(volatile size_t i = 0; i < LOAD; i++) {
        // auto value = h02.find(_key);
        auto value = h02._pre_find((size_t)_key, _index);
        if(value != h02.end()) {
          volatile auto v = value->second;
          // std::cout << value->second << std::endl;
        }

      }
      end_time;
    }
    std::cout << "isolate index_of policy pre_find: hash policy benchmark end" << std::endl;

  }

  #endif


  // In-memory lookup, effectively
  std::cout << "start: In-memory lookup, effectively" << std::endl;
  {
    std::vector<size_t> h02;
    h02.reserve(2);

    // Insertion
    for(size_t i = 0; i < 2; i++) {
      h02[i] = i;
    }

    volatile size_t _key = 0;

    for(size_t ii = 0; ii < 10; ii++) {
      start_time;
      for(volatile size_t i = 0; i < LOAD; i++) {
        volatile size_t value = h02[(size_t)_key];

        if(value != i) { // NOTE: Replicating CMP OF entry != end()
          value = i;
        }

      }
      end_time;
    }

  }
  std::cout << "end: In-memory lookup, effectively" << std::endl;


  {
    std::cout << "find string start" << std::endl;
    for(volatile size_t i = 0; i < 10; i++) {
      hash_table<std::string, size_t
      //  ska::power_of_two_std_hash<std::string>
        > h02;
      // hash_table<size_t, size_t> h02;
      h02.reserve(2);

      // Insertion
      h02[std::string("AAAA")] = 0;
      h02[std::string("AAAAB")] = 1;


      std::string _key = "AAAA";

#ifdef SKA_SPECIFIC
      auto hash_policy = decltype(h02)::hash_policy_selector{};


      size_t num_slots_minus_one = h02.get_num_slots_minus_one();
      size_t _index = hash_policy.index_for_hash( h02.hash_object(_key), num_slots_minus_one);
      _index = ptrdiff_t(_index);
#endif

      start_time;
      for(volatile size_t i = 0; i < LOAD; i++) {
        auto value = h02.find(_key); // COMPARE TO THE h02._pre_find BELOW
        // auto value = h02._pre_find(_key, _index);
        if(value != h02.end()) {
          volatile auto v = value->second;
          // std::cout << value->second << std::endl;
        }

      }
      end_time;
    }
    std::cout << "find string end" << std::endl;

  }

  return 0;
}

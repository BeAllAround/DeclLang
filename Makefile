
# export PATH := ${HOME}/Project/skarupke:$(PATH)

# CXXFLAGS += -I$(HOME)/Project

# gedit test.log &
# make run_parser_tests > test.log

# -fsanitize=address
# -Wno-maybe-uninitialized

# TODO: Script for running tests and valgrind using -O0, -O1, -O2, -O3 individually
# Default behavior: Major compilers typically perform dynamic linking to the standard runtime library by default
# though you can force static linking using specific compiler flags (like -static-libstdc++).
# -static-libstdc++ -static-libgcc -static
# For more, see https://gcc.gnu.org/onlinedocs/gcc-7.3.0/gcc/Link-Options.html

# With valgrind -s ./out when using -static, we are getting various "Conditional jump or move depends on uninitialised value(s)" errors (from std::cout and libc startup, etc.) but these are considered false positives as long as our program doesn't cause memory leaks, and mostly importanly, - does not crash, resulting in UB (Undefined Behaviour) and similar.
# We also get these false positives with ska::detailv3::sherwood_v3_table and ::rehash(unsigned long), however, this is not an indication of a real bug in your program or of undefined behavior that will bite you in practice; the library is written this way deliberately for speed and is widely used.


run_lexer_tests:
	g++ tests/lexer/main.cpp -o out.out -I ~/Project -I./ -Wunreachable-code -Wold-style-cast -Wall -Werror --std=c++11 -O1 && ./out.out

run_parser_tests:
	g++ tests/parser/main.cpp -o out.out -I ~/Project -I./ -Wunreachable-code -Wold-style-cast -Wall -Werror --std=c++11 -O1 && ./out.out


run_api_tests:
	g++ tests/api/main.cpp -o out.out -I ~/Project -I./ -Wno-maybe-uninitialized -Wunreachable-code -Wold-style-cast -Wall -Werror --std=c++11 -O1 && ./out.out

run_runtime_tests:
	g++ tests/astruntime/main.cpp -o out.out -I ~/Project -I./ -Wunreachable-code -Wold-style-cast -Wall -Werror --std=c++11 -O1 && ./out.out

depth_run_parser_tests:
	g++ tests/parser/main.cpp -o out.out -I ~/Project -I./ -Wunreachable-code -Wold-style-cast -Wall -Werror --std=c++11 -O3 && ./out.out

depth_run_api_tests:
	g++ tests/api/main.cpp -o out.out -I ~/Project -I./ -Wunreachable-code -Wold-style-cast -Wall -Werror --std=c++11 -O3 && ./out.out

depth_run_runtime_tests:
	g++ tests/astruntime/main.cpp -o out.out -I ~/Project -I./ -Wunreachable-code -Wold-style-cast -Wall -Werror --std=c++11 -O3 && ./out.out

depth_run_lexer_tests:
	g++ tests/lexer/main.cpp -o out.out -I ~/Project -I./ -Wunreachable-code -Wold-style-cast -Wall -Werror --std=c++11 -O3 && ./out.out

check_leak:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./out.out

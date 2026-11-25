
# export PATH := ${HOME}/Project/skarupke:$(PATH)

# CXXFLAGS += -I$(HOME)/Project

# gedit test.log &
# make run_parser_tests > test.log

# -fsanitize=address
# -Wno-maybe-uninitialized

# TODO: Script for running tests and valgrind using -O0, -O1, -O2, -O3 individually

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
	valgrind --verbose --leak-check=full --show-leak-kinds=all ./out.out

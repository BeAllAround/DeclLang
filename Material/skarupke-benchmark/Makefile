_make_run:
	g++ benchmark01.cpp -o out.out -O1 --std=c++11 -I ~/Project/emhash/include -I /home/alex/Project/skarupke/flat_hash_map -I ../../../ && ./out.out
_make_depth_run:
	g++ benchmark01.cpp -o out.out -O3 --std=c++11 -I ~/Project/emhash/include -I /home/alex/Project/skarupke/flat_hash_map -I ../../../ && ./out.out
_valgrind_py:
	export PYTHONMALLOC=malloc_debug && valgrind -s python3 _python.py

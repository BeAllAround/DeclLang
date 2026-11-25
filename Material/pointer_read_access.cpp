#include <iostream>

#include <cstdio>

// -O3 -Wall

/*
# GNU C++20 (Compiler-Explorer-Build-gcc--binutils-2.44) version 16.1.0 (x86_64-linux-gnu)
#       compiled by GNU C version 11.4.0, GMP version 6.3.0, MPFR version 4.2.2, MPC version 1.3.1, isl version isl-0.24-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -masm=intel -mtune=generic -march=x86-64 -g -O3
#APP
#NO_APP
"main":
# /app/example.cpp:9: int main() {
        sub     rsp, 8    #,
# /app/example.cpp:17:         unsigned char* b = new unsigned char[100];
        mov     edi, 100  #,
        call    "operator new[](unsigned long)" #
        mov     rdi, rax  # tmp110, tmp122
# /app/example.cpp:25:         unsigned char* p1 = b+1;
        lea     rax, [rax+1]      # p1_7,
# /app/example.cpp:26:         DO_NOT_OPTIMIZE_AWAY(p1);
# /app/example.cpp:29:         unsigned char* p2 = b+7;
        lea     rax, [rdi+7]      # p2_9,
# /app/example.cpp:30:         DO_NOT_OPTIMIZE_AWAY(p2);
# /app/example.cpp:33:         unsigned char* p3 = b+30;
        lea     rax, [rdi+30]     # p3_11,
# /app/example.cpp:34:         DO_NOT_OPTIMIZE_AWAY(p3);
# /app/example.cpp:36:         unsigned char* p4 = b+40;
        lea     rax, [rdi+40]     # p4_13,
# /app/example.cpp:37:         DO_NOT_OPTIMIZE_AWAY(p4);
# /app/example.cpp:39:         delete[] b;
        call    "operator delete[](void*)"        #
# /app/example.cpp:43:         unsigned char* b = new unsigned char[100];
        mov     edi, 100  #,
        call    "operator new[](unsigned long)" #
        mov     rdi, rax  # tmp115, tmp123
# /app/example.cpp:49:         b = b+1;
        lea     rax, [rax+1]      # b_20,
# /app/example.cpp:50:         DO_NOT_OPTIMIZE_AWAY(b);
# /app/example.cpp:54:         b += 7;
        lea     rax, [rdi+8]      # b_22,
# /app/example.cpp:55:         DO_NOT_OPTIMIZE_AWAY(b);
# /app/example.cpp:56:         b = b+7; // Same as b += 7
        lea     rax, [rdi+15]     # b_24,
# /app/example.cpp:57:         DO_NOT_OPTIMIZE_AWAY(b);
# /app/example.cpp:60:         b = b+30;
        lea     rax, [rdi+45]     # b_26,
# /app/example.cpp:61:         DO_NOT_OPTIMIZE_AWAY(b);
# /app/example.cpp:63:         b = b+40;
        lea     rax, [rdi+85]     # b_28,
# /app/example.cpp:64:         DO_NOT_OPTIMIZE_AWAY(b);
# /app/example.cpp:66:         delete[] b1;
        call    "operator delete[](void*)"        #
# /app/example.cpp:72: }
        xor     eax, eax  #
        add     rsp, 8    #,
        ret     
*/

#define DO_NOT_OPTIMIZE_AWAY(var) asm volatile("" : : "g"(var) : "memory")

int main() {

    // std::vector<S> v;

    // v.push_back(make_s(/* ... */ ));


    {
        unsigned char* b = new unsigned char[100];
        DO_NOT_OPTIMIZE_AWAY(b);

        unsigned char* p = b+0;

        DO_NOT_OPTIMIZE_AWAY(p);
        // printf("%p\n", p);

        unsigned char* p1 = b+1;
        DO_NOT_OPTIMIZE_AWAY(p1);
        // printf("%p\n", p1);

        unsigned char* p2 = b+7;
        DO_NOT_OPTIMIZE_AWAY(p2);
        // printf("%p\n", p2);

        unsigned char* p3 = b+30;
        DO_NOT_OPTIMIZE_AWAY(p3);

        unsigned char* p4 = b+40;
        DO_NOT_OPTIMIZE_AWAY(p4);

        delete[] b;
    }

    {
        unsigned char* b = new unsigned char[100];
        unsigned char* b1 = b;
        
        DO_NOT_OPTIMIZE_AWAY(b);

        // printf("%p\n", b);

        b = b+1;
        DO_NOT_OPTIMIZE_AWAY(b);
        // DO_NOT_OPTIMIZE_AWAY(p1);
        // printf("%p\n", p1);

        b += 7;
        DO_NOT_OPTIMIZE_AWAY(b);
        
        b = b+7; // Same as b += 7
        DO_NOT_OPTIMIZE_AWAY(b);
        // printf("%p\n", p2);

        b = b+30;
        DO_NOT_OPTIMIZE_AWAY(b);

        b = b+40;
        DO_NOT_OPTIMIZE_AWAY(b);

        delete[] b1;
    }



    return 0;
}


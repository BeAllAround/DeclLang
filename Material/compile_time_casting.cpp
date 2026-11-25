#include <iostream>
#include <cstdio>



struct Value {
    const char* s;
    int i;

    Value(int i) : i{i} {}

};



int main() {

    Value v = 120;


    void* v1 = (void*)&v;


    int* v2 = (int*)v1;

    char* v3 = (char*)v1;



    printf("%d\n", ((*v2) + 1) );

    printf("%d\n", ((*v3) + 1) );



    return 0;
}


#include <iostream>

#include <variant>






#define DO_NOT_OPTIMIZE_AWAY(var) asm volatile("" : : "g"(var) : "memory")


// -O0, -O1, -O3

int main()
{

    volatile int i = 0;

    DO_NOT_OPTIMIZE_AWAY(i);

    std::variant<char, int, double> s = 'a';

    std::cout << "sizeof(std::variant): " << sizeof(s) << std::endl;

    std::visit([](auto x){ std::cout << x;}, s);

    s = 123;

    if(i == 1) {
        s = 'b';
    } else {
        s = (double)2.3;
    }

    std::visit([](auto x){ std::cout << x;}, s);


    return 0;
}

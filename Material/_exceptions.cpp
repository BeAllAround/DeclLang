#include <stdexcept>
#include <string>

#include <iostream>


/*
// OK TOO
class Exception {
    public:

    std::string message;

    Exception() {}

    Exception(const std::string& message) : message{message} {
    }

    ~Exception() noexcept {}

    const char* what() const noexcept {
       return message.c_str();
    }


};

*/

class Exception : public std::exception {
    public:

    std::string message;

    Exception() {}

    Exception(const std::string& message) : message{message} {
    }

    virtual ~Exception() noexcept {}

    virtual const char* what() const noexcept {
       return message.c_str();
    }


};


class RuntimeException : public Exception {

public:
    RuntimeException() = delete;


    RuntimeException(const std::string& message) : Exception(message) {

    }

};


class IndexException : public Exception {
    public:

    IndexException() = delete;


    IndexException(const std::string& message) : Exception(message) {

    }


};



void foo(const char* CALLER_LINE) {
    throw RuntimeException("Error in " + std::string(__FILE__) + 
                             " at line " + CALLER_LINE);


}


void bar(const char* CALLER_LINE) {
    throw IndexException("Error in " + std::string(__FILE__) + 
                             " at line " + CALLER_LINE);

}




#define STR2(x) #x
#define STR(x) STR2(x)

int main() {


    std::cout << "sizeof(std::runtime_error): " << sizeof(std::runtime_error) << std::endl;

    try {
        foo(STR(__LINE__));
    } catch(RuntimeException& error) {
        std::cout << error.what() << std::endl;
    }


    try {
        // WILL BE A PROBLEM FOR NESTED CALLS. HOWEVER, IT IS ENOUGH FOR BUILT-IN FUNCTIONS CALLS OF DECL
        bar(STR(__LINE__));
    } catch(Exception& error) {
        std::cout << error.what() << std::endl;
    }

    return 0;
}

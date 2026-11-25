#include <iostream>

#include <vector>

#include <cassert>

/*

// FIND OUT IF THE EXCEPTION IS HEAVILY COPIED DURING THE STACK REWINDING
// THE ANSWER IS, NO. See the following.

#include <iostream>



struct ExcBase {

    ExcBase() {
        std::cout << "ExcBase()" << std::endl;
    }

    ExcBase(const ExcBase&) {
        std::cout << "ExcBase(const EcxBase&)" << std::endl;
    }

    ExcBase(ExcBase&&) {
        std::cout << "ExcBase(EcxBase&&)" << std::endl;
    }

    ExcBase&operator=(const ExcBase&) {
        std::cout << "operator=(const ExcBase&)" << std::endl;

        return *this;
    }

    ExcBase&operator=(ExcBase&&) {
        std::cout << "operator=(ExcBase&&)" << std::endl;
        return *this;
    }

    ~ExcBase() {
        std::cout << "~ExcBase()" << std::endl;
    }

};

void f(int n) {
    if(n == 10) {
        throw ExcBase();
    }


    return f(n + 1);
}



int main() {


    {

        try {
            f(1);
        }
        // COPY REF CALLED FOR catch (ExcBase err) obviously
        catch(const ExcBase& err) {

        }
        // ~ExcBase() CALLED

    }


    {

    }

    return 0;
}


*/



const size_t CALL_STACK_MAX_SIZE = 100;




struct CallFrame {

    const char* trace_message = "";


    CallFrame() {}

    CallFrame(const char* trace_message) : trace_message {trace_message} {}

};

struct Exception {

    std::vector<CallFrame> stack_trace;


    std::string message;

    Exception(const std::string& m) : message{m} {}


    Exception(const Exception& other) : message{other.message}, stack_trace{other.stack_trace} {

    }


};

struct CallStack {
    CallFrame call_stack[CALL_STACK_MAX_SIZE];
    size_t call_stack_size = 0;

    CallStack() {

    }


    void push(const CallFrame& call_frame) {
        call_stack[call_stack_size++] = call_frame;
        // TODO: Maybe placement new constructor?
        // new(&call_stack[call_stack_size++]) CallFrame(call_frame);
    }

    CallFrame& pop() {

        if(call_stack_size == 0) {
            assert(1); // TODO: ADD a message or throw an exception
        }

        return call_stack[--call_stack_size];
    }

    bool is_empty() {
        return call_stack_size == 0;
    }

};

struct Function;

struct Context;

using function_type = int(*)(Function*, int);

struct Context {

    CallStack callstack;

    std::vector<Function*> global_funcs;

    Context() {

    }


    inline Function makeFunction(function_type func);

};




struct Function {

    Context* context;

    function_type func;


    Function(function_type func) : func{func} {}


    void attach_context(Context* context) {
        this->context = context;
    }

    // NOTE: ASSUME intializer_list FOR ARGS
    inline int call(const char*, int) __attribute__((always_inline));

};


inline Function Context::makeFunction(function_type func) {
    Context* current_context = this;
    Function f = Function(func);

    f.attach_context(current_context);

    return f;
}



// the most effective way to test the stack trace is via recursion and other
// AND THE FOLLOWING CODE:
/*

def c() {
    print("C CALLED")
    b();
}

def b() {
    print("B CALLED")
    a();
};


def a() {
    print("A CALLED")
}

// See: https://en.wikipedia.org/wiki/Stack_trace


*/

#define STACK_TRACE_ENABLED
// USED BY UNIT TESTS IN CASE THERE IS NOTHING TO THROW
#define ETHROW_

#ifdef ETHROW_
#define THROW(MESSAGE) { \
Exception excp = Exception(MESSAGE); \
throw excp; \
}
#else
#define THROW(MESSAGE) { }
#endif


int _foo(Function* _this, int n) {
    if(n == 3) { 
        THROW("_foo throw")
        // return 0;
    }
    std::cout << "inside _foo(int)" << std::endl;

    return _this->call("_foo 161", n + 1);
}



// DONE: Provide a way to pass the following "const char*" message via call method since this DEF ISN'T ENOUGH TO KEEP THE REAL STACK TRACE INCLUDING __LINE__, etc.
// I always make sure to search for Function::call in the assembly output in order to be 100% certain that every call is inlined.
inline int Function::call(const char* trace_message, int args) {
    Function* current_function_this = this;

#ifdef STACK_TRACE_ENABLED
    try
    {
        this->context->callstack.push(CallFrame(trace_message));
        int r = func(current_function_this, args);
        this->context->callstack.pop();
        return r;
    }
    catch (Exception &excp)
    {
        CallFrame& call_frame = this->context->callstack.pop();
        excp.stack_trace.push_back(call_frame);

        throw excp; // re-throw. This effectively causes a COPY BUT WITH REFERENCE COUNTING, THIS IS NOT SIGNIFICANT.
    }
#else
    int r = func(current_function_this, args);
    return r;
#endif

}


int a(Function*, int);
int b(Function*, int);
int c(Function*, int);


// global_func[0]
int c(Function* _this, int n) {


    // call b();
    return _this->context->global_funcs[1]->call("_b 195", 0);
}

// global_func[1]
int b(Function* _this, int n) {


    // call a();
    return _this->context->global_funcs[2]->call("_a 202", 0);
}

// global_func[2]
int a(Function* _this, int n) {


    THROW("_a throw")

    return 0;
}



int foo(Function*, int);
int bar(Function*, int);
int zoo(Function*, int);


// global_func[0]
int zoo(Function* _this, int n) {


    // NOTE: Testing CATCH
    try {
        // call bar();
        int r = _this->context->global_funcs[1]->call("_bar 195", 0);

        return r;

    } catch (Exception &err) {
        // throw err; // See the "IF RE_THROWN FROM ZOO" comment below

        assert(err.stack_trace[0].trace_message == "_foo 202");
        assert(err.stack_trace[1].trace_message == "_bar 195");

        assert(err.stack_trace.size() == 2);

        THROW("_zoo throw")

    }

    return 0;

}

// global_func[1]
int bar(Function* _this, int n) {

    // call foo();
    return _this->context->global_funcs[2]->call("_foo 202", 0);
}

// global_func[2]
int foo(Function* _this, int n) {


    THROW("_foo throw")

    return 0;
}


int main() {

    Context context;

    Function foo_func = context.makeFunction(_foo);

    try {

        foo_func.call("_foo 193", 0);
        // NOTE: IF THERE ARE NO THROWS, MUST BE EMPTY
        assert(context.callstack.is_empty());
    } catch(Exception& err) {

        // NOTE: MUST BE EMPTY
        assert(context.callstack.is_empty());

        std::cout << err.message << std::endl;

        const auto& stack_trace = err.stack_trace;

        size_t _size = stack_trace.size();
        assert(_size == 4);

        for(size_t i = 0; i < stack_trace.size(); i++) {
            const CallFrame& call_frame = stack_trace[i];


            std::cout << call_frame.trace_message << std::endl;

        }

    }

    {
        Context context;

        Function c_func = context.makeFunction(c);
        Function b_func = context.makeFunction(b);
        Function a_func = context.makeFunction(a);

        // WEAK REFERENCE IS OK - we are trying to just demontrate this concept.
        context.global_funcs.push_back(&c_func);
        context.global_funcs.push_back(&b_func);
        context.global_funcs.push_back(&a_func);


        try
        {
            context.global_funcs[0]->call("_c 259", 0);
            // NOTE: IF THERE ARE NO THROWS, MUST BE EMPTY
            assert(context.callstack.is_empty());
        }
        catch (Exception &err)
        {

            // NOTE: MUST BE EMPTY
            assert(context.callstack.is_empty());

            std::cout << err.message << std::endl;

            const auto &stack_trace = err.stack_trace;

            size_t _size = stack_trace.size();
            assert(_size == 3);

            // NOTE: THE SIZE_T == UNSINGED LONG, which opposite is LONG
            for (long i = _size - 1; i >= 0; i--)
            {
                const CallFrame &call_frame = stack_trace[i];

                std::cout << call_frame.trace_message << std::endl;
            }
        }
    }

    {
        Context context;

        Function zoo_func = context.makeFunction(zoo);
        Function bar_func = context.makeFunction(bar);
        Function foo_func = context.makeFunction(foo);

        // WEAK REFERENCE IS OK - we are trying to just demontrate this concept.
        context.global_funcs.push_back(&zoo_func);
        context.global_funcs.push_back(&bar_func);
        context.global_funcs.push_back(&foo_func);


        try
        {
            // call zoo();
            context.global_funcs[0]->call("_zoo 259", 0);
            // NOTE: IF THERE ARE NO THROWS, MUST BE EMPTY
            assert(context.callstack.is_empty());
        }
        catch (Exception &err)
        {
            // NOTE: MUST BE EMPTY
            assert(context.callstack.is_empty());


            assert(err.message == "_zoo throw");

            std::cout << err.message << std::endl;

            const auto &stack_trace = err.stack_trace;

            size_t _size = stack_trace.size();
            // assert(_size == 3); // IF RE_THROWN FROM ZOO

            assert(_size == 1);
            assert(stack_trace[0].trace_message == "_zoo 259");


            // NOTE: THE SIZE_T == UNSINGED LONG, which opposite is LONG
            for (long i = _size - 1; i >= 0; i--)
            {
                const CallFrame &call_frame = stack_trace[i];

                std::cout << call_frame.trace_message << std::endl;
            }
        }
    }

    return 0;
}

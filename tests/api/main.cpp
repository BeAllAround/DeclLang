#include <iostream>
#include <string>
#include <vector>

#include <cassert>

#define DECL_COMPILER_VERBOSE

#define DECL_VERBOSE_LOGGING

#define DECL_LOG_INFO

#define VALUE_SAFETY_CHECK

// #define TEST_SECTION_CPU_TIME_USED

#include <decl/lib/Utility.hpp>

#include <decl/lib/DeclAPI.hpp>

#include <decl/lib/AstParser.hpp>


using namespace DeclAPI;


Value foo_num(ObjFunction* _this, func_args args) {

    Value a = DECL_ARGS_GET(args, 0); 

    assertm("foo_num:: Context ATTACHED", _this->context != nullptr);

    return a;
}

Value foo_bool(ObjFunction* _this, func_args args) {
    Value b = makeBoolean(true);

    assertm("foo_bool:: Context ATTACHED", _this->context != nullptr);

    return b;
}


Value foo_array(ObjFunction* _this, func_args args) {
    Value a = DECL_ARGS_GET(args, 0);

    assertm("foo_array:: Context ATTACHED", _this->context != nullptr);

    Value arr = _this->context->makeArray();

    size_t push_sym_offset = _this->context->insert_if_not_and_get_symbol_offset("push");

    // DECL SOURCE: arr.push(a);
    callFunc("LINE 48",
        getMember("LINE 48", arr, cacl_member_offset(push_sym_offset)),
        {
            a,
        }
    );

    return arr;
}

Value foo_sys(ObjFunction* _this, func_args args) {
    Value a = DECL_ARGS_GET(args, 0);

    assertm("foo_sys:: Context ATTACHED", _this->context != nullptr);

    return a;
}


int main()
{

    auto shrd_context = makeUniqueContext();
    Context* main_context = obj_cast<Context*>(shrd_context.get());
    setUpContext(main_context);
    assertm("Context Initialized", main_context->initialized == true);


    TEST_SECTION("Context Tests") {

        UniquePtr<Context> shrd_context = makeUniqueContext();

        Context* context = obj_cast<Context*>(shrd_context.get());
    
        setUpContext(context);
        assertm("Context Initialized", context->initialized == true);

        DataSegment& data_segment = context->data_segment;

        data_segment.add_global_string();
        data_segment.add_global_function(context, foo_num);
        data_segment.add_global_function(context, foo_bool);
        data_segment.add_global_function(context, foo_array);


        TEST_BLOCK("Decl API Basic Global Function Call") {
            num_type n = 11;

            Value v = callFunc("CALL FOO LINE 53", context->get_global_function(0), { makeNumber(n), } );

            assertm("Return from Global Function Sanity Check", v.get_num().n == n);

            assertm(
                "Return from Global Function Sanity Check", 
                v.get_num().n == 11);

            
            // NOTE: MAKING SURE THERE IS NOT COMPILE-TIME ERRORS HERE
            Value vv = std::move(v);
            assertm("V MOVED",
                v.type == ValueType::Null
            );
            assertm("Return from Global Function Sanity Check", vv.get_num().n == 11);

            // NOTE: MAKING SURE THERE IS NOT COMPILE-TIME ERRORS HERE
            Value vvv = vv;
            assertm("Number Value(const Value&)", vvv.get_num().n == 11);

        }

        /*
g++ tests/api/main.cpp -o out.out -I ~/Project -I./ -Wunreachable-code -Wold-style-cast -Wall -Werror --std=c++11 -O1 && ./out.out
In file included from tests/api/main.cpp:15:
In copy constructor ‘DeclAPI::Boolean::Boolean(const DeclAPI::Boolean&)’,
    inlined from ‘DeclAPI::Boolean DeclAPI::Value::_get_boolean() const’ at ./decl/lib/DeclAPI.hpp:480:24,
    inlined from ‘int main()’ at tests/api/main.cpp:128:13:
./decl/lib/DeclAPI.hpp:241:53: error: ‘vv.DeclAPI::Value::<unnamed>.DeclAPI::Value::._anon_97::boolean.DeclAPI::Boolean::_bool’ may be used uninitialized [-Werror=maybe-uninitialized]
  241 |         Boolean(const Boolean& other) : _bool{other._bool} {}
      |                                               ~~~~~~^~~~~
tests/api/main.cpp: In function ‘int main()’:
tests/api/main.cpp:124:19: note: ‘vv’ declared here
  124 |             Value vv = std::move(v);
      |                   ^~
In copy constructor ‘DeclAPI::Boolean::Boolean(const DeclAPI::Boolean&)’,
    inlined from ‘DeclAPI::Boolean DeclAPI::Value::_get_boolean() const’ at ./decl/lib/DeclAPI.hpp:480:24,
    inlined from ‘int main()’ at tests/api/main.cpp:136:13:
./decl/lib/DeclAPI.hpp:241:53: error: ‘vvv.DeclAPI::Value::<unnamed>.DeclAPI::Value::._anon_97::boolean.DeclAPI::Boolean::_bool’ may be used uninitialized [-Werror=maybe-uninitialized]
  241 |         Boolean(const Boolean& other) : _bool{other._bool} {}
      |                                               ~~~~~~^~~~~
tests/api/main.cpp: In function ‘int main()’:
tests/api/main.cpp:135:19: note: ‘vvv’ declared here
  135 |             Value vvv = vv;
      |                   ^~~
cc1plus: all warnings being treated as errors
make: *** [Makefile:11: run_api_tests] Error 1
*/
        /*
        // NOTE: We are testing this since we had some trobule testing plain Boolean Type so we need to wrap it inside of a function pointer so that the compiler can't detect move constructor by deep analysis at compile time
        // ALL FAILING AT COMPILE TIME
        TEST_BLOCK("Decl API Basic Global Function Call | function foo_bool | Boolean Value(const& Value)") {
            num_type n = 11;

            Value v = callFunc("CALL FOO_BOOL LINE 74", context->get_global_function(1), { makeNumber(n), } );


            assertm(
                "Return from Global Function Sanity Check", 
                // v.type == ValueType::Boolean &&  // THIS CHECK (ASSERTION OR THROW) IS REQUIRED. OTHERWISE, WE GET COMPILE TIME ERROR THAT THE ANONYMOUS UNION MAY BE UNINITIALIZED
                v._get_boolean()._bool == true);

            
            // NOTE: MAKING SURE THERE IS NOT COMPILE-TIME ERRORS HERE
            Value vv = std::move(v);
            // assertm("V MOVED",
            //    v.type == ValueType::Null
            // );
            assertm(
                "Return from Global Function Sanity Check", 
                vv._get_boolean()._bool == true);

            // assertm("Return from Global Function Sanity Check", vv.get_boolean()._bool == true);

            // NOTE: MAKING SURE THERE IS NOT COMPILE-TIME ERRORS HERE
            Value vvv = vv;
            assertm(
                "Return from Global Function Sanity Check", 
                vvv._get_boolean()._bool == true);
           //  assertm("Boolean Value(const Value&)", vvv.get_boolean()._bool == true);

        }
        */

        // NOTE: We are testing this since we had some trobule testing plain Boolean Type so we need to wrap it inside of a function pointer so that the compiler can't detect move constructor by deep analysis at compile time
        TEST_BLOCK("Decl API Basic Global Function Call | function foo_bool | Boolean Value(const& Value)") {
            num_type n = 11;

            Value v = callFunc("CALL FOO_BOOL LINE 74", context->get_global_function(1), { makeNumber(n), } );


            assertm(
                "Return from Global Function Sanity Check", 
                v.get_boolean()._bool == true);

            
            // NOTE: MAKING SURE THERE IS NOT COMPILE-TIME ERRORS HERE
            Value vv = std::move(v);

            assertm("V MOVED",
                v.type == ValueType::Null
            );

            assertm("Return from Global Function Sanity Check", vv.get_boolean()._bool == true);

            // NOTE: MAKING SURE THERE IS NOT COMPILE-TIME ERRORS HERE
            Value vvv = vv;
            assertm("Boolean Value(const Value&)", vvv.get_boolean()._bool == true);

        }

        TEST_BLOCK("Decl API Basic Global Function Call") {
            num_type n = 13;

            Value v = callFunc("CALL FOO LINE 158", context->get_global_function(2), { makeNumber(n), } );


            // TODO: WRITE THE TEST FOR SUBSCRIPT_GET AND SUBSCRIPT_STORE
            assertm("Return from Global Function Sanity Check", 
                v.type == ValueType::Array &&
                v._cast_array_ptr()->arr.at(0).get_num().n == n
            );

            assertm("Return from Global Function Sanity Check", 
                v.type == ValueType::Array &&
                v._cast_array_ptr()->arr.at(0).get_num().n == 13
            );

            
            // NOTE: MAKING SURE THERE IS NOT COMPILE-TIME ERRORS HERE
            Value vv = std::move(v);

            assertm("V MOVED",
                v.type == ValueType::Null
            );

            assertm("Return from Global Function Sanity Check", 
                vv.type == ValueType::Array &&
                vv._cast_array_ptr()->arr.at(0).get_num().n == 13
            );

            // NOTE: MAKING SURE THERE IS NOT COMPILE-TIME ERRORS HERE
            Value vvv = vv;
            assertm("Array Value(const Value&)",
                vvv.type == ValueType::Array &&
                vvv._cast_array_ptr()->arr.at(0).get_num().n == 13
            );

        }

    }

    TEST_SECTION("Basic Tests")
    {

        std::cout << "sizeof(SharedPtr) " << sizeof(SharedPtr) << std::endl;
        std::cout << "sizeof(Number) " <<  sizeof(Number) << std::endl;
        std::cout << "sizeof(ObjBase) " << sizeof(ObjBase) << std::endl;
        std::cout << "sizeof(ObjArray) " << sizeof(ObjArray) << std::endl;
        std::cout << "sizeof(ObjHashTable) " << sizeof(ObjHashTable) << std::endl;
        std::cout << "sizeof(ObjFunction) " << sizeof(ObjFunction) << std::endl;
        std::cout << "sizeof(FunctionBind)" << sizeof(FunctionBind) << std::endl;
        std::cout << "sizeof(Value) " << sizeof(Value) << std::endl;

        TEST_BLOCK("DeclAPI ObjArray") {
            {
                ObjArray* arr_ptr = main_context->makeObjArray();

                assertm(
                    "MAKE SURE THE ObjBase IS ONLY REF-INCEREMENTED ONCE PICKED UP BY SharedPtr",
                    arr_ptr->ref_count() == 0
                );

                delete arr_ptr;
            }

            {
                ObjString* str_ptr = main_context->makeObjString(String("1234"));

                assertm(
                    "MAKE SURE THE ObjBase IS ONLY REF-INCEREMENTED ONCE PICKED UP BY SharedPtr",
                    str_ptr->ref_count() == 0
                );

                delete str_ptr;
            }
        }

        TEST_BLOCK("DeclAPI SharedPtr Basic Test | Derived Shared Objects destruction OK")
        {

            {
                SharedPtr arr = main_context->makeSharedArray();

                assertm(
                    "MAKE SURE THE ObjBase IS ONLY REF-INCEREMENTED ONCE PICKED UP BY SharedPtr",
                    arr.get_ptr()->ref_count() == 1
                );


                ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());

                for(num_type i = 0; i < 10; i++) {
                    arr_ptr->push(makeNumber(i));
                }

                assertm("ObjArray:: Context ATTACHED", arr_ptr->context != nullptr);

                for(num_type i = 0; i < 10; i++) {
                    // std::cout << arr_ptr->at(size_t(i)).get_num().n  << std::endl;
                    assertm("Array::at Sanity Check", arr_ptr->at(size_t(i)).get_num().n == i);
                }

            }


        }

        TEST_BLOCK("DeclAPI SharedPtr Basic Test | Self Referencing")
        {
        
            SharedPtr arr = main_context->makeSharedArray();

            assertm(
                "ObjBase Ref Count 1",
                arr.get_ptr()->ref_count() == 1
            );

            arr = arr;

            assertm("SharedPtr NULL Check", arr.get_ptr() != nullptr);

            assertm(
                "ObjBase Ref Count 1",
                arr.get_ptr()->ref_count() == 1
            );

            ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());
            arr_ptr->arr.push_back(makeNumber(221));

            arr = arr;

            assertm(
                "ObjBase Ref Count 1",
                arr.get_ptr()->ref_count() == 1
            );

            assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);

            /*
            TEST_BLOCK("SharedPtr | Self Referencing Move") {

                SharedPtr arr = main_context->makeSharedArray();

                assertm(
                    "ObjBase Ref Count 1",
                    arr.get_ptr()->ref_count() == 1
                );
                // NOTE: STOPPED BY THE COMPILER ANYWAY
                // tests/api/main.cpp:361:21: error: moving ‘arr’ of type ‘DeclAPI::SharedPtr’ to itself [-Werror=self-move]
                //  361 |                 arr = std::move(arr);
                arr = std::move(arr);

                assertm("SharedPtr NULL Check", arr.get_ptr() != nullptr);

                assertm(
                    "ObjBase Ref Count 1",
                    arr.get_ptr()->ref_count() == 1
                );

                ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());
                arr_ptr->arr.push_back(makeNumber(221));


                // NOTE: STOPPED BY THE COMPILER ANYWAY
                // tests/api/main.cpp:361:21: error: moving ‘arr’ of type ‘DeclAPI::SharedPtr’ to itself [-Werror=self-move]
                //  361 |                 arr = std::move(arr);
                arr = std::move(arr);

                assertm(
                    "ObjBase Ref Count 1",
                    arr.get_ptr()->ref_count() == 1
                );

                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
            }
            */

        }

        TEST_BLOCK("DeclAPI SharedPtr Basic Test | Shared Ref Scope OK")
        {
        
            SharedPtr arr = main_context->makeSharedArray();

            {

                SharedPtr arr_scoped = arr;

                ObjArray* arr_ptr = obj_cast<ObjArray*>(arr_scoped.get_ptr());

                arr_ptr->push(makeNumber(221));

                assertm("SharedArray Same Pointer Reference OK", arr.get_ptr() == arr_scoped.get_ptr());

                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
                assertm("Array SIZE MUST BE 1", arr_ptr->arr.size() == 1);

            }
            
            ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());
            assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
            assertm("Array SIZE MUST BE 1", arr_ptr->arr.size() == 1);


        }

        TEST_BLOCK("DeclAPI SharedPtr Basic Test | Move Assignment OK")
        {
        
            SharedPtr arr = main_context->makeSharedArray();

            for(int i = 0; i < 2; i++) {
                arr = main_context->makeSharedArray();
                ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());
                arr_ptr->push(makeNumber(221));
            }

            ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());
            assertm("Array::at Sanity Check", arr_ptr->at(size_t(0)).get_num().n == 221);

        }

        TEST_BLOCK("DeclAPI SharedPtr Basic Test | Copy Assignment OK")
        {
        
            SharedPtr arr = main_context->makeSharedArray();

            for(int i = 0; i < 2; i++) {
                SharedPtr my_arr = main_context->makeSharedArray();
                arr = my_arr;
                ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());
                arr_ptr->arr.push_back(makeNumber(221));
            }

            ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());
            assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);

        }

        TEST_BLOCK("DeclAPI SharedPtr Basic Test | Object MOVED OK")
        {
        
            SharedPtr arr = main_context->makeSharedArray();

            SharedPtr my_arr = main_context->makeSharedArray();
            arr = std::move(my_arr);

            ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());
            arr_ptr->arr.push_back(makeNumber(221));

            assertm("MOVED SharedPtr NULL Check", my_arr.get_ptr() == nullptr);

            assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);

        }

        TEST_BLOCK("DeclAPI::Value Basic Test | Derived Shared Objects destruction OK")
        {

            {
                SharedPtr arr = main_context->makeSharedArray();


                ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());

                for(num_type i = 0; i < 10; i++) {
                    arr_ptr->arr.push_back(makeNumber(i));
                }

                assertm("Array::size Sanity Check", arr_ptr->arr.size() == 10);

                for(num_type i = 0; i < 10; i++) {
                    // std::cout << arr_ptr->at(size_t(i)).get_num().n  << std::endl;
                    assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(i)).get_num().n == i);
                }

            }


        }


        TEST_BLOCK("DeclAPI::Value Basic Test | Value OK")
        {

            {
                Value arr = main_context->makeArray();


                ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.obj.get_ptr());

                assertm("Value Array::Context ATTACHED", arr_ptr->context != nullptr);

                for(num_type i = 0; i < 10; i++) {
                    arr_ptr->arr.push_back(makeNumber(i));
                }

                assertm("Array::size Sanity Check", arr_ptr->arr.size() == 10);

                for(num_type i = 0; i < 10; i++) {
                    // std::cout << arr_ptr->at(size_t(i)).get_num().n  << std::endl;
                    assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(i)).get_num().n == i);
                }

            }


        }

        TEST_BLOCK("DeclAPI::Value Basic Test | Basic Number Value OK")
        {


            TEST_BLOCK("DeclAPI::Value::Number Basic Test | Copy Assignment and Constructor OK | Value(Value&), Value& operator=(const Value&) and operator=(Number&&)")
            {
                Value num = makeNumber(10.3);
                Value num1 = makeNumber(20);
                Value num2 = num1;


                assertm("Value Number OK", 
                    num.type == ValueType::Number &&
                    num.get_num().n == 10.3);

                num = makeNumber(11);

                assertm("Value new Number OK", 
                    num.type == ValueType::Number &&
                    num.get_num().n == 11);

                num = num1;

                assertm("Value Number ASSIGNED OK", 
                    num.type == ValueType::Number &&
                    num.get_num().n == 20);
                
                assertm("Value Number ASSIGNED OK", 
                    num2.type == ValueType::Number &&
                    num2.get_num().n == 20);

                // Compare to the "Value::SharedArray Same Pointer Reference OK" assertion below
                // Primitives should always be different memory address as there is no reference counter assigned to the same

                assertm("Value Type OK", num.type == ValueType::Number);
                assertm("Value Type OK", num1.type == ValueType::Number);
                assertm("Value Type OK", num2.type == ValueType::Number);
                assertm("Different memory address for Primitives OK", &num.num != &num1.num);
                assertm("Different memory address for Primitives OK", &num1.num != &num2.num);
            }

            TEST_BLOCK("DeclAPI::Value::Number Basic Test | Shared Object Released | Copy Assignment OK | Value& operator=(const Value&)")
            {
                Value num = makeNumber(10.3);
                Value arr = main_context->makeArray();

                arr = num;

                assertm("Value Number OK", 
                    num.type == ValueType::Number &&
                    num.get_num().n == 10.3);

                assertm("Value Number ASSIGNED OK", 
                    arr.type == ValueType::Number &&
                    arr.get_num().n == 10.3);


            }

            // Inner TEST BLOCK FOR THE RESPECTIVE VALUE TYPE FOR THE C++ STANDARD FOR USING Value AS A C++ API OUTSIDE OF THE DECL
            TEST_BLOCK("C++ Standard TEST SUITE") {

                TEST_BLOCK("DeclAPI::Value::Number Basic Test | Move Constructor OK | Value(Value&&)")
                {
        
                    Value num = makeNumber(19);

                    Value num_m = std::move(num);

                    assertm("MOVED Value TYPE NULL Check", num.type == ValueType::Null);

                    assertm("ValueType::Number Check OK", num_m.type == ValueType::Number);
                    assertm("Value Number Sanity Check", num_m.get_num().n == 19);
                }

                TEST_BLOCK("DeclAPI::Value Basic Test | Move Assignment OK | Value& operator=(Value&&)")
                {
                    Value num = makeNumber(19);

                    Value num_m = makeNumber(20);

                    num = std::move(num_m);

                    assertm("MOVED Value TYPE NULL Check", num_m.type == ValueType::Null);

                    assertm("ValueType::Number Check OK", num.type == ValueType::Number);
                    assertm("Value Number Sanity Check", num.get_num().n == 20);
                }

            }

        }

        TEST_BLOCK("DeclAPI::Value Basic Test | Basic Boolean Value OK")
        {

            TEST_BLOCK("DeclAPI::Value::Boolean Basic Test | Copy Assignment OK | Value(const Value&), Value& operator=(const Value&) and operator=(Boolean&&)")
            {
                Value boolean = makeBoolean(true);
                Value boolean1 = makeBoolean(true);


                assertm("Value Boolean OK", 
                    boolean.get_boolean()._bool == true);

                boolean = makeBoolean(false);

                assertm("Value Boolean FALSE", 
                    boolean.get_boolean()._bool == false);

                boolean = boolean1;

                assertm("Value Boolean ASSIGNED OK", 
                    boolean.get_boolean()._bool == true);

                
                Value boolean_true = makeBoolean(true);
                Value boolean2 = boolean_true;


                /*
                {
                    // FIXME: VERY BIZZARE BUG HERE WHEN WE SAY:  case ValueType::Boolean: { break; } for the copy constructor
                    // Value _BB;

                    // std::cout << "START: _BB" << std::endl;
                    // std::cout << _BB.boolean._bool << std::endl;
                    // std::cout << &_BB.boolean << std::endl;
                    // std::cout << "END: _BB" << std::endl;
                }
                */

                assertm("Value Boolean ASSIGNED OK", 
                    boolean2.get_boolean()._bool == true);   

                // REQUIRED: DUE TO THE COMPILE TIME ERROR THAT THE ANONYMOUS UNION MAY BE UNINITIALIZED
                assertm("Value Type OK", boolean.type == ValueType::Boolean);
                assertm("Value Type OK", boolean1.type == ValueType::Boolean);
                assertm("Value Type OK", boolean_true.type == ValueType::Boolean);
                assertm("Value Type OK", boolean2.type == ValueType::Boolean);
                // Compare to the "Value::SharedArray Same Pointer Reference OK" assertion below
                // Primitives should always be different memory address as there is no reference counter assigned to the same
                assertm("Different memory address for Primitives OK", &boolean.boolean != &boolean1.boolean);
                assertm("Different memory address for Primitives OK", &boolean_true.boolean != &boolean2.boolean);

            }


            TEST_BLOCK("DeclAPI::Value::Boolean Basic Test | Shared Object Released | Copy Assignment OK | Value& operator=(const Value&)")
            {
                Value boolean = makeBoolean(true);
                Value arr = main_context->makeArray();

                arr = boolean;

                assertm("Value Boolean OK", 
                    boolean.get_boolean()._bool == true);

                assertm("Value Boolean ASSIGNED OK", 
                    boolean.get_boolean()._bool == true);


            }

            // Inner TEST BLOCK FOR THE RESPECTIVE VALUE TYPE FOR THE C++ STANDARD FOR USING Value AS A C++ API OUTSIDE OF THE DECL
            TEST_BLOCK("C++ Standard TEST SUITE") {

                TEST_BLOCK("DeclAPI::Value::Boolean Basic Test | Move Constructor OK | Value(Value&&)")
                {
        
                    Value boolean = makeBoolean(true);

                    Value boolean_m = std::move(boolean);

                    assertm("MOVED Value TYPE NULL Check", boolean.type == ValueType::Null);

                    assertm("ValueType::Boolean Check OK", boolean_m.type == ValueType::Boolean);
                    assertm("Value Boolean Sanity Check", boolean_m.get_boolean()._bool == true);
                }

                TEST_BLOCK("DeclAPI::Value Basic Test | Move Assignment OK | Value& operator=(Value&&)")
                {
                    Value boolean = makeBoolean(false);

                    Value boolean_m = makeBoolean(true);

                    boolean = std::move(boolean_m);

                    assertm("MOVED Value TYPE NULL Check", boolean_m.type == ValueType::Null);

                    assertm("ValueType::Boolean Check OK", boolean.type == ValueType::Boolean);
                    assertm("Value Boolean Sanity Check", boolean.get_boolean()._bool == true);
                }

            }


        }

        TEST_BLOCK("Value:Array | Tests") {

            TEST_BLOCK("DeclAPI::Value Basic Test | Value Scope OK and Copy Constructor OK | Value& operator=(const Value&)")
            {

                Value arr = main_context->makeArray();

                assertm("ValueType::Array Check OK", arr.type == ValueType::Array);

                {
                    // Copy Constructor OK
                    Value arr_scoped = arr;

                    ObjArray *arr_ptr = obj_cast<ObjArray *>(arr_scoped.obj.get_ptr());

                    assertm("Value::SharedArray Same Pointer Reference OK", arr.obj.get_ptr() == arr_scoped.obj.get_ptr());

                    arr_ptr->arr.push_back(makeNumber(221));
                    assertm("Array::at Sanity Check", arr_ptr->arr.at(0).get_num().n == 221);
                }

                assertm("Value SharedArray COPED Check", arr.obj.get_ptr() != nullptr);

                ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                assertm("Array::at Sanity Check", arr_ptr->arr.at(0).get_num().n == 221);
            }

            TEST_BLOCK("DeclAPI::Value Basic Test | Move Constructor OK | Value(Value&&)")
            {

                Value arr = main_context->makeArray();

                Value arr_m = std::move(arr);

                assertm("ValueType::Array Check OK", arr_m.type == ValueType::Array);

                ObjArray *arr_ptr = obj_cast<ObjArray *>(arr_m.obj.get_ptr());
                arr_ptr->arr.push_back(makeNumber(221));

                assertm("MOVED Value TYPE NULL Check", arr.type == ValueType::Null);
                assertm("Value SharedArray MOVED Check", arr.obj.get_ptr() == nullptr);

                assertm("Array::at Sanity Check", arr_ptr->arr.at(0).get_num().n == 221);
            }

            TEST_BLOCK("DeclAPI::Value Basic Test | Move Assignment OK | Value& operator=(Array&&)")
            {

                Value arr = main_context->makeArray();

                for (int i = 0; i < 2; i++)
                {
                    arr = main_context->makeArray();
                    ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                    arr_ptr->arr.push_back(makeNumber(221));
                }

                ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
            }

            TEST_BLOCK("DeclAPI::Value Basic Test | Move Assignment LEAVES ValueType::Null | Value& operator=(Value&&)")
            {

                Value arr = main_context->makeArray();

                for (int i = 0; i < 2; i++)
                {
                    Value arr02 = main_context->makeArray();
                    arr = std::move(arr02);

                    assertm("Array arr02 MOVED CHECK | ptr == nullptr", arr02.obj.get_ptr() == nullptr);
                    assertm("Array arr02 MOVED CHECK | ValueType::Null", arr02.type == ValueType::Null);

                    ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                    arr_ptr->arr.push_back(makeNumber(221));
                }

                ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
            }

            TEST_BLOCK("DeclAPI::Value Basic Test | Copy Assignment OK | Value& operator=(const Value&)")
            {

                Value arr = main_context->makeArray();

                for (int i = 0; i < 2; i++)
                {
                    Value my_arr = main_context->makeArray();
                    arr = my_arr;
                    assertm("Value Object: Same Reference CHECK", arr.obj.get_ptr() == my_arr.obj.get_ptr());
                    ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                    arr_ptr->arr.push_back(makeNumber(221));
                }

                ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
            }

            TEST_BLOCK("DeclAPI::Value Basic Test | Self Referencing OK")
            {

                Value arr = main_context->makeArray();
                ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());

                for (int i = 0; i < 5; i++)
                {
                    arr = arr;
                    ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                    arr_ptr->arr.push_back(makeNumber(221));
                }

                assertm("Array SIZE MUST BE 5", arr_ptr->arr.size() == 5);
                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
            }

            TEST_BLOCK("DeclAPI::Value Basic Test | Value MOVE to Primitive, Number | Value& operator=(Number&&)")
            {

                Value arr;

                for (int i = 0; i < 2; i++)
                {
                    arr = main_context->makeArray();
                    ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                    arr_ptr->arr.push_back(makeNumber(221));
                    assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);

                    arr = makeNumber(11);

                    assertm("Value holds the number", arr.get_num().n == 11);
                }

                // Back to Array
                arr = main_context->makeArray();
                ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                arr_ptr->arr.push_back(makeNumber(221));
                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
            }

            TEST_BLOCK("DeclAPI::Value | Value MOVE to VALUE Primitive, Number | Value& operator=(Value&&)")
            {

                Value arr;

                for (int i = 0; i < 2; i++)
                {
                    arr = main_context->makeArray();
                    ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                    arr_ptr->arr.push_back(makeNumber(221));
                    assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);

                    Value n_value = makeNumber(11);
                    arr = std::move(n_value);

                    assertm("Value holds the number", arr.get_num().n == 11);
                }

                // Back to Array
                arr = main_context->makeArray();
                ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                arr_ptr->arr.push_back(makeNumber(221));
                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
            }

            TEST_BLOCK("DeclAPI::Value Basic Test | MOVE assignment Array to Value | Value& operator=(Array&&), Value& operator=(Number&&)")
            {

                Value arr;

                arr = main_context->makeArray();

                assertm("ValueType::Array Check OK", arr.type == ValueType::Array);

                ObjArray *arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());

                arr_ptr->arr.push_back(makeNumber(221));
                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);

                arr = makeNumber(11);

                assertm("ValueType::Number Check OK", arr.type == ValueType::Number);
                assertm("Value holds the number", arr.get_num().n == 11);

                // Back to Array
                arr = main_context->makeArray();
                assertm("ValueType::Array Check OK", arr.type == ValueType::Array);

                arr_ptr = obj_cast<ObjArray *>(arr.obj.get_ptr());
                arr_ptr->arr.push_back(makeNumber(221));
                assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);
            }
        }


        TEST_BLOCK("DeclAPI::Value Basic Test | Member Access OK") {
            Value arr = main_context->makeArray();

            ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.obj.get_ptr());


            size_t push_sym_offset = main_context->insert_if_not_and_get_symbol_offset("push"); 

            Value v_function_binding = getMember("LINE 429", arr, cacl_member_offset(push_sym_offset));


            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                v_function_binding.type == ValueType::FunctionBind
            );

            // NOTE: USING CONST REF& TO PREVENT ref_count() == 3
            const FunctionBind& function_binding = v_function_binding.function_bind;

            assertm(
                "Array::size Empty Check",
                arr_ptr->arr.size() == 0
            );

            // Logger::info(function_binding.obj.get_ptr());

            assertm(
                "OBJ ACCURATELY ASSIGNED",
                function_binding.obj.get_ptr() == arr_ptr &&
                function_binding.obj.get_ptr() == arr.obj.get_ptr()
            );

            assertm(
                "OBJ REF COUNT CHECK",
                function_binding.obj.get_ptr()->ref_count() == 2
            );

            assertm(
                "FUNCTION EXISTS/ASSIGNED SANITY CHECK",
                // function_binding.type == ValueType::FunctionBind &&
                function_binding.function_bind_p == MemberFunctionPointers::array_push
            );


            Value r_value = callFunc("LINE 445", function_binding, { makeNumber(22), });

            assertm(
                "MEMBER FUNCTION RETURN OK",
                r_value.type == ValueType::Null
            );

            assertm(
                "Array::size NOW 1 Check",
                arr_ptr->arr.size() == 1
            );

            // DECL SOURCE: arr.push(23);
            callFunc("LINE 464", 
                getMember("LINE 464", arr, cacl_member_offset(push_sym_offset)),
                { makeNumber(23), }
            );

            callFunc("LINE 464", 
                getMember("LINE 464", arr, main_context->insert_if_not_and_get_symbol_offset("push") ),
                { makeNumber(24), }
            );
            /*
            // DECL SOURCE: arr.push(24);
            callFunc("LINE 464", 
                getMember("LINE 464", arr, "push"),
                { makeNumber(24), }
            );
            */

            assertm(
                "Array::size NOW 2 Check",
                arr_ptr->arr.size() == 3
            );

            assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 22);
            assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(1)).get_num().n == 23);
            assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(2)).get_num().n == 24);

            TEST_BLOCK("SWAPPING DECL VALUES OK")
            {
                Value v_function_binding1 = makeNumber(11);
                // Value v_function_binding2 = std::move(v_function_binding);
                Value v_function_binding = getMember("LINE 429", arr, cacl_member_offset(push_sym_offset));
                // Value v_function_binding1; // = getMember("LINE 429", arr, cacl_member_offset(push_sym_offset));


                std::swap(v_function_binding1, v_function_binding);

                assertm(
                    "IS FUNCTION BINDING ASSSIGNED",
                    v_function_binding1.type == ValueType::FunctionBind
                );

                assertm(
                    "FUNCTION EXISTS/ASSIGNED SANITY CHECK",
                    v_function_binding1.type == ValueType::FunctionBind &&
                    v_function_binding1.function_bind.obj.get_ptr() == arr_ptr &&
                    v_function_binding1.function_bind.obj.get_ptr() == arr.obj.get_ptr() &&
                    v_function_binding1.function_bind.function_bind_p == MemberFunctionPointers::array_push
                );

                assertm("Value::number swapped",
                    v_function_binding.type == ValueType::Number &&
                    v_function_binding.get_num().n == 11
                );


                TEST_BLOCK("CUSTOM SWAP | SWAP BACK") {

                    Value tmp = std::move(v_function_binding1);

                    v_function_binding1 = std::move(v_function_binding);

                    v_function_binding = std::move(tmp);

                    assertm(
                        "IS FUNCTION BINDING ASSSIGNED",
                        v_function_binding.type == ValueType::FunctionBind
                    );

                    assertm(
                        "FUNCTION EXISTS/ASSIGNED SANITY CHECK",
                        v_function_binding.type == ValueType::FunctionBind &&
                        v_function_binding.function_bind.obj.get_ptr() == arr_ptr &&
                        v_function_binding.function_bind.obj.get_ptr() == arr.obj.get_ptr() &&
                        v_function_binding.function_bind.function_bind_p == MemberFunctionPointers::array_push
                    );

                    assertm("Value::number swapped",
                        v_function_binding1.type == ValueType::Number &&
                        v_function_binding1.get_num().n == 11
                    );

                }

                // OK
                {
                    Value a;
                    Value b;
                    std::swap(a,b);
                }

                /*
                {
                    hash_map<size_t, Value> map;

                    map[0] = FunctionBind();
                }
                */

            }


        }

        TEST_BLOCK("DeclAPI::Value Basic Test | Basic FunctionBind Value OK") {
            Value arr = main_context->makeArray();

            ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.obj.get_ptr());

            // NOTE: BE AWARE THAT THIS LOOKUP WILL CREATE THE DEFAULT ENTRY IF NOT FOUND BUT WE SHOULD BE SAFE HERE AS THE SIZE_T VALUES DEFAULTS TO 0 AND THE HASH INCREMENTAL DEFAULTS TO 1
            size_t push_sym_offset = main_context->insert_if_not_and_get_symbol_offset("push"); 

            Value v_function_binding = getMember("LINE 429", arr, cacl_member_offset(push_sym_offset));


            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                v_function_binding.type == ValueType::FunctionBind
            );

            assertm(
                "ARR OBJ REF COUNT 2",
                arr_ptr->ref_count() == 2
            );

            // Value::FunctionBind Copy Constructor
            Value v1_function_binding = v_function_binding; 

            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                v1_function_binding.type == ValueType::FunctionBind &&
                v1_function_binding.function_bind.obj.get_ptr() == v_function_binding.function_bind.obj.get_ptr() &&
                v1_function_binding.function_bind.function_bind_p == v_function_binding.function_bind.function_bind_p
            );


            assertm(
                "ARR OBJ REF COUNT 3",
                arr_ptr->ref_count() == 3
            );

            Value v2_function_binding;

            // Value::FunctionBind Copy Assignment
            v2_function_binding = v1_function_binding;

            assertm(
                "ARR OBJ REF COUNT 4",
                arr_ptr->ref_count() == 4
            );

            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                v2_function_binding.type == ValueType::FunctionBind &&
                v1_function_binding.function_bind.obj.get_ptr() == v2_function_binding.function_bind.obj.get_ptr() &&
                v1_function_binding.function_bind.function_bind_p == v2_function_binding.function_bind.function_bind_p
            );

            Value v3_function_binding = v2_function_binding;

            // Value::FunctionBind Copy Assignment & ~FunctionBind() Cleaned Up: This is not an assertion - it is detected by valgrind or similar
            v3_function_binding = v2_function_binding;

            assertm(
                "ARR OBJ REF COUNT 5 AFTER COPY ASSIGNMENT",
                arr_ptr->ref_count() == 5
            );


            // Value::FunctionBind Move Assignment & ~FunctionBind() Cleaned Up: This is not an assertion - it is detected by valgrind or similar
            Value v4_function_binding = v3_function_binding;

            // Value::FunctionBind Move Assignment
            v4_function_binding = std::move(v2_function_binding);

            assertm(
                "IS MOVED FUNCTION BINDING NULL",
                v2_function_binding.type == ValueType::Null
            );

            // Logger::info(arr_ptr->ref_count());

            assertm(
                "ARR OBJ REF COUNT REMAINS 5 as v2_function_binding is now moved",
                arr_ptr->ref_count() == 5
            );

            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                v4_function_binding.type == ValueType::FunctionBind &&
                v1_function_binding.function_bind.obj.get_ptr() == v4_function_binding.function_bind.obj.get_ptr() &&
                v1_function_binding.function_bind.function_bind_p == v4_function_binding.function_bind.function_bind_p
            );

            // Value::FunctionBind Move Constructor
            Value v5_function_binding = std::move(v4_function_binding);

            assertm(
                "IS MOVED FUNCTION BINDING NULL",
                v4_function_binding.type == ValueType::Null
            );


            assertm(
                "ARR OBJ REF COUNT REMAINS 5 as v4_function_binding is now moved",
                arr_ptr->ref_count() == 5
            );

            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                v5_function_binding.type == ValueType::FunctionBind &&
                v1_function_binding.function_bind.obj.get_ptr() == v5_function_binding.function_bind.obj.get_ptr() &&
                v1_function_binding.function_bind.function_bind_p == v5_function_binding.function_bind.function_bind_p
            );


            v5_function_binding = makeBoolean(true);

            assertm("Value Boolean OK", 
                v5_function_binding.type == ValueType::Boolean &&
                v5_function_binding.get_boolean()._bool == true);

            assertm(
                "ARR OBJ REF COUNT 4",
                arr_ptr->ref_count() == 4
            );

            v1_function_binding = makeNumber(12);

            assertm("Value Number ASSIGNED OK", 
                v1_function_binding.type == ValueType::Number &&
                v1_function_binding.get_num().n == 12);

            assertm(
                "ARR OBJ REF COUNT 3",
                arr_ptr->ref_count() == 3
            );

        }

        TEST_BLOCK("DeclAPI::Value Basic Test | Basic String Value OK") {
            TEST_BLOCK("BASIC TESTS") {
                Value str = main_context->makeString(String("1234"));
                Value str1 = main_context->makeString(String("12345"));

                #define _cast_to_obj_str(v) v._cast_string_ptr()

                assertm(
                    "Value String ASSIGNED",
                    str.type == ValueType::String &&
                    _cast_to_obj_str(str)->str._str == "1234"
                );

                assertm(
                    "Value String ASSIGNED",
                    str1.type == ValueType::String &&
                    _cast_to_obj_str(str1)->str._str == "12345"
                );

                str = str1;

                assertm(
                    "Strings DO NOT SHARE AND ARE NOT OF THE SAME REF | THEY ARE TREATED AS PRIMIVES AND ARE COPIED",
                    _cast_to_obj_str(str) != _cast_to_obj_str(str1)
                );
                assertm(
                    "Value String Copy Assigment=",
                    _cast_to_obj_str(str)->str._str == "12345"
                );

                Value str_2 = main_context->makeString(std::string("123"));
                Value str_3 = str_2;

                assertm(
                    "ValueType ASSIGNED",
                    str_2.type == ValueType::String &&
                    str_3.type == ValueType::String
                );

                assertm(
                    "Context ATTACHED",
                    _cast_to_obj_str(str_2)->context != nullptr &&
                    _cast_to_obj_str(str_3)->context != nullptr
                );

                assertm(
                    "Strings DO NOT SHARE AND ARE NOT OF THE SAME REF | THEY ARE TREATED AS PRIMIVES AND ARE COPIED",
                    _cast_to_obj_str(str_2) != _cast_to_obj_str(str_3)
                );
                assertm(
                    "Value Strings Content MATCHES",
                    _cast_to_obj_str(str_2)->str._str == _cast_to_obj_str(str_3)->str._str
                );

            }

            TEST_BLOCK("DeclAPI::Value::String Basic Test | Shared Object Released | Copy Assignment OK | Value& operator=(const Value&)")
            {
                Value str = main_context->makeString(String("12345"));;
                Value arr = main_context->makeArray();

                arr = str;

                /*
                Logger::info(
                    _cast_to_obj_str(arr)->context
                );
                */

                assertm(
                    "Context ATTACHED",
                    _cast_to_obj_str(arr)->context != nullptr &&
                    _cast_to_obj_str(str)->context != nullptr
                );

                assertm(
                    "Value String ASSIGNED",
                    str.type == ValueType::String &&
                    _cast_to_obj_str(str)->str._str == "12345"
                );

                assertm(
                    "Value String ASSIGNED",
                    arr.type == ValueType::String &&
                    _cast_to_obj_str(arr)->str._str == "12345"
                );

                assertm(
                    "Strings DO NOT SHARE AND ARE NOT OF THE SAME REF | THEY ARE TREATED AS PRIMIVES AND ARE COPIED",
                    _cast_to_obj_str(str) != _cast_to_obj_str(arr)
                );


            }

            TEST_BLOCK("DeclAPI::Value::String | operator=(Boolean&&)") {
                Value str = main_context->makeString(String("12345"));;

                str = makeBoolean(true);

                assertm("Value Boolean OK", 
                    str.type == ValueType::Boolean &&
                    str.get_boolean()._bool == true);
            }

            TEST_BLOCK("DeclAPI::Value::String | operator=(Number&&)") {
                Value str = main_context->makeString(String("12345"));;
                str = makeNumber(12);

                assertm("Value Number ASSIGNED OK", 
                    str.type == ValueType::Number &&
                    str.get_num().n == 12);
            }

            TEST_BLOCK("DeclAPI::Value::String | operator=(Array&&)") {
                Value str = main_context->makeString(String("12345"));
                str = main_context->makeArray();

                assertm("Value Array ASSIGNED OK", 
                    str.type == ValueType::Array &&
                    (obj_cast<ObjArray *>(str.obj.get_ptr()))->arr.size() == 0
                );
            }

            // Inner TEST BLOCK FOR THE RESPECTIVE VALUE TYPE FOR THE C++ STANDARD FOR USING Value AS A C++ API OUTSIDE OF THE DECL
            TEST_BLOCK("C++ Standard TEST SUITE") {

                TEST_BLOCK("DeclAPI::Value::String Basic Test | Move Constructor OK | Value(Value&&)")
                {
        
                    Value str = main_context->makeString(String("12345"));

                    Value str_m = std::move(str);

                    assertm(
                        "Context ATTACHED",
                        _cast_to_obj_str(str_m)->context != nullptr
                    );

                    assertm("MOVED Value TYPE NULL Check", str.type == ValueType::Null);

                    assertm("ValueType::String Check OK", str_m.type == ValueType::String);
                    assertm("Value String Sanity Check", _cast_to_obj_str(str_m)->str._str == "12345");
                }

                TEST_BLOCK("DeclAPI::Value Basic Test | Move Assignment OK | Value& operator=(Value&&)")
                {
                    Value str = main_context->makeString(String("12345"));

                    Value str_m = main_context->makeString(String("1234"));

                    str = std::move(str_m);

                    assertm(
                        "Context ATTACHED",
                        _cast_to_obj_str(str)->context != nullptr
                    );

                    assertm("MOVED Value TYPE NULL Check", str_m.type == ValueType::Null);

                    assertm("ValueType::String Check OK", str.type == ValueType::String);
                    assertm("Value String Sanity Check", _cast_to_obj_str(str)->str._str == "1234");
                }

                TEST_BLOCK("DeclAPI::Value Basic Test | Move Assignment XVALUE OK | Value& operator=(Value&&)")
                {
                    Value str = main_context->makeString(String("12345"));


                    str = main_context->makeString(String("1234"));

                    assertm(
                        "Context ATTACHED",
                        _cast_to_obj_str(str)->context != nullptr
                    );


                    assertm("ValueType::String Check OK", str.type == ValueType::String);
                    assertm("Value String Sanity Check", _cast_to_obj_str(str)->str._str == "1234");
                }

            }

            TEST_BLOCK("DeclAPI::Value Basic Test | Member Access OK | Member Invocation for Value::String") {
                Value str = main_context->makeString(String("12345"));

                Value str1 = str;

                ObjString* str_ptr = _cast_to_obj_str(str);

                size_t append_sym_offset = main_context->insert_if_not_and_get_symbol_offset("append");
            
                Value v_function_binding = getMember(
                    "LINE 1025", 
                    str,
                    cacl_member_offset(append_sym_offset)
                );

                // Check out the "C++ | Bypassing the Compiler Part II | Unions Undefined Behavior" video as to why this happens
                /*
                Logger::info(
                    &str,
                    &v_function_binding,
                    v_function_binding.obj.get_ptr(), str_ptr,
                    v_function_binding.obj.get_ptr() == str_ptr
                ); // NOTE: SAME MEMORY ADDRESS!? VERY WEIRD
                assert(false);
                */

                assertm(
                    "IS FUNCTION BINDING ASSSIGNED",
                    v_function_binding.type == ValueType::FunctionBind
                );

                // NOTE: USING CONST REF& TO PREVENT ref_count() == 3
                const FunctionBind& function_binding = v_function_binding.function_bind;


                assertm(
                    "ObjBase RefCount 2 DUE TO THE FUNCTION BIND",
                    str_ptr->ref_count() == 2
                );

                assertm(
                    "OBJ ACCURATELY ASSIGNED | Same ObjBase Reference",
                    function_binding.obj.get_ptr() == str_ptr &&
                    function_binding.obj.get_ptr() == str.obj.get_ptr()
                );

                assertm(
                    "FUNCTION EXISTS/ASSIGNED SANITY CHECK",
                    // function_binding.type == ValueType::FunctionBind &&
                    function_binding.function_bind_p == MemberFunctionPointers::string_append
                );


                Value r_value = callFunc("LINE 1068", function_binding, {
                    main_context->makeString(String("123"))
                });

                assertm(
                    "MEMBER FUNCTION RETURN OK",
                    r_value.type == ValueType::Null
                );

                assertm(
                    "Value String ASSIGNED",
                    str.type == ValueType::String &&
                    _cast_to_obj_str(str)->str._str == "12345123"
                );

                callFunc("LINE 1088", 
                    getMember("LINE 1088", str, main_context->insert_if_not_and_get_symbol_offset("append") ),
                    { main_context->makeString(String("_1")), }
                );

                callFunc("LINE 1088", 
                    getMember("LINE 1088", str1, main_context->insert_if_not_and_get_symbol_offset("append") ),
                    { main_context->makeString(String("_1")), }
                );

                assertm(
                    "Value String ASSIGNED",
                    str.type == ValueType::String &&
                    _cast_to_obj_str(str)->str._str == "12345123_1"
                );

                assertm(
                    "Value String ASSIGNED",
                    str1.type == ValueType::String &&
                    _cast_to_obj_str(str1)->str._str == "12345_1"
                );

                TEST_BLOCK("REFERENCE STILL ACTIVE") {
                    Value str = main_context->makeString(String("12345"));

                    ObjString* str_ptr = _cast_to_obj_str(str);
            
                    Value function_binding = getMember(
                        "LINE 1025", 
                        str,
                        cacl_member_offset(append_sym_offset)
                    );

                    assertm(
                        "IS FUNCTION BINDING ASSSIGNED",
                        function_binding.type == ValueType::FunctionBind
                    );

                    assertm(
                        "ObjBase RefCount 2 DUE TO THE FUNCTION BIND",
                        str_ptr->ref_count() == 2
                    );

                    str = main_context->makeString(String("1234"));

                    assertm(
                        "ObjBase RefCount 1 DUE TO THE FUNCTION BIND",
                        str_ptr->ref_count() == 1
                    );

                    assertm(
                        "Value String ASSIGNED",
                        str.type == ValueType::String &&
                        _cast_to_obj_str(str)->str._str == "1234" &&
                        _cast_to_obj_str(str)->ref_count() == 1
                    );

                    Value r_value = callFunc("LINE 1068", function_binding, {
                        main_context->makeString(String("123"))
                    });

                    assertm(
                        "MEMBER FUNCTION RETURN OK",
                        r_value.type == ValueType::Null
                    );

                    assertm(
                        "Value String PERSISTS",
                        str_ptr->ref_count() == 1 &&
                        str_ptr->str._str == "12345123"
                    );

                }

            }



        }


        TEST_BLOCK("DeclAPI::Value ASTNode* | Basic Tests") {
            // Value as ASTNode is, for now, exclusively neeeded for FuncDef so that it can be evaluated by AstEvaluator at runtime
            using namespace AstParser;

            NodeParser node_parser;

            UniquePtr<Context> shrd_context = makeUniqueContext();
            Context* main_context = obj_cast<Context*>(shrd_context.get());
            setUpContext(main_context);  

            SmartASTNode node01 = AstGenerator::generateNode(
                R"(
                    func func1(a, b) {
                        return a + b
                    }
                )", 
                main_context
            );

            
            Logger::info(node_parser.node_to_string(node01));


            Value v = main_context->makeObjAstRuntimeFuncDef(node01.get());
            // Value(ValueType::AstRuntimeFuncDef, node01.get());


            assertm("AstFuncDef Node Assigned",
                v.type == ValueType::AstRuntimeFuncDef &&
                v.obj.get_ptr()->ref_count() == 1 &&
                v.get_astruntime_func_def()->ast_node == node01.get()
            );

            // COPY CONSTRUCTOR
            Value v1 = v;

            assertm("AstFuncDef Node Assigned",
                v1.type == ValueType::AstRuntimeFuncDef &&
                v.obj.get_ptr()->ref_count() == 2 &&
                v1.obj.get_ptr() == v.obj.get_ptr() &&
                v1.get_astruntime_func_def()->ast_node == node01.get()
            );

            // MOVE CONSTRUCTOR
            Value v2 = std::move(v1);

            assertm("Value v1 MOVED", v1.type == ValueType::Null);

            assertm("AstFuncDef Node Assigned",
                v2.type == ValueType::AstRuntimeFuncDef &&
                v2.obj.get_ptr() == v.obj.get_ptr() &&
                v.obj.get_ptr()->ref_count() == 2 && // STILL 2 AS IT IS MOVED
                v2.get_astruntime_func_def()->ast_node == node01.get()
            );

            // COPY ASSIGNMENT
            Value v3;

            v3 = v2;

            assertm("AstFuncDef Node Assigned",
                v3.type == ValueType::AstRuntimeFuncDef &&
                v3.obj.get_ptr() == v.obj.get_ptr() &&
                v3.obj.get_ptr()->ref_count() == 3 &&
                v3.get_astruntime_func_def()->ast_node == node01.get()
            );

            // MOVE ASSIGNMENT
            Value v4;

            v4 = std::move(v3);

            assertm("Value v3 MOVED", v3.type == ValueType::Null);

            assertm("AstFuncDef Node Assigned",
                v4.type == ValueType::AstRuntimeFuncDef &&
                v4.obj.get_ptr() == v.obj.get_ptr() &&
                v.obj.get_ptr()->ref_count() == 3 && // STILL 3 AS IT IS MOVED
                v4.get_astruntime_func_def()->ast_node == node01.get()
            );


            TEST_BLOCK("COPY AND MOVE ASSIGNMENT CLEANS UP THE PREV INSTANCE")
            {
                // MOVE ASSIGNMENT - CLEANUP OF THE PREV INSTANCE
                Value v = main_context->makeObjAstRuntimeFuncDef(node01.get());

                v = std::move(v4);

                assertm("Value v4 MOVED", v4.type == ValueType::Null);

                assertm("AstFuncDef Node Assigned",
                    v.type == ValueType::AstRuntimeFuncDef &&
                    v.get_astruntime_func_def()->ast_node == node01.get()
                );

                // COPY ASSIGNMENT - CLEANUP OF THE PREV INSTANCE
                Value v1 = main_context->makeObjAstRuntimeFuncDef(node01.get());

                v1 = v;

                assertm("AstFuncDef Node Assigned",
                    v.type == ValueType::AstRuntimeFuncDef &&
                    v.get_astruntime_func_def()->ast_node == node01.get()
                );

            }

        }


        TEST_BLOCK("Function Object | Basic Tests") {
            // With global built-in functions, there is no need to increment the ref count as if it were an object with a lifespan - it is a global function, which is released at the end of the program life - that's why we are using SharedPtr sorely and referecing it weakly by pointer via Value
            SharedPtr foo_sys_shared = main_context->makeSharedFunction(foo_sys);

            Value foo_sys_value = obj_cast<ObjFunction*>(foo_sys_shared.get_ptr());

            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                foo_sys_value.type == ValueType::Function &&
                foo_sys_value.func->func_ptr == foo_sys
            );

            Value v = callFunc("Line 1435",
                foo_sys_value,
                {
                    makeNumber(22)
                }
            );

            assertm("Function returns",
                v.get_num().n == 22
            );

            // COPY CONSTRUCTOR OK
            Value foo_sys_value1 = foo_sys_value;

            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                foo_sys_value1.type == ValueType::Function &&
                foo_sys_value1.func->func_ptr == foo_sys
            );

            v = callFunc("Line 1435",
                foo_sys_value1,
                {
                    makeNumber(23)
                }
            );

            assertm("Function returns",
                v.get_num().n == 23
            );

            // MOVE CONSTRUCTOR OK
            Value foo_sys_value2 = std::move(foo_sys_value1);

            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                foo_sys_value2.type == ValueType::Function &&
                foo_sys_value2.func->func_ptr == foo_sys
            );

            assertm(
                "MOVED FOO_SYS OK",
                foo_sys_value1.type == ValueType::Null
            );

            v = callFunc("Line 1435",
                foo_sys_value2,
                {
                    makeNumber(24)
                }
            );

            assertm("Function returns",
                v.get_num().n == 24
            );

            Value foo_sys_value4;

            // COPY ASSIGNMENT OK
            foo_sys_value4 = foo_sys_value2;

            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                foo_sys_value4.type == ValueType::Function &&
                foo_sys_value4.func->func_ptr == foo_sys
            );

            v = callFunc("Line 1435",
                foo_sys_value4,
                {
                    makeNumber(25)
                }
            );

            assertm("Function returns",
                v.get_num().n == 25
            );


            Value foo_sys_value5;

            // MOVE ASSIGNMENT OK
            foo_sys_value5 = std::move(foo_sys_value2);

            assertm(
                "IS FUNCTION BINDING ASSSIGNED",
                foo_sys_value5.type == ValueType::Function &&
                foo_sys_value5.func->func_ptr == foo_sys
            );

            assertm(
                "MOVED FOO_SYS OK",
                foo_sys_value2.type == ValueType::Null
            );

            v = callFunc("Line 1435",
                foo_sys_value5,
                {
                    makeNumber(26)
                }
            );

            assertm("Function returns",
                v.get_num().n == 26
            );


        }

        TEST_BLOCK("Value Scope Test") {
            hash_map<size_t, Value> scope;

            // With global built-in functions, there is no need to increment the ref count as if it were an object with a lifespan - it is a global function, which is released at the end of the program life - that's why we are using SharedPtr sorely
            SharedPtr foo_sys_shared = main_context->makeSharedFunction(foo_sys);

            size_t foo_sys_offset = main_context->insert_if_not_and_get_symbol_offset("foo_sys");

            scope[foo_sys_offset] = obj_cast<ObjFunction*>(foo_sys_shared.get_ptr());




            /*
            // Value arr = main_context->makeNamespace();
            // scope[main_context->insert_if_not_and_get_symbol_offset("zoo")] = FunctionBind(arr.obj, NamespaceFunctions::zoo);
            storeMember(
                "LINE 1558",
                arr,
                main_context->insert_if_not_and_get_symbol_offset("zoo"),
                FunctionBind(arr.obj, NamespaceFunctions::zoo)
                // std::move(f)
                // makeNumber(555)
            );
            */

            Value v = callFunc("Line 1435",
                scope[
                    // foo_sys_offset
                    main_context->insert_if_not_and_get_symbol_offset("foo_sys")
                ],
                {
                    makeNumber(30)
                }
            );

            assertm("Function returns",
                v.get_num().n == 30
            );
        }


        TEST_BLOCK("Namspace Basic Tests") {
            Value namespace1 = main_context->makeNamespace();

            SharedPtr zoo_shared = main_context->makeSharedFunction(NamespaceFunctions::zoo);



            assertm(
                "Namespace OK",
                namespace1.type == ValueType::Namespace
            );

            storeMember(
                "LINE 1558",
                namespace1,
                main_context->insert_if_not_and_get_symbol_offset("n"),
                makeNumber(11)
            );

            /*
            hash_map<size_t, Value> vv;
            // vv[0] = v;
            */

            /*
            storeMember(
                "LINE 1558",
                namespace1,
                main_context->insert_if_not_and_get_symbol_offset("zoo"),
                FunctionBind(namespace1.obj, NamespaceFunctions::zoo)
            );
            namespace1.obj.get_ptr()->dec_ref();
            // NOTE: THIS IS A WORK-AROUND FOR THE NAMESPACE_TABLE REFERENCING ITSELF SO ONCE WE REACH the obj.release_and_nullify() of the makeNameSpace(), THE REF DOES NOT REACH ZERO SO THE FunctionBind(namespace1.obj) is foverer HANGING!
            // TODO: REFACTOR THIS IN THE CASE FUNCTIND BINDING WITH ONLY OBJBASE POINTER REFERENCE
            // NOTE: FOR THIS REASON, WE ARE USING makeSharedFunction, WHICH IS DEF OKAY SINCE THIS IS INTEDED FOR GLOBAL NAMESPACES ONLY AND IS NOT POSSIBLE TO SPECIFY THROUGH THE DECL ITSELF SO THERE DOESN'T HAVE TO BE ANY OWNSHIP AND DEF WON'T RESULT IN UB
            */

            storeMember(
                "LINE 1558",
                namespace1,
                main_context->insert_if_not_and_get_symbol_offset("zoo"),
                obj_cast<ObjFunction*>(zoo_shared.get_ptr())
            );

            assertm(
                "Namespace reference count still 1",
                namespace1.obj.get_ptr()->ref_count() == 1
            );

            Value v = callFunc(
                "LINE 1617",
                getMember(
                    "LINE 1155",
                    namespace1,
                    main_context->insert_if_not_and_get_symbol_offset("zoo")
                ),
                {
                    makeNumber(79)
                }
            );

            Value v1 = getMember(
                "LINE 1155",
                namespace1,
                main_context->insert_if_not_and_get_symbol_offset("n")
            );

            assertm("Function returns",
                v.get_num().n == 79
            );

            assertm("Function returns",
                v1.get_num().n == 11
            );

            // COPY CONSTRUCTOR OK
            Value namespace2 = namespace1;

            assertm(
                "Namespace OK",
                namespace2.type == ValueType::Namespace
            );

            assertm(
                "Same Namespace Reference",
                namespace1.obj.get_ptr() == namespace2.obj.get_ptr() 
            );

            v = callFunc(
                "LINE 1617",
                getMember(
                    "LINE 1155",
                    namespace2,
                    main_context->insert_if_not_and_get_symbol_offset("zoo")
                ),
                {
                    makeNumber(80)
                }
            );

            v1 = getMember(
                "LINE 1155",
                namespace2,
                main_context->insert_if_not_and_get_symbol_offset("n")
            );

            assertm("Function returns",
                v.get_num().n == 80
            );

            assertm("Function returns",
                v1.get_num().n == 11
            );

            // MOVE CONSTRUCTOR OK
            Value namespace3 = std::move(namespace2);

            assertm(
                "MOVED Namespace OK",
                namespace3.type == ValueType::Namespace &&
                namespace2.type == ValueType::Null
            );

            assertm(
                "Same Namespace Reference",
                namespace3.obj.get_ptr() == namespace1.obj.get_ptr() 
            );

            v = callFunc(
                "LINE 1617",
                getMember(
                    "LINE 1155",
                    namespace3,
                    main_context->insert_if_not_and_get_symbol_offset("zoo")
                ),
                {
                    makeNumber(80)
                }
            );

            v1 = getMember(
                "LINE 1155",
                namespace3,
                main_context->insert_if_not_and_get_symbol_offset("n")
            );

            assertm("Function returns",
                v.get_num().n == 80
            );

            assertm("Function returns",
                v1.get_num().n == 11
            );

            // MOVE ASSIGNMENT OK
            Value namespace4 = namespace1; // DOING THIS SO THAT WE CAN TRIGGER CURRENT OBJ DEC IN THE CASE OF THE COPY/MOVE ASSIGNMENT
            namespace4 = std::move(namespace3);

            assertm(
                "MOVED Namespace OK",
                namespace3.type == ValueType::Null &&
                namespace4.type == ValueType::Namespace
            );

            assertm(
                "Same Namespace Reference",
                namespace4.obj.get_ptr() == namespace1.obj.get_ptr() 
            );

            v = callFunc(
                "LINE 1617",
                getMember(
                    "LINE 1155",
                    namespace4,
                    main_context->insert_if_not_and_get_symbol_offset("zoo")
                ),
                {
                    makeNumber(80)
                }
            );

            v1 = getMember(
                "LINE 1155",
                namespace4,
                main_context->insert_if_not_and_get_symbol_offset("n")
            );

            assertm("Function returns",
                v.get_num().n == 80
            );

            assertm("Function returns",
                v1.get_num().n == 11
            );

            // COPY ASSIGNMENT OK
            Value namespace5 = namespace1; // DOING THIS SO THAT WE CAN TRIGGER CURRENT OBJ DEC IN THE CASE OF THE COPY/MOVE ASSIGNMENT
            namespace5 = namespace4;

            assertm(
                "COPIED Namespace OK",
                namespace5.type == ValueType::Namespace
            );

            assertm(
                "Same Namespace Reference",
                namespace5.obj.get_ptr() == namespace1.obj.get_ptr() 
            );

            v = callFunc(
                "LINE 1617",
                getMember(
                    "LINE 1155",
                    namespace5,
                    main_context->insert_if_not_and_get_symbol_offset("zoo")
                ),
                {
                    makeNumber(80)
                }
            );

            v1 = getMember(
                "LINE 1155",
                namespace5,
                main_context->insert_if_not_and_get_symbol_offset("n")
            );

            assertm("Function returns",
                v.get_num().n == 80
            );

            assertm("Function returns",
                v1.get_num().n == 11
            );

        }

        TEST_BLOCK("Namespace Nested/Chaining") {
            Value zoo = main_context->makeNamespace();

            Value foo = main_context->makeNamespace();

            SharedPtr func_shared = main_context->makeSharedFunction(NamespaceFunctions::zoo);

            storeMember(
                "LINE 1886",
                zoo,
                main_context->insert_if_not_and_get_symbol_offset("foo"),
                foo
            );

            storeMember(
                "LINE 1886",
                foo,
                main_context->insert_if_not_and_get_symbol_offset("func_shared"),
                obj_cast<ObjFunction*>(func_shared.get_ptr())
            );

            // zoo.foo.func_shared, effectively
            Value func1 = getMember(
                "LINE 1903",
                getMember(
                    "LINE 1903",
                    zoo,
                    main_context->insert_if_not_and_get_symbol_offset("foo")
                ),
                main_context->insert_if_not_and_get_symbol_offset("func_shared")
            );

            assertm(
                "func_shared returned",
                func1.type == ValueType::Function &&
                func1.func == func_shared.get_ptr()
            );

            Value v = callFunc(
                "LINE 1920",
                func1,
                { makeNumber(10) }
            );

            assertm("Function returns",
                v.get_num().n == 10
            );

        }

        TEST_BLOCK("Built-in Context Symbols including functions, namespaces, and variables") {

            UniquePtr<Context> shrd_context = makeUniqueContext();
            Context* context = obj_cast<Context*>(shrd_context.get());
            setUpContext(context);
            assertm("Context Initialized", context->initialized == true);

            setUpBuilts(context);
            assertm("Context Builtins Set", context->built_ins_set == true);

            Value sys = context->builtins.get_sym(
                context->insert_if_not_and_get_symbol_offset("sys")
            );


            // Logger::info(sys.obj.get_ptr()->ref_count());

            assertm(
                "Namespace Returned",
                sys.type == ValueType::Namespace &&
                sys.obj.get_ptr()->ref_count() != 0 // Object Alive
            );

            // sys.stdout
            Value sys_stdout = getMember(
                "LINE 1903",
                sys,
                context->insert_if_not_and_get_symbol_offset("stdout")
            );

            assertm(
                "Function Returned",
                sys_stdout.type == ValueType::Function &&
                sys_stdout.func->func_ptr == NamespaceFunctions::_sys_stdout
            );

            Value v = callFunc(
                "LINE 1920",
                sys_stdout,
                { makeNumber(10) }
            );

            assertm("Function returns",
                v.get_num().n == 1
            );

            // the namespace() function NOT TO BE CONFUSED!
            Value _namespace = context->builtins.get_sym(
                context->insert_if_not_and_get_symbol_offset("namespace")
            );

            assertm(
                "Namespace Function OK",
                _namespace.type == ValueType::Function &&
                _namespace.func->func_ptr == NamespaceFunctions::create_namespace
            );

            Value my_namespace = callFunc(
                "LINE 1987",
                _namespace,
                {}
            );

            assertm(
                "Namespace CREATED via namespace from Decl",
                my_namespace.type == ValueType::Namespace &&
                my_namespace.obj.get_ptr()->ref_count() == 1 // REF COUNT is 1 SINCE THIS IS A NEWLY CREATED NAMESPACE
            );

            // Testing sys.zoo Namespace
            // sys.zoo
            Value zoo = getMember(
                "LINE 1903",
                sys,
                context->insert_if_not_and_get_symbol_offset("zoo")
            ); 

            assertm(
                "Namespace CREATED via namespace from Decl",
                zoo.type == ValueType::Namespace && 
                zoo.obj.get_ptr()->ref_count() != 0 // Object Alive
            );

            // zoo.zoo_func
            Value zoo_zoo_func = getMember(
                "LINE 1903",
                zoo,
                context->insert_if_not_and_get_symbol_offset("zoo_func")
            );

            assertm(
                "Namespace Function OK",
                zoo_zoo_func.type == ValueType::Function &&
                zoo_zoo_func.func->func_ptr == NamespaceFunctions::zoo
            );


            Value zoo_returned = callFunc(
                "LINE 2205",
                zoo_zoo_func,
                {
                    makeNumber(10)
                }
            );

            assertm("Function returns",
                zoo_returned.get_num().n == 10
            );

        }




    }


    TEST_BLOCK("Decl Exceptions OK") {

        {
            std::string exp_message;

            try {
                throw Exception("123");
            }
            catch(const RuntimeException& runtime_exp) {

            }
            catch(const Exception& exp) {
                exp_message = exp.message;
            }

            assertm(
                "Exception thrown and caught",
                exp_message == "123"
            );


        }

        {
            std::string exp_message;

            try {
                throw RuntimeException("1234");
            }
            catch(const RuntimeException& runtime_exp) {
                exp_message = runtime_exp.message;
            }
            catch(const Exception& exp) {
            }

            assertm(
                "RuntimeException thrown and caught",
                exp_message == "1234"
            );

        }


    }


    return 0;
}
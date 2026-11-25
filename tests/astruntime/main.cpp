#include <iostream>
#include <string>
#include <vector>

#include <cassert>

#define DECL_COMPILER_VERBOSE

#define DECL_VERBOSE_LOGGING

#define DECL_LOG_INFO

// #define VALUE_SAFETY_CHECK

#define _DECL_API_IGNORE_UNINIT

#include <decl/lib/AstVirtualMachine.hpp>

/*
#include <decl/lib/Utility.hpp>

#include <decl/lib/DeclAPI.hpp>
*/


using namespace AstVirtualMachine;

bool compareValues(const Value& left, const Value& right) {
    if(left.type != right.type) {
        return false;
    }

    // At this point, left.type == right.type already match
    if(left.type == ValueType::Number) {
        return left.get_num().n == right.get_num().n;
    }

    return false;
}

int main() {

  TEST_BLOCK("Happy Tests") {

    TEST_BLOCK("AstVirtualMachine | AstEvaluator OK")
    {
      // START: SETTING UP CONTEXT
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* context = obj_cast<Context*>(shrd_context.get()); 
      setUpContext(context);
      setUpBuilts(context);
      // END: SETTING UP CONTEXT

      // START: SETTING UP AST PARSER AND ERROR HANDLING
      AstGenerator ast_generator;
      NodeParser node_parser;

      const std::string &source_code = R"(1 + 1 + 1)";

      ast_generator.set_source(source_code.c_str());
      ast_generator.attach_context(context);

      assertm("Block Stack Empty", ast_generator.block_stack.size() == 0);
      SmartASTNode ast_root = ast_generator.parse();
      assertm("Block Stack Clearned", ast_generator.block_stack.size() == 0);

      const auto& errors = ast_generator.get_syntax_errors();

      if(errors.size() != 0) { Logger::info("----------START ERROR----------"); }
      for (size_t i = 0; i < errors.size(); i++) {                                                              
        const SyntaxError &syntax_error = errors[i];
        Logger::info(syntax_error.get_error());
      }
      if(errors.size() != 0) { Logger::info("----------END ERROR----------"); }


      std::string result = node_parser.node_to_string(ast_root);
      Logger::info(result); // Show/Display the invalid tree for further analysis

      assertm("Must NOT Contain Errors", errors.size() == 0); 

      // END: SETTING UP AST PARSER AND ERROR HANDLING

      // START: SETTING UP AstEvaluator
      AstEvaluator ast_evaluator = AstEvaluator();

      ast_evaluator.attach_context(context);
      // END: SETTING UP AstEvaluator

      std::string err_message;

      try {

        RuntimeEntry output = ast_evaluator.evaluateProgramNode(ast_root);

        assertm("Scope Stack zero emptied if eval OK",
            ast_evaluator.runtime_scope_stack.size() == 0
        );

        // TODO: Replace this with printValue
        Logger::info("output.val", output.val.get_num().n);

        assertm(
            "Ouput Matches",
            compareValues(output.val, makeNumber(3))
        );
      }
      catch(const Exception& error) {
        err_message = error.message;

        std::cout << "Exception thrown at " << __FILE__ << ":" << CURRENT_LINE_STR << ' ' << err_message << std::endl;

        // Make sure that ast_evalutor.runtime_scope_stack is not empty if an exception is thrown. This helps with the stack trace
        assertm("Scope Stack NOT ZERO IF A (RUNTIME) EXCEPTION IS THROWN",
            ast_evaluator.runtime_scope_stack.size() != 0
        );
      }

      assertm("Program OK | Ensure No Exceptions thrown during runtime",
        err_message.size() == 0
      );
      
    }


  }


  return 0;

}

























































































/*
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

    size_t push_sym_offset = _this->context->get_symbol_offset("push");

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

int main01() {
    SharedPtr shrd_context = makeSharedContext();
    Context* main_context = obj_cast<Context*>(shrd_context.get_ptr());
    setUpContext(main_context);
    assertm("Context Initialized", main_context->initialized == true);



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

            arr = arr;

            assertm("SharedPtr NULL Check", arr.get_ptr() != nullptr);

            ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.get_ptr());
            arr_ptr->arr.push_back(makeNumber(221));

            arr = arr;


            assertm("Array::at Sanity Check", arr_ptr->arr.at(size_t(0)).get_num().n == 221);

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


            size_t push_sym_offset = main_context->get_symbol_offset("push"); 

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
                getMember("LINE 464", arr, main_context->get_symbol_offset("push") ),
                { makeNumber(24), }
            );
            // DECL SOURCE: arr.push(24);
            // callFunc("LINE 464", 
            //    getMember("LINE 464", arr, "push"),
            //    { makeNumber(24), }
            // );

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
                    v_function_binding1.function_bind.function_bind_p == MemberFunctionPointers::array_push
                );

                assertm("Value::number swapped",
                    v_function_binding.type == ValueType::Number &&
                    v_function_binding.get_num().n == 11
                );

                {
                    Value a;
                    Value b;
                    std::swap(a,b);
                }

                {
                    hash_map<size_t, Value> map;

                    map[0] = FunctionBind();
                }

            }


        }
        TEST_BLOCK("DeclAPI::Value Basic Test | Basic FunctionBind Value OK") {
            Value arr = main_context->makeArray();

            ObjArray* arr_ptr = obj_cast<ObjArray*>(arr.obj.get_ptr());

            // NOTE: BE AWARE THAT THIS LOOKUP WILL CREATE THE DEFAULT ENTRY IF NOT FOUND BUT WE SHOULD BE SAFE HERE AS THE SIZE_T VALUES DEFAULTS TO 0 AND THE HASH INCREMENTAL DEFAULTS TO 1
            size_t push_sym_offset = main_context->get_symbol_offset("push"); 

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

            // Value::FunctionBind Copy Assignment: FunctionBind Cleaned Up: This is not an assertion - it is detected by valgrind or similar
            v3_function_binding = v2_function_binding;

            assertm(
                "ARR OBJ REF COUNT 5 AFTER COPY ASSIGNMENT",
                arr_ptr->ref_count() == 5
            );

            Value v4_function_binding;

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

                size_t append_sym_offset = main_context->get_symbol_offset("append");
            
                Value v_function_binding = getMember(
                    "LINE 1025", 
                    str,
                    cacl_member_offset(append_sym_offset)
                );

                // Check out the "C++ | Bypassing the Compiler Part II | Unions Undefined Behavior" video as to why this happens

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
                    getMember("LINE 1088", str, main_context->get_symbol_offset("append") ),
                    { main_context->makeString(String("_1")), }
                );

                callFunc("LINE 1088", 
                    getMember("LINE 1088", str1, main_context->get_symbol_offset("append") ),
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
  return 0;

}
*/
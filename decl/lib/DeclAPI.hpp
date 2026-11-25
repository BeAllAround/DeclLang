// ORDER of Inclusion of Header Directives
// 1. C (prefix-ed) headers
// 2. STD C++
// 3. Third Party
// 4. Local

#ifndef HEADER_DECL_API // OR #pragma once

#define HEADER_DECL_API

#include <cassert>


#include <iostream>
#include <initializer_list>
#include <string>
#include <vector>
#include <memory>


#include <skarupke/flat_hash_map/flat_hash_map.hpp>
#include <unordered_map>

// #include <decl/lib/AstParserNamespace.hpp>

// USED STRICTLY WHEN NOT IN THE TESTS/API/MAIN.CPP AS THE COMPILER DOES NOT KNOW HOW TO USE THE UNION CLASS AND WILL GENERATE WARNINGS FOR POTENTIAL UNINTIALIZED VALUES/MEMBERS
// NOTE THAT THIS IS A SKARUPKE PROBLEM. See the "using hash_map = ..." below
#ifdef _DECL_API_IGNORE_UNINIT
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif


#define DECL_ARGS_GET(arr, inx) (*(((arr).begin()) + inx) )


#define _BRANCH_UNREACHABLE()  __builtin_unreachable()


namespace DeclExceptions {
/*
    // OK TOO. However, we won't use this as we always catch in Decl so we don't need to inherit from the built-in std::exception
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
*/

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

/*
        TODO: IF NEEDED:

        virtual ~Exception() noexcept {}

        virtual const char* what() const noexcept {
            return message.c_str();
        }

*/

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

}

namespace DeclAPI
{

    // Empty Class Design by default and by design. However, adding members,etc is OK but that increases the sizeof Context, of course.
    struct CustomMemoryManagement {

        // CustomMemoryManagement() = default;

        template<class T, class... Args>
        inline T* custom_new(Args&&... args) {
            return new T(std::forward<Args>(args)...);
        }


        template<class T>
        inline void custom_delete(T* rawMemory) {
            delete rawMemory;
        }

    };

    using namespace DeclExceptions;

    using std::string;

    enum class ValueType
    {
        Boolean,
        Number,
        String,
        FunctionBind,
        Function,
        Null,
        Array,
        HashTable,
        Namespace,

        AstRuntimeFuncDef,
    };

    #define obj_cast reinterpret_cast
    // #define obj_cast dynamic_cast


    template<class T>
    class _Context;


    using Context = _Context<CustomMemoryManagement>;


    class RefCount {

        public:

        RefCount() {
            // ref_c = 0;
        }

        RefCount(RefCount&& other) : ref_c{other.ref_c} {
            // Swap, essentially
            other.ref_c = 0;
        }

        RefCount(const RefCount& other) : ref_c{other.ref_c} {
        }

        void inc_ref() {
            ref_c++;
        }

        void dec_ref() {
            ref_c--;
        }

        size_t ref_count() {
            return ref_c;
        }

        private:
            // TODO: thread-safe Atomic
            size_t ref_c {0};
    };

    class ObjBase : public RefCount {

        public:

        Context* context {nullptr};

        ObjBase() = default;

        void attach_context(Context* context) {
            this->context = context;
        }

        virtual ~ObjBase() {

        }

    };

    class SharedPtr {
        public:

        SharedPtr() = default;


        SharedPtr(SharedPtr&& other) : ptr{other.ptr} {
            // Swap, essentially
            other.ptr = nullptr;
        }

        SharedPtr(const SharedPtr& other) : ptr{other.ptr} {
            if(ptr != nullptr) {
                ptr->inc_ref();
            }
        }

        explicit SharedPtr(ObjBase* obj) {
            // assert(obj != nullptr);

            // NOTE: SAFETY CHECK
            if(obj != nullptr) {
                obj->inc_ref();
                ptr = obj;
            }

            // ptr = obj;
        }


        SharedPtr& operator=(const SharedPtr& other) {

            if(this == &other) {
                return *this;
            }

            // release_ref_obj_base();
            release_ref();

            ptr = other.ptr;

            if(ptr != nullptr) {
                ptr->inc_ref();
            }

            return *this;

        }

        SharedPtr& operator=(SharedPtr&& other) {

            // NOTE: MIGHT NOT NEEDED SINCE DECL WON'T ALLOW, AT COMPILE TIME: RVALUE = RVALUE, ONLY LVALUE = RVALUE e.g. makeArray() = makeArray()
            // NOTE: STOPPED BY THE COMPILER ANYWAY
            // tests/api/main.cpp:361:21: error: moving ‘arr’ of type ‘DeclAPI::SharedPtr’ to itself [-Werror=self-move]
            //  361 |                 arr = std::move(arr);
            if(this == &other) {
                return *this;
            }


            // release_ref_obj_base();
            release_ref();

            ptr = other.ptr;
            other.ptr = nullptr;

            return *this;

        }

        inline void release_ref();

        inline void release_ref_and_nullify();


        ObjBase* get_ptr() const {
            return ptr;
        }


        // void release_ref_obj_base() {
            // release_ref<ObjBase>();
        // }


        ~SharedPtr() {
            release_ref_and_nullify();
        }

        private:
            ObjBase* ptr { nullptr };


    };


    inline void SharedPtr::release_ref_and_nullify()
    {
        release_ref();
        ptr = nullptr;

    }


    class FunctionBind;
    class ObjArray;
    class ObjHashTable;
    class ObjFunction;
    class ObjString;
    class ObjNamespace;


    // START: Ast AstRuntime nodes
    class ObjAstRuntimeFuncDef;
    // END: Ast AstRuntime nodes


    class Number;
    class String;
    class Value;



    // typedef double num_type;
    using num_type = double;

    /*
    // NOTE:
    initializer_list is used for source-2-source (C++) as it is much more efficient being on stack.
    const vector& must be used when evaluating Decl via a virtual machine or similar runtime evaluation.
    The point being is none of the unit tests crash in either case since a vector allows the initializer list constructor.
    As a result, this allows for great compatibility between the two use cases: runtime args vs compile time (source-2-source C++) args.
    */
    #ifdef DECLS2S
    using func_args = std::initializer_list<Value>;
    #else
    using func_args = const std::vector<Value>&;
    #endif




    class Boolean {
        public:

        bool _bool { false };


        explicit Boolean() {}
        explicit Boolean(bool _bool) : _bool{_bool} {}



        Boolean(const Boolean& other) : _bool{other._bool} {}

    };

    class Number {

        public:
        num_type n {0};

        // NOTE: ALL CONSTRUCTORS OF PRIMITIVES MUST BE EXPLICIT
        explicit Number() {}

        explicit Number(num_type n): n{n} {}

        Number(const Number& other) : n{other.n} {}

        // NOTE: Move Semantics for primivites are always treated as COPY
        /*
        Number(Number&& other) : n{other.n} {
            other.n = 0;
        }

        Number& operator=(Number&& other) {

            n = other.n;
            other.n = 0;

            return *this;
        }
        */

        Number& operator=(const Number& other) {

            n = other.n;

            return *this;
        }

        Number plus(const Number& other) const {
            return Number(n + other.n);
        }


    };

    class String {

        public:
        std::string _str;

        String() = default;

        String(std::string&& str) : _str{std::move(str)} {};
        String(const std::string& str) : _str{str} {};

    };

    /*
        // Naming Convention Rules:

        // ObjArray - class that inherits ObjBase
        // Array - class that is a wrapper of the said ObjArray -> SharedArray (makeSharedArray)

        // makeObjArray - making of the Array ObjBase - Pointer
        // makeSharedArray - making of SharedArray
        // makeArray - making of the SharedArray Wrapper picked up by Value

    */

    #define DEFINE_OBJ_WRAPPER(CLASS) class CLASS { \
            public: \
            SharedPtr obj; \
            CLASS() {} \
            CLASS(SharedPtr&& shared_arr) : obj{std::move(shared_arr)} {} \
            CLASS(const SharedPtr& shared_arr) : obj{shared_arr} {} \
            CLASS(CLASS&& other) { \
                *this = std::move(other); \
            } \
            CLASS(const CLASS& other) { \
                *this = other; \
            } \
            CLASS& operator=(const CLASS& other) { \
                obj = other.obj; \
                return *this; \
            } \
            CLASS& operator=(CLASS&& other) { \
                obj = std::move(other.obj); \
                return *this; \
            } \
            ~CLASS() { } \
        }; \

    namespace ObjWrapper {
        // This ObjWrapper is made for the purpose of optimizing out the jump table overhead when making an Object that's an rvalue in the code. Just like with Number, String, and Boolean.
        // Based on the note:
        // TODO MUST: In order to, in some cases, avoid the jump table overhead, implement operator=(SharedPtr&&) optimization, (and operator=(Array*) more or less but that won't work for Array* since our Array will be hanging as rvalue if NOT USED where it can live on)
        // We can't have Array* or similar since the value will be left hanging in some cases so if we wrap it into the specialized class as SharedPtr, not only do we exactly know which type it is without the enum but also its destructor will be called upon its lifetime ending.
        // Via implicit Value constructors, these are used in order to determine the type of Value once it is (std::moved as rvalue) onto (class Value).
        // See Value(Array&&) and the rest below.
        // This approach also optimizes out the jump table of the Value union class for built-in operators for literal constants such as plus(Array, Array), plus(Number, Number), plus(String, String), plus(String, Value), plus(Value, String)

        /*
        class Array {
            public:
            SharedPtr arr;

            Array(const SharedPtr& shared_arr) : arr{shared_arr} {}

            Array(SharedPtr&& shared_arr) : arr{std::move(shared_arr)} {}

            Array(Array&& other) {
                *this = std::move(other);
            }

            Array(const Array& other) {
                *this = other;
            }

            Array& operator=(const Array& other) {
                arr = other.arr;
                return *this;
            }

            Array& operator=(Array&& other) {
                arr = std::move(other.arr);
                return *this;
            }

            ~Array() {

            }

        };
        */

        DEFINE_OBJ_WRAPPER(StringWrapper)
        DEFINE_OBJ_WRAPPER(Array)
        DEFINE_OBJ_WRAPPER(HashTable)
        DEFINE_OBJ_WRAPPER(Namespace)

        class Null {
            public:
            Null() = default;
        };

    }


    using bind_function_type = Value(*)(const char*, ObjBase*, func_args);

    class FunctionBind {
        public:
        // CallBack Function
        bind_function_type function_bind_p {nullptr};
        // Bound Object
        // NOTE: Making sure to treat it as the shared object as function bindings can go out of scope as well.
        SharedPtr obj; // {SharedPtr(nullptr)};



        FunctionBind() {
            function_bind_p = nullptr;
        }

        /*
        FunctionBind(const SharedPtr& obj) {
            this->obj = obj;
        }

        FunctionBind(SharedPtr&& obj) {
            this->obj = std::move(obj);
        }

        FunctionBind(bind_function_type func_ptr) {
            this->function_bind_p = func_ptr;
        }
        */

        FunctionBind(const SharedPtr& shared_ptr, bind_function_type func_ptr) : 
        function_bind_p{func_ptr} ,
        obj {shared_ptr}
        {
        }

        FunctionBind(const FunctionBind& function_bind) : 
            function_bind_p{function_bind.function_bind_p} ,
            obj{function_bind.obj}
            {}

        // TODO: function_bind_p nullptr swap for the move?
        FunctionBind(FunctionBind&& function_bind) noexcept
            :
            function_bind_p{function_bind.function_bind_p},
            obj{std::move(function_bind.obj)}

        {
            function_bind.function_bind_p = nullptr;

            // function_bind_p = std::move(function_bind.function_bind_p);
            // assert(this!=nullptr);
            // assert(function_bind_p != nullptr);
            // assert(function_bind.function_bind_p != nullptr);

            /*
            #ifdef _DECL_API_IGNORE_UNINIT
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
            // function_bind_p = nullptr;
            obj = std::move(function_bind.obj);
            this->function_bind_p = std::move(function_bind.function_bind_p);
            function_bind.function_bind_p = nullptr;

            // if(function_bind.function_bind_p != nullptr) {
            // }
            #pragma GCC diagnostic pop
            #else
            obj = std::move(function_bind.obj);
            this->function_bind_p = function_bind.function_bind_p;
            function_bind.function_bind_p = nullptr;
            #endif
            */
        }

        FunctionBind&operator=(const FunctionBind& function_bind) {
            obj = function_bind.obj;
            function_bind_p = function_bind.function_bind_p;
            return *this;
        }

        FunctionBind&operator=(FunctionBind&& function_bind) {
            obj = std::move(function_bind.obj);
            function_bind_p = function_bind.function_bind_p;

            function_bind.function_bind_p = nullptr;
            return *this;
        }


        inline Value invoke(const char*, func_args) const __attribute__((always_inline));


        ~FunctionBind() {
            // function_bind_p = nullptr;

        }

    };


    using namespace ObjWrapper;

// NOTE: MAKING SURE THESE ARE DENSELY PACKED FOR THE SWITCH CASE
#define PRIMITIVE_VALUES_SWITCH_CASE \
    case ValueType::Boolean:         \
    case ValueType::Number:          \
    case ValueType::String:          \
    case ValueType::Null

    class Value {

        public:
            ValueType type { ValueType::Null };

            union {
                // void* _null;
                Boolean boolean;
                Number num;
                // String str;
                SharedPtr obj;

                ObjFunction* func;

                FunctionBind function_bind;

                // NOT NEEDED - WE MOVED ONTO the ObjAstRuntime* functionality
                // void* ast_func_def; // weak pointer reference for the Function definition AST Node
            };



            Value() {
                // type = ValueType::Null;
                // _null = nullptr;
            }




            inline Value(const Value& value);
            inline Value(Value&&);

            // NOTE: Constructors for Value Primitives are needed for implicit creation
            // NOTE that these implicit constructors are always of rvalue since once you store a primitive into a Value, it is always of (class Value).
            // Also see Value& operator=(Number&&) below.
            inline Value(Boolean&& _boolean);

            inline Value(Number&& number);

            inline Value(Array&&);

            inline Value(Null&&);

            inline Value(StringWrapper&&);

            inline Value(FunctionBind&&);

            inline Value(ObjFunction*);

            inline Value(Namespace&&);

            // NOTE: More of an auxiliary constructor for AstNodes for the AstVirtualMachine
            // inline Value(ValueType, void*);
            // ObjAstRuntimeFuncDef doesn't need its respective OBJ_WRAPPER AS THIS IS SOLELY USED BY THE AstVirtualMachine so there will be no pointers hanging or memory leaking
            inline Value(ObjAstRuntimeFuncDef*);


            inline Value& operator=(Value&& rvalue) noexcept;
            inline Value& operator=(const Value& lvalue);


            // START: COMPILE-TIME OPTIMIZATIONS
            inline Value& operator=(Boolean&&);
            inline Value& operator=(Number&&);
            inline Value& operator=(Array&&);
            // END: COMPILE-TIME OPTIMIZATIONS





            // START: DIRECT UNION GET AND CAST. This is represented by the underscore (_) in the function naming convention
            // THESE ARE USED IN COMBINATION OF A SWITCH CASE OR IF STATEMENT CHECK. THIS IS USED SO THAT WE DON'T DO DOUBLE CHECK (JNE OR JMP) IF NOT NECESSARY
            // NOTE: THIS IS UNSAFE AND WILL GENERATE A COMPILE-TIME ERROR IF NOT USED IN COMBINATION OF A SWITCH CASE OR IF STATEMENT CHECK. THIS IS USED SO THAT WE DON'T DO DOUBLE CHECK (JNE OR JMP) IF NOT NECESSARY
            inline Number _cast_num() const {
                return num;
            }

            inline Boolean _cast_boolean() const {
                return boolean;
            }


            inline ObjArray* _cast_array_ptr() const;

            inline ObjString* _cast_string_ptr() const;
            inline ObjNamespace* _cast_namespace_ptr() const;

            // END: DIRECT UNION GET AND CAST



            // START: SAFE GET - THERE IS NO SWITCH CASE OR NECESSARY CHECK
            inline ObjAstRuntimeFuncDef* get_astruntime_func_def() const;

            inline Boolean get_boolean() const {
                // NOTE: THIS CHECK (ASSERTION OR THROW) IS REQUIRED. OTHERWISE, WE GET COMPILE TIME ERROR THAT THE ANONYMOUS UNION MAY BE UNINITIALIZED
                // TODO: MAYBE throw?
                // See for example, https://github.com/nlohmann/json/blob/develop/include/nlohmann/json.hpp#L1433 or https://github.com/nlohmann/json/blob/develop/include/nlohmann/json.hpp#L1944
                // TRY COMMENTING THIS OUT AND RUNNING THE TESTS AND YOU WILL SEE THE OUTPUT
                assert(type == ValueType::Boolean);
                return boolean;
            }

            inline Number get_num() const {
                // NOTE: THIS CHECK (ASSERTION OR THROW) IS REQUIRED. OTHERWISE, WE GET COMPILE TIME ERROR THAT THE ANONYMOUS UNION MAY BE UNINITIALIZED
                // TODO: MAYBE throw?
                assert(type == ValueType::Number);
                return num;
            }
            // END: SAFE GET - THERE IS NO SWITCH CASE OR NECESSARY CHECK


            ~Value() {
                switch(type) {
                    // NOTE: We are leaving these any other enums out and enforcing to (default case) since it is a conditional jump (CMP + JA) and it is considered to be faster if we leave those out. Since the compiler always generates a jump table with CMP+JA before the enum JMP.
                    // For more, see: https://github.com/BeAllAround/S2S-Compilers-Blog-Series/blob/main/SWITCH_CASE.md#out-of-bounds-benchmark
                    // However, we will be using _BRANCH_UNREACHABLE() to eliminate this concern
                   /*
                   case ValueType::Boolean: {
                    boolean.~Boolean();
                   }
                   */
                  /*
                    PRIMITIVE_VALUES_SWITCH_CASE: {
                        return;
                    }
                  */
                    case ValueType::Array:
                    case ValueType::HashTable:
                    case ValueType::String:
                    case ValueType::Namespace:
                    case ValueType::AstRuntimeFuncDef:
                    {

                        obj.release_ref_and_nullify();

                        #ifdef VALUE_SAFETY_CHECK
                        assert(obj.get_ptr() == nullptr);
                        #endif

                        return;
                    }
                    case ValueType::FunctionBind: {
                        function_bind.~FunctionBind();
                        return;
                    }
                    default: {
                        // _BRANCH_UNREACHABLE();
                        // break;
                        return;
                    }
                }
                return;
            }


            // NOTE: While we can do something like this, it is not necessary since all of the members are public. Perhaps, down the line, this changes.
            // friend Value plus(Value&&, Value&&);
    };


    inline Value FunctionBind::invoke(const char* message, func_args args) const {
        return function_bind_p(message, obj.get_ptr(), args);
    }

    // Primitives:
    // makeNumber(), makeBoolean()
    // Objects:
    // makeString(), makeValueArray(), makeValueHashTable(), etc. THIS IS DUE TO THE FACT ObjBase doesn't hold the obj_type so makeSharedArray() or similar won't be possible since we need to determine the object type in order to apply operations such as plus, get_attr, etc.
    // Hopefully, this is improved and refactored at some point
    /// UPDATE: This is resolved with the ObjWrapper namespace. See above.

    Number makeNumber(num_type);
    Boolean makeBoolean(bool);

    Null makeNull();


    using array_container = std::vector<Value>;

    class ObjArray : public ObjBase {
        public:
        array_container arr;
        /* NOTE: In the beginning, there was an idea to use global Context* in order to avoid Context* member alignment of each instance. However, this does become difficult to maintain and error and bug-prone. */
        // Context* context {nullptr};

        ObjArray() : ObjBase() {}


        Value push(const Value& item) {
            arr.push_back(item);
            return makeNull(); // return null;
        }



        // NOTE: AT IS ALWAYS RVALUE -> NEVER LVALUE SINCE WE ARE USING REFERENCE COUNTING SO HEAVY OBJECTS WILL NEVER BE DEEPLY COPIED OTHER THAN THE PRIMITIVE TYPES, WHICH IS OK
        // THIS IS ESSENTIALLY "SUBSCRIPT_GET", and "SUBSCRIPT_STORE" IS ALWAYS EXPLICIT FOR PERFORMANCE-PURPOSES - WE DON'T WANT DOUBLE HASH KEY LOOKUP FOR EXAMPLE
        Value at(size_t i) {
            return arr.at(i);
        }


        ~ObjArray() {

        }


    };


    class ObjString : public ObjBase {
        public:
        String str;

        // Context* context {nullptr};

        ObjString() : ObjBase() {}

        ObjString(const String& _str) : str{_str} {}
        ObjString(String&& _str) : str{std::move(_str)} {}


        ~ObjString() {

        }

    };



    template<class K, class V>
    // No skarupke MAY BE UNINITIALIZED (sherwood_v3_entry)->value
    // using hash_map = std::unordered_map<K, V>;
    using hash_map = ska::flat_hash_map<K, V>;


    using hash_table_container = hash_map<std::string, Value>;

    class ObjHashTable : public ObjBase {
        public:

            hash_table_container hash_table;
            /* NOTE: In the beginning, there was an idea to use global Context* in order to avoid Context* member alignment of each instance. However, this does become difficult to maintain and error and bug-prone. */
            // Context* context {nullptr};

            ObjHashTable() : ObjBase() {}


            ~ObjHashTable() {

            }

    };

    // START: ObjAstRuntimeFuncDef
    class ObjAstRuntimeFuncDef : public ObjBase {
        public:
        void* ast_node { nullptr };
        /* NOTE: In the beginning, there was an idea to use global Context* in order to avoid Context* member alignment of each instance. However, this does become difficult to maintain and error and bug-prone. */
        // Context* context { nullptr };

        ObjAstRuntimeFuncDef() : ObjBase() {}


        void set_node(void* ast_node) {
            this->ast_node = ast_node;
        }

        ~ObjAstRuntimeFuncDef() {

        }


    };

    inline ObjAstRuntimeFuncDef* Value::get_astruntime_func_def() const {
        assert(type == ValueType::AstRuntimeFuncDef);
        return obj_cast<ObjAstRuntimeFuncDef*>(obj.get_ptr());
    }

    // END: ObjAstRuntimeFuncDef



    using global_function_type = Value(*)(ObjFunction*, func_args); // this pointer and func_args


    class DataSegment;

    class DataSegment {
        public:
        struct GlobalFunctions {
            std::vector<SharedPtr> global_funcs;

            GlobalFunctions() {
                global_funcs = std::vector<SharedPtr>();
            }

            void push(ObjFunction* func) {
                global_funcs.push_back(SharedPtr(obj_cast<ObjBase*>(func)));
            }



        };

        // Global Text Data essentially, in other words
        struct GlobalStrings {
        };


        GlobalFunctions global_functions;


        void add_global_function(Context*, global_function_type);

        ObjFunction* get_global_function(size_t inx) {
            return obj_cast<ObjFunction*>(global_functions.global_funcs.at(inx).get_ptr());
        }

        void add_global_string() {
        }

    };

    class MemberFunctions;

    class MemberFunctions {
        public:
        using offset_table = hash_map<size_t, bind_function_type>;


        offset_table string_members;
        offset_table array_members;
        offset_table hash_table_members;

        MemberFunctions() {
            string_members = offset_table();
            array_members = offset_table();
            hash_table_members = offset_table();
        }

    };

    // SYMBOL LOOKUP TABLE IN ORDER TO REPLACE THE READ-ONLY STRINGS DATA SEGMENT
    // USING THIS METHOD, WE CAN USE INCREMENTAL REFERENCE COUNTING FOR ALL THE CONST_IDS AND MEMBER ATTRIBUTES AND CONSTANT LOOKUP WITHOUT CALCULTING ANY HASH VALUES
    // THE LOGIC IS AS FOLLOWS AT COMPILE TIME - WHETHER THAT IS THE BYTE COMPILER OR DURING THE PARSING, OR ANY OTHER CODE OPTIMIZATION PROCESS
    // if(is_offset_symbol_defined("a")) { } // do nothing
    // else context->offset_table["a"] = context->hash_incremental++; // insert_symbol_offset("a")
    // This "incremental" approach ensures perfect uniqueness without having to use perfect hash function or similar
    // WE ARE ALSO AIMING TO ENSURE THE (NEAR) CONSTANT IN-MEMORY LOOKUP OF VARIABLES AND MEMBER ATTRIBUTES AT RUNTIME - Check out the /Material/skarupke-benchmark for the benchmarks
    // This doesn't cause ANY conflicts because IDs and members, even though are of the same hash reference, have different lookup points in memory:
    // - Ids: the hash table of a scope (variable lookup)
    // - Members: the hash table within an object instance or the attached context
    using offset_symbol_lookup_table = hash_map<std::string, size_t>;
    // using offset_symbol_lookup_table = std::unordered_map<std::string, size_t>;

    template<class _CustomMemoryManagement = CustomMemoryManagement>
    class _Context : public _CustomMemoryManagement // Achieving Empty Base Optimization (EBO)
    {
        public:

        bool initialized { false };
        bool built_ins_set { false };

        struct BuiltIns {
            using scope_table = hash_map<size_t, Value>;
            // Lightweight built-in functions 
            // This is needed as the ObjFunction is referenced by pointer through Value so there is no ownership so we need to make sure all the funcs are still alive (lifecycle is still kept) in the running program without resulting in UB
            hash_map<std::string, SharedPtr> funcs; 
            scope_table scope;

            BuiltIns() {

            }

            void add_func(const std::string& func_name, const SharedPtr& func) {
                funcs[func_name] = func;
            }

            SharedPtr get_func(const std::string& func_name) {
                // TODO: SAFETY CHECK
                return funcs[func_name];
            }

            void add_sym(size_t sym, const Value& value) {
                scope[sym] = value;
            }

            Value get_sym(size_t sym) {
                scope_table::iterator entry = scope.find(sym);

                if(entry != scope.end()) {
                    // COPY CONSTRUCTOR INVOKED HERE OK
                    return entry->second;
                }

                return Null();
            }

        };


        BuiltIns builtins;


        DataSegment data_segment;
        MemberFunctions member_functions;
        offset_symbol_lookup_table offset_table;
        size_t hash_incremental {1}; // USE 0 AS THE NON-EXISTENT ITEM FOR THE OFFSET TABLE

        _Context() {
            offset_table = offset_symbol_lookup_table(); // NOT NEEDED BUT SOME HASH TABLE IMPLEMENTATIONS MIGHT REQUIRE IT IF THE CONSTRUCTOR IS EXPLICIT?
        }

        /*
        bool is_offset_symbol_defined(const std::string& sym) {
            offset_symbol_lookup_table::iterator entry = offset_table.find(sym);

            return entry != offset_table.end();
        }

        size_t insert_symbol_offset(const std::string& sym) {
            // NOTE: USED IN COMBINATION WITH is_offset_symbol_defined
            // ONLY IF is_offset_symbol_defined(sym) evaluates to false
            if(!is_offset_symbol_defined(sym)) {
                return (offset_table[sym] = hash_incremental++);
            }
            return 0; // NOT INSERTED
        }

        size_t get_symbol_offset(const std::string& sym) {
            offset_symbol_lookup_table::iterator entry = offset_table.find(sym);

            if(entry != offset_table.end()) {
                return entry->second;
            }

            return 0; // NOT-EXISTENT ITEM
        }
        */

        size_t insert_if_not_and_get_symbol_offset(const std::string& sym) {
            offset_symbol_lookup_table::iterator entry = offset_table.find(sym);

            // INSERT IF NOT PRESENT
            if(entry == offset_table.end()) {
                return (offset_table[sym] = hash_incremental++);
            }

            return entry->second;

        }

        // TODO: Perhaps, refactor to class Context: public ObjBase, public DataSegment?
        void add_global_function(Context* context, global_function_type func_ptr) {
            data_segment.add_global_function(context, func_ptr);
        }

        ObjFunction* get_global_function(size_t inx) {
            return data_segment.get_global_function(inx);
        }


        inline ObjArray* makeObjArray();
        inline SharedPtr makeSharedArray();
        inline Array makeArray();


        // inline String makeString(const String&);
        inline ObjString* makeObjString(String&&);
        inline SharedPtr makeSharedString(String&&);
        inline StringWrapper makeString(String&&);


        ObjFunction* makeObjFunction(global_function_type);
        SharedPtr makeSharedFunction(global_function_type);


        ObjNamespace* makeObjNamespace();
        SharedPtr makeSharedNamespace();
        Namespace makeNamespace();


        ObjAstRuntimeFuncDef* makeObjAstRuntimeFuncDef(void*);

    };


    // TODO: template<class T> _Context<T> ?
    template<>
    ObjAstRuntimeFuncDef* Context::makeObjAstRuntimeFuncDef(void* ast_node) {
        Context* current_context = this;
        ObjAstRuntimeFuncDef* obj_ast_runtime_func_def = 
                            this->custom_new<ObjAstRuntimeFuncDef>();

        obj_ast_runtime_func_def->set_node(ast_node);

        obj_ast_runtime_func_def->attach_context(current_context);

        return obj_ast_runtime_func_def;
    }


    // START: NAMESPACE DEF
    using namespace_look_up_table = hash_map<size_t, Value>;

    class ObjNamespace : public ObjBase {
        public:
        namespace_look_up_table namespace_table;
        // Context* context { nullptr };

        ObjNamespace() : ObjBase() {
            namespace_table = namespace_look_up_table();
        }


        ~ObjNamespace() {
        }

    };

    template<>
    ObjNamespace* Context::makeObjNamespace() {
        Context* current_context = this;

        ObjNamespace* obj_namespace = 
                                this->custom_new<ObjNamespace>();

        obj_namespace->attach_context(current_context);

        // assert(obj_str->ref_count() == 0);


        return obj_namespace;
    }

    template<>
    SharedPtr Context::makeSharedNamespace() {
        return SharedPtr(
            this->makeObjNamespace()
        );
    }

    template<>
    Namespace Context::makeNamespace() {
        return Namespace(
            this->makeSharedNamespace()
        );
    }
    // END: NAMESPACE DEF


    template<>
    inline ObjString* Context::makeObjString(String&& str) {
        Context* current_context = this;

        ObjString* obj_str = this->custom_new<ObjString>(std::move(str));


        obj_str->attach_context(current_context);

        // assert(obj_str->ref_count() == 0);


        return obj_str;
    }

    template<>
    inline SharedPtr Context::makeSharedString(String&& str) {
        return SharedPtr(
            makeObjString(std::move(str))
        );
    }

    template<>
    inline StringWrapper Context::makeString(String&& str) {
        return StringWrapper(
            makeSharedString(std::move(str))
        );
    }

    template<class T>
    using UniquePtr = std::unique_ptr<T>;

    Context* makeContext();
    UniquePtr<Context> makeUniqueContext();

    Context* makeContext() {

        Context* ctxptr = new Context();

        return ctxptr;
    }

    UniquePtr<Context> makeUniqueContext() {
        return UniquePtr<Context>(makeContext());
    }



    // Basic Global Function
    // NOTE: It is optimal that all objects inherit ObjBase as it is much more flexible to wrap them into Value whether as a direct pointer or SharedPtr
    class ObjFunction : public ObjBase {

        public:

        global_function_type func_ptr { nullptr };
        // Context* context { nullptr };

        ObjFunction(global_function_type func_ptr) : ObjBase(), func_ptr{func_ptr} {

        }


        // Invoker, essentially in other words
        // NOTE: always_inline is needed and safe to use here since the function is a one-liner essentially and plus it inlines for -O0 optimizations
        Value call(func_args args) __attribute__((always_inline)) {
            // NOTE: Move and Copy semantics are redundant for initializer_list. Referencing it means no move or copy involved whatsoever. It simply passes on the read-only pointer.
            // For more, refer to https://en.cppreference.com/w/cpp/utility/initializer_list.html
            return func_ptr(this, args);
        }


    };


    
    template<>
    ObjFunction* Context::makeObjFunction(global_function_type func_ptr) {

        Context* current_context = this;

        ObjFunction* func = this->custom_new<ObjFunction>(func_ptr);


        func->attach_context(current_context);

        return func;

    }

    template<>
    SharedPtr Context::makeSharedFunction(global_function_type func_ptr) {
        return SharedPtr(
            this->makeObjFunction(func_ptr)
        );
    }


    void DataSegment::add_global_function(Context* context, global_function_type func_ptr) {

        global_functions.push(
            context->makeObjFunction(func_ptr)
        );

    }

    Boolean makeBoolean(bool _bool) {
        return Boolean(_bool);
    }

    Number makeNumber(num_type n) {
        return Number(n);
    }


    Null makeNull() {
        return Null();
    }

    /*
    // ObjArray* arrptr = new ObjArray();
    // This line produces the following for whatever reason
In constructor ‘DeclAPI::SharedPtr::SharedPtr(DeclAPI::SharedPtr&&)’,
    inlined from ‘DeclAPI::FunctionBind::FunctionBind(DeclAPI::FunctionBind&&)’ at ./decl/lib/DeclAPI.hpp:532:13,
    inlined from ‘DeclAPI::Value& DeclAPI::Value::operator=(DeclAPI::Value&&)’ at ./decl/lib/DeclAPI.hpp:1894:17,
    inlined from ‘std::_Require<std::__not_<std::__is_tuple_like<_Tp> >, std::is_move_constructible<_Tp>, std::is_move_assignable<_Tp> > std::swap(_Tp&, _Tp&) [with _Tp = DeclAPI::Value]’ at /usr/include/c++/13/bits/move.h:199:11:
./decl/lib/DeclAPI.hpp:192:40: error: ‘__tmp.DeclAPI::Value::<unnamed>.DeclAPI::Value::._anon_97::obj.DeclAPI::SharedPtr::ptr’ may be used uninitialized [-Werror=maybe-uninitialized]
  192 |         SharedPtr(SharedPtr&& other) : ptr{other.ptr} {
      |                                        ^~~~~~~~~~~~~~
In file included from /usr/include/c++/13/bits/exception_ptr.h:41,
                 from /usr/include/c++/13/exception:164,
                 from /usr/include/c++/13/ios:41,
                 from /usr/include/c++/13/ostream:40,
                 from /usr/include/c++/13/iostream:41,
                 from tests/api/main.cpp:1:
/usr/include/c++/13/bits/move.h: In function ‘std::_Require<std::__not_<std::__is_tuple_like<_Tp> >, std::is_move_constructible<_Tp>, std::is_move_assignable<_Tp> > std::swap(_Tp&, _Tp&) [with _Tp = DeclAPI::Value]’:
/usr/include/c++/13/bits/move.h:197:11: note: ‘__tmp.DeclAPI::Value::<unnamed>.DeclAPI::Value::._anon_97::obj.DeclAPI::SharedPtr::ptr’ was declared here
  197 |       _Tp __tmp = _GLIBCXX_MOVE(__a);
      |           ^~~~~
cc1plus: all warnings being treated as errors
make: *** [Makefile:22: run_api_tests] Error 1
    */

    template<>
    inline ObjArray* Context::makeObjArray() {

        Context* current_context = this;


        ObjArray* arrptr = this->custom_new<ObjArray>();

        arrptr->attach_context(current_context);


        return arrptr;
    }


    template<>
    inline SharedPtr Context::makeSharedArray() {
        return SharedPtr(this->makeObjArray());
    }

    template<>
    inline Array Context::makeArray() {
        return Array(this->makeSharedArray());
    }

    #define function_always_inline __attribute__((always_inline))

    // START: DECL OPERATIONS

    inline Value callFunc(const char*, ObjFunction*, func_args) function_always_inline;
    inline Value callFunc(const char*, const FunctionBind&, func_args) function_always_inline;
    inline Value callFunc(const char*, const Value&, func_args) function_always_inline;

    inline Value plus(const char*, const Value&, const Value&);
    inline Value storeMember(const char*, const Value&, size_t, const Value&) function_always_inline;
    inline Value getMember(const char*, const Value&, size_t) function_always_inline;

    // END: DECL OPERATIONS

    // inline FunctionBind getMember(const char*, const Value&, size_t); // NOTE: OPTIMIZATION: PROBABLY FOR THE S2S COMPILER LEVEL GENERATION
    // inline FunctionBind getMember(const char*, const Value&, const char*); // NOTE: DEPRICATED

  
    /*
    // NOTE: DEPRICATED DUE TO THE Context::Offset_Table
    inline FunctionBind getMember(const char* message, const Value& value, const char* member_const_char) {
        return getMember(message, value, cacl_member_offset(member_const_char));
    }
    */

    // NOTE: Function Pointers for the built-in namespace references such sys.stdout, sys.getcwd, etc.
    namespace NamespaceFunctions {

        /*
        // NOT USING FUNCTION_BIND DUE TO THE SELF-REFENCING NAMESPACE BUG IF THE NAMESPACE IS THE ONE BOUND
        Value zoo(const char* message, ObjBase* obj_base, func_args args) {
            Value item = DECL_ARGS_GET(args, 0);

            return item;
        }
        */

        Value zoo(ObjFunction* _this, func_args args) {
            Value a = DECL_ARGS_GET(args, 0);

            return a;
        }


        Value _sys_stdout(ObjFunction* _this, func_args args) {
            for(size_t i = 0; i < args.size(); i++) {
                Value item = DECL_ARGS_GET(args, i);
                // TODO: print(item)
            }

            // TODO: Decide if we want to return 1 in this case ...
            return makeNumber(1);
            // return Null();
        }

        Value create_namespace(ObjFunction* _this, func_args args) {
            if(args.size() != 0) {
                // TODO: Throw?
            }
            return _this->context->makeNamespace();
        }

    }

    namespace MemberFunctionPointers {


        Value array_push(const char* message, ObjBase* obj_base, func_args args) {
            if(args.size() != 1) {
                // TODO: THROW message
                assert(args.size() == 1);
            }
            Value item = DECL_ARGS_GET(args, 0);

            ObjArray* arr = obj_cast<ObjArray*>(obj_base);


            return arr->push(item);

        }

        Value string_append(const char* message, ObjBase* obj_base, func_args args) {
            if(args.size() != 1) {
                // TODO: THROW message
                assert(args.size() == 1);
            }
            Value item = DECL_ARGS_GET(args, 0);

            ObjString* str = obj_cast<ObjString*>(obj_base);

            assert(item.type == ValueType::String);


            str->str._str.append(item._cast_string_ptr()->str._str);

            return Value(Null());

        }
    } // MemberFunctionPointers


    /*
    namespace MemberSymbols {
        const char* _push = "push";
        const char* _append = "append";

    } // MemberSymbols
    */

    #define cacl_member_offset(member_symbol) reinterpret_cast<size_t>(member_symbol) 

    // NOTE: Initialize the Decl Environment
    void setUpContext(Context* context) {

        #define ASSIGN_ARRAY_MEMBER(member_symbol, value) context->member_functions.array_members[cacl_member_offset(member_symbol)] = value
        #define ASSIGN_STRING_MEMBER(member_symbol, value) context->member_functions.string_members[cacl_member_offset(member_symbol)] = value


        // ADDING THE BUILT-IN SYMBOLS. The hash_incremental just keeps incrementing for the new symbols at compile time.
        size_t push_sym_offset = context->insert_if_not_and_get_symbol_offset("push"); // (context->offset_table["push"] = context->hash_incremental++);
        size_t append_sym_offset = context->insert_if_not_and_get_symbol_offset("append"); // (context->offset_table["append"] = context->hash_incremental++);


        assertm(
            "ENSURE BUILT-IN SYMBOLS ARE CORRECTLY DEFINED AND INSERTED",
            push_sym_offset != 0 &&
            append_sym_offset != 0
        );

        ASSIGN_ARRAY_MEMBER(
            // MemberSymbols::_push,
            push_sym_offset,
            MemberFunctionPointers::array_push
        );

        ASSIGN_STRING_MEMBER(
            // MemberSymbols::_append,
            append_sym_offset,
            MemberFunctionPointers::string_append
        );



        context->initialized = true;

        #undef ASSIGN_ARRAY_MEMBER 

    }

    void setUpBuilts(Context* context) {

        Context::BuiltIns& builtins = context->builtins; // alias

        // START: FUNCTIONS

        builtins.add_func(
            "zoo_func",
            context->makeSharedFunction(NamespaceFunctions::zoo)
        );

        builtins.add_func(
            "stdout",
            context->makeSharedFunction(NamespaceFunctions::_sys_stdout)
        );

        builtins.add_func(
            "namespace",
            context->makeSharedFunction(NamespaceFunctions::create_namespace)
        );

        // END: FUNCTIONS

        // START: Sys namespace
        Value sys = context->makeNamespace();

        Value zoo = context->makeNamespace(); // FOR TESTING NESTED NAMESPACE such as sys.zoo.zoo_func()


        // zoo.zoo_func = () => {}
        storeMember(
            "LINE 1886",
            zoo,
            context->insert_if_not_and_get_symbol_offset("zoo_func"),
            obj_cast<ObjFunction*>(
                builtins.get_func("zoo_func").get_ptr()
            )
        );

        // sys.zoo = zoo
        storeMember(
            "LINE 1886",
            sys,
            context->insert_if_not_and_get_symbol_offset("zoo"),
            zoo
        );

        // sys.stdout = () => {}
        storeMember(
            "LINE 1886",
            sys,
            context->insert_if_not_and_get_symbol_offset("stdout"),
            obj_cast<ObjFunction*>(
                builtins.get_func("stdout").get_ptr()
            )
        );

        // END: Sys namespace


        // START: ADDING SYMBOLS

        builtins.add_sym(
            context->insert_if_not_and_get_symbol_offset("sys"),
            sys
        );

        builtins.add_sym(
            context->insert_if_not_and_get_symbol_offset("namespace"),
            obj_cast<ObjFunction*>(
                builtins.get_func("namespace").get_ptr()
            )
        );


        // END: ADDING SYMBOLS

        context->built_ins_set = true;
    }






} // namespace DeclAPI


// DEFINITIONS OF the DeclAPI namespace
// Value/SharedPtr Constructors and Assignments
namespace DeclAPI {

    inline void SharedPtr::release_ref()
    {
        if(ptr == nullptr) {
            return;
        }

        ptr->dec_ref();

        if (ptr->ref_count() == 0)
        {
            ObjBase* derived = ptr; // obj_cast<T*>(ptr);

            // alloc.destroy(derived); // derived->~T();

            // size_t __n is Unused unless __cpp_sized_deallocation is defined.
            // See: https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/bits/new_allocator.h#L156
            // Simplified: https://gcc.gnu.org/onlinedocs/gcc-4.6.3/libstdc++/api/a00958_source.html
            // alloc.deallocate(derived, 1); // ::operator delete(derived);

            this->ptr->context->custom_delete(derived);
        }

        // Moved to release_ref_and_nullify for micro optimization
        // ptr = nullptr;

    }


    inline Value::Value(Boolean&& _boolean) {
        new(&boolean) Boolean(std::move(_boolean));
        type = ValueType::Boolean;
    }

    inline Value::Value(Number&& number) {
        // operator= for primitive values is ALSO OK for unions. However, since this is actually a Wrapper class - we are totally OK.
        // num = number;
        new(&num) Number(std::move(number));
        type = ValueType::Number;
    }

    inline Value::Value(Null&&) {
        type = ValueType::Null;
    }

    inline Value::Value(Array&& array) {
        new(&obj) SharedPtr(std::move(array.obj));
        type = ValueType::Array;
    }

    inline Value::Value(StringWrapper&& str_wrapper) {
        new(&obj) SharedPtr(
            std::move(str_wrapper.obj)
            // new ObjString(std::move(str))
        );

        type = ValueType::String;

    }

    inline Value::Value(Namespace&& _namespace) {
        new(&obj) SharedPtr(
            std::move(_namespace.obj)
        );
        type = ValueType::Namespace;
    }

    inline Value::Value(FunctionBind&& function_bind) {
        new(&this->function_bind) FunctionBind(
            std::move(function_bind)
        );
        // this->function_bind.obj = SharedPtr(nullptr);
        // this->function_bind.function_bind_p = nullptr;

        type = ValueType::FunctionBind;
        // function_bind.function_bind_p = nullptr;
    }

    inline Value::Value(ObjFunction* function) {
        // Assignment OK with Primitives if within a union
        this->func = function;
        type = ValueType::Function;
    }

    inline Value::Value(ObjAstRuntimeFuncDef* ast_runtime_func_def) {
        type = ValueType::AstRuntimeFuncDef;
        new(&obj) SharedPtr(
            ast_runtime_func_def
        );
    }


    inline Value::Value(const Value& lvalue) {
        type = lvalue.type;
        switch (type)
        {
            case ValueType::Boolean: {
                /*
                std::cout << "START: AAAAA" << std::endl;
                std::cout << (this->boolean._bool) << std::endl;
                std::cout << &this->boolean << std::endl;
                std::cout << "END: AAAAA" << std::endl;
                // boolean = Boolean(false);
                */
                new(&boolean) Boolean(lvalue.boolean);
                break;
            }
            case ValueType::Number:
            {
                new(&num) Number(lvalue.num);
                break;
            }
            case ValueType::String: {
                ObjString* l_str_obj = lvalue._cast_string_ptr();
                ObjString* str_obj = l_str_obj->context->custom_new<ObjString>(
                    l_str_obj->str
                );
                // NOTE: RE-ATTACH THE CONTEXT SINCE WE NEED TO CREATE A NEW STRING OBJECT
                str_obj->attach_context(l_str_obj->context);
                new(&obj) SharedPtr(
                    str_obj
                );
                break;
            }
            case ValueType::Array: case ValueType::HashTable: case ValueType::Namespace: {
               new(&obj) SharedPtr(lvalue.obj);
               break;
            }
            case ValueType::FunctionBind: {
                new(&function_bind) FunctionBind(
                    lvalue.function_bind
                );
                break;
            }
            case ValueType::Function: {
                // Assignment OK with Primitives if within a union
                this->func = lvalue.func;
                break;
            }
            case ValueType::AstRuntimeFuncDef: { // HAVING THE separate/standalone ASTRuntime branch
               new(&obj) SharedPtr(lvalue.obj);
               break;
            }
            default:
            {
                break;
            }
        }

    }

    inline Value::Value(Value&& rvalue) {
        type = rvalue.type;
        switch (type)
        {
            case ValueType::Boolean: {
                new(&boolean) Boolean(std::move(rvalue.boolean));
                break;
            }
            case ValueType::Number:
            {
                new(&num) Number(std::move(rvalue.num));
                break;
            }
            case ValueType::Array: case ValueType::HashTable: case ValueType::String: case ValueType::Namespace: {
                new(&obj) SharedPtr(std::move(rvalue.obj));
                break;
            }
            case ValueType::FunctionBind: {
                // assert(rvalue.type == ValueType::FunctionBind);

                // new(&rvalue.function_bind) FunctionBind(); // THIS LINE COMPLETELY FIXES THE SKARUPKE "MAY BE UNITIALIZED" WARNING. Good clue as far as how to potentially debug this

                new(&function_bind) FunctionBind(
                    std::move(rvalue.function_bind)
                );
                break;
            }
            case ValueType::Function: {
                // Assignment OK with Primitives if within a union
                this->func = rvalue.func;
                break;
            }
            case ValueType::AstRuntimeFuncDef: { // HAVING THE separate/standalone ASTRuntime branch
               new(&obj) SharedPtr(std::move(rvalue.obj));
               break;
            }
            /*
            case ValueType::Null: { // BETTER OFF WITH CONDITIONAL JMP BUT IF THIS IS ENABLED, IT IS CONDITIONAL JMP AND INDIRECT JMP
                break;
            }
            */
            default:
            {
                break;
            }
        }

        // NOTE: Nullify the moved Value&& value
        // Note that it is not necessary to swap the Value (union) member with nullptr, or similar. based on its ValueType since ~Value() simply ignores "ValueType::Null" so while the union member may be "hanging", it is never ACTUALLY USED afterwards
        rvalue.type = ValueType::Null;
    }

    inline Value& Value::operator=(const Value& lvalue) {

        if (this == &lvalue)
        {
            return *this;
        }

        switch (type) {
            /*
                PRIMITIVE_VALUES_SWITCH_CASE: {
                break;
            }
            */
            case ValueType::Array:
            case ValueType::HashTable:
            case ValueType::String:
            case ValueType::Namespace:
            case ValueType::AstRuntimeFuncDef:
            {
                obj.release_ref_and_nullify();
                break;
            }
            case ValueType::FunctionBind: {
                function_bind.~FunctionBind();
                break;
            }
            default:
            {
                break;
                // _BRANCH_UNREACHABLE();
            }
        }

        type = lvalue.type;

        switch (type) {

            case ValueType::Boolean:
            {
                new(&boolean) Boolean(lvalue.boolean);
                break;
            }
            case ValueType::Number:
            {
                new(&num) Number(lvalue.num);
                break;
            }
            case ValueType::String: {
                ObjString* l_str_obj = lvalue._cast_string_ptr();
                ObjString* str_obj = l_str_obj->context->custom_new<ObjString>(
                    l_str_obj->str
                );
                // NOTE: RE-ATTACH THE CONTEXT SINCE WE NEED TO CREATE A NEW STRING OBJECT
                str_obj->attach_context(l_str_obj->context);
                new(&obj) SharedPtr(
                    str_obj
                );

                break;
            }
            case ValueType::Array:
            case ValueType::HashTable:
            case ValueType::Namespace:
            {
                new(&obj) SharedPtr(lvalue.obj);
                break;
            }
            case ValueType::FunctionBind: {
                new(&function_bind) FunctionBind(
                    lvalue.function_bind
                );
                break;
            }
            case ValueType::Function: {
                // Assignment OK with Primitives if within a union
                this->func = lvalue.func;
                break;
            }
            case ValueType::AstRuntimeFuncDef: { // HAVING THE separate/standalone ASTRuntime branch
               new(&obj) SharedPtr(lvalue.obj);
               break;
            }
            default:
            {
                break;
            }
        }

        return *this;
    }

    // NOTE: TODO MUST: In order to, in some cases, avoid the jump table overhead, implement operator=(SharedPtr&&) optimization, (and operator=(Array*) more or less but that won't work for Array* since our Array will be hanging as rvalue if NOT USED where it can live on)
    // NOTE: MIGHT NOT BE NEEDED AFTER ALL DUE TO "START: OPTIMIZATIONS". HOWEVER, TO MAKE THE INTERFACE C++ FRIENDLY, WE ALSO IMPLEMENT the Value move assignment.
    Value& Value::operator=(Value&& rvalue) noexcept {

        // NOTE: MIGHT NOT BE NEEDED FOR operator=(Value&&) SINCE DECL WON'T ALLOW, AT COMPILE TIME, RVALUE = RVALUE, ONLY LVALUE = RVALUE
        // NOTE: STOPPED BY THE COMPILER ANYWAY
        // tests/api/main.cpp:361:21: error: moving ‘arr’ of type ‘DeclAPI::SharedPtr’ to itself [-Werror=self-move]
        //  361 |                 arr = std::move(arr);
        if (this == &rvalue)
        {
            return *this;
        }

        switch(type) {
            /*
            PRIMITIVE_VALUES_SWITCH_CASE: {
                break;
            }
            */
            case ValueType::Array:
            case ValueType::HashTable:
            case ValueType::String:
            case ValueType::Namespace:
            case ValueType::AstRuntimeFuncDef:
            {
                obj.release_ref_and_nullify();
                break;
            }
            case ValueType::FunctionBind: {
                function_bind.~FunctionBind();
                break;
            }
            default: {
                break;
                // _BRANCH_UNREACHABLE();
            }

        }

        type = rvalue.type;

        switch(type) {
            case ValueType::Boolean: {
                new(&boolean) Boolean(std::move(rvalue.boolean));
                break;
            }
            case ValueType::Number: {
                new(&num) Number(std::move(rvalue.num));
                break;
            }
            case ValueType::Array: case ValueType::HashTable: case ValueType::String: case ValueType::Namespace: {
                new(&obj) SharedPtr(std::move(rvalue.obj));
                break;
            }
            case ValueType::FunctionBind: {
                // assert(rvalue.type == ValueType::FunctionBind);
                // function_bind = std::move(rvalue.function_bind);
                new(&function_bind) FunctionBind(
                    std::move(rvalue.function_bind)
                );
                break;
            }
            case ValueType::Function: {
                // Assignment OK with Primitives if within a union
                this->func = rvalue.func;
                break;
            }
            case ValueType::AstRuntimeFuncDef: { // HAVING THE separate/standalone ASTRuntime branch
               new(&obj) SharedPtr(std::move(rvalue.obj));
               break;
            }
            /*
            case ValueType::Null: {
                break;
            }
            */
            default: {
                break;
            }
        }

        // NOTE: Nullify the moved Value&& value
        // Note that it is not necessary to swap the Value (union) member with nullptr, or similar. based on its ValueType since ~Value() simply ignores "ValueType::Null" so while the union member may be "hanging", it is never ACTUALLY USED afterwards
        rvalue.type = ValueType::Null;

        return *this;
    }

            
    // START: OPTIMIZATIONS
    // NOTE: This can only be RVALUE since once you store a primitive into a Value, it is always of (class Value).

    Value& Value::operator=(Boolean&& _boolean) {
        switch(type) {
            /*
            PRIMITIVE_VALUES_SWITCH_CASE: {
                break;
            }
            */
            case ValueType::Array: case ValueType::HashTable: case ValueType::String: {
                obj.release_ref_and_nullify();
                break;
            }
            case ValueType::FunctionBind: {
                function_bind.~FunctionBind();
                break;
            }
            default: {
                break;
            }

        }

        type = ValueType::Boolean;

        new(&boolean) Boolean(std::move(_boolean));

        return *this;
    }

    Value& Value::operator=(Number&& number) {
        switch(type) {
            case ValueType::Array: case ValueType::HashTable: case ValueType::String: {
                obj.release_ref_and_nullify();
                break;
            }
            case ValueType::FunctionBind: {
                function_bind.~FunctionBind();
                break;
            }
            default: {
                break;
            }

        }

        type = ValueType::Number;

        new(&num) Number(std::move(number));

        return *this;
    }

    Value& Value::operator=(Array&& array) {
        switch(type) {
            case ValueType::Array: case ValueType::HashTable: case ValueType::String: {
                obj.release_ref_and_nullify();
                break;
            }
            default: {
                break;
            }

        }

        type = ValueType::Array;
        new(&obj) SharedPtr(std::move(array.obj));

        return *this;

    }
    // END: OPTIMIZATIONS

}

// DEFINITIONS OF _cast_*
namespace DeclAPI {

    inline ObjArray* Value::_cast_array_ptr() const {
        return obj_cast<ObjArray*>(obj.get_ptr());
    }

    inline ObjString* Value::_cast_string_ptr() const {
        return obj_cast<ObjString*>(obj.get_ptr());
    }

    inline ObjNamespace* Value::_cast_namespace_ptr() const {
        return obj_cast<ObjNamespace*>(obj.get_ptr());
    }

}

// DEFINITIONS OF Built-in Operators of Decl
namespace DeclAPI {

  inline Value callFunc(const char* message, ObjFunction* func_obj, func_args args) {
        // TODO: HANDLE MESSAGE BY THROWING AN EXCEPTION and message

        assert(func_obj->func_ptr != nullptr);

        return func_obj->call(args);
    }

    inline Value callFunc(const char* message, const FunctionBind& function_bind, func_args args) {
        // TODO: HANDLE MESSAGE BY THROWING AN EXCEPTION and message
        assert(function_bind.function_bind_p != nullptr);

        return function_bind.invoke(message, args);
    }

    inline Value callFunc(const char* message, const Value& value, func_args args) {
        ValueType t = value.type;

        switch(t) {
            case ValueType::FunctionBind: {
                return callFunc(message, value.function_bind, args);
            }
            case ValueType::Function: {
                return callFunc(message, value.func, args);
            }
            default: {
                break;
            }
        }

        // TODO: THROW: NOT A FUNCTION
        return Value(Null());

    }


    inline Value plus(const char* message, const Value& left, const Value& right) {


        /*
            switch(t) {
                ...
                case ValueType::Number: {
                    // THE SINGLE IF STATEMENT IS MORE EFFICIENT THAN THE SWITCH CASE IF WE DON'T NEED THE WHOLE SWITCH CASE. THIS IS BECAUSE THE SWITCH CASE IS "CONDITIONAL JMP + INDIRECT JMP" WHERE THE IF IS JUST A "CONDITIONAL JMP". PLUS, IT IS MUCH MORE EASIER TO MAINTAIN SINCE THERE IS NO NESTED SWITCH CASE.
                    // WE CAN DO THIS SINCE THE RIGHT VALUE IS REQUIRED TO BE OF THE TYPE OF THE LEFT VALUE IN DECL WHEN USING BUILT-IN OPERATORS SUCH AS PLUS, MINUS, MULTIPICATION, ETC. IN OTHER WORDS, WE CAN'T DO THE FOLLOWING IN DECL: 12 + "100".
                    // THIS IS SPECIFIED FOR THE PURPOSES OF CONSISTENCY OF THE LANGUAGE ITSELF AND PERFORMANCE.

                    if(right.type != ValueType::Number) {
                    }
                }
                default: {
                break;
                }
            }
        */

        ValueType t = left.type;



        switch(t) {
            case ValueType::Boolean: {
                break;
            }
            case ValueType::Null: {
                break;
            }
            case ValueType::Number: {
                if(right.type != ValueType::Number) {
                    throw RuntimeException(
                        message
                    );
                    // TODO: THROW
                }

                return left.num.plus(right.num);
            }
            case ValueType::String: {
                break;
            }
            case ValueType::Array: {
                break;
            }
            case ValueType::HashTable: {
                break;
            }
            default: {
                break;
               // _BRANCH_UNREACHABLE();
            }

        }

        // throw mayble?
        return Null();

    }


    inline Value storeMember(const char*, const Value& value, size_t member_offset, const Value& to_store) {
        ValueType t = value.type;

        switch(t) {
            case ValueType::Namespace: {
                ObjNamespace* _namespace = value._cast_namespace_ptr();
                // obj_cast<ObjNamespace*>(value.obj.get_ptr());
                namespace_look_up_table& namespace_table = _namespace->namespace_table;

                // namespace_table.insert({member_offset, to_store});
                namespace_table[member_offset] = to_store;

                // return Null();
                return to_store;
            }
            default: {
                // TODO: throw exception or assert?
                break;
            }
        }

        return Null();

    }

    inline Value getMember(const char* message, const Value& value, size_t member_offset) {

        ValueType t = value.type;

        switch(t) {
            case ValueType::String: {
                ObjString* obj_str = obj_cast<ObjString*>(value.obj.get_ptr());

                MemberFunctions::offset_table& string_members = obj_str->context->member_functions.string_members;

                MemberFunctions::offset_table::iterator entry = string_members.find(member_offset);

                if(entry != string_members.end()) {
                    return FunctionBind(value.obj, entry->second);
                }

                // TODO: THROW message?
                // return FunctionBind();
                return Null();

            }
            case ValueType::Array: {
                ObjArray* obj_arr = obj_cast<ObjArray*>(value.obj.get_ptr());

                MemberFunctions::offset_table& array_members = obj_arr->context->member_functions.array_members;

                MemberFunctions::offset_table::iterator entry = array_members.find(member_offset);

                if(entry != array_members.end()) {
                    return FunctionBind(value.obj, entry->second);
                }

                // TODO: THROW message?
                // return FunctionBind();
                return Null();

            }
            case ValueType::HashTable: {
                break;
            }
            case ValueType::Namespace: {
                ObjNamespace* _namespace = obj_cast<ObjNamespace*>(value.obj.get_ptr());
                namespace_look_up_table& namespace_table = _namespace->namespace_table;

                namespace_look_up_table::iterator entry = namespace_table.find(member_offset);

                if(entry != namespace_table.end()) {
                    // COPY CONSTRUCTOR INVOKED HERE
                    return entry->second;
                }

                return Null();
            }
            default: {
                // TODO: THROW message
                break;
            }
        }

        return Null();

    }

}

#endif
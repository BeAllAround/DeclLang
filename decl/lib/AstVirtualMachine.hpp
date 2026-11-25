
#ifndef HEADER_DECL_AST_VIRTUAL_MACHINE // OR #pragma once

#define HEADER_DECL_AST_VIRTUAL_MACHINE


#include <iostream>
#include <string>
#include <vector>

#include <cassert>

#define DECL_COMPILER_VERBOSE

#define DECL_VERBOSE_LOGGING

#define DECL_LOG_INFO

#include <decl/lib/Utility.hpp>

#include <decl/lib/AstParser.hpp>

#include <decl/lib/DeclAPI.hpp>

using namespace DeclAPI;
using namespace AstParser;

namespace AstVirtualMachine {
  inline Value plus(const LexAnalysis::Token&, const Value&, const Value&) function_always_inline;

  // Moving all the necessary DECL Operations Function for the AstVirtualMachine alone - as it is separate from the S2S Usage.
  // In this case, we need to pass around the token instead of builing the error message at the time of the DeclAPI operation - which would be really slow and the error message is supposed to be built at the time of throwing the exception determined by the if statement: The "Built it only when you NEED TO THROW IT" approach
  // TODO: For this reason, I need to introduce the DeclAPI::S2S namespace
  // THERE ARE NO EXPLITIC C++ UNIT TESTS FOR THIS. HOWEVER, WE UNIT TEST EVERYTHING THROUGH /tests/astruntime/main.cpp
  inline Value plus(const LexAnalysis::Token& token, const Value& left, const Value& right) {

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
                    "Right operand not a number at " + std::to_string(token.get_line()) // TODO: PUT TOGETHER A BETTER ERROR MESSAGE
                  );
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

}

namespace AstRuntime {

  // THE ENUM THAT USED TO IDENTIFY A SCOPE FROM THE std::vector<RuntimeScopeFrame> runtime_scope_stack
  enum class RuntimeScopeWhence {
    TOP_LEVEL, // Global variables, functions, and built-in vars, namespaces and functions
    IF,
    WHILE,
    FUNCTION,
  };

  // Scope_table is used for definitions/declarations of variables and function definitions
  // YIKES: Decl keyword is a bit ambigious since you are both declaring and definiting a variable ...
  using scope_table = hash_map<size_t, Value>;

  class RuntimeScopeFrame {
    public:
      RuntimeScopeWhence scope_whence;
      scope_table scope;

      RuntimeScopeFrame() {}

      RuntimeScopeFrame(RuntimeScopeWhence whence) : scope_whence{whence} {}

      // NO IS_DEFINED LOOKUP FUNCTION OR SIMILAR SINCE THIS WAY, WE ARE AVODING THE POTENTIAL DOUBLE-LOOK SO I JUST CHECK IF THE ENTRY IS OUT OF REACH WHEREEVER I NEED TO PERFORM THE LOOKUP
      scope_table::iterator out_of_reach() {
        return scope.end();
      }

      // MAYBE TODO: insert_def_entry(size_t, Value&&)


      void insert_def_entry(size_t sym, const Value& value) {
        // TODO: Is scope.insert more efficient?
        scope[sym] = value;
      }

  };

}


namespace AstVirtualMachine {
  using namespace AstRuntime;


  // TODO: HERE SET UP ALL THE BUILT-IN symbols, functions such as stdout, etc. NOTE WE GET ALL OF THE BUILT-IN STUFF FROM THE CONTEXT AND REINSERTED IT HERE

  // THE ENUM THAT THE EVAL RETURNS
  enum class RuntimeEntryWhence {
    TOP_LEVEL,
    RETURN,
    BREAK,
    CONTINUE,
    IGNORED, // NOTE: USED WHEN THE WHENCE DOESN'T MATTER
  };

  class RuntimeEntry {
    public:
      RuntimeEntryWhence whence;
      Value val;

      RuntimeEntry() {

      }

      RuntimeEntry(RuntimeEntryWhence whence, Value&& val) : whence{whence}, val{std::move(val)} {}

      RuntimeEntry(const RuntimeEntry& other) : whence{other.whence}, val{other.val} {}

      RuntimeEntry(RuntimeEntry&& other) : whence{other.whence}, val{std::move(other.val)} {}


      RuntimeEntry& operator=(const RuntimeEntry& other) {
        whence = other.whence;
        val = other.val;
        return *this;
      }

      RuntimeEntry& operator=(RuntimeEntry&& other) {
        whence = other.whence; // NOTE: PRIMITIVE SO NO STD::MOVE OR SWAPPING IT IN ANY WAY
        val = std::move(other.val);
        return *this;
      }
  };

  class AstEvaluator {
    public:
      std::vector<RuntimeScopeFrame> runtime_scope_stack;
      Context* context { nullptr };

      AstEvaluator() {

      }

      void attach_context(Context* context) {
        this->context = context;
      }

      void push_runtime_scope(RuntimeScopeFrame&& runtime_scope_frame) {
        runtime_scope_stack.push_back(std::move(runtime_scope_frame));
      }

      void pop_runtime_scope() {
        runtime_scope_stack.pop_back();
      }

      RuntimeScopeFrame& last() {
        assert(runtime_scope_stack.size() != 0);

        return runtime_scope_stack[
          runtime_scope_stack.size() - 1
        ];
      }

      void setUpBuilts(RuntimeScopeFrame& runtime_scope_frame) {
        Context::BuiltIns& builtsin = context->builtins;

        for(auto start = builtsin.scope.begin(); start != builtsin.scope.end(); start++) {
          size_t key_sym = start->first;
          const Value& value = start->second;
          runtime_scope_frame.insert_def_entry(key_sym, value);
        }

      }


      // evaluateProgramNode means that we are evaluting the entire program - not a single expression or similar
      RuntimeEntry evaluateProgramNode(const SmartASTNode& node) {
        RuntimeScopeFrame runtime_scope_frame = RuntimeScopeFrame(RuntimeScopeWhence::TOP_LEVEL);

        setUpBuilts(runtime_scope_frame); // NOTE: USING A FUNCTION IN CASE WE WANT TO ISOLATE setUpBuilts and set them up somewhere else in the code

        push_runtime_scope(std::move(runtime_scope_frame));

        RuntimeEntry runtime_entry = evaluateNode(node.get());

        pop_runtime_scope();

        return runtime_entry;

      }

      RuntimeEntry evaluateNode(ASTNode* node) {

        NodeType t = node->node_type;

        switch(t) {
          case NodeType::BLOCK: {
            NodeBlock* node_block = node_cast<NodeBlock*>(node);

            size_t block_size = node_block->block_size();

            for(size_t i = 0; i < block_size - 1; i++) {

            }

            // NOTE: INSTEAD OF DOING:
            // Value result;
            // for (...)  {
            //  result = eval(...)
            // }
            // return result;
            // DO THE FOLLOWING (with block_size-1) AS THE ABOVE IS THE UNNECESARY MOVE ASSIGNMENT

            Value result = evaluateNode(
              node_block->at(block_size - 1).get()
            ).val;

            return RuntimeEntry(
              RuntimeEntryWhence::TOP_LEVEL,
              std::move(result)
            );

          }
          // START: OPERATORS
          case NodeType::OP_ADDITIVE: {

            NodeAdditive *node_additive = node_cast<NodeAdditive *>(node);

            // NOTE: MAKING SURE TO EVALUATE FROM LEFT TO RIGHT
            RuntimeEntry left = evaluateNode(node_additive->left.get());
            RuntimeEntry right = evaluateNode(node_additive->right.get());
            // NOTE: In case of left or right error handling or simliar - having it as RuntimeEntry

            // Preventing the copy with using (const ref&) of val
            Value result = plus(
              node_additive->token, // TODO: Make an actual error message from the node token
              left.val,
              right.val
            );

            return RuntimeEntry(
              RuntimeEntryWhence::IGNORED,
              std::move(result)
            );
          }
          // END: OPERATORS
          case NodeType::CONST_NUM: {
            NodeNum* node_num = node_cast<NodeNum*>(node);
            num_type num_val = node_num->num_val;

            return RuntimeEntry(
              RuntimeEntryWhence::IGNORED,
              Value(makeNumber(num_val))
            );

          }
          default: {
            break;
          }
        }

        return RuntimeEntry(
            RuntimeEntryWhence::IGNORED,
            Null()
        );

      }

      // RuntimeEntry eval(ASTNode* node);

  };


}

#endif

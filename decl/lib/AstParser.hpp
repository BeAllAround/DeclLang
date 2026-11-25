

#ifndef HEADER_DECL_AST_PARSER

#define HEADER_DECL_AST_PARSER

#include <cassert>

#include <iostream>
#include <string>
#include <memory>

#include <deque>

#include <vector>

// #include <decl/lib/AstParserNamespace.hpp>

#include <decl/lib/LexAnalysis.hpp> // MAYBE DO THE FOLLOWING? #include "LexAnalysis.h" as it can be more efficient when looking up headers in the current working directory

#include <decl/lib/DeclAPI.hpp>



// Consider using reinterpret_cast
// TODO: Time it with reinterpret_cast
#define node_cast dynamic_cast
// #define node_cast reinterpret_cast


// AstParser Namespace Declarations for potentially reusing "ASTNode*" in the DeclAPI namespace for the ASTNode as union member. However, we can just use a void ptr: (void*) and do the cast based on the enum value anyways. However, even using the "ASTNode*" causes the shared_ptr ownership problems IF that ownership matters at all, in which case SmartASTNode is preferable - HOWEVER, GIVEN that the strict shared ownership is not critical at this point, the ptr type is a preference when using a union unless the sizeof(union) is already > sizeof(SmartASTNode) as it takes the size of the largest member. In conclusion, all of the aforementioned MUST be taken into consideration before making a decision.
// NOTE: This is without the "using namespace DeclAPI" so as to avoid the recursive include
namespace AstParser {

  using LexAnalysis::Token;
  using LexAnalysis::TokenType;

  

  class ASTNode;
  class NodeId;
  class NodeMultiply;

  template<class T>
  using ASTPtr = std::unique_ptr<T>;
  // using ASTPtr = std::shared_ptr<T>;

  // This can be done since template type of shared_ptr is ptr*
  using SmartASTNode = ASTPtr<ASTNode>;

  // using ListType = std::deque<SmartASTNode>;
  using ListType = std::vector<SmartASTNode>;
  using SharedParamNode = ASTPtr<ListType>;

  /*
  template <typename... Args>
  SharedParamNode makeSharedParamNode(Args &&...args)
  {
    return std::make_shared<ListType>(std::forward<Args>(args)...);
  }
  */

  #if __cplusplus == 201103L
  template<class T, class ... Args>
  inline std::unique_ptr<T> make_unique (Args&&... args) {
    return std::unique_ptr<T>( new T( std::forward<Args>(args) ... ));
  }
  #endif

  /*
    AstNode -> NodeLiteral -> Num, String, etc.
    AstNode -> NodeExpression -> Multiply, Divide, Subtract
  */

  template<class T, class ... Args>
  inline ASTPtr<T> make_node (Args&&... args) {
    return std::unique_ptr<T>( new T( std::forward<Args>(args) ... ));
    // return std::make_shared<T>(std::forward<Args>(args)...);
  }


  // NOTE: Determine Value Types: Rvalue, lvalue, xvalue
  // This will done by the following criteria: if CONST:NUM -> Rvalue, if ID -> Lvalue (unless specified otherwise), if nodeMultiply -> Rvalue, if nodeSubscribe -> Lvalue etc.
  enum class ValueCategory {
    RVALUE,
    LVALUE,
    XVALUE,
  };

  ValueCategory determineNodeValue(const SmartASTNode&);


  enum class NodeType
  {

    CONST_ID,
    CONST_NUM,

    // Expressions
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_ADDITIVE,
    OP_SUBTRACT,

    OP_LESS_THAN,
    OP_GREATER_THAN,
    OP_GREATER_THAN_EQUAL_TO,
    OP_LESS_THAN_EQUAL_TO,
    OP_EQUAL_TO,
    OP_NOT_EQUAL_TO,

    OP_LOGICAL_AND,
    OP_LOGICAL_OR,
    OP_LOGICAL_NOT,
    OP_ASSIGNMENT,

    OP_UNARY_PLUS,
    OP_UNARY_MINUS,


    // Postfix Expressions
    CALL_FUNC,
    MEMBER_ACCESS,



    // BLOCK OF CODE NODE
    BLOCK,
    WHILE_BLOCK,
    IF_BLOCK,
    FUNCTION,
    DECL,

    // Other Keywords
    CONTINUE,
    BREAK,
    RETURN,

    TRUE,
    FALSE,
    NILL,


    INVALID,

  };

}


namespace AstParser {

  // NOTE: APPEND THE DeclAPI namespace to the AstParser when needed
  // NOTE: This will be needed should we ever transition to the decl/lib/AstParserNamespace.hpp refactor
  using namespace DeclAPI; 


  class ASTNode
  {
  public:
    NodeType node_type;
    Token token;

    ASTNode(NodeType, const Token&);

    // Not exactly needed if we only use make_node<Derived>() since make_node already creates a destructor handler (function pointer) with the "derived" type.
    // However, we do keep it for consistency. Virtual allows dynamic_cast since the derived class is polymorphic.
    // Otherwise, reinterpret_cast must be used. For this reason, we defined node_cast macro.
    // For more, see https://uvdn7.github.io/why-you-dont-need-virtual-destructor-with-smart-pointers/
    virtual ~ASTNode() {}
  };


  ASTNode::ASTNode(NodeType _node_type, const Token& token) : node_type{_node_type}, token{token} {}

  ValueCategory determineNodeValue(const SmartASTNode& node) {
    NodeType node_type = node.get()->node_type;

    switch (node_type)
    {
      /*
      case NodeType::CONST_NUM:
      case NodeType::OP_ADDITIVE:
      case NodeType::OP_MULTIPLY:
      case NodeType::OP_DIVIDE:
      {
        return ValueCategory::RVALUE;
      }
      */
      case NodeType::CONST_ID:
      {
        return ValueCategory::LVALUE;
      }
      default: {
        // return ValueCategory::XVALUE; // NO XVALUE FOR NOW // TODO: HOWEVER, SHOULD WE CALL THIS XVALUE?
        return ValueCategory::RVALUE;
      }

    }

  }


  class NodeId : public ASTNode
  {

  public:

    size_t symbol_ref {0}; // NOTE: If symbol_ref is zero, that means it is NOT ASSIGNED SINCE THE SYMBOL INCREMENTAL STARTS FROM 1

    NodeId(const Token& token) : ASTNode(NodeType::CONST_ID, token) {}

    void set_symbol_ref(size_t symbol_ref) {
      this->symbol_ref = symbol_ref;
    }

  };

  class NodeNum : public ASTNode
  {

  public:

    num_type num_val { 0 }; // TODO: DO Number num_val {0}; MAYBE?

    NodeNum(const Token& token) : ASTNode(NodeType::CONST_NUM, token) {}


    void set_num(const num_type& num) {
      this->num_val = num;
    }

  };

  class NodeAdditive : public ASTNode
  {

  public:
    // Parameter Nodes is essentially supposed to represent a Binary Tree with left and right but it is more flexible as we can insert more than two nodes on one level
    // It also represents the function arguments from left-to-right for the given expression
    // SharedParamNode param_nodes;
    SmartASTNode left;
    SmartASTNode right;

    NodeAdditive(const Token& token) : ASTNode(NodeType::OP_ADDITIVE, token)
    {
    }

    void insert_left(SmartASTNode && ast_node)
    {
      left = std::move(ast_node);
      // ListType* p_param_nodes = param_nodes.get();

      // TODO: Write a macro
      // p_param_nodes->push_front(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
      /*
      ListType *p_param_nodes = param_nodes.get();

      p_param_nodes->push_back(ast_node);
      */
    }
  };

  class NodeSubtract : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeSubtract(const Token& token) : ASTNode(NodeType::OP_SUBTRACT, token) {}

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }
  };

  class NodeMultiply : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeMultiply(const Token& token) : ASTNode(NodeType::OP_MULTIPLY, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }
  };

  class NodeDivide : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeDivide(const Token& token) : ASTNode(NodeType::OP_DIVIDE, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };

  class NodeLessThan : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeLessThan(const Token& token) : ASTNode(NodeType::OP_LESS_THAN, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };

  class NodeGreaterThan : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeGreaterThan(const Token& token) : ASTNode(NodeType::OP_GREATER_THAN, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };

  class NodeGreaterThanEqualTo : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeGreaterThanEqualTo(const Token& token) : ASTNode(NodeType::OP_GREATER_THAN_EQUAL_TO, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };

  class NodeLessThanEqualTo : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeLessThanEqualTo(const Token& token) : ASTNode(NodeType::OP_LESS_THAN_EQUAL_TO, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };

  class NodeEqualTo : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeEqualTo(const Token& token) : ASTNode(NodeType::OP_EQUAL_TO, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };

  class NodeNotEqualTo : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeNotEqualTo(const Token& token) : ASTNode(NodeType::OP_NOT_EQUAL_TO, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };

  class NodeLogicalAnd : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeLogicalAnd(const Token& token) : ASTNode(NodeType::OP_LOGICAL_AND, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };

  class NodeLogicalOr : public ASTNode
  {

  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeLogicalOr(const Token& token) : ASTNode(NodeType::OP_LOGICAL_OR, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };

  class NodeLogicalNot : public ASTNode
  {
  public:
    SmartASTNode expression;

    NodeLogicalNot(const Token& token) : ASTNode(NodeType::OP_LOGICAL_NOT, token)
    {
    }

    void set_expression(SmartASTNode &&ast_node)
    {
      expression = std::move(ast_node);
    }

  };

  class NodeUnaryMinus : public ASTNode
  {
  public:
    SmartASTNode expression;

    NodeUnaryMinus(const Token& token) : ASTNode(NodeType::OP_UNARY_MINUS, token)
    {
    }

    void set_expression(SmartASTNode &&ast_node)
    {
      expression = std::move(ast_node);
    }

  };

  class NodeUnaryPlus : public ASTNode
  {
  public:
    SmartASTNode expression;

    NodeUnaryPlus(const Token& token) : ASTNode(NodeType::OP_UNARY_PLUS, token)
    {
    }

    void set_expression(SmartASTNode &&ast_node)
    {
      expression = std::move(ast_node);
    }

  };

  class NodeAssignment : public ASTNode
  {
  public:
    SmartASTNode left;
    SmartASTNode right;

    NodeAssignment(const Token& token) : ASTNode(NodeType::OP_ASSIGNMENT, token)
    {
    }

    void insert_left(SmartASTNode &&ast_node)
    {
      left = std::move(ast_node);
    }

    void insert_right(SmartASTNode &&ast_node)
    {
      right = std::move(ast_node);
    }

  };


  class NodeFuncCall : public ASTNode
  {
  public:
    SmartASTNode callee;
    SharedParamNode args;

    NodeFuncCall(const Token& token) : ASTNode(NodeType::CALL_FUNC, token)
    {
      args = make_node<ListType>();
    }

    void set_callee(SmartASTNode &&ast_node)
    {
      callee = std::move(ast_node);
    }

    void add_arg(SmartASTNode &&ast_node)
    {
      (args.get())->push_back(
        std::move(ast_node)
      );
    }

  };


  class NodeGetMember : public ASTNode
  {
    public:
      SmartASTNode obj;
      SmartASTNode member_identifier;

      NodeGetMember(const Token& token) : ASTNode(NodeType::MEMBER_ACCESS, token) {}


  };


  // THIS IS CONSIDERED TO BE TOP-LEVEL BLOCK ON ITS OWN
  // I WILL INTRODUCE THE STATEMENT BLOCKS LATER ON
  // WHILE I AM REUSING THE SAME FUNCTION FOR PRINING OUT BLOCKS FOR PRINTING OUT WHILE BLOCK, THAT IS DONE PURELY FOR CONVINIENCE - THE TWO HAVE NOTHING TO DO WITH EACH OTHER AS WE ARE REUSING THE BODY MEMBER - NOT THE BLOCK ITSELF
  class NodeBlock : public ASTNode
  {
    public:
      SharedParamNode body;

      NodeBlock() : ASTNode(NodeType::BLOCK, Token()) {
        body = make_node<ListType>();
      }

      NodeBlock(const Token& token) : ASTNode(NodeType::BLOCK, token)
      {
        body = make_node<ListType>();
      }

      void push_back(SmartASTNode&& ast_node) {

        ListType* list = node_cast<ListType*>(body.get());

        list->push_back(
          std::move(ast_node)
        );

      }

      void set_body(SharedParamNode&& param_node) {
        body = std::move(param_node);
      }

      size_t block_size() {
        return node_cast<ListType*>(body.get())->size();
      }

      SmartASTNode& at(size_t i) { // SmartASTNode at(size_t) IS OK TOO DUE TO REFERENCE COUNTING BUT THE BENIFIT HERE IS THAT WE ARE NOT INCREMENTING THE REF COUNT
        return node_cast<ListType*>(body.get())->at(i);
      }

  };

  class NodeWhileBlock : public ASTNode
  {
    public:
      SmartASTNode condition;
      SharedParamNode body;

      NodeWhileBlock(const Token& token) : ASTNode(NodeType::WHILE_BLOCK, token)
      {
        body = make_node<ListType>();
      }

      void push_back(SmartASTNode&& ast_node) {

        ListType* list = node_cast<ListType*>(body.get());

        list->push_back(
          std::move(ast_node)
        );

      }

      void set_condition(SmartASTNode&& ast_node) {
        condition = std::move(ast_node);
      }

      void set_body(SharedParamNode&& param_node) {
        body = std::move(param_node);
      }

  };

  class NodeIfBlock : public ASTNode
  {
    public:
      SmartASTNode condition;
      SharedParamNode body;

      NodeIfBlock(const Token& token) : ASTNode(NodeType::IF_BLOCK, token)
      {
        body = make_node<ListType>();
      }

      void push_back(SmartASTNode&& ast_node) {

        ListType* list = node_cast<ListType*>(body.get());

        list->push_back(
          std::move(ast_node)
        );

      }

      void set_condition(SmartASTNode&& ast_node) {
        condition = std::move(ast_node);
      }

      void set_body(SharedParamNode&& param_node) {
        body = std::move(param_node);
      }

  };

  class NodeFunction : public ASTNode
  {
    public:
      std::string function_name;
      SharedParamNode params;
      SharedParamNode body;
      size_t symbol_ref {0};

      NodeFunction(const Token& token) : ASTNode(NodeType::FUNCTION, token)
      {
        body = make_node<ListType>();
        params = make_node<ListType>();
      }

      void push_back(SmartASTNode&& ast_node) {

        ListType* list = node_cast<ListType*>(body.get());

        list->push_back(
          std::move(ast_node)
        );

      }

      void add_param(SmartASTNode&& ast_node) {
        (params.get())->push_back(
          std::move(ast_node)
        );
      }

      void set_body(SharedParamNode&& param_node) {
        body = std::move(param_node);
      }

      void set_function_name(const std::string& function_name) {
        this->function_name = function_name;
      }

      void set_symbol_ref(size_t symbol_ref) {
        this->symbol_ref = symbol_ref;
      }
  };


  class NodeContinue : public ASTNode
  {
    public:

      NodeContinue(const Token& token) : ASTNode(NodeType::CONTINUE, token)
      {
      }

  };

  class NodeBreak : public ASTNode
  {
    public:

      NodeBreak(const Token& token) : ASTNode(NodeType::BREAK, token)
      {
      }

  };

  class NodeReturn : public ASTNode
  {
    public:
      SmartASTNode expression;

      NodeReturn(const Token& token) : ASTNode(NodeType::RETURN, token)
      {
      }

      void set_expression(SmartASTNode&& ast_node) {
        this->expression = std::move(ast_node);
      }

  };

  class NodeTrue : public ASTNode
  {
    public:

      NodeTrue(const Token& token) : ASTNode(NodeType::TRUE, token)
      {
      }

  };

  class NodeFalse : public ASTNode
  {
    public:

      NodeFalse(const Token& token) : ASTNode(NodeType::FALSE, token)
      {
      }

  };

  class NodeNill : public ASTNode
  {
    public:

      NodeNill(const Token& token) : ASTNode(NodeType::NILL, token)
      {
      }

  };

  class NodeDecl : public ASTNode
  {
    public:
      std::string decl_name;
      SmartASTNode expression;
      size_t symbol_ref {0};

      NodeDecl(const Token& token) : ASTNode(NodeType::DECL, token)
      {
      }

      void set_expression(SmartASTNode&& ast_node) {
        this->expression = std::move(ast_node);
      }

      void set_name(const std::string& decl_name) {
        this->decl_name = decl_name;
      }

      void set_symbol_ref(size_t symbol_ref) {
        this->symbol_ref = symbol_ref;
      }

  };

  class NodeParser
  {
  public:
    NodeParser() = default;


    std::string repeat_string(const std::string& str, size_t r) {
      std::string result;

      result.reserve(str.length() * r);

      for(size_t i = 0; i < r; i++) {
        result += str;
      }

      return result;
    }

    std::string node_to_string(const SmartASTNode& node) {

      // Starting indent count for inner children is the (user-defined) indent level
      std::string result = _node_to_string(node, indent_level);


      return result;

    }

    // NOTE: Unsafe and it requires switch case NodeType
    // TODO: Consider turning this into a macro or (__attribute__(always_inline))

    template<class BlockType = NodeBlock*>
    std::string block_node_to_string(ASTNode* node_ptr, const size_t indent_count) {
      BlockType node_block = node_cast<BlockType>(node_ptr);

      ListType *list = node_cast<ListType *>(node_block->body.get());

      std::string indent_string = repeat_string(" ", indent_count);

      std::string result = "Block=[";

      size_t list_size = list->size();

      for (size_t i = 0; i < list_size; i++)
      {

        result += "\n" + indent_string;
        result += _node_to_string(list->at(i), indent_count + indent_level);

        if (i < list_size - 1)
        {
          result += ",";
        }
      }

      if(list_size != 0) {
        // Closing backwards indent_count - indent_level
        result += "\n" + repeat_string(" ", indent_count - indent_level);
      }
      result += "]";

      return result;
    }

    std::string while_node_to_string(ASTNode* node_ptr, const size_t indent_count) {
      NodeWhileBlock* node_block = node_cast<NodeWhileBlock *>(node_ptr);

      std::string indent_string = repeat_string(" ", indent_count);

      std::string result = "WhileBlock=(";

      result += "\n" + indent_string;
      result += "Condition=(";
      result += _node_to_string(node_block->condition, indent_count + indent_level);
      result += ")";

      result += "\n" + indent_string;

      // START: Format the Block
      result += block_node_to_string<NodeWhileBlock*>(node_block, indent_count + indent_level);
      // END: Format the Block


      // Enclose WhileBlock
      // Closing backwards indent_count - indent_level
      result += "\n" + repeat_string(" ", indent_count - indent_level);
      result += ")";

      return result;
    }

    std::string if_node_to_string(ASTNode* node_ptr, const size_t indent_count) {
      NodeIfBlock* node_block = node_cast<NodeIfBlock *>(node_ptr);

      std::string indent_string = repeat_string(" ", indent_count);

      std::string result = "IfBlock=(";

      result += "\n" + indent_string;
      result += "Condition=(";
      result += _node_to_string(node_block->condition, indent_count + indent_level);
      result += ")";

      result += "\n" + indent_string;

      // START: Format the Block
      result += block_node_to_string<NodeIfBlock*>(node_block, indent_count + indent_level);
      // END: Format the Block


      // Enclose WhileBlock
      // Closing backwards indent_count - indent_level
      result += "\n" + repeat_string(" ", indent_count - indent_level);
      result += ")";

      return result;
    }

    std::string func_args_to_string(ListType* list, const size_t indent_count) {
      std::string indent_string = repeat_string(" ", indent_count);

      std::string args_result = "Args=(";

      size_t list_size = list->size();

      for (size_t i = 0; i < list_size; i++)
      {

        args_result += "\n" + indent_string;
        args_result += _node_to_string(list->at(i), indent_count + indent_level);

        if (i < list_size - 1)
        {
          args_result += ",";
        }
      }

      if(list_size != 0) {
        // Closing backwards indent_count - indent_level
        args_result += "\n" + repeat_string(" ", indent_count - indent_level);
      }

      args_result += ")";

      return args_result;
    }

    std::string func_params_to_string(ListType* list, const size_t indent_count) {
      std::string indent_string = repeat_string(" ", indent_count);

      std::string args_result = "Params=(";

      size_t list_size = list->size();

      for (size_t i = 0; i < list_size; i++)
      {

        args_result += "\n" + indent_string;
        args_result += _node_to_string(list->at(i), indent_count + indent_level);

        if (i < list_size - 1)
        {
          args_result += ",";
        }
      }

      if(list_size != 0) {
        // Closing backwards indent_count - indent_level
        args_result += "\n" + repeat_string(" ", indent_count - indent_level);
      }

      args_result += ")";

      return args_result;
    }

    std::string func_node_to_string(ASTNode* node_ptr, const size_t indent_count) {
      NodeFunction* node_func = node_cast<NodeFunction *>(node_ptr);

      std::string indent_string = repeat_string(" ", indent_count);

      std::string result = "FuncDef=(";
      result += "\n" + indent_string;
      result += "Name=\"" + node_func->function_name + '"';

      result += ",\n" + indent_string;
      // START: Format the Function Parameters
      ListType *list = node_cast<ListType *>(node_func->params.get());
      result += func_params_to_string(list, indent_count + indent_level);
      // END: Format the Function Parameters
      result += ",\n" + indent_string;

      result += block_node_to_string<NodeFunction*>(node_func, indent_count + indent_level);


      // Enclose FuncCall
      // Closing backwards indent_count - indent_level
      result += "\n" + repeat_string(" ", indent_count - indent_level);
      result += ")";

      // NOTE: SINCE WE ARE NOT EXACTLY UNIT TESTING THE NodeId symbol ref, IT IS SUITABLE THAT WE PUT IT HERE WE DISPLAY ALL THE NODES BEFORE WE UNIT-TEST/COMPARE THEIR AST CONTENT 
      assert(
        node_func->symbol_ref != 0
      );

      return result;
    }

    std::string func_call_node_to_string(ASTNode* node_ptr, const size_t indent_count) {
      NodeFuncCall* node_func_call = node_cast<NodeFuncCall *>(node_ptr);

      std::string indent_string = repeat_string(" ", indent_count);

      std::string result = "FuncCall=(";

      result += "\n" + indent_string;
      result += "Callee=(";
      result += _node_to_string(node_func_call->callee, indent_count + indent_level);
      result += ")";

      result += "\n" + indent_string;

      // START: Format the Function Arguments
      ListType *list = node_cast<ListType *>(node_func_call->args.get());
      result += func_args_to_string(list, indent_count + indent_level);
      // END: Format the Function Arguments


      // Enclose FuncCall
      // Closing backwards indent_count - indent_level
      result += "\n" + repeat_string(" ", indent_count - indent_level);
      result += ")";

      return result;
    }

    std::string _node_to_string(const SmartASTNode &node, const size_t indent_count)
    {
      std::string result;

      ASTNode *node_ptr = node.get();

      NodeType node_type = node_ptr->node_type;

      switch (node_type)
      {
      case NodeType::BLOCK:
      {
        return block_node_to_string(node_ptr, indent_count);
      }
      case NodeType::WHILE_BLOCK:
      {
        return while_node_to_string(node_ptr, indent_count);
      }
      case NodeType::IF_BLOCK: {
        return if_node_to_string(node_ptr, indent_count);
      }
      case NodeType::CALL_FUNC:
      {
        return func_call_node_to_string(node_ptr, indent_count);
      }
      case NodeType::FUNCTION: {
        return func_node_to_string(node_ptr, indent_count);
      }
      case NodeType::CONST_ID:
      {
        NodeId *node_id = node_cast<NodeId *>(node_ptr);
        
        std::string ID_result = "Id(";
        ID_result += "value=";
        ID_result += '"' + node_id->token.get_val() + '"';
        ID_result += ")";


        // NOTE: SINCE WE ARE NOT EXACTLY UNIT TESTING THE NodeId symbol ref, IT IS SUITABLE THAT WE PUT IT HERE WE DISPLAY ALL THE NODES BEFORE WE UNIT-TEST/COMPARE THEIR AST CONTENT 
        assert(
          node_id->symbol_ref != 0
        );

        result += ID_result;

        break;
      }
      case NodeType::CONST_NUM:
      {
        NodeNum *node_num = node_cast<NodeNum *>(node_ptr);

        // NOTE: WE ARE NOT UNIT TESTING WHETHER THE NUM_VAL IS ASSIGNED LIKE WE DO FOR SYM_REF DUE TO SOME DOUBLE 1.0000 problems and such. We are better off just displaying the token-verified number as a tokenized string
        // THIS UNIT TESTING TAKES PLACE IN ASTVIRTUALMACHINE
        std::string NUM_result = "Num(";
        NUM_result += "value=";
        NUM_result += '"' + node_num->token.get_val() + '"';
        NUM_result += ")";

        result += NUM_result;

        break;
      }
      case NodeType::OP_ADDITIVE:
      {
        NodeAdditive *node_additive = node_cast<NodeAdditive *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string ADD_result = "Add(";

        ADD_result += "\n" + indent_string;
        ADD_result += "left=";
        ADD_result += _node_to_string(node_additive->left, indent_count + indent_level);

        ADD_result += ", \n" + indent_string;
        ADD_result += "right=";
        ADD_result += _node_to_string(node_additive->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        ADD_result += "\n" + repeat_string(" ", indent_count - indent_level);
        ADD_result += ")";

        result += ADD_result;

        break;
      }
      case NodeType::OP_SUBTRACT:
      {
        NodeSubtract *node_subtract = node_cast<NodeSubtract *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string SUBTRACT_result = "Subtract(";

        SUBTRACT_result += "\n" + indent_string;
        SUBTRACT_result += "left=";
        SUBTRACT_result += _node_to_string(node_subtract->left, indent_count + indent_level);

        SUBTRACT_result += ", \n" + indent_string;
        SUBTRACT_result += "right=";
        SUBTRACT_result += _node_to_string(node_subtract->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        SUBTRACT_result += "\n" + repeat_string(" ", indent_count - indent_level);
        SUBTRACT_result += ")";

        result += SUBTRACT_result;

        break;
      }
      case NodeType::OP_MULTIPLY:
      {
        NodeMultiply *node_multiply = node_cast<NodeMultiply *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string MULTIPLY_result = "Multiply(";

        MULTIPLY_result += "\n" + indent_string;
        MULTIPLY_result += "left=";
        MULTIPLY_result += _node_to_string(node_multiply->left, indent_count + indent_level);

        MULTIPLY_result += ", \n" + indent_string;
        MULTIPLY_result += "right=";
        MULTIPLY_result += _node_to_string(node_multiply->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        MULTIPLY_result += "\n" + repeat_string(" ", indent_count - indent_level);
        MULTIPLY_result += ")";

        result += MULTIPLY_result;

        break;
      }
      case NodeType::OP_DIVIDE:
      {
        NodeDivide *node_divide = node_cast<NodeDivide *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string DIV_result = "Divide(";

        DIV_result += "\n" + indent_string;
        DIV_result += "left=";
        DIV_result += _node_to_string(node_divide->left, indent_count + indent_level);

        DIV_result += ", \n" + indent_string;
        DIV_result += "right=";
        DIV_result += _node_to_string(node_divide->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        DIV_result += "\n" + repeat_string(" ", indent_count - indent_level);
        DIV_result += ")";

        result += DIV_result;

        break;
      }
      case NodeType::OP_LESS_THAN:
      {
        NodeLessThan *node_less_than = node_cast<NodeLessThan *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string LESS_THAN_result = "RelationalLessThan(";

        LESS_THAN_result += "\n" + indent_string;
        LESS_THAN_result += "left=";
        LESS_THAN_result += _node_to_string(node_less_than->left, indent_count + indent_level);

        LESS_THAN_result += ", \n" + indent_string;
        LESS_THAN_result += "right=";
        LESS_THAN_result += _node_to_string(node_less_than->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        LESS_THAN_result += "\n" + repeat_string(" ", indent_count - indent_level);
        LESS_THAN_result += ")";

        result += LESS_THAN_result;

        break;
      }
      case NodeType::OP_GREATER_THAN:
      {
        NodeGreaterThan *node_greater_than = node_cast<NodeGreaterThan* >(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string GREATER_THAN_result = "RelationalGreaterThan(";

        GREATER_THAN_result += "\n" + indent_string;
        GREATER_THAN_result += "left=";
        GREATER_THAN_result += _node_to_string(node_greater_than->left, indent_count + indent_level);

        GREATER_THAN_result += ", \n" + indent_string;
        GREATER_THAN_result += "right=";
        GREATER_THAN_result += _node_to_string(node_greater_than->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        GREATER_THAN_result += "\n" + repeat_string(" ", indent_count - indent_level);
        GREATER_THAN_result += ")";

        result += GREATER_THAN_result;

        break;
      }
      case NodeType::OP_GREATER_THAN_EQUAL_TO:
      {
        NodeGreaterThanEqualTo *node_greater_than_equal_to = node_cast<NodeGreaterThanEqualTo *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string GREATER_THAN_EQUAL_TO_result = "RelationalGreaterThanEqualTo(";

        GREATER_THAN_EQUAL_TO_result += "\n" + indent_string;
        GREATER_THAN_EQUAL_TO_result += "left=";
        GREATER_THAN_EQUAL_TO_result += _node_to_string(node_greater_than_equal_to->left, indent_count + indent_level);

        GREATER_THAN_EQUAL_TO_result += ", \n" + indent_string;
        GREATER_THAN_EQUAL_TO_result += "right=";
        GREATER_THAN_EQUAL_TO_result += _node_to_string(node_greater_than_equal_to->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        GREATER_THAN_EQUAL_TO_result += "\n" + repeat_string(" ", indent_count - indent_level);
        GREATER_THAN_EQUAL_TO_result += ")";

        result += GREATER_THAN_EQUAL_TO_result;

        break;
      }
      case NodeType::OP_LESS_THAN_EQUAL_TO:
      {
        NodeLessThanEqualTo *node_less_than_equal_to = node_cast<NodeLessThanEqualTo *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string LESS_THAN_EQUAL_TO_result = "RelationalLessThanEqualTo(";

        LESS_THAN_EQUAL_TO_result += "\n" + indent_string;
        LESS_THAN_EQUAL_TO_result += "left=";
        LESS_THAN_EQUAL_TO_result += _node_to_string(node_less_than_equal_to->left, indent_count + indent_level);

        LESS_THAN_EQUAL_TO_result += ", \n" + indent_string;
        LESS_THAN_EQUAL_TO_result += "right=";
        LESS_THAN_EQUAL_TO_result += _node_to_string(node_less_than_equal_to->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        LESS_THAN_EQUAL_TO_result += "\n" + repeat_string(" ", indent_count - indent_level);
        LESS_THAN_EQUAL_TO_result += ")";

        result += LESS_THAN_EQUAL_TO_result;

        break;
      }
      case NodeType::OP_EQUAL_TO:
      {
        NodeEqualTo *node_equal = node_cast<NodeEqualTo *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string EQUAL_result = "RelationalEqualTo(";

        EQUAL_result += "\n" + indent_string;
        EQUAL_result += "left=";
        EQUAL_result += _node_to_string(node_equal->left, indent_count + indent_level);

        EQUAL_result += ", \n" + indent_string;
        EQUAL_result += "right=";
        EQUAL_result += _node_to_string(node_equal->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        EQUAL_result += "\n" + repeat_string(" ", indent_count - indent_level);
        EQUAL_result += ")";

        result += EQUAL_result;

        break;
      }
      case NodeType::OP_NOT_EQUAL_TO:
      {
        NodeNotEqualTo *node_not_equal = node_cast<NodeNotEqualTo *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string NOT_EQUAL_result = "RelationalNotEqualTo(";

        NOT_EQUAL_result += "\n" + indent_string;
        NOT_EQUAL_result += "left=";
        NOT_EQUAL_result += _node_to_string(node_not_equal->left, indent_count + indent_level);

        NOT_EQUAL_result += ", \n" + indent_string;
        NOT_EQUAL_result += "right=";
        NOT_EQUAL_result += _node_to_string(node_not_equal->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        NOT_EQUAL_result += "\n" + repeat_string(" ", indent_count - indent_level);
        NOT_EQUAL_result += ")";

        result += NOT_EQUAL_result;

        break;
      }
      case NodeType::OP_LOGICAL_AND:
      {
        NodeLogicalAnd *node_logical_and = node_cast<NodeLogicalAnd *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string LOGICAL_AND_result = "LogicalAnd(";

        LOGICAL_AND_result += "\n" + indent_string;
        LOGICAL_AND_result += "left=";
        LOGICAL_AND_result += _node_to_string(node_logical_and->left, indent_count + indent_level);

        LOGICAL_AND_result += ", \n" + indent_string;
        LOGICAL_AND_result += "right=";
        LOGICAL_AND_result += _node_to_string(node_logical_and->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        LOGICAL_AND_result += "\n" + repeat_string(" ", indent_count - indent_level);
        LOGICAL_AND_result += ")";

        result += LOGICAL_AND_result;

        break;
      }
      case NodeType::OP_LOGICAL_OR:
      {
        NodeLogicalOr *node_logical_or = node_cast<NodeLogicalOr *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string LOGICAL_OR_result = "LogicalOr(";

        LOGICAL_OR_result += "\n" + indent_string;
        LOGICAL_OR_result += "left=";
        LOGICAL_OR_result += _node_to_string(node_logical_or->left, indent_count + indent_level);

        LOGICAL_OR_result += ", \n" + indent_string;
        LOGICAL_OR_result += "right=";
        LOGICAL_OR_result += _node_to_string(node_logical_or->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        LOGICAL_OR_result += "\n" + repeat_string(" ", indent_count - indent_level);
        LOGICAL_OR_result += ")";

        result += LOGICAL_OR_result;

        break;
      }
      case NodeType::OP_ASSIGNMENT:
      {
        NodeAssignment *node_assignment = node_cast<NodeAssignment*>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string ASSIGNMENT_result = "AssignmentExpression(";

        ASSIGNMENT_result += "\n" + indent_string;
        ASSIGNMENT_result += "left=";
        ASSIGNMENT_result += _node_to_string(node_assignment->left, indent_count + indent_level);

        ASSIGNMENT_result += ", \n" + indent_string;
        ASSIGNMENT_result += "right=";
        ASSIGNMENT_result += _node_to_string(node_assignment->right, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        ASSIGNMENT_result += "\n" + repeat_string(" ", indent_count - indent_level);
        ASSIGNMENT_result += ")";

        result += ASSIGNMENT_result;

        break;
      }
      case NodeType::OP_LOGICAL_NOT:
      {
        NodeLogicalNot *node_logical_not = node_cast<NodeLogicalNot*>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string LOGICAL_NOT_result = "LogicalNot(";

        LOGICAL_NOT_result += "\n" + indent_string;
        LOGICAL_NOT_result += "expr=";
        LOGICAL_NOT_result += _node_to_string(node_logical_not->expression, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        LOGICAL_NOT_result += "\n" + repeat_string(" ", indent_count - indent_level);
        LOGICAL_NOT_result += ")";

        result += LOGICAL_NOT_result;

        break;
      }
      case NodeType::OP_UNARY_MINUS:
      {
        NodeUnaryMinus *node_unary_minus = node_cast<NodeUnaryMinus*>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string UNARY_MINUS_result = "UnaryMinus(";

        UNARY_MINUS_result += "\n" + indent_string;
        UNARY_MINUS_result += "expr=";
        UNARY_MINUS_result += _node_to_string(node_unary_minus->expression, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        UNARY_MINUS_result += "\n" + repeat_string(" ", indent_count - indent_level);
        UNARY_MINUS_result += ")";

        result += UNARY_MINUS_result;

        break;
      }
      case NodeType::OP_UNARY_PLUS:
      {
        NodeUnaryPlus *node_unary_plus = node_cast<NodeUnaryPlus*>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string UNARY_PLUS_result = "UnaryPlus(";

        UNARY_PLUS_result += "\n" + indent_string;
        UNARY_PLUS_result += "expr=";
        UNARY_PLUS_result += _node_to_string(node_unary_plus->expression, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        UNARY_PLUS_result += "\n" + repeat_string(" ", indent_count - indent_level);
        UNARY_PLUS_result += ")";

        result += UNARY_PLUS_result;

        break;
      }
      case NodeType::BREAK: {
        result += "Break()";
        break;
      }
      case NodeType::CONTINUE: {
        result += "Continue()";
        break;
      }
      case NodeType::TRUE: {
        result += "True()";
        break;
      }
      case NodeType::FALSE: {
        result += "False()";
        break;
      }
      case NodeType::NILL: {
        result += "Nill()";
        break;
      }
      case NodeType::RETURN:
      {
        NodeReturn *node_return = node_cast<NodeReturn *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string RETURN_result = "Return(";

        RETURN_result += "\n" + indent_string;
        RETURN_result += "expr=";
        RETURN_result += _node_to_string(node_return->expression, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        RETURN_result += "\n" + repeat_string(" ", indent_count - indent_level);
        RETURN_result += ")";

        result += RETURN_result;

        break;
      }
      case NodeType::DECL: {
        NodeDecl *node_decl = node_cast<NodeDecl *>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string DECL_result = "Decl(";

        DECL_result += "\n" + indent_string;
        DECL_result += "Name=\"" + node_decl->decl_name + '"';

        DECL_result += ",\n" + indent_string;
        DECL_result += "Init=";
        DECL_result += _node_to_string(node_decl->expression, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        DECL_result += "\n" + repeat_string(" ", indent_count - indent_level);
        DECL_result += ")";
      
        // NOTE: SINCE WE ARE NOT EXACTLY UNIT TESTING THE NodeId symbol ref, IT IS SUITABLE THAT WE PUT IT HERE WE DISPLAY ALL THE NODES BEFORE WE UNIT-TEST/COMPARE THEIR AST CONTENT 
        // HOWEVER, THIS COULD CAUSE ISSUES FOR FAILING TESTS IF WE BUILD THE NODE IN SOME CASES NOT SET THE SYMBOL
        assert(
          node_decl->symbol_ref != 0
        );

        result += DECL_result;

        break;
      }
      case NodeType::MEMBER_ACCESS: {
        NodeGetMember *node_member = node_cast<NodeGetMember*>(node_ptr);

        std::string indent_string = repeat_string(" ", indent_count);

        std::string MEMBER_result = "GetMember(";

        MEMBER_result += "\n" + indent_string;
        MEMBER_result += "obj=";
        MEMBER_result += _node_to_string(node_member->obj, indent_count + indent_level);

        MEMBER_result += ", \n" + indent_string;
        MEMBER_result += "member_id=";
        MEMBER_result += _node_to_string(node_member->member_identifier, indent_count + indent_level);

        // Closing backwards indent_count - indent_level
        MEMBER_result += "\n" + repeat_string(" ", indent_count - indent_level);
        MEMBER_result += ")";

        /*
        // NOTE: node_member->member_identifier is NodeId already and we do this check
        assert(
          node_member->symbol_ref != 0
        );
        */

        result += MEMBER_result;

        break;
      }
      default:
      {
        result += "Node(Invalid)";
        break;
      }

      }

      return result;
    }

    private:
      size_t indent_level{2}; // by 2
  };

  // TODO: Move inside of the class somehow

  using LexAnalysis::Lexer;

  using TokenGroup = LexAnalysis::TokenGroup;


  class SyntaxError {
    public:
      std::string err_message;

      SyntaxError() = default;

      SyntaxError(const std::string& err_message) : err_message{err_message} {}
      SyntaxError(std::string &&err_message) : err_message{std::move(err_message)} {}

      const std::string& get_error() const {
        return err_message; // NOTE: Avoiding the copy (constructor) here by having the const ref in the function return signature. NOTE THAT THIS IS NOT COPY ELISION AS WE ARE NOT RETURNING THE VARIABLE LOCAL TO THE FUNCTION SCOPE - THIS IS this->err_messagr in other words
      }

  };

  enum class BlockType {
    TOP_LEVEL,
    FUNC,
    WHILE,
    IGNORED, // NOTE: FOR IF STATEMENTS, ETC.
  };

  class BlockFrame {
    public:
      BlockType block_type {BlockType::IGNORED};

      BlockFrame() {
      }

      BlockFrame(BlockType blocktype) : block_type {blocktype} {}

      BlockFrame(const BlockFrame& blockframe) {
        this->block_type = blockframe.block_type;
      }

      /*
      BlockFrame(BlockFrame&& blockframe) {
        this->block_type = blockframe.block_type;
      }
      */
  };

  class AstGenerator
  {

  private:
    Lexer lexer;
    std::vector<SyntaxError> error_queue;
    /// TODO: Include the full directory path to the file
    std::string file_name = "main.dcl";
    Context* context { nullptr };

  public:

    std::vector<BlockFrame> block_stack;

    AstGenerator() = default;


    static SmartASTNode generateNode(const std::string& source_code, Context* context) {
      AstGenerator ast_generator;
      NodeParser node_parser;

      ast_generator.set_source(source_code.c_str());

      ast_generator.attach_context(context);

      SmartASTNode ast_root = ast_generator.parse();

      const auto& errors = ast_generator.get_syntax_errors();

      assert(errors.size() == 0);

      return ast_root;

    }

    const std::vector<SyntaxError>& get_syntax_errors() const {
      return error_queue;
    }

    void attach_context(Context* context) {
      this->context = context;
    }

    void set_source(const char* source) {
      lexer.set_stream(Lexer::Stream(source));
    }

    SmartASTNode parse() {
      assert(context != nullptr);

      block_stack.push_back(
        BlockFrame(BlockType::TOP_LEVEL)
      );

      SmartASTNode node = parse_block(false);

      block_stack.pop_back();

      return node;
    }

    SmartASTNode parse_block(bool nested)
    {
      SmartASTNode block_node = make_node<NodeBlock>();

      NodeBlock* raw_block_node = node_cast<NodeBlock*>(block_node.get());


      while (!lexer.is_over())
      {

        Lexer::Peekable peek_block_token = lexer.peek_token(TokenGroup::SYMBOL);

        // Note: Only a nested block can return '}'
        if(nested && peek_block_token.get_type() == TokenType::SYM_CLOSE_B) {
          return block_node;
        }
        // Ignoring the semicolon ';'
        else if (peek_block_token.get_type() == TokenType::SEMI_COLON) {
          lexer.eat_peekable(peek_block_token);
          lexer.skip_whitespace();
          continue;
        } else if(peek_block_token.get_type() == TokenType::NULL_CHAR) {
          // NOTE: This is needed for "while(1) {    " or "     " cases so we must make sure NOT to parse null terminated tokens
          // Once the loop continues, (!lexer.is_over()) above evaluates to false
          lexer.eat_peekable(peek_block_token);
          continue;
        }

        Lexer::Peekable keyword_token = lexer.peek_token(TokenGroup::KEYWORD);


        if(keyword_token.get_type() == TokenType::KEYWORD_BREAK) {
          lexer.eat_peekable(keyword_token);

          SmartASTNode node_break = make_node<NodeBreak>(keyword_token.token);

          raw_block_node->push_back(std::move(node_break));

          // Back trace the block stack until the first control-flow (while, for) wins!
          // NOTE: Same logic applied for the continue keyword

          bool control_flow_loop_found = false;


          // for(size_t i = block_stack.size() - 1; i >= 0; i--) { // PLEASE NEVER USE the size_t in this loop like this
          for(size_t i = block_stack.size(); i > 0; i--) {
            BlockFrame& block_frame = block_stack[i - 1];
            if(block_frame.block_type == BlockType::WHILE) {
              control_flow_loop_found = true;
              break;
            }
            // TODO: else if(block_frame.block_type == BlockType::STATEMENT_BLOCK) SHOULD BE HARD STOP
          }


          if(!control_flow_loop_found) {
            error_queue.push_back(
              SyntaxError("No matching loop for the break at ...") // TODO: Improve the error message, of course
            );
          }

          continue;
        }

        if(keyword_token.get_type() == TokenType::KEYWORD_RETURN) {
          lexer.eat_peekable(keyword_token);

          SmartASTNode node_return = make_node<NodeReturn>(keyword_token.token);
          NodeReturn* raw_node_return = node_cast<NodeReturn*>(node_return.get());

          SmartASTNode expr = expression();

          raw_node_return->set_expression(std::move(expr));

          raw_block_node->push_back(std::move(node_return));

          // Back trace the block stack until the first function scope (func) wins!

          bool func_scope_found = false;


          // for(size_t i = block_stack.size() - 1; i >= 0; i--) { // PLEASE NEVER USE the size_t in this loop like this
          for(size_t i = block_stack.size(); i > 0; i--) {
            BlockFrame& block_frame = block_stack[i - 1];
            if(block_frame.block_type == BlockType::FUNC) {
              func_scope_found = true;
              break;
            }
            // TODO: else if(block_frame.block_type == BlockType::STATEMENT_BLOCK) SHOULD BE HARD STOP
          }


          if(!func_scope_found) {
            error_queue.push_back(
              SyntaxError("Return outside of function reach at ...") // TODO: Improve the error message, of course
            );
          }

          continue;
        }


        // Note that the function definition is only possible at the TOP LEVEL
        if(!nested && keyword_token.get_type() == TokenType::KEYWORD_FUNC) {
          // Consume FUNC
          lexer.eat_peekable(keyword_token);

          Lexer::Peekable func_id = lexer.peek_token(TokenGroup::IDENF);

          if(func_id.get_type() == TokenType::CONST_IDENF) {
            // Consume function id/function name
            lexer.eat_peekable(func_id);
            std::string function_name = func_id.get_val();

            // Extract the (conditional) via Lexer
            Lexer::Peekable open_cond = lexer.peek_token(TokenGroup::SYMBOL);

            if (open_cond.get_type() == TokenType::SYM_OPEN_P)
            {
              lexer.eat_peekable(open_cond);

              SmartASTNode func_node = make_node<NodeFunction>(open_cond.token);

              NodeFunction *raw_func_node = node_cast<NodeFunction *>(func_node.get());

              raw_func_node->set_function_name(function_name);
              raw_func_node->set_symbol_ref(
                context->insert_if_not_and_get_symbol_offset(function_name)
              );

              // Parsing function parameters and the (arg1, args2) logic
              while (1)
              {
                Lexer::Peekable postfix_closed_call = lexer.peek_token(TokenGroup::SYMBOL);

                // Finised parsing the function parameters
                if (postfix_closed_call.get_type() == TokenType::SYM_CLOS_P)
                {
                  lexer.eat_peekable(postfix_closed_call);

                  // START: Parsing Function Body {}
                  Lexer::Peekable open_block = lexer.peek_token(TokenGroup::SYMBOL);

                  if (open_block.get_type() == TokenType::SYM_OPEN_B)
                  {
                    lexer.eat_peekable(open_block);

                    block_stack.push_back(
                        BlockFrame(BlockType::FUNC));
                    SmartASTNode recursive_node_block = parse_block(true);
                    block_stack.pop_back();

                    NodeBlock *raw_recursive_node_block = node_cast<NodeBlock *>(recursive_node_block.get());

                    raw_func_node->set_body(
                      std::move(raw_recursive_node_block->body)
                    );


                    Lexer::Peekable close_block = lexer.peek_token(TokenGroup::SYMBOL);

                    if (close_block.get_type() == TokenType::SYM_CLOSE_B)
                    {
                      // NOTE: FuncDef Node Block appended only when there are no Lexical/Syntax Errors!
                      raw_block_node->push_back(std::move(func_node));
                      lexer.eat_peekable(close_block);
                    }
                    else
                    {
                      std::string err_message = "Expected '}' at the end of input for " + std::string("'") + open_block.get_val() + "'" + " at LINE: " + std::to_string(open_block.get_line()) + " Column: " + std::to_string(open_block.get_col());

                      error_queue.push_back(
                          SyntaxError(
                              std::move(err_message)));
                    }
                  }
                  else
                  {
                    std::string err_message = "The If keyword missing body";

                    error_queue.push_back(
                        SyntaxError(
                            std::move(err_message)));
                  }
                  // END: Parsing Function Body {}

                  break;
                }

                SmartASTNode expr = expression(); // Keeping expression so that we can feature optional arguments such as (args = 1)


                if(expr.get()->node_type != NodeType::CONST_ID) {
                  error_queue.push_back(
                    SyntaxError(
                      "Expected Identifer for a function parameter at"
                    )
                  );
                } else {
                  raw_func_node->add_param(std::move(expr));
                }

                Lexer::Peekable sym_comma = lexer.peek_token(TokenGroup::SYMBOL);
                if (sym_comma.get_type() == TokenType::SYM_COMMA)
                {

                  lexer.eat_peekable(sym_comma);
                  continue;
                }
                else if (sym_comma.get_type() != TokenType::SYM_CLOS_P)
                {
                  std::string err_message = "Error while parsing the function parameters at";
                  error_queue.push_back(
                      SyntaxError(std::move(err_message)));
                  break;
                }
              }
            }
            else
            {
              std::string err_message = "Error while parsing the function parameters at";
              error_queue.push_back(
                  SyntaxError(std::move(err_message)));
            }
          } else {
            std::string err_message = "Function Name Missing at";
            error_queue.push_back(
                SyntaxError(std::move(err_message)));
          }

        }
        else if(keyword_token.get_type() == TokenType::KEYWORD_IF) {
          // Consume IF
          lexer.eat_peekable(keyword_token);

          // Extract the (conditional) via Lexer
          Lexer::Peekable open_cond = lexer.peek_token(TokenGroup::SYMBOL);

          if (open_cond.get_type() == TokenType::SYM_OPEN_P)
          {
            lexer.eat_peekable(open_cond);
            SmartASTNode expression_node = parse_expression();

            Lexer::Peekable close_cond = lexer.peek_token(TokenGroup::SYMBOL);

            if (close_cond.get_type() == TokenType::SYM_CLOS_P)
            {
              lexer.eat_peekable(close_cond);
              Lexer::Peekable open_block = lexer.peek_token(TokenGroup::SYMBOL);

              if (open_block.get_type() == TokenType::SYM_OPEN_B)
              {
                lexer.eat_peekable(open_block);

                block_stack.push_back(
                  BlockFrame(BlockType::IGNORED)
                );
                SmartASTNode recursive_node_block = parse_block(true);
                block_stack.pop_back();

                NodeBlock* raw_recursive_node_block = node_cast<NodeBlock*>(recursive_node_block.get());

                SmartASTNode if_node_block = make_node<NodeIfBlock>(keyword_token.token);
                NodeIfBlock* raw_if_node_block = node_cast<NodeIfBlock*>(if_node_block.get());

                raw_if_node_block->set_condition(std::move(expression_node));
                raw_if_node_block->set_body(std::move(raw_recursive_node_block->body));

                raw_block_node->push_back(std::move(if_node_block));

                Lexer::Peekable close_block = lexer.peek_token(TokenGroup::SYMBOL);

                if(close_block.get_type() == TokenType::SYM_CLOSE_B) {
                  lexer.eat_peekable(close_block);
                } else {
                  std::string err_message = "Expected '}' at the end of input for "
                    + std::string("'") + open_block.get_val() + "'"
                    + " at LINE: " + std::to_string(open_block.get_line())
                    + " Column: " + std::to_string(open_block.get_col());

                  error_queue.push_back(
                    SyntaxError(
                      std::move(err_message)
                    )
                  );
                }

              } else {
                std::string err_message = "The If keyword missing body";

                error_queue.push_back(
                  SyntaxError(
                    std::move(err_message)
                  )
                );

              }
            }
            else
            {
              std::string err_message = "Expected Closing ')' conditional expression"
                + std::string(" at LINE: ") + std::to_string(close_cond.get_line())
                + std::string(" Column: " ) + std::to_string(close_cond.get_col());

              error_queue.push_back(
                SyntaxError(
                  std::move(err_message)
                )
              );
            }
          } else {
          std::string err_message = file_name + ':' + std::to_string(open_cond.get_line()) + ':'
            + std::to_string(open_cond.get_col()) + " Error: Expected conditional expression for the If Statement";

            error_queue.push_back(
              SyntaxError(
                std::move(err_message)
              )
            );

          }

        }
        else if(keyword_token.get_type() == TokenType::KEYWORD_WHILE) { // NOTE THAT THIS LOGIC IS HEAVILY UNIT-TESTED AND THE SAME LOGIC IS APPLIED FOR THE IF STATEMENT
          // Consume WHILE
          lexer.eat_peekable(keyword_token);

          // Extract the (conditional) via Lexer
          Lexer::Peekable open_cond = lexer.peek_token(TokenGroup::SYMBOL);

          if (open_cond.get_type() == TokenType::SYM_OPEN_P)
          {
            lexer.eat_peekable(open_cond);
            SmartASTNode expression_node = parse_expression();

            Lexer::Peekable close_cond = lexer.peek_token(TokenGroup::SYMBOL);

            if (close_cond.get_type() == TokenType::SYM_CLOS_P)
            {
              lexer.eat_peekable(close_cond);
              Lexer::Peekable open_block = lexer.peek_token(TokenGroup::SYMBOL);

              if (open_block.get_type() == TokenType::SYM_OPEN_B)
              {
                lexer.eat_peekable(open_block);

                block_stack.push_back(
                  BlockFrame(BlockType::WHILE)
                );
                SmartASTNode recursive_node_block = parse_block(true);
                block_stack.pop_back();

                NodeBlock* raw_recursive_node_block = node_cast<NodeBlock*>(recursive_node_block.get());

                SmartASTNode while_node_block = make_node<NodeWhileBlock>(keyword_token.token);
                NodeWhileBlock* raw_while_node_block = node_cast<NodeWhileBlock*>(while_node_block.get());

                raw_while_node_block->set_condition(std::move(expression_node));
                raw_while_node_block->set_body(std::move(raw_recursive_node_block->body));

                raw_block_node->push_back(std::move(while_node_block));

                Lexer::Peekable close_block = lexer.peek_token(TokenGroup::SYMBOL);

                if(close_block.get_type() == TokenType::SYM_CLOSE_B) {
                  lexer.eat_peekable(close_block);
                } else {
                  std::string err_message = "Expected '}' at the end of input for "
                    + std::string("'") + open_block.get_val() + "'"
                    + " at LINE: " + std::to_string(open_block.get_line())
                    + " Column: " + std::to_string(open_block.get_col());

                  error_queue.push_back(
                    SyntaxError(
                      std::move(err_message)
                    )
                  );
                }

              } else {
                std::string err_message = "While keyword missing body";

                error_queue.push_back(
                  SyntaxError(
                    std::move(err_message)
                  )
                );

              }
            }
            else
            {
              std::string err_message = "Expected Closing ')' conditional expression"
                + std::string(" at LINE: ") + std::to_string(close_cond.get_line())
                + std::string(" Column: " ) + std::to_string(close_cond.get_col());

              error_queue.push_back(
                SyntaxError(
                  std::move(err_message)
                )
              );
            }
          } else {
          std::string err_message = file_name + ':' + std::to_string(open_cond.get_line()) + ':'
            + std::to_string(open_cond.get_col()) + " Error: Expected conditional expression for While loop";

            error_queue.push_back(
              SyntaxError(
                std::move(err_message)
              )
            );

          }

        }
        else if(keyword_token.get_type() == TokenType::KEYWORD_DECL) {
          // Consume decl
          lexer.eat_peekable(keyword_token);

          Lexer::Peekable decl_name = lexer.peek_token(TokenGroup::IDENF);

          if(decl_name.get_type() == TokenType::CONST_IDENF) {
            lexer.eat_peekable(decl_name);

            Lexer::Peekable init_sym = lexer.peek_token(TokenGroup::OPERATOR);

            if(init_sym.get_type() == TokenType::OP_ASSIGNMENT) {
              lexer.eat_peekable(init_sym);

              const std::string& decl_id = decl_name.get_val();

              SmartASTNode expr = expression();

              SmartASTNode decl_node = make_node<NodeDecl>(decl_name.token);

              NodeDecl* raw_decl_node = node_cast<NodeDecl*>(decl_node.get());

              raw_decl_node->set_expression(std::move(expr));

              raw_decl_node->set_name(decl_id);

              raw_decl_node->set_symbol_ref(
                context->insert_if_not_and_get_symbol_offset(decl_id)
              );


              raw_block_node->push_back(std::move(decl_node));

              // START: END OF LINE REQUIREMENT
              lexer.skip_whitespace_except_newline();

              char current_char = lexer.read_char();

              // NOTE: New line break and make sure to cover the end of the input
              // NOTE: peek_token, get_token skips ahead of the token - not after the token - we CANNOT consume the whitespace after the token by default. This is done manually via the line_defer below SINCE THE END OF AN EXPRESSION CAN BE '\n'
              if (current_char == '\n' || current_char == 0) {
                goto line_defer;
              }

              Lexer::Peekable sym_token = lexer.peek_token(TokenGroup::SYMBOL);

              if (sym_token.get_type() == TokenType::SEMI_COLON) {
                lexer.eat_peekable(sym_token);
                goto line_defer;
              }

              // Ensure that '}' is NOT consumed
              if (sym_token.get_type() == TokenType::SYM_CLOSE_B) {
                goto line_defer;
              }

              std::string err_message = file_name + ':' + std::to_string(lexer.get_current_line()) + ':'
                + std::to_string(lexer.get_col()) + " Error: Expected '\\n', '}', or ';' at the end of expression";

              error_queue.push_back(
                SyntaxError(
                  std::move(err_message)
                )
              );
              // END: END OF LINE REQUIREMENT

            }
            else {
              error_queue.push_back(
                SyntaxError("Declaration missing the init at")
              );
            }

          }
          else {
            error_queue.push_back(
              SyntaxError("Variable Declaration missing the name at")
            );

          }

        }
        else {
          SmartASTNode expression_node = parse_expression();
          raw_block_node->push_back(std::move(expression_node));

          // START: END OF LINE REQUIREMENT
          lexer.skip_whitespace_except_newline();

          char current_char = lexer.read_char();

          // NOTE: New line break and make sure to cover the end of the input
          // NOTE: peek_token, get_token skips ahead of the token - not after the token - we CANNOT consume the whitespace after the token by default. This is done manually via the line_defer below SINCE THE END OF AN EXPRESSION CAN BE '\n'
          if (current_char == '\n' || current_char == 0) {
            goto line_defer;
          }

          Lexer::Peekable sym_token = lexer.peek_token(TokenGroup::SYMBOL);

          if (sym_token.get_type() == TokenType::SEMI_COLON) {
            lexer.eat_peekable(sym_token);
            goto line_defer;
          }

          // Ensure that '}' is NOT consumed
          if (sym_token.get_type() == TokenType::SYM_CLOSE_B) {
            goto line_defer;
          }

          std::string err_message = file_name + ':' + std::to_string(lexer.get_current_line()) + ':'
            + std::to_string(lexer.get_col()) + " Error: Expected '\\n', '}', or ';' at the end of expression";

          error_queue.push_back(
            SyntaxError(
              std::move(err_message)
            )
          );

          // END: END OF LINE REQUIREMENT

        }


        line_defer: {
          // NOTE: Eat up white space on each expression line. Otherwise, the compiler results in fatal: "Terminated: Unexpected null character at LINE"
          // NOTE: NOT NEEDED DUE TO else if(peek_block_token.get_type() == TokenType::NULL_CHAR). However, it is a plus to skip/consume whitespace ahead of time for performance reasons
          lexer.skip_whitespace();
        }



      }

      // print out the status
      // state.print();

      return block_node;
    }

    SmartASTNode parse_expression()
    {
      assert(context != nullptr);

      return expression();
    }

    SmartASTNode expression()
    {
      return assignment_expression();
    }

    SmartASTNode assignment_expression() {
      SmartASTNode left = logical_or_expression();

      LOOP
      {
        Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);

        switch(op_token.get_type()) {
          case TokenType::OP_ASSIGNMENT: {
            // TODO: storeSubscript and storeMember exceptions
            if(determineNodeValue(left) == ValueCategory::RVALUE) {
              error_queue.push_back(
                SyntaxError("RValue not assignable at")
              );
            }

            lexer.eat_peekable(op_token);

            SmartASTNode right = logical_or_expression();

            SmartASTNode left_assign = make_node<NodeAssignment>(op_token.token);

            NodeAssignment* raw_left = node_cast<NodeAssignment*>(left_assign.get());

            raw_left->insert_left(std::move(left));
            raw_left->insert_right(std::move(right));

            left = std::move(left_assign);
            break;
          }
          default: {
            return left;
          }

        }
      }

    }

    SmartASTNode logical_or_expression() {
      SmartASTNode left = logical_and_expression();

      LOOP
      {
        Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);


        switch(op_token.get_type()) {
          case TokenType::OP_LOGICAL_OR: {
            lexer.eat_peekable(op_token);

            SmartASTNode right = logical_and_expression();

            SmartASTNode left_logical_or = make_node<NodeLogicalOr>(op_token.token);

            NodeLogicalOr* raw_left = node_cast<NodeLogicalOr*>(left_logical_or.get());

            raw_left->insert_left(std::move(left));
            raw_left->insert_right(std::move(right));

            left = std::move(left_logical_or);
            break;
          }
          default: {
            return left;
          }

        }
      }

    }

    SmartASTNode logical_and_expression() {
      SmartASTNode left = relational_expression_equal_to();

      LOOP
      {
        Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);


        switch(op_token.get_type()) {
          case TokenType::OP_LOGICAL_AND: {
            lexer.eat_peekable(op_token);

            SmartASTNode right = relational_expression_equal_to();

            SmartASTNode left_logical_and = make_node<NodeLogicalAnd>(op_token.token);

            NodeLogicalAnd* raw_left = node_cast<NodeLogicalAnd*>(left_logical_and.get());

            raw_left->insert_left(std::move(left));
            raw_left->insert_right(std::move(right));

            left = std::move(left_logical_and);
            break;
          }
          default: {
            return left;
          }

        }
      }

    }

    SmartASTNode relational_expression_equal_to() {
      SmartASTNode left = relational_expression_or_equal_to();

      LOOP
      {
        Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);


        switch(op_token.get_type()) {
          case TokenType::OP_EQUAL_TO: {
            lexer.eat_peekable(op_token);

            SmartASTNode right = relational_expression_or_equal_to();

            SmartASTNode left_eq_to = make_node<NodeEqualTo>(op_token.token);

            NodeEqualTo* raw_left = node_cast<NodeEqualTo*>(left_eq_to.get());

            raw_left->insert_left(std::move(left));
            raw_left->insert_right(std::move(right));

            left = std::move(left_eq_to);
            break;

          }
          case TokenType::OP_NOT_EQUAL_TO: {
            lexer.eat_peekable(op_token);

            SmartASTNode right = relational_expression_or_equal_to();

            SmartASTNode left_not_eq_to = make_node<NodeNotEqualTo>(op_token.token);

            NodeNotEqualTo* raw_left = node_cast<NodeNotEqualTo*>(left_not_eq_to.get());

            raw_left->insert_left(std::move(left));
            raw_left->insert_right(std::move(right));

            left = std::move(left_not_eq_to);
            break;

          }
          default: {
            return left;
          }

        }
      }

    }


    SmartASTNode relational_expression_or_equal_to() {
      SmartASTNode left = additive_expression();

      LOOP
      {
        Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);


        switch(op_token.get_type()) {
          case TokenType::OP_LESS_THAN: {
            lexer.eat_peekable(op_token);

            SmartASTNode right = additive_expression();

            SmartASTNode left_less_than = make_node<NodeLessThan>(op_token.token);

            NodeLessThan* raw_left = node_cast<NodeLessThan*>(left_less_than.get());

            raw_left->insert_left(std::move(left));
            raw_left->insert_right(std::move(right));

            left = std::move(left_less_than);
            break;

          }
          case TokenType::OP_GREATER_THAN: {
            lexer.eat_peekable(op_token);

            SmartASTNode right = additive_expression();

            SmartASTNode left_greater_than = make_node<NodeGreaterThan>(op_token.token);

            NodeGreaterThan* raw_left = node_cast<NodeGreaterThan*>(left_greater_than.get());

            raw_left->insert_left(std::move(left));
            raw_left->insert_right(std::move(right));

            left = std::move(left_greater_than);
            break;

          }
          case TokenType::OP_EQ_GREATER_THAN: {
            lexer.eat_peekable(op_token);

            SmartASTNode right = additive_expression();

            SmartASTNode left_greater_than_eq_to = make_node<NodeGreaterThanEqualTo>(op_token.token);

            NodeGreaterThanEqualTo* raw_left = node_cast<NodeGreaterThanEqualTo*>(left_greater_than_eq_to.get());

            raw_left->insert_left(std::move(left));
            raw_left->insert_right(std::move(right));

            left = std::move(left_greater_than_eq_to);
            break;

          }
          case TokenType::OP_EQ_LESS_THAN: {
            lexer.eat_peekable(op_token);

            SmartASTNode right = additive_expression();

            SmartASTNode left_less_than_eq_to = make_node<NodeLessThanEqualTo>(op_token.token);

            NodeLessThanEqualTo* raw_left = node_cast<NodeLessThanEqualTo*>(left_less_than_eq_to.get());

            raw_left->insert_left(std::move(left));
            raw_left->insert_right(std::move(right));

            left = std::move(left_less_than_eq_to);
            break;

          }
          default: {
            return left;
          }

        }

      }


      return left;

    }

    SmartASTNode additive_expression()
    {
      SmartASTNode left = multiplicative_expression();

      LOOP
      {
        // TODO: Consider something that would work like this: peek_token(PLUS | MINUS)
        // The solution, for now, is to do TokenGroup::ANY for primary and have a switch case
        Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);

        // std::cout << (token.get_type() == TokenType::INVALID) << std::endl;

        switch (op_token.get_type())
        {
        case TokenType::OP_PLUS:
        {
          lexer.eat_peekable(op_token);

          SmartASTNode right = multiplicative_expression();


          SmartASTNode left_additive = make_node<NodeAdditive>(op_token.token);

          NodeAdditive* raw_left = node_cast<NodeAdditive*>(left_additive.get());

          // Shift left and right nodes
          raw_left->insert_left(std::move(left));
          raw_left->insert_right(std::move(right));

          left = std::move(left_additive);

          break;
        }
        case TokenType::OP_MINUS:
        {
          lexer.eat_peekable(op_token);

          SmartASTNode right = multiplicative_expression();


          SmartASTNode left_subtract = make_node<NodeSubtract>(op_token.token);

          NodeSubtract* raw_left = node_cast<NodeSubtract*>(left_subtract.get());

          // Shift left and right nodes
          raw_left->insert_left(std::move(left));
          raw_left->insert_right(std::move(right));

          left = std::move(left_subtract);

          break;
        }
        default:
          return left;
        }
      }

      return left;
    }

    SmartASTNode multiplicative_expression()
    {
      SmartASTNode left = unary_expression();

      LOOP
      {
        Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);

        switch (op_token.get_type())
        {
        case TokenType::OP_MULTIPLY:
        {
          lexer.eat_peekable(op_token);

          SmartASTNode right = unary_expression();

          SmartASTNode left_multiply = make_node<NodeMultiply>(op_token.token);

          NodeMultiply* raw_left = node_cast<NodeMultiply*>(left_multiply.get());
          // Shift left and right nodes
          raw_left->insert_left(std::move(left));
          raw_left->insert_right(std::move(right));

          left = std::move(left_multiply);

          break;
        }
        case TokenType::OP_DIVIDE:
        {
          lexer.eat_peekable(op_token);

          SmartASTNode right = unary_expression();

          SmartASTNode left_divide = make_node<NodeDivide>(op_token.token);

          NodeDivide* raw_left = node_cast<NodeDivide*>(left_divide.get());
          // Shift left and right nodes
          raw_left->insert_left(std::move(left));
          raw_left->insert_right(std::move(right));

          left = std::move(left_divide);

          break;
        }
        default:
          return left;
        }
      }
    }



    SmartASTNode unary_expression() {
      Lexer::Peekable unary_op = lexer.peek_token(TokenGroup::OPERATOR);

      switch(unary_op.get_type()) {
        case TokenType::OP_LOGICAL_NOT: {
          lexer.eat_peekable(unary_op);

          SmartASTNode expr = unary_expression();

          SmartASTNode node_logical_not = make_node<NodeLogicalNot>(unary_op.token);


          NodeLogicalNot* raw_logical_not = node_cast<NodeLogicalNot*>(node_logical_not.get());
          // Shift left and right nodes
          raw_logical_not->set_expression(std::move(expr));

          return node_logical_not;
        }
        case TokenType::OP_MINUS: {
          lexer.eat_peekable(unary_op);

          SmartASTNode expr = unary_expression();

          SmartASTNode node_unary_minus = make_node<NodeUnaryMinus>(unary_op.token);


          NodeUnaryMinus* raw_unary_minus = node_cast<NodeUnaryMinus*>(node_unary_minus.get());
          // Shift left and right nodes
          raw_unary_minus->set_expression(std::move(expr));

          return node_unary_minus;
        }
        case TokenType::OP_PLUS: {
          lexer.eat_peekable(unary_op);

          SmartASTNode expr = unary_expression();

          SmartASTNode node_unary_plus = make_node<NodeUnaryPlus>(unary_op.token);

          NodeUnaryPlus* raw_unary_plus = node_cast<NodeUnaryPlus*>(node_unary_plus.get());
          // Shift left and right nodes
          raw_unary_plus->set_expression(std::move(expr));

          return node_unary_plus;
        }
        default: {
          return postfix_expression();
        }
      }

    }


    SmartASTNode postfix_expression() {
      SmartASTNode prim_exp = primary_expression();


      // Not using the switch case here as it is easier to use the if-else due to the while loop
      while(1) {
        Lexer::Peekable postfix_sym = lexer.peek_token(TokenGroup::SYMBOL);
        TokenType postfix_t = postfix_sym.get_type();

        // () Function Call
        if(postfix_t == TokenType::SYM_OPEN_P) {
          lexer.eat_peekable(postfix_sym);

          SmartASTNode func_call = make_node<NodeFuncCall>(postfix_sym.token);

          NodeFuncCall* raw_func_call = node_cast<NodeFuncCall*>(func_call.get());

          raw_func_call->set_callee(std::move(prim_exp));


          // Parsing expression arguments and the (arg1, args2) logic
          while(1) {
            Lexer::Peekable postfix_closed_call = lexer.peek_token(TokenGroup::SYMBOL);

            if(postfix_closed_call.get_type() == TokenType::SYM_CLOS_P) {
              lexer.eat_peekable(postfix_closed_call);
              break;
            }

            SmartASTNode expr = expression();

            raw_func_call->add_arg(std::move(expr));

            Lexer::Peekable sym_comma = lexer.peek_token(TokenGroup::SYMBOL);
            if(sym_comma.get_type() == TokenType::SYM_COMMA) {
              lexer.eat_peekable(sym_comma);
              continue;
            }
            else if(sym_comma.get_type() != TokenType::SYM_CLOS_P) {
              std::string err_message = "Error while parsing the function arguments at";
              error_queue.push_back(
                SyntaxError(std::move(err_message))
              );
              break;
            }

          }

          prim_exp = std::move(func_call);

        }
        // Member Access "."
        else if(postfix_t == TokenType::SYM_DOT) {
          lexer.eat_peekable(postfix_sym);

          // MUST BE TokenGroup::ANY DUE TO THE a.while EDGE CASE
          Lexer::Peekable token_id = lexer.peek_token(TokenGroup::ANY);

          if(token_id.get_type() == TokenType::CONST_IDENF) {
            lexer.eat_peekable(token_id);

            SmartASTNode node_id = make_node<NodeId>(token_id.token);
            NodeId* raw_node_id = node_cast<NodeId*>(node_id.get());

            const std::string& id_string = raw_node_id->token.get_val();

            raw_node_id->set_symbol_ref(
              context->insert_if_not_and_get_symbol_offset(id_string)
            );


            SmartASTNode node_member = make_node<NodeGetMember>(token_id.token);
            NodeGetMember* raw_node_member = node_cast<NodeGetMember*>(node_member.get());

            raw_node_member->obj = std::move(prim_exp);

            raw_node_member->member_identifier = std::move(node_id);


            prim_exp = std::move(node_member);

          }
          else {
            std::string err_message = "Error while parsing the member access at Expected The ID";

            error_queue.push_back(
              SyntaxError(std::move(err_message))
            );
          }

        }
        else {
          break;
        }

      }

      return prim_exp;

    }


    // Primary Expression
    SmartASTNode primary_expression()
    {
      // TODO: Refactor double-reading of a token O(2n): (peek(n) + get_token(n)): return the current pointer or something
      // NOTE: ANY for now as we need to match for the opening parenthesis, constants, etc.
      Lexer::Peekable p_token = lexer.peek_token(TokenGroup::ANY);

      switch (p_token.get_type())
      {
      case TokenType::CONST_NUM:
      {

        SmartASTNode node_num = make_node<NodeNum>(p_token.token);

        NodeNum* raw_node_num = node_cast<NodeNum*>(node_num.get());

        raw_node_num->set_num(
          // static_cast<int>(

          // NOTE: STRING TO DOUBLE BUT DO NOTE THAT THIS DEPENDS ON THE NUMBER IMPLEMENTION FOR DECL - IT MIGHT ALSO BE LONG DOUBLE AND SIMILAR
          // SEE: https://en.cppreference.com/cpp/string/basic_string/stof
          // TODO: WRITE A MACRO, ISOLATE A FUNCTION FOR PARSING A NUMBER OR SIMILAR
          // TODO: WRITE LOGIC FOR THE NUMBER OVERFLOW. Overflow such as DOUBLE_MAX++;
          std::stod(
            p_token.token.get_val()
          )
          // )
        );



        lexer.eat_peekable(p_token);
        return node_num;
      }
      case TokenType::CONST_IDENF:
      {

        SmartASTNode node_id = make_node<NodeId>(p_token.token);

        NodeId* raw_node_id = node_cast<NodeId*>(node_id.get());

        const std::string& id_string = raw_node_id->token.get_val();

        raw_node_id->set_symbol_ref(
          context->insert_if_not_and_get_symbol_offset(id_string)
        );


        /*
        if(p_token.get_val() == "while") {
          std::string err_message = "Reserved keyword: " + p_token.get_val()
            + " at LINE: " + std::to_string(p_token.get_line()) 
            + " Column: " + std::to_string(p_token.get_col());
          error_queue.push_back(
            SyntaxError(
              std::move(err_message)
            )
          );

        }
        */

        lexer.eat_peekable(p_token);
        return node_id;
      }
      case TokenType::SYM_OPEN_P:
      {
        // eat '('
        lexer.eat_peekable(p_token);

        SmartASTNode expression_node = expression();

        Lexer::Peekable right = lexer.peek_token(TokenGroup::SYMBOL);

        if (right.get_type() != TokenType::SYM_CLOS_P)
        {
          std::string err_message = "Missing ')'";
          error_queue.push_back(
            SyntaxError(std::move(err_message))
          );
          // std::cout << (right.get_type() == TokenType::NULL_CHAR) << std::endl;
          // NOTE: the default of this switch case eats this token on next iteration at the top level.
        }
        else
        {
          // eat ')'
          lexer.eat_peekable(right);
        }

        return expression_node;
      }
      case TokenType::KEYWORD_TRUE: {
        lexer.eat_peekable(p_token);

        SmartASTNode node_true = make_node<NodeTrue>(p_token.token);

        return node_true;
      }
      case TokenType::KEYWORD_FALSE: {
        lexer.eat_peekable(p_token);

        SmartASTNode node_false = make_node<NodeFalse>(p_token.token);

        return node_false;
      }
      case TokenType::KEYWORD_NILL: {
        lexer.eat_peekable(p_token);

        SmartASTNode node_nill = make_node<NodeNill>(p_token.token);

        return node_nill;
      }
      default: {

          // NOTE: eat the peekable token by swapping the end stream
          // Eat the invalid token
          lexer.eat_peekable(p_token);

          if (p_token.get_type() == TokenType::NULL_CHAR)
          {
            std::string err_message = "Terminated: Unexpected null character at LINE: "
                + std::to_string(p_token.get_line())
                + " Column: " + std::to_string(p_token.get_col());
            error_queue.push_back(
              SyntaxError(
                  std::move(err_message)));
          }
          else if(p_token.get_type() == TokenType::SYM_CLOSE_B) {
            std::string err_message = "Unmatched: '" + p_token.get_val()
                + "' at LINE: " + std::to_string(p_token.get_line()) 
                + " Column: " + std::to_string(p_token.get_col());
            error_queue.push_back(
              SyntaxError(
                  std::move(err_message)));
          }
          else
          {
            std::string err_message = "Invalid token: " + p_token.get_val()
                + " at LINE: " + std::to_string(p_token.get_line()) 
                + " Column: " + std::to_string(p_token.get_col());
            error_queue.push_back(
              SyntaxError(
                  std::move(err_message)));
          }

          // throw "1";
          return make_node<ASTNode>(NodeType::INVALID, p_token.token);
        }
      }

    }
  };

} // namespace AstParser


#endif

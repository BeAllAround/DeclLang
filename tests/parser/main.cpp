#include <iostream>
#include <string>
#include <vector>

#include <cassert>

#define DECL_COMPILER_VERBOSE

#define DECL_VERBOSE_LOGGING

#define DECL_LOG_INFO

#define _DECL_API_IGNORE_UNINIT

#include <decl/lib/Utility.hpp>

#include <decl/lib/AstParser.hpp>



using namespace AstParser;


// TODO: Maybe a function? BUT THIS IS PROBABLY BETTER AS WE GET THE EXACT LINE IN THE CASE OF THE FAILED ASSERTION AFTER THE MACRO GETS REPLACED
#define BASIC_AST_TEST(_source, _ast_output) ({                  \
  UniquePtr<Context> shrd_context = makeUniqueContext();                   \
  Context* main_context = obj_cast<Context*>(shrd_context.get()); \
  setUpContext(main_context);                                    \
  AstGenerator ast_generator;                                    \
  NodeParser node_parser;                                        \
  const std::string &source_code = (_source);                    \
  const std::string ast_output = strip_white_space(_ast_output); \
  ast_generator.set_source(source_code.c_str());                 \
  ast_generator.attach_context(main_context);                    \
  assertm("Block Stack Empty", ast_generator.block_stack.size() == 0);               \
  SmartASTNode ast_root = ast_generator.parse();                \
  assertm("Block Stack Clearned", ast_generator.block_stack.size() == 0);               \
  const auto &errors = ast_generator.get_syntax_errors();        \
  if(errors.size() != 0) { Logger::info("----------START ERROR----------"); } \
  for (size_t i = 0; i < errors.size(); i++)                     \
  {                                                              \
    const SyntaxError &syntax_error = errors[i];                 \
    Logger::info(syntax_error.get_error());                      \
  }                                                              \
  if(errors.size() != 0) { Logger::info("----------END ERROR----------"); } \
  std::string result = node_parser.node_to_string(ast_root);     \
  Logger::info(result);                                          \
  result = strip_white_space(result);                            \
  assertm("Must NOT Contain Errors", errors.size() == 0);        \
  assertm("AST Structure MUST MATCH", result == ast_output);     \
})

#define BASIC_AST_EXP_TEST(_source, _ast_output) ({              \
  UniquePtr<Context> shrd_context = makeUniqueContext();                   \
  Context* main_context = obj_cast<Context*>(shrd_context.get()); \
  setUpContext(main_context);                                    \
  AstGenerator ast_generator;                                    \
  NodeParser node_parser;                                        \
  const std::string &source_code = (_source);                    \
  const std::string ast_output = strip_white_space(_ast_output); \
  ast_generator.set_source(source_code.c_str());                 \
  ast_generator.attach_context(main_context);                    \
  SmartASTNode ast_root = ast_generator.parse_expression();     \
  const auto &errors = ast_generator.get_syntax_errors();        \
  if(errors.size() != 0) { Logger::info("----------START ERROR----------"); } \
  for (size_t i = 0; i < errors.size(); i++)                     \
  {                                                              \
    const SyntaxError &syntax_error = errors[i];                 \
    Logger::info(syntax_error.get_error());                      \
  }                                                              \
  if(errors.size() != 0) { Logger::info("----------END ERROR----------"); } \
  std::string result = node_parser.node_to_string(ast_root);     \
  Logger::info(result);                                          \
  result = strip_white_space(result);                            \
  assertm("Must NOT Contain Errors", errors.size() == 0);        \
  assertm("AST Structure MUST MATCH", result == ast_output);     \
})

int main() {

  TEST_SECTION("Basic Tests")
  {

    TEST_BLOCK("SmartASTNode | NodeParser Basic Test")
    {
      SmartASTNode node = make_node<NodeMultiply>(
        Token(
          "+",
          TokenType::OP_MULTIPLY,
          1,
          1
        )
      );

      SmartASTNode node01 = make_node<NodeId>(
        Token(
          "a01",
          TokenType::CONST_IDENF,
          1,
          1
        )
      );

      node_cast<NodeId*>(node01.get())->set_symbol_ref(1);

      NodeParser node_parser0;

      std::string node_result0 = node_parser0.node_to_string(node01);

      Logger::info(node_result0);
    }

    TEST_BLOCK("SmartASTNode | Basic Arithmetic Expression Test")
    {
      // Construct AST for expression: a * b
      SmartASTNode multiply_node = make_node<NodeMultiply>(
        Token(
          "+",
          TokenType::OP_MULTIPLY,
          1,
          1
        )
      );

      SmartASTNode left_node = make_node<NodeId>(
        Token(
          "a",
          TokenType::CONST_IDENF,
          1,
          1
        )
      );
      SmartASTNode right_node = make_node<NodeId>(
        Token(
          "b",
          TokenType::CONST_IDENF,
          1,
          1
        )
      );

      node_cast<NodeId*>(left_node.get())->set_symbol_ref(1);
      node_cast<NodeId*>(right_node.get())->set_symbol_ref(2);

      node_cast<NodeMultiply *>(multiply_node.get())->insert_left(std::move(left_node));
      node_cast<NodeMultiply *>(multiply_node.get())->insert_right(std::move(right_node));

      NodeParser node_parser;

      std::string result = node_parser.node_to_string(multiply_node);

      Logger::info(result);
    }

    TEST_BLOCK("SmartASTNode | Nested Arithmetic Expression Test")
    {
      // Construct AST for expression: (a * b) * c
      SmartASTNode multiply_node_outer = make_node<NodeMultiply>(
        Token(
          "+",
          TokenType::OP_MULTIPLY,
          1,
          1
        )
      );

      SmartASTNode multiply_node_inner = make_node<NodeMultiply>(
        Token(
          "+",
          TokenType::OP_MULTIPLY,
          1,
          1
        )
      );
      SmartASTNode right_node = make_node<NodeId>(
        Token(
          "c",
          TokenType::CONST_IDENF,
          1,
          1
        )
      );

      SmartASTNode left_inner_node = make_node<NodeId>(
        Token(
          "a",
          TokenType::CONST_IDENF,
          1,
          1
        )
      );
      SmartASTNode right_inner_node = make_node<NodeId>(
        Token(
          "b",
          TokenType::CONST_IDENF,
          1,
          1
        )
      );

      node_cast<NodeId*>(right_node.get())->set_symbol_ref(1);
      node_cast<NodeId*>(left_inner_node.get())->set_symbol_ref(2);
      node_cast<NodeId*>(right_inner_node.get())->set_symbol_ref(3);

      node_cast<NodeMultiply *>(multiply_node_inner.get())->insert_left(std::move(left_inner_node));
      node_cast<NodeMultiply *>(multiply_node_inner.get())->insert_right(std::move(right_inner_node));

      node_cast<NodeMultiply *>(multiply_node_outer.get())->insert_left(std::move(multiply_node_inner));
      node_cast<NodeMultiply *>(multiply_node_outer.get())->insert_right(std::move(right_node));

      NodeParser node_parser;

      std::string result = node_parser.node_to_string(multiply_node_outer);

      Logger::info(result);
    }

    TEST_BLOCK("AstGenerator Basics Test")
    {
      BASIC_AST_TEST(
        "a * b * c",
        R"(
Block=[
  Multiply(
    left=Multiply(
      left=Id(value="a"), 
      right=Id(value="b")
    ), 
    right=Id(value="c")
  )
]
        )"
      );

      BASIC_AST_EXP_TEST(
        "a * b * c",
        R"(
Multiply(
  left=Multiply(
    left=Id(value="a"), 
    right=Id(value="b")
  ), 
  right=Id(value="c")
)
        )"
      );

      TEST_BLOCK("Happy SUBTRACTION")
      {
        BASIC_AST_TEST(
            "1 - 3 - 4",
            R"(
Block=[
  Subtract(
    left=Subtract(
      left=Num(value="1"), 
      right=Num(value="3")
    ), 
    right=Num(value="4")
  )
]
        )");
      }





    }

    TEST_BLOCK("AstGenerator Errors #0") {

      std::string source_code = "1 + 2 + 3)";

      AstGenerator ast_generator;

      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context); 

      ast_generator.attach_context(main_context);
      ast_generator.set_source(source_code.c_str());

      SmartASTNode ast_root = ast_generator.parse();

      const auto& errors = ast_generator.get_syntax_errors();
      assertm("Must Contain Errors", errors.size() > 0);

      for(size_t i = 0; i < errors.size(); i++) {

        const SyntaxError& syntax_error = errors[i];

        Logger::info(syntax_error.get_error());

      }

    }

    TEST_BLOCK("AstGenerator Errors #1") {

      std::string source_code = "(1 + 2 + 3) * 10";

      AstGenerator ast_generator;
      NodeParser node_parser;

      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context); 

      ast_generator.attach_context(main_context);
      ast_generator.set_source(source_code.c_str());

      SmartASTNode ast_root = ast_generator.parse();

      const auto& errors = ast_generator.get_syntax_errors();

      assertm("Must NOT Contain Errors", errors.size() == 0);

      std::string result = node_parser.node_to_string(ast_root);

      Logger::info(result);

    }


    TEST_BLOCK("Happy Tests") {
      const std::string source_arr[] = {
        // No code passes
        R"(






        )",

      };

      BASIC_AST_TEST(
        R"()",

         R"(
         Block=[]
        )"
      );

      BASIC_AST_TEST(
        R"(
        
        
        
        
        
        
        
        
        )",

         R"(
         Block=[]
        )"
      );

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context);
      for(size_t i = 0; i < get_arr_size(source_arr); i++) {
        AstGenerator ast_generator;
        NodeParser node_parser;

        ast_generator.attach_context(main_context);

        const std::string& source_code = source_arr[i];

        ast_generator.set_source(source_code.c_str());

        SmartASTNode ast_root = ast_generator.parse();

        const auto &errors = ast_generator.get_syntax_errors();
        
        for(size_t i = 0; i < errors.size(); i++) {
          const SyntaxError& syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        assertm("Must NOT Contain Errors", errors.size() == 0);

      }

    }

    TEST_BLOCK("Basic Arithmetics") {

      const std::string source_arr[][2] = {
          {
            R"(
              3 * 10;
            )",
            R"(
              Block=[
                Multiply(
                  left=Num(value="3"), 
                  right=Num(value="10") 
                )
              ]
            )",
          },

          {
            R"(
              1 + 3 + 10;
            )",
            R"(
              Block=[
                Add(
                  left=Add(
                    left=Num(value="1"), 
                    right=Num(value="3")
                  ), 
                  right=Num(value="10")
                )
              ]
            )",
          },

          {
            R"(
              1 + 3 + 10 * 10;
            )",
            R"(
              Block=[
                Add(
                  left=Add(
                    left=Num(value="1"), 
                    right=Num(value="3")
                  ), 
                  right=Multiply(
                    left=Num(value="10"), 
                    right=Num(value="10")
                  )
                )
              ]
            )",
          },


          {
            R"(
              10 / 10 + 1;
            )",
            R"(
              Block=[
                Add(
                  left=Divide(
                    left=Num(value="10"), 
                    right=Num(value="10")
                  ), 
                  right=Num(value="1")
                )
              ]
            )",
          },
          {
            R"(
              10 / 10;
            )",
            R"(
              Block=[
                Divide(
                  left=Num(value="10"), 
                  right=Num(value="10")
                )
              ]
            )",
          },

      };

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context); 

      for(size_t i = 0; i < get_arr_size(source_arr); i++) {
        AstGenerator ast_generator;
        NodeParser node_parser;

        ast_generator.attach_context(main_context);

        const std::string& source_code = source_arr[i][0];
        const std::string ast_output = strip_white_space(source_arr[i][1]);

        ast_generator.set_source(source_code.c_str());

        SmartASTNode ast_root = ast_generator.parse();

        const auto &errors = ast_generator.get_syntax_errors();
        
        for(size_t i = 0; i < errors.size(); i++) {
          const SyntaxError& syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        std::string result = node_parser.node_to_string(ast_root);

        Logger::info(result);


        result = strip_white_space(result);

        assertm("Must NOT Contain Errors", errors.size() == 0);

        assertm("AST Structure MUST MATCH", result == ast_output);

      }


      BASIC_AST_TEST(
        "(8 + 1) / 2",
        R"(
        Block=[
          Divide(
            left=Add(
              left=Num(value="8"), 
              right=Num(value="1")
            ), 
            right=Num(value="2")
          )
        ]
        )"
      );


      



    }

    TEST_BLOCK("Happy Function Definition") {
      BASIC_AST_TEST(
        R"(
          func func1() {
            print(1)
          }
        )",

        R"(
Block=[
  FuncDef=(
    Name="func1",
    Params=(),
    Block=[
      FuncCall=(
        Callee=(Id(value="print"))
        Args=(
          Num(value="1")
        )
      )
    ]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          func func1(a) {
            print(1)
          }
        )",

        R"(
Block=[
  FuncDef=(
    Name="func1",
    Params=(
      Id(value="a")
    ),
    Block=[
      FuncCall=(
        Callee=(Id(value="print"))
        Args=(
          Num(value="1")
        )
      )
    ]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          func func1(a, b) {
            print(a, b)
          }
        )",

        R"(
Block=[
  FuncDef=(
    Name="func1",
    Params=(
      Id(value="a"),
      Id(value="b")
    ),
    Block=[
      FuncCall=(
        Callee=(Id(value="print"))
        Args=(
          Id(value="a"),
          Id(value="b")
        )
      )
    ]
  )
]
        )"
      );

    }

    TEST_BLOCK("Happy Return Statement") {
      BASIC_AST_TEST(
        R"(
          func func1(a, b) {
            print(a, b)
            return 1
          }
        )",

        R"(
Block=[
  FuncDef=(
    Name="func1",
    Params=(
      Id(value="a"),
      Id(value="b")
    ),
    Block=[
      FuncCall=(
        Callee=(Id(value="print"))
        Args=(
          Id(value="a"),
          Id(value="b")
        )
      ),
      Return(
        expr=Num(value="1")
      )
    ]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          func func1(a, b) {
            print(a, b)
            return 1}
        )",

        R"(
Block=[
  FuncDef=(
    Name="func1",
    Params=(
      Id(value="a"),
      Id(value="b")
    ),
    Block=[
      FuncCall=(
        Callee=(Id(value="print"))
        Args=(
          Id(value="a"),
          Id(value="b")
        )
      ),
      Return(
        expr=Num(value="1")
      )
    ]
  )
]
        )"
      );

      // Nested Return OK
      BASIC_AST_TEST(
        R"(
          func func1(a, b) {
            if(1) {
              if(a == 1) {
                while(0) {
                  return 1
                }
              }
            }
          }
        )",

        R"(
Block=[
  FuncDef=(
    Name="func1",
    Params=(
      Id(value="a"),
      Id(value="b")
    ),
    Block=[
      IfBlock=(
        Condition=(Num(value="1"))
        Block=[
          IfBlock=(
            Condition=(RelationalEqualTo(
              left=Id(value="a"), 
              right=Num(value="1")
            ))
            Block=[
              WhileBlock=(
                Condition=(Num(value="0"))
                Block=[
                  Return(
                    expr=Num(value="1")
                  )
                ]
              )
            ]
          )
        ]
      )
    ]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          func func1(a, b) {
            print(a, b)
            if(a == 1) {
              return 1 + 1 == 2
            }
          }
        )",

        R"(
Block=[
  FuncDef=(
    Name="func1",
    Params=(
      Id(value="a"),
      Id(value="b")
    ),
    Block=[
      FuncCall=(
        Callee=(Id(value="print"))
        Args=(
          Id(value="a"),
          Id(value="b")
        )
      ),
      IfBlock=(
        Condition=(RelationalEqualTo(
          left=Id(value="a"), 
          right=Num(value="1")
        ))
        Block=[
          Return(
            expr=RelationalEqualTo(
              left=Add(
                left=Num(value="1"), 
                right=Num(value="1")
              ), 
              right=Num(value="2")
            )
          )
        ]
      )
    ]
  )
]
        )"
      );


    }


    TEST_BLOCK("Happy If Statement") {

      // NESTED IF STATEMENTS
      BASIC_AST_TEST(
        R"(
          if(22+1) {
            if(1) {
              if(3) {
              }
            }
          }
        )",

        R"(
Block=[
  IfBlock=(
    Condition=(Add(
      left=Num(value="22"), 
      right=Num(value="1")
    ))
    Block=[
      IfBlock=(
        Condition=(Num(value="1"))
        Block=[
          IfBlock=(
            Condition=(Num(value="3"))
            Block=[]
          )
        ]
      )
    ]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          if(1) {
          }
        )",
        R"(
Block=[
  IfBlock=(
    Condition=(Num(value="1"))
    Block=[]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          if(1+1) {
          }
        )",
        R"(
Block=[
  IfBlock=(
    Condition=(Add(
      left=Num(value="1"), 
      right=Num(value="1")
    ))
    Block=[]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          if(1+1) {
          }
          1 + 2
          if(3) {
            print(1)
          }
        )",

        R"(
Block=[
  IfBlock=(
    Condition=(Add(
      left=Num(value="1"), 
      right=Num(value="1")
    ))
    Block=[]
  ),
  Add(
    left=Num(value="1"), 
    right=Num(value="2")
  ),
  IfBlock=(
    Condition=(Num(value="3"))
    Block=[
      FuncCall=(
        Callee=(Id(value="print"))
        Args=(
          Num(value="1")
        )
      )
    ]
  )
]
        )"
      );

    }

    TEST_BLOCK("Happy While Loop") {


      // NESTED WHILE LOOP
      BASIC_AST_TEST(
        R"(
          while(22+1) {
            while(1) {
              while(3) {
              }
            }
          }
        )",

        R"(
Block=[
  WhileBlock=(
    Condition=(Add(
      left=Num(value="22"), 
      right=Num(value="1")
    ))
    Block=[
      WhileBlock=(
        Condition=(Num(value="1"))
        Block=[
          WhileBlock=(
            Condition=(Num(value="3"))
            Block=[]
          )
        ]
      )
    ]
  )
]
        )"
      );


      BASIC_AST_TEST(
        R"(
          while(22+1) {
            1+1
            2+2
          }

          (1 + 2 + 3) * 10

          while(1) {}
          while(1) {      }

          while(1) {
      
          }

        )",

        R"(

Block=[
  WhileBlock=(
    Condition=(Add(
      left=Num(value="22"), 
      right=Num(value="1")
    ))
    Block=[
      Add(
        left=Num(value="1"), 
        right=Num(value="1")
      ),
      Add(
        left=Num(value="2"), 
        right=Num(value="2")
      )
    ]
  ),
  Multiply(
    left=Add(
      left=Add(
        left=Num(value="1"), 
        right=Num(value="2")
      ), 
      right=Num(value="3")
    ), 
    right=Num(value="10")
  ),
  WhileBlock=(
    Condition=(Num(value="1"))
    Block=[]
  ),
  WhileBlock=(
    Condition=(Num(value="1"))
    Block=[]
  ),
  WhileBlock=(
    Condition=(Num(value="1"))
    Block=[]
  )
]
    )"

      );


      BASIC_AST_TEST(
        R"(
          while(1) {
            break
          }
        )",

        R"(
Block=[
  WhileBlock=(
    Condition=(Num(value="1"))
    Block=[
      Break()
    ]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          while(1) {
            break
            1 + 2
          }
        )",

        R"(
Block=[
  WhileBlock=(
    Condition=(Num(value="1"))
    Block=[
      Break(),
      Add(
        left=Num(value="1"), 
        right=Num(value="2")
      )
    ]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          while (1) {
            while(1) {
              break
            }
            break
          }
        )",

        R"(
Block=[
  WhileBlock=(
    Condition=(Num(value="1"))
    Block=[
      WhileBlock=(
        Condition=(Num(value="1"))
        Block=[
          Break()
        ]
      ),
      Break()
    ]
  )
]
        )"
      );

      // NOTE: IF statement backtracks the while loop in the outer scope
      BASIC_AST_TEST(
        R"(
          while (1) {
            while(1) {
              break
            }
            if(0) {
              break
            }
          }
        )",

        R"(
Block=[
  WhileBlock=(
    Condition=(Num(value="1"))
    Block=[
      WhileBlock=(
        Condition=(Num(value="1"))
        Block=[
          Break()
        ]
      ),
      IfBlock=(
        Condition=(Num(value="0"))
        Block=[
          Break()
        ]
      )
    ]
  )
]
        )"
      );

      BASIC_AST_TEST(
        R"(
          while(1) {
            if(0) {
              break
            }
            1 + 2
          }
        )",

        R"(
Block=[
  WhileBlock=(
    Condition=(Num(value="1"))
    Block=[
      IfBlock=(
        Condition=(Num(value="0"))
        Block=[
          Break()
        ]
      ),
      Add(
        left=Num(value="1"), 
        right=Num(value="2")
      )
    ]
  )
]
      )");

    }

    TEST_BLOCK("Error Handling | Expression line breaks") {
      const std::string source_arr[][1] = {
        {
          R"(

            while(22+1) {
              1+1
              2+2

              (1 + 2 + 3) * 10

          )",
        },

        {

          R"(

            while(22+1 {
              1+1
              2+2

              (1 + 2 + 3) * 10

          )",
        },

        {

          // Expecting end of expression '\n', ';', '}' 
          R"(

            1 + 2 11

          )",
        },

        {

          R"(
            12;12 4

          )",

        },


        {
          R"(
            while(1) {
              1 1
            }
          )",
        },

        {

          // Unmatched '}'
          R"(

            }
            }
          )",
        },

        {

          R"(

            while(1) {


          )",
        },

        {

          R"(

            while() 

          )",
        },

        {

          R"(

            while() {}

          )",

        },

        {
          // Making sure there are no errors from valgrind while matching a keyword at the end of the stream
          R"(

            while)",

        },

      };

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context); 

      for(size_t i = 0; i < get_arr_size(source_arr); i++) {
        AstGenerator ast_generator;
        NodeParser node_parser;

        const std::string& source_code = source_arr[i][0];

        ast_generator.attach_context(main_context);
        ast_generator.set_source(source_code.c_str());

        SmartASTNode ast_root = ast_generator.parse();

        const auto &errors = ast_generator.get_syntax_errors();
        
        for(size_t i = 0; i < errors.size(); i++) {
          const SyntaxError& syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        assertm("Must Contain Errors", errors.size() > 0);

      }

    }

    TEST_BLOCK("SEMI COLON IGNORED")
    {
      const std::string source_arr[][1] = {
          {
            R"(
              ;while(22+1) {
              1+1
              2+2
            };

            (1 + 2 + 3) * 10; 11;;

            )",

          },

          {

            R"(
              ;;;;;
              1 + 2; 11;

            )",
          },

          {

            R"(
              12;12;;;;

            )",
          },

          {

            R"(
              1   
              44
            )",
          },

          {

            R"(
              while(1+1) {
                1+1
              }
            )",
          },

          {
            R"(
              while (1+1) {
                1+1          }
            )",
          },

          {
            R"(
              while (1+1) {
                1+1;          }
            )",
          },

          {
            R"(
              while1 + 2;
            )",
          },

          {
            R"(
              while_ + 2;
            )",
          },

          {
            R"(
              whileeP + 2;
            )",
          },

      };

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context); 

      for(size_t i = 0; i < get_arr_size(source_arr); i++) {
        AstGenerator ast_generator;
        NodeParser node_parser;

        ast_generator.attach_context(main_context);

        const std::string& source_code = source_arr[i][0];

        ast_generator.set_source(source_code.c_str());

        SmartASTNode ast_root = ast_generator.parse();

        const auto &errors = ast_generator.get_syntax_errors();
        
        for(size_t i = 0; i < errors.size(); i++) {
          const SyntaxError& syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        std::string result = node_parser.node_to_string(ast_root);

        Logger::info(result);


        assertm("Must NOT Contain Errors", errors.size() == 0);

    }
    }

    TEST_BLOCK("RESERVED KEYWORD TESTS") {
      const std::string source_arr[][1] = {
        {

          R"(
            while + 1;
          )",

        },

        {
          R"(
            while+2;
          )",
        },

        {
          R"(
            while{};
          )",
        },

      };

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context); 

      for(size_t i = 0; i < get_arr_size(source_arr); i++) {
        AstGenerator ast_generator;
        NodeParser node_parser;

        const std::string& source_code = source_arr[i][0];

        ast_generator.attach_context(main_context);
        ast_generator.set_source(source_code.c_str());

        SmartASTNode ast_root = ast_generator.parse();

        const auto &errors = ast_generator.get_syntax_errors();
        
        for(size_t i = 0; i < errors.size(); i++) {
          const SyntaxError& syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        std::string result = node_parser.node_to_string(ast_root);

        Logger::info(result);

        assertm("Must Contain Errors", errors.size() != 0);

    }

  }



  }


  TEST_SECTION("Advanced Tests") {

    TEST_BLOCK("Function Definition | Failing Tests")
    {
      const std::string source_arr[][1] = {
          {

          // Missing Function Name
          R"(
            func 
          )",

          },

          // NOT A FUNCTION CALL
          {
            R"(
              func()
            )",
          },

          // Missing Opening '{'
          {
            R"(
              func a() 

            )",
          },
          {
            R"(
              func a() 
              }

            )",
          },

          // Missing Closing '}'
          {
            R"(
              func a() {

            )",
          },

          // Missing Closing ')'
          {
            R"(
              func a(
            )",
          },

          // Missing Closing ')'
          {
            R"(
              func a(b,
            )",
          },


          // Parameters as expressions
          {
            R"(
              func a(b+1, c+1) {
              }
            )",
          },

          // Function Definition inside of a function forbidden
          // Function Definition only possible at the top level
          // TODO: Introduce lambdas for this reason
          {
            R"(
              func a(b, c) {
                func a1(d, e) {
                }
              }
            )",
          },

          {
            R"(
              while(1) {
                func a1(d, e) {
                  return d + e
                }
              }
            )",
          },
      };

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context *main_context = obj_cast<Context *>(shrd_context.get());
      setUpContext(main_context);

      for (size_t i = 0; i < get_arr_size(source_arr); i++)
      {
        AstGenerator ast_generator;
        NodeParser node_parser;

        const std::string &source_code = source_arr[i][0];

        ast_generator.attach_context(main_context);
        ast_generator.set_source(source_code.c_str());

        SmartASTNode ast_root = ast_generator.parse();

        const auto &errors = ast_generator.get_syntax_errors();

        for (size_t i = 0; i < errors.size(); i++)
        {
          const SyntaxError &syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        std::string result = node_parser.node_to_string(ast_root);

        Logger::info(result);

        assertm("Must Contain Errors", errors.size() != 0);
      }
    }



    TEST_BLOCK("Failing Tests | Break Keyword Out Of The Control Flow Loop") {
      const std::string source_arr[][1] = {
        {

          R"(
            1
            break
          )",

        },

        {
          R"(
            while(2) {
            }
            break
          )",
        },

        {
          R"(
            break;
          )",
        },

        {
          R"(
            if(0) {
              break;
            }
          )",
        },

        // NESTED
        {
          R"(
            if(0) {
              if(1) {
                break;
              }
            }
          )",
        },

        // INSIDE OF A FUNCTION
        {
          R"(
            func func1() {
              break
            }
          )",
        },


      };

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context); 

      for(size_t i = 0; i < get_arr_size(source_arr); i++) {
        AstGenerator ast_generator;
        NodeParser node_parser;

        const std::string& source_code = source_arr[i][0];

        ast_generator.attach_context(main_context);
        ast_generator.set_source(source_code.c_str());

        assertm("Block Stack Empty", ast_generator.block_stack.size() == 0);

        SmartASTNode ast_root = ast_generator.parse();

        assertm("Block Stack Clearned", ast_generator.block_stack.size() == 0);

        const auto &errors = ast_generator.get_syntax_errors();
        
        for(size_t i = 0; i < errors.size(); i++) {
          const SyntaxError& syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        std::string result = node_parser.node_to_string(ast_root);

        Logger::info(result);

        assertm("Must Contain Errors", errors.size() != 0);

      }

    }

    TEST_BLOCK("Failing Tests | Return Keyword Out Of The Function Reach/Scope") {
      const std::string source_arr[][1] = {
        {

          R"(
            return 1
          )",

        },

        {
          R"(
            while(2) {
            }
            return 2
          )",
        },

        {
          R"(
            if(0) {
              return 0;
            }
          )",
        },

        {
          R"(
            while(0) {
              return 0;
            }
          )",
        },

        {
          R"(
            if(0) {
              while(0) {
                return 0
              }
            }
          )",
        },

        // NESTED
        {
          R"(
            if(0) {
              if(1) {
                return 1;
              }
            }
          )",
        },

      };

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context); 

      for(size_t i = 0; i < get_arr_size(source_arr); i++) {
        AstGenerator ast_generator;
        NodeParser node_parser;

        const std::string& source_code = source_arr[i][0];

        ast_generator.attach_context(main_context);
        ast_generator.set_source(source_code.c_str());

        assertm("Block Stack Empty", ast_generator.block_stack.size() == 0);

        SmartASTNode ast_root = ast_generator.parse();

        assertm("Block Stack Clearned", ast_generator.block_stack.size() == 0);

        const auto &errors = ast_generator.get_syntax_errors();
        
        for(size_t i = 0; i < errors.size(); i++) {
          const SyntaxError& syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        std::string result = node_parser.node_to_string(ast_root);

        Logger::info(result);

        assertm("Must Contain Errors", errors.size() != 0);

      }

    }

    TEST_BLOCK("Arithmetic Operator Precedence") {
      const std::string source_arr[][2] = {
          {
              R"(
              1 + 3 * 10;
            )",
              R"(
              Block=[
                Add(
                  left=Num(value="1"), 
                  right=Multiply(
                    left=Num(value="3"), 
                    right=Num(value="10")
                  )
                )
              ]
            )",
          },

          {
              R"(
              1 * 3 + 10 * 2;
            )",
              R"(
Block=[
  Add(
    left=Multiply(
      left=Num(value="1"), 
      right=Num(value="3")
    ), 
    right=Multiply(
      left=Num(value="10"), 
      right=Num(value="2")
    )
  )
]
            )",
          },
          {
              R"(
    (1 + (2 * (3 + 4))) * 5;
  )",
              R"(
Block=[
  Multiply(
    left=Add(
      left=Num(value="1"),
      right=Multiply(
        left=Num(value="2"),
        right=Add(
          left=Num(value="3"),
          right=Num(value="4")
        )
      )
    ),
    right=Num(value="5")
  )
]
  )",
          },

// Long Precedence Chain
{
  R"(
    1 + 2 * 3 + 4 * 5 + 6;
  )",
  R"(
Block=[
  Add(
    left=Add(
      left=Add(
        left=Num(value="1"),
        right=Multiply(
          left=Num(value="2"),
          right=Num(value="3")
        )
      ),
      right=Multiply(
        left=Num(value="4"),
        right=Num(value="5")
      )
    ),
    right=Num(value="6")
  )
]
  )",
},
// Parentheses + Mixed Precedence
{
  R"(
    (1 + 2) * (3 + 4 * 5);
  )",
  R"(
Block=[
  Multiply(
    left=Add(
      left=Num(value="1"),
      right=Num(value="2")
    ),
    right=Add(
      left=Num(value="3"),
      right=Multiply(
        left=Num(value="4"),
        right=Num(value="5")
      )
    )
  )
]
  )",
},
{
  R"(
    1 + 2;
    3 * 4;
    5 + 6 * 7;
  )",
  R"(
Block=[
  Add(
    left=Num(value="1"),
    right=Num(value="2")
  ),
  Multiply(
    left=Num(value="3"),
    right=Num(value="4")
  ),
  Add(
    left=Num(value="5"),
    right=Multiply(
      left=Num(value="6"),
      right=Num(value="7")
    )
  )
]
  )",
},
      };


      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context* main_context = obj_cast<Context*>(shrd_context.get());
      setUpContext(main_context); 

      for(size_t i = 0; i < get_arr_size(source_arr); i++) {
        AstGenerator ast_generator;
        NodeParser node_parser;

        const std::string& source_code = source_arr[i][0];
        const std::string ast_output = strip_white_space(source_arr[i][1]);

        ast_generator.attach_context(main_context);
        ast_generator.set_source(source_code.c_str());

        SmartASTNode ast_root = ast_generator.parse();

        const auto &errors = ast_generator.get_syntax_errors();
        
        for(size_t i = 0; i < errors.size(); i++) {
          const SyntaxError& syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        std::string result = node_parser.node_to_string(ast_root);

        Logger::info(result);


        result = strip_white_space(result);

        assertm("Must NOT Contain Errors", errors.size() == 0);

        assertm("AST Structure MUST MATCH", result == ast_output);

      }

    }

  }

  TEST_BLOCK("Relational Operators | Basic Tests") {

    BASIC_AST_TEST(
      R"(
        1 < 2
      )",

      R"(
Block=[
  RelationalLessThan(
    left=Num(value="1"), 
    right=Num(value="2")
  )
]
      )"
    );

    BASIC_AST_TEST(
      R"(
        1 > 2
      )",

      R"(
Block=[
  RelationalGreaterThan(
    left=Num(value="1"), 
    right=Num(value="2")
  )
]
      )"
    );

    BASIC_AST_TEST(
      R"(
        1 >= 2
      )",

      R"(
Block=[
  RelationalGreaterThanEqualTo(
    left=Num(value="1"), 
    right=Num(value="2")
  )
]
      )"
    );

    BASIC_AST_TEST(
      R"(
        1 <= 2
      )",

      R"(
Block=[
  RelationalLessThanEqualTo(
    left=Num(value="1"), 
    right=Num(value="2")
  )
]
      )"
    );

    BASIC_AST_TEST(
      R"(
        1 == 2
      )",

      R"(
Block=[
  RelationalEqualTo(
    left=Num(value="1"), 
    right=Num(value="2")
  )
]
      )"
    );

    BASIC_AST_TEST(
      R"(
        1 != 2
      )",

      R"(
Block=[
  RelationalNotEqualTo(
    left=Num(value="1"), 
    right=Num(value="2")
  )
]
      )"
    );


    BASIC_AST_TEST(
      R"(
        1 + 1 > 2 + 2
      )",

      R"(
Block=[
  RelationalGreaterThan(
    left=Add(
      left=Num(value="1"), 
      right=Num(value="1")
    ), 
    right=Add(
      left=Num(value="2"), 
      right=Num(value="2")
    )
  )
]
      )"
    );

    BASIC_AST_TEST(
      R"(
        1 + 1 < 2 + 2
      )",

      R"(
Block=[
  RelationalLessThan(
    left=Add(
      left=Num(value="1"), 
      right=Num(value="1")
    ), 
    right=Add(
      left=Num(value="2"), 
      right=Num(value="2")
    )
  )
]
      )"
    );


  }

  TEST_BLOCK("Logical Operators | Basic Tests") {

    BASIC_AST_TEST(
      R"(
        1 && 2
      )",

      R"(
Block=[
  LogicalAnd(
    left=Num(value="1"), 
    right=Num(value="2")
  )
]
      )"
    );

    BASIC_AST_TEST(
      R"(
        1 == 1 && 2
      )",

      R"(
Block=[
  LogicalAnd(
    left=RelationalEqualTo(
      left=Num(value="1"), 
      right=Num(value="1")
    ), 
    right=Num(value="2")
  )
]
      )"
    );

    BASIC_AST_TEST(
      R"(
        1 + 1 && 2
      )",

      R"(
Block=[
  LogicalAnd(
    left=Add(
      left=Num(value="1"), 
      right=Num(value="1")
    ), 
    right=Num(value="2")
  )
]
      )"
    );


    BASIC_AST_TEST(
      R"(
        1 || 2
      )",

      R"(
Block=[
  LogicalOr(
    left=Num(value="1"), 
    right=Num(value="2")
  )
]
      )"
    );


    // NOTE: THE LOGICAL OR TAKES PRECEDENCE/PRIORITY OVER THE LOGICAL AND
    BASIC_AST_TEST(
      R"(
        1 && 4 || 2 && 3
      )",

      R"(
Block=[
  LogicalOr(
    left=LogicalAnd(
      left=Num(value="1"), 
      right=Num(value="4")
    ), 
    right=LogicalAnd(
      left=Num(value="2"), 
      right=Num(value="3")
    )
  )
]
      )"
    );


  }


  TEST_SECTION("Assignment Expressions") {

    TEST_BLOCK("Assignment Expressions | Basic Tests")
    {

      BASIC_AST_TEST(
          R"(
          d = 1
        )",

          R"(
        Block=[
          AssignmentExpression(
            left=Id(value="d"), 
            right=Num(value="1")
          )
        ]
        )");

      BASIC_AST_TEST(
          R"(
        ((d) = 1)
      )",

          R"(
Block=[
  AssignmentExpression(
    left=Id(value="d"), 
    right=Num(value="1")
  )
]
      )");

      BASIC_AST_TEST(
        R"(
        ((d)) = 1
        )",

          R"(
Block=[
  AssignmentExpression(
    left=Id(value="d"), 
    right=Num(value="1")
  )
]
      )");

      BASIC_AST_TEST(
          R"(
        d = 1
        d = 2
      )",

          R"(
Block=[
  AssignmentExpression(
    left=Id(value="d"), 
    right=Num(value="1")
  ),
  AssignmentExpression(
    left=Id(value="d"), 
    right=Num(value="2")
  )
]
      )");

      BASIC_AST_TEST(
          R"(
        while(a = 3) {
          d = 1
          d = 2
        }
      )",

          R"(
Block=[
  WhileBlock=(
    Condition=(AssignmentExpression(
      left=Id(value="a"), 
      right=Num(value="3")
    ))
    Block=[
      AssignmentExpression(
        left=Id(value="d"), 
        right=Num(value="1")
      ),
      AssignmentExpression(
        left=Id(value="d"), 
        right=Num(value="2")
      )
    ]
  )
]
      )");
    }

    TEST_BLOCK("Assignment Expressions | Failing Tests")
    {
      const std::string source_arr[][1] = {
          {

              R"(
            a =
          )",

          },

          // NOTE: ONLY LValue is assignable
          {
            R"(
              1 = 3;
            )",
          },
          {
            R"(
              1+1 = 3;
            )",
          },
          {
            R"(
              a+b = 3;
            )",
          },
          // TODO: storeSubsript is also assignable

          {
            R"(
              y = == 1
            )",
          },

          // CAN'T ASSIGN RESERVED KEYWORDS
          {
            R"(
              func = 1
            )"
          },

          {
            R"(
              nill = 1
            )"
          },

          {
            R"(
              true = 1
            )"
          },

          {
            R"(
              false = 1
            )"
          },

      };

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context *main_context = obj_cast<Context *>(shrd_context.get());
      setUpContext(main_context);

      for (size_t i = 0; i < get_arr_size(source_arr); i++)
      {
        AstGenerator ast_generator;
        NodeParser node_parser;

        const std::string &source_code = source_arr[i][0];

        ast_generator.attach_context(main_context);
        ast_generator.set_source(source_code.c_str());

        SmartASTNode ast_root = ast_generator.parse();

        const auto &errors = ast_generator.get_syntax_errors();

        for (size_t i = 0; i < errors.size(); i++)
        {
          const SyntaxError &syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        std::string result = node_parser.node_to_string(ast_root);

        Logger::info(result);

        assertm("Must Contain Errors", errors.size() != 0);
      }
    }

  }

  TEST_SECTION("Unary Operator") {

    TEST_BLOCK("Unary Operator | Basic Tests") {
      BASIC_AST_TEST(
          R"(
            !1
      )",

          R"(
Block=[
  LogicalNot(
    expr=Num(value="1")
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            !!1
      )",

          R"(
Block=[
  LogicalNot(
    expr=LogicalNot(
      expr=Num(value="1")
    )
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            !(!(1))
      )",

          R"(
Block=[
  LogicalNot(
    expr=LogicalNot(
      expr=Num(value="1")
    )
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            !(1 && !3)
      )",

          R"(
Block=[
  LogicalNot(
    expr=LogicalAnd(
      left=Num(value="1"), 
      right=LogicalNot(
        expr=Num(value="3")
      )
    )
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            -1
      )",

          R"(
Block=[
  UnaryMinus(
    expr=Num(value="1")
  )
]

      )");

      BASIC_AST_TEST(
          R"(
            +1
      )",

          R"(
Block=[
  UnaryPlus(
    expr=Num(value="1")
  )
]

      )");

      BASIC_AST_TEST(
          R"(
            --1
      )",

          R"(
Block=[
  UnaryMinus(
    expr=UnaryMinus(
      expr=Num(value="1")
    )
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            !-1
      )",

          R"(
Block=[
  LogicalNot(
    expr=UnaryMinus(
      expr=Num(value="1")
    )
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            1 - -1 - --1
      )",

          R"(
Block=[
  Subtract(
    left=Subtract(
      left=Num(value="1"), 
      right=UnaryMinus(
        expr=Num(value="1")
      )
    ), 
    right=UnaryMinus(
      expr=UnaryMinus(
        expr=Num(value="1")
      )
    )
  )
]
      )");

    }

  }

  TEST_SECTION("Postfix Expressions") {


    TEST_BLOCK("Postfix Expressions | Basic Tests | Member Access") {

      BASIC_AST_TEST(
          R"(
            a.b
      )",

          R"(
Block=[
  GetMember(
    obj=Id(value="a"), 
    member_id=Id(value="b")
  )
]

      )");


      BASIC_AST_TEST(
          R"(
            a
            .
            b
      )",

          R"(
Block=[
  GetMember(
    obj=Id(value="a"), 
    member_id=Id(value="b")
  )
]

      )");


      // Treated as expression
      BASIC_AST_TEST(
          R"(
            a.b + b
      )",

          R"(
Block=[
  Add(
    left=GetMember(
      obj=Id(value="a"), 
      member_id=Id(value="b")
    ), 
    right=Id(value="b")
  )
]
      )");



      BASIC_AST_TEST(
          R"(
            a.b.c
      )",

          R"(
Block=[
  GetMember(
    obj=GetMember(
      obj=Id(value="a"), 
      member_id=Id(value="b")
    ), 
    member_id=Id(value="c")
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            a.b.c.d
      )",

          R"(
Block=[
  GetMember(
    obj=GetMember(
      obj=GetMember(
        obj=Id(value="a"), 
        member_id=Id(value="b")
      ), 
      member_id=Id(value="c")
    ), 
    member_id=Id(value="d")
  )
]
      )");

      // Obj as Expression
      BASIC_AST_TEST(
          R"(
            (1+1).b.c.d
      )",

          R"(
Block=[
  GetMember(
    obj=GetMember(
      obj=GetMember(
        obj=Add(
          left=Num(value="1"), 
          right=Num(value="1")
        ), 
        member_id=Id(value="b")
      ), 
      member_id=Id(value="c")
    ), 
    member_id=Id(value="d")
  )
]
      )");

      // Chaining Member Access and Function Calls
      BASIC_AST_TEST(
          R"(
            a.b().c()()
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(FuncCall=(
      Callee=(GetMember(
        obj=FuncCall=(
          Callee=(GetMember(
            obj=Id(value="a"), 
            member_id=Id(value="b")
          ))
          Args=()
        ), 
        member_id=Id(value="c")
      ))
      Args=()
    ))
    Args=()
  )
]
      )");


      TEST_BLOCK("Member Access | Failing Tests")
      {
        const std::string source_arr[][1] = {

          {
            R"(
              a.1
            )",
          },

          {
            R"(
              a.
            )",
          },

          {
            R"(
              a.)",
          },

          {
            R"(
              a
              .
              1
            )",
          },

          // Member Access + Keyword
          {
            R"(
              a.while
            )",
          },

          {
            R"(
              a.func
            )",
          },



        };

        // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
        UniquePtr<Context> shrd_context = makeUniqueContext();
        Context *main_context = obj_cast<Context *>(shrd_context.get());
        setUpContext(main_context);

        for (size_t i = 0; i < get_arr_size(source_arr); i++)
        {
          AstGenerator ast_generator;
          NodeParser node_parser;

          const std::string &source_code = source_arr[i][0];

          ast_generator.attach_context(main_context);
          ast_generator.set_source(source_code.c_str());

          SmartASTNode ast_root = ast_generator.parse();

          const auto &errors = ast_generator.get_syntax_errors();

          for (size_t i = 0; i < errors.size(); i++)
          {
            const SyntaxError &syntax_error = errors[i];
            Logger::info(syntax_error.get_error());
          }

          std::string result = node_parser.node_to_string(ast_root);

          Logger::info(result);

          assertm("Must Contain Errors", errors.size() != 0);
        }

      }


    }


    TEST_BLOCK("Postfix Expressions | Basic Tests | Function Call") {

      BASIC_AST_TEST(
          R"(
            func_call()
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(Id(value="func_call"))
    Args=()
  )
]
      )");

      // CALL WITH WHITESPACE IN-BETWEEN OK
      BASIC_AST_TEST(
          R"(
            func_call(    )
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(Id(value="func_call"))
    Args=()
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            func_call(
            
              )
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(Id(value="func_call"))
    Args=()
  )
]
      )");


      // NOTE: Chaining Postfix Operators
      BASIC_AST_TEST(
          R"(
            func_call()()
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(FuncCall=(
      Callee=(Id(value="func_call"))
      Args=()
    ))
    Args=()
  )
]
      )");

        BASIC_AST_TEST(
          R"(
            func_call(a)(b)
          )",

          R"(
Block=[
  FuncCall=(
    Callee=(FuncCall=(
      Callee=(Id(value="func_call"))
      Args=(
        Id(value="a")
      )
    ))
    Args=(
      Id(value="b")
    )
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            func_call(a)(b)(c)
          )",

          R"(
Block=[
  FuncCall=(
    Callee=(FuncCall=(
      Callee=(FuncCall=(
        Callee=(Id(value="func_call"))
        Args=(
          Id(value="a")
        )
      ))
      Args=(
        Id(value="b")
      )
    ))
    Args=(
      Id(value="c")
    )
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            (func_call())()
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(FuncCall=(
      Callee=(Id(value="func_call"))
      Args=()
    ))
    Args=()
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            func_call(1)
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(Id(value="func_call"))
    Args=(
      Num(value="1")
    )
  )
]
      )");


      BASIC_AST_TEST(
          R"(
            func_call(1, 2)
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(Id(value="func_call"))
    Args=(
      Num(value="1"),
      Num(value="2")
    )
  )
]
      )");

      BASIC_AST_TEST(
          R"(
            func_call(1, 2,)
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(Id(value="func_call"))
    Args=(
      Num(value="1"),
      Num(value="2")
    )
  )
]
      )");

      // Nested function call arguments
      BASIC_AST_TEST(
          R"(
            func_call(func_call(1), func_call(2))
      )",

          R"(
Block=[
  FuncCall=(
    Callee=(Id(value="func_call"))
    Args=(
      FuncCall=(
        Callee=(Id(value="func_call"))
        Args=(
          Num(value="1")
        )
      ),
      FuncCall=(
        Callee=(Id(value="func_call"))
        Args=(
          Num(value="2")
        )
      )
    )
  )
]
      )");

      TEST_BLOCK("Function Call | Failing Tests")
      {
        const std::string source_arr[][1] = {

          // NOT A FUNCTION CALL: func is a keyword
          {
            R"(
              func()
            )",

          },

          {
            R"(
              func_call(b,
            )",

          },

          {

            R"(
              func_call(b, b
            )",

          },

          {
            R"(
              func_call(b, b,
            )",

          },

          {
            R"(
              func_call(
            )",
          },

          {
            R"(
              func_call()(
            )",
          },

          {
            R"(
              func_call(,)
            )",
          },

          {
            "func_call(",
          },

          {
            "func_call()(",
          },

          {
            "func1(a,,b)"
          },
          {
            "func1(a,,b"
          },
          {
            "func1(a,,"
          },
          {
            "func1(a,,)"
          },

          // Arguments must be separated by a comma
          {
            "func1(a b)"
          },
        };

        // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
        UniquePtr<Context> shrd_context = makeUniqueContext();
        Context *main_context = obj_cast<Context *>(shrd_context.get());
        setUpContext(main_context);

        for (size_t i = 0; i < get_arr_size(source_arr); i++)
        {
          AstGenerator ast_generator;
          NodeParser node_parser;

          const std::string &source_code = source_arr[i][0];

          ast_generator.attach_context(main_context);
          ast_generator.set_source(source_code.c_str());

          SmartASTNode ast_root = ast_generator.parse();

          const auto &errors = ast_generator.get_syntax_errors();

          for (size_t i = 0; i < errors.size(); i++)
          {
            const SyntaxError &syntax_error = errors[i];
            Logger::info(syntax_error.get_error());
          }

          std::string result = node_parser.node_to_string(ast_root);

          Logger::info(result);

          assertm("Must Contain Errors", errors.size() != 0);
        }

      }



    }

  }

  TEST_BLOCK("Postfix in combination with Unary Operators | Basic Tests") {

    BASIC_AST_TEST(
      R"(
        -func_call()
      )",

          R"(
Block=[
  UnaryMinus(
    expr=FuncCall=(
      Callee=(Id(value="func_call"))
      Args=()
    )
  )
]
      )");


  }


  TEST_BLOCK("Decl Keyword Basic Tests") {
    BASIC_AST_TEST(
      R"(
        decl d = 1
      )",

      R"(
Block=[
  Decl(
    Name="d",
    Init=Num(value="1")
  )
]
      )");

    BASIC_AST_TEST(
      R"(
        decl d = 1
        decl d1 = 2
      )",

      R"(
Block=[
  Decl(
    Name="d",
    Init=Num(value="1")
  ),
  Decl(
    Name="d1",
    Init=Num(value="2")
  )
]
      )");

    BASIC_AST_TEST(
      R"(
        decl d = 1
        decl d1 = 2
        + 1
      )",

      R"(
Block=[
  Decl(
    Name="d",
    Init=Num(value="1")
  ),
  Decl(
    Name="d1",
    Init=Add(
      left=Num(value="2"), 
      right=Num(value="1")
    )
  )
]
      )");

    BASIC_AST_TEST(
      R"(
        decl d = 1
        decl d1 = 2
        1 + 1
      )",

      R"(
Block=[
  Decl(
    Name="d",
    Init=Num(value="1")
  ),
  Decl(
    Name="d1",
    Init=Num(value="2")
  ),
  Add(
    left=Num(value="1"), 
    right=Num(value="1")
  )
]
      )");


    BASIC_AST_TEST(
      R"(
        decl d = 1 + 1;
      )",

      R"(
Block=[
  Decl(
    Name="d",
    Init=Add(
      left=Num(value="1"), 
      right=Num(value="1")
    )
  )
]
      )");

    BASIC_AST_TEST(
      R"(
        while (1) {
          decl d = 1 + 1}
      )",

      R"(
Block=[
  WhileBlock=(
    Condition=(Num(value="1"))
    Block=[
      Decl(
        Name="d",
        Init=Add(
          left=Num(value="1"), 
          right=Num(value="1")
        )
      )
    ]
  )
]
      )");

    TEST_BLOCK("Decl Keyword | Failing Tests")
    {
      const std::string source_arr[][1] = {
          {

            R"(
              decl d = 1 1
            )",

          },

          {

            R"(
              decl d = 1  decl d1 = 1
            )",

          },

          {

            R"(
              (decl d = 1)
            )",

          },

          {
            R"(
              decl 
            )",
          },

          {
            R"(
              decl d
            )",
          },

          {
            R"(
              decl d =
            )",
          },

          {
            R"(
              func decl d = 1
            )",
          },
      };

      // IT IS OK FOR CONTEXT TO BE REUSED IN THIS CASE
      UniquePtr<Context> shrd_context = makeUniqueContext();
      Context *main_context = obj_cast<Context *>(shrd_context.get());
      setUpContext(main_context);

      for (size_t i = 0; i < get_arr_size(source_arr); i++)
      {
        AstGenerator ast_generator;
        NodeParser node_parser;

        const std::string &source_code = source_arr[i][0];

        ast_generator.attach_context(main_context);
        ast_generator.set_source(source_code.c_str());

        SmartASTNode ast_root = ast_generator.parse();

        const auto &errors = ast_generator.get_syntax_errors();

        for (size_t i = 0; i < errors.size(); i++)
        {
          const SyntaxError &syntax_error = errors[i];
          Logger::info(syntax_error.get_error());
        }

        std::string result = node_parser.node_to_string(ast_root);

        Logger::info(result);

        assertm("Must Contain Errors", errors.size() != 0);
      }
    }
  }

  TEST_BLOCK("Happy True, False, Nill Keywords") {

      BASIC_AST_TEST(
      R"(
        true && false && nill
      )",

      R"(
Block=[
  LogicalAnd(
    left=LogicalAnd(
      left=True(), 
      right=False()
    ), 
    right=Nill()
  )
]
      )");

      BASIC_AST_TEST(
      R"(
        while(true) {
          decl b = nill
        }
      )",

      R"(
Block=[
  WhileBlock=(
    Condition=(True())
    Block=[
      Decl(
        Name="b",
        Init=Nill()
      )
    ]
  )
]
      )");

  }


  return 0;

}
#include <iostream>
#include <string>
#include <vector>

#include <cassert>

#define DECL_COMPILER_VERBOSE

#include <decl/lib/Compiler.h>

#include <decl/lib/Utility.h>

// TODO: Implement a variant of assert where we actually output the arguments to a stream and see them exactly since the C assert is macro.


using code_pair = std::pair<std::string, std::string>;
using code_pair_iter = const code_pair*;


int main() {
  TEST_BLOCK("Sizeof Compiler", {
      std::cout << sizeof(Lexer) << std::endl;
      std::cout << sizeof(Compiler) << std::endl;
      })

  // TODO: MORE EDGE-CASES

  TEST_BLOCK("Single Literal", {
      TEST_BLOCK("Number: Integer",
          {
          const std::string code = "1";

          Compiler compiler = Compiler(code.c_str());

          ExpressionFragment output = compiler.compile_expression();


          // std::cout << "Fragment Source: " << output.get_source() << std::endl;

          assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
          assert(output.get_xvalue() == ExpressionFragment::XValue::RVALUE);
          assert(output.get_source()
              == "1");


          })

      TEST_BLOCK("Number: Integer",
          {
          const std::string code = "12345";

          Compiler compiler = Compiler(code.c_str());

          ExpressionFragment output = compiler.compile_expression();


          // std::cout << "Fragment Source: " << output.get_source() << std::endl;

          assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
          assert(output.get_xvalue() == ExpressionFragment::XValue::RVALUE);
          assert(output.get_source()
              == "12345");


          })

      TEST_BLOCK("Enclosed Literal xvalue should still persist",
          {

          const std::string code = "(512)";

          Compiler compiler = Compiler(code.c_str());

          ExpressionFragment output = compiler.compile_expression();

          assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
          assert(output.get_xvalue() == ExpressionFragment::XValue::RVALUE);
          assert(output.get_source()
              == "512");
          })

  });

  TEST_BLOCK("Expressions: ONE LINE", {
      // ONE LINE

      const std::string code = "1 + 1 * 2";

      // std::cout << code.c_str() << std::endl;

      Compiler compiler = Compiler(code.c_str());

      Fragment output = compiler.compile();


      // std::cout << "Fragment Source: " << output.get_source() << std::endl;

      assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
      assert(output.get_source()
          == "plus(1, multiply(1, 2))");

      TEST_BLOCK("Division", {
          const std::string code = "1 / 2";

          Compiler compiler = Compiler(code.c_str());

          Fragment output = compiler.compile();


          // std::cout << "Fragment Source: " << output.get_source() << std::endl;

          assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
          assert(output.get_source()
              == "divide(1, 2)");

          })

      TEST_BLOCK("Subtraction", {
          const std::string code = "1 - 2";

          Compiler compiler = Compiler(code.c_str());

          Fragment output = compiler.compile();


          // std::cout << "Fragment Source: " << output.get_source() << std::endl;

          assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
          assert(output.get_source()
              == "minus(1, 2)");
          })

      TEST_BLOCK("More Complex", ({
            std::initializer_list<code_pair> expressions {
            { "26 * 49 - 42 + 2 / 23 / 88 - 12 * 68", "minus(plus(minus(multiply(26, 49), 42), divide(divide(2, 23), 88)), multiply(12, 68))" },
            { "(54) / (6 / 55 / (84)) * (95 + ((((41 / 76)))) - 65)", "multiply(divide(54, divide(divide(6, 55), 84)), minus(plus(95, divide(41, 76)), 65))" },
            // TODO: Add more tests/checks after implementing unary operators (unary plus and minus, even the rest)

            };

            for(code_pair_iter start = expressions.begin(); start != expressions.end(); start++) {
            Compiler compiler = Compiler(start->first.c_str());

            Fragment output = compiler.compile();

            std::cout << "Fragment Source: " << output.get_source() << std::endl;

            assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
            assert(output.get_source()
                == start->second);
            }



      }))

  })

  TEST_BLOCK("Expressions: TWO-LINE", {
      // TWO LINES

      const std::string code = "1 + 1 * 2\n1+2+3";

      // std::cout << code.c_str() << std::endl;

      Compiler compiler = Compiler(code.c_str());

      Fragment output = compiler.compile();


      // std::cout << "Fragment Source: " << output.get_source() << std::endl;

      assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
      assert(output.get_source()
          == "plus(1, multiply(1, 2))\nplus(plus(1, 2), 3)");

      })


  TEST_BLOCK("Expressions: TWO LINES OR MORE", ({
        // TWO-LINES OR MORE

        std::initializer_list<code_pair> expressions {
        { "1 + 1 * 2\n1+2+3\n1*33", "plus(1, multiply(1, 2))\nplus(plus(1, 2), 3)\nmultiply(1, 33)" },
        { "1 + 1 * \n\n 4", "plus(1, multiply(1, 4))" },
        { "1 + 1 / \n\n 4", "plus(1, divide(1, 4))" },
        };

        for(code_pair_iter start = expressions.begin(); start != expressions.end(); start++) {
        Compiler compiler = Compiler(start->first.c_str());

        Fragment output = compiler.compile();

        // std::cout << "Fragment Source: " << output.get_source() << std::endl;

        assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
        assert(output.get_source()
            == start->second);
        }

  }))

  TEST_BLOCK("Expressions: Opening and Closing Parenthesis", ({

        std::initializer_list<code_pair> expressions { 
        { "1+2 + (1+2)", "plus(plus(1, 2), plus(1, 2))" },
        { "(11*22)", "multiply(11, 22)" },
        { "((11*22))", "multiply(11, 22)" },

        { "11*22*11+1+1", "plus(plus(multiply(multiply(11, 22), 11), 1), 1)" },
        };

        for(code_pair_iter start = expressions.begin(); start != expressions.end(); start++) {
        Compiler compiler = Compiler(start->first.c_str());

        ExpressionFragment output = compiler.compile_expression();

        // std::cout << "Fragment Source: " << output.get_source() << std::endl;

        assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
        // NOTE: check xvalue so long as it is expression and one-liner
        assert(output.get_xvalue() == ExpressionFragment::XValue::RVALUE);
        assert(output.get_source()
            == start->second);
        }


        TEST_BLOCK("Invalid Pair", {

            const std::string code = "1+2) + (1+2))";
            Compiler compiler = Compiler(code.c_str());

            Fragment output = compiler.compile();
            assert(compiler.get_state().get_flag() == State::Flag::FAILURE);

            TEST_BLOCK("More Complex", ({

                  std::initializer_list<code_pair> expressions { 
                  { "(1+2 + ((1+2))", "" },
                  { "(((1+2))", "" },
                  { "(((1+2))))", "" },
                  { "((((1+2)", "" },
                  { "(1+2))))", "" },
                  { "(1+2+))", "" },
                  { "(1+)", "" },
                  { "(1-)", "" },
                  { "(22*)", "" },
                  { "(22**)", "" },
                  { "(111/)", "" },
                  { "(/)", "" },
                  // NULL_CHAR cases
                  { "(1+", "" },
                  { "(1*", "" },
                  { "1*", "" },
                  { "1* ", "" },
                  { "1* \n", "" },
                  { "1* \n ", "" },
                  { "1* \n\n", "" },
                  { "()", "" },
                  { "(1 <= 1", "" },
                  { "1 <= 1)", "" },
                  };


                  for(code_pair_iter start = expressions.begin(); start != expressions.end(); start++) {
                    Compiler compiler = Compiler(start->first.c_str());

                    // NOTE: Make sure to always apply full compile when checking for FAILURE
                    Fragment output = compiler.compile();

                    assert(compiler.get_state().get_flag() == State::Flag::FAILURE);
                  }

            }))

        })


  }))

  TEST_BLOCK("Expressions: relational operators", ({

        std::initializer_list<code_pair> expressions {
        { "11 < 22", "lessthan(11, 22)" },
        { "11 > 22", "greaterthan(11, 22)" },

        { "(1+1) < (1+2)", "lessthan(plus(1, 1), plus(1, 2))" },
        { "1+1 < (1+2)", "lessthan(plus(1, 1), plus(1, 2))" },
        { "1+1 < 1+2", "lessthan(plus(1, 1), plus(1, 2))" },
        { "1+1 < 1+2*3", "lessthan(plus(1, 1), plus(1, multiply(2, 3)))" },

        // GREATER THAN
        { "(1+1) > (1+2)", "greaterthan(plus(1, 1), plus(1, 2))" },
        { "1+1 > (1+2)", "greaterthan(plus(1, 1), plus(1, 2))" },
        { "1+1 > 1+2", "greaterthan(plus(1, 1), plus(1, 2))" },
        { "1+1 > 1+2*3", "greaterthan(plus(1, 1), plus(1, multiply(2, 3)))" },

        // CHAINING
        { "1 < 2 < 3", "lessthan(lessthan(1, 2), 3)" },
        { "1 < 2 > 3", "greaterthan(lessthan(1, 2), 3)" },
        { "1 < 2 <= 3", "eqlessthan(lessthan(1, 2), 3)" },
        { "1 < 2 >= 3", "eqgreaterthan(lessthan(1, 2), 3)" },


        // EQUAL TO OR LESS/GREATER THAN
        { "1 <= 1", "eqlessthan(1, 1)" },
        { "1 <= 1+1", "eqlessthan(1, plus(1, 1))" },
        { "(1+1) <= (1+2)", "eqlessthan(plus(1, 1), plus(1, 2))" },

        { "1 >= 1", "eqgreaterthan(1, 1)" },
        { "1 >= 1+1", "eqgreaterthan(1, plus(1, 1))" },
        { "(1+1) >= (1+2)", "eqgreaterthan(plus(1, 1), plus(1, 2))" },

        // ENCLOSED Expression CHECK
        { "(1 < 2)", "lessthan(1, 2)" },
        { "(1 > 2)", "greaterthan(1, 2)" },
        { "(1 >= 2)", "eqgreaterthan(1, 2)" },
        { "(1 <= 2)", "eqlessthan(1, 2)" },



        // EQUAL TO/NOT EQUAL TO
        { "(1+1) == (1+2)", "equalto(plus(1, 1), plus(1, 2))" },
        { "1+1 == 1+2", "equalto(plus(1, 1), plus(1, 2))" },
        { "(123) == (123)", "equalto(123, 123)" },

        // EQUALITY OVER (Equal to) Less/greater than relational operators
        { "1+1 < 3 == 1+2 > 2", "equalto(lessthan(plus(1, 1), 3), greaterthan(plus(1, 2), 2))" },
        { "1+1 < 123 == 1+2 > 2", "equalto(lessthan(plus(1, 1), 123), greaterthan(plus(1, 2), 2))" },
        { "1+1 <= 123 == 1+2 >= 2", "equalto(eqlessthan(plus(1, 1), 123), eqgreaterthan(plus(1, 2), 2))" },
        { "1*1 <= 123 == 1*2 >= 2", "equalto(eqlessthan(multiply(1, 1), 123), eqgreaterthan(multiply(1, 2), 2))"  },
        { "(1+1 < 3) == (1+2 > 2)", "equalto(lessthan(plus(1, 1), 3), greaterthan(plus(1, 2), 2))" },
        { "(1+1 < 123) == (1+2 > 2)", "equalto(lessthan(plus(1, 1), 123), greaterthan(plus(1, 2), 2))" },
        { "(1+1 <= 123) == (1+2 >= 2)", "equalto(eqlessthan(plus(1, 1), 123), eqgreaterthan(plus(1, 2), 2))" },
        { "(1*1 <= 123) == (1*2 >= 2)", "equalto(eqlessthan(multiply(1, 1), 123), eqgreaterthan(multiply(1, 2), 2))" },

        // ENCLOSED Expression CHECK
        { "(1==2) == (1==3)", "equalto(equalto(1, 2), equalto(1, 3))" },

        };


        for(code_pair_iter start = expressions.begin(); start != expressions.end(); start++) {

          // NOTE: compile_expression cuts off a non-match so this is another layer of testing
          Compiler compiler = Compiler(start->first.c_str());
          Fragment fragment_output = compiler.compile();

          assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
          assert(fragment_output.get_source()
              == start->second);


          compiler = Compiler(start->first.c_str());

          ExpressionFragment expr_output = compiler.compile_expression();

          assert(compiler.get_state().get_flag() == State::Flag::SUCCESS);
          // NOTE: check xvalue so long as it is expression and one-liner
          assert(expr_output.get_xvalue() == ExpressionFragment::XValue::RVALUE);
          // std::cout << expr_output.get_source() << std::endl;
          assert(expr_output.get_source()
              == start->second);

        }

        TEST_BLOCK("Failing Tests", ({
              std::initializer_list<code_pair> expressions {
              { "11 < ", "" },
              { "11 < \n", "" },
              { "11 < 22 < ", "" },
              { "11 > 22 >", "" },

              { "(1+1) < (1+2) <+", "" },
              { "1+1 < (1+2) > ", "" },
              { "1+1 < 1+2 > ", "" },
              { "1+1 < 1+2*3 +< ", "" },

              // GREATER THAN
              { "(1+1) > (1+2) \n < ", "" },
              { "1+1 > (1+2) \n > ", "" },
              { "(1+1 > 1+2) > ", "" },
              { "1+1 > 1+2*3 <", "" },


              // EQUAL TO OR LESS/GREATER THAN
              { "11 <= ", "" },
              { "11 <= \n", "" },
              { "11 <= \n\n", "" },
              { "11 <== \n\n", "" },
              { "11 <=== \n\n", "" },

              { "11 >= ", "" },
              { "11 >= \n", "" },
              { "11 >= \n\n", "" },
              { "11 >== \n\n", "" },
              { "11 >=== \n\n", "" },

              { "11 === ", "" },
              { "11 === \n\n", "" },
              { "11 === \n\n=", "" },
              };


              for(code_pair_iter start = expressions.begin(); start != expressions.end(); start++) {
                Compiler compiler = Compiler(start->first.c_str());

                Fragment output = compiler.compile();

                // std::cout << output.get_source() << std::endl;

                assert(compiler.get_state().get_flag() == State::Flag::FAILURE);
                // assert(output.get_xvalue() == XValue::RVALUE);
                // assert(output.get_source() == start->second);
              }

        }))



  }))



#ifdef DUMMY_BENCHMARK
  TEST_BLOCK("Dummy Benchmark", {
      start_time;
      for(size_t i = 0; i < 100; i++) {
      Compiler compiler = Compiler("1111111111 + 111222111111211");

      Fragment output = compiler.compile_expression();


      }
      end_time;

      });
#endif

  UTILITY_LOG_TEST_BLOCKS;


  return 0;
}

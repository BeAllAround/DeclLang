#include <iostream>
#include <string>
#include <vector>

#include <cassert>

#define DECL_COMPILER_VERBOSE

#define DECL_VERBOSE_LOGGING

#define DECL_LOG_INFO

#include <decl/lib/Utility.hpp>

#include <decl/lib/LexAnalysis.hpp>



using namespace LexAnalysis;





int main() {



  TEST_SECTION("Basic Tests")
  {

    TEST_BLOCK("Happy Tests | Happy Lexer")
    {
        const std::string source = R"(1 + 2
12
        11
        
)";
        Lexer lexer;
        lexer.set_stream(Lexer::Stream(source.c_str()));

        Lexer::Peekable token = lexer.peek_token(TokenGroup::ANY);

        Logger::info(token.to_string());

        assertm("Number Token", 
            compareTokens(token.token, Token("1", TokenType::CONST_NUM, 1, 2))
        );
        /*
            token.get_type() == TokenType::CONST_NUM &&
            token.to_string() == "Token(val=\"1\",line=1,col=2)"); 
        */

        lexer.eat_peekable(token);

        assertm("Lexer NOT DONE", lexer.is_over() == false);

        token = lexer.peek_token(TokenGroup::ANY);

        Logger::info(token.to_string());

        assertm("Plus Token", 
            compareTokens(token.token, Token("+", TokenType::OP_PLUS, 1, 3))
        );

        lexer.eat_peekable(token);
        assertm("Lexer NOT DONE", lexer.is_over() == false);

        token = lexer.peek_token(TokenGroup::ANY);

        Logger::info(token.to_string());

        assertm("Number Token", 
            compareTokens(token.token, Token("2", TokenType::CONST_NUM, 1, 6))
        );

        lexer.eat_peekable(token);
        assertm("Lexer NOT DONE", lexer.is_over() == false);


        token = lexer.peek_token(TokenGroup::ANY);

        Logger::info(token.to_string());

        assertm("Number Token", 
            compareTokens(token.token, Token("12", TokenType::CONST_NUM, 2, 3))
        );

        lexer.eat_peekable(token);

        assertm("Lexer NOT DONE", lexer.is_over() == false);


        token = lexer.peek_token(TokenGroup::ANY);

        Logger::info(token.to_string());

        assertm("Number Token", 
            compareTokens(token.token, Token("11", TokenType::CONST_NUM, 3, 11))
        );

        lexer.eat_peekable(token);

        // NOTE: The lexer stream is not OVER in this case since there is remaining white space at the end
        // NOTE: This was done on purpose so that we can detect new line at the end of expressions while parsing
        // line_defer: { lexer.skip_whitespace(); }

        assertm("Lexer NOT DONE", lexer.is_over() == false);



        // NULL CHAR
        {

            token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            lexer.eat_peekable(token); // NO UB (Undefined Behaviour)

            assertm("Lexer DONE", lexer.is_over() == true);

            assertm("NULL CHAR Token", 
                compareTokens(token.token, Token("", TokenType::NULL_CHAR, 5, 1))
            );
        }


    }

    TEST_BLOCK("Edge Cases | Keyword Tokens | While") {
        {
            const std::string source = R"(while)"; // NOTE: EDGE CASE OF WHILE BEING THE END OF LINE
            Lexer lexer;
            lexer.set_stream(Lexer::Stream(source.c_str()));

            Lexer::Peekable token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            assertm("While Keyword Token CHECK", 
                compareTokens(token.token, Token("while", TokenType::KEYWORD_WHILE, 1, 1))
            );
            
            lexer.eat_peekable(token);

            assertm("Lexer DONE", lexer.is_over() == true);
        }

        {
            const std::string source = R"( while ( ))";
            Lexer lexer;
            lexer.set_stream(Lexer::Stream(source.c_str()));

            Lexer::Peekable token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            assertm("While Keyword Token CHECK", 
                compareTokens(token.token, Token("while", TokenType::KEYWORD_WHILE, 1, 2))
            );

            lexer.eat_peekable(token);

            assertm("Lexer NOT DONE", lexer.is_over() == false);

            token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            assertm("Open parenthesis CHECK", 
                compareTokens(token.token, Token("(", TokenType::SYM_OPEN_P, 1, 8))
            );
 
        }

        {
            const std::string source = R"( while+2 ))";
            Lexer lexer;
            lexer.set_stream(Lexer::Stream(source.c_str()));

            Lexer::Peekable token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            assertm("While Keyword Token CHECK", 
                compareTokens(token.token, Token("while", TokenType::KEYWORD_WHILE, 1, 2))
            );

            lexer.eat_peekable(token);

            assertm("Lexer NOT DONE", lexer.is_over() == false);

            token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            assertm("Open parenthesis CHECK", 
                compareTokens(token.token, Token("+", TokenType::OP_PLUS, 1, 7))
            );

        }

        {
            const std::string source = R"( while{ ))";
            Lexer lexer;
            lexer.set_stream(Lexer::Stream(source.c_str()));

            Lexer::Peekable token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            assertm("While Keyword Token CHECK", 
                compareTokens(token.token, Token("while", TokenType::KEYWORD_WHILE, 1, 2))
            );

            lexer.eat_peekable(token);

            assertm("Lexer NOT DONE", lexer.is_over() == false);

            token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            assertm("Open parenthesis CHECK", 
                compareTokens(token.token, Token("{", TokenType::SYM_OPEN_B, 1, 7))
            );
 
        }

        {
            const std::string source = R"( while_ + 2 ))";
            Lexer lexer;
            lexer.set_stream(Lexer::Stream(source.c_str()));

            Lexer::Peekable token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            assertm("While Keyword Token CHECK", 
                compareTokens(token.token, Token("while_", TokenType::CONST_IDENF, 1, 8))
            );

            lexer.eat_peekable(token);

            assertm("Lexer NOT DONE", lexer.is_over() == false);

            token = lexer.peek_token(TokenGroup::ANY);

            Logger::info(token.to_string());

            assertm("Open parenthesis CHECK", 
                compareTokens(token.token, Token("+", TokenType::OP_PLUS, 1, 9))
            );
        }
    }

    TEST_BLOCK("Token Not within the TokenGroup")
    {
        const std::string source = R"( a )";
        Lexer lexer;
        lexer.set_stream(Lexer::Stream(source.c_str()));

        Lexer::Peekable token = lexer.peek_token(TokenGroup::SYMBOL);

        Logger::info(token.to_string());

        assertm("Invalid Token CHECK IF NOT WITHIN THE SAID TOKEN GROUP",
                compareTokens(token.token, Token("a", TokenType::INVALID, 1, 2)));

        lexer.eat_peekable(token);

        assertm("Lexer NOT DONE", lexer.is_over() == false);

        // NOTE: line defer as the lexer stream is not over
        lexer.skip_whitespace();

        assertm("Lexer DONE", lexer.is_over() == true);

    }

    TEST_BLOCK("Comparing Tokens | Compare ALL Tokens")
    {
        // NOTE: <=, >= takes priority over '<' and '>' in order to avoid conflict.
        // NOTE: So write the unit tests in that order
        // ALSO SEE THE KEYWORD WHILE TESTS. NOTE: Keywords take priority over CONST_IDENF, etc. as the id can be matched to be "while" and similar 
        const std::string source = R"(33
            _a
            a
            bb
            1
            2

            +
            -
            *
            /

            !=
            ==
            <=
            >=
            <
            >

            while
            while1

            {

            }

            (

            )

            ;

            while_
            whilee
            whileP
            while{
            while(
            &&
            ||
            =
            .
            [
            ]
            "
            func
            func_
            func1
            if
            if_
            if1
            return
            return_
            return1
            decl
            decl_
            decl1
            !
            ,
            continue
            continue_
            continue1
            break
            break_
            break1
            true
            true_
            true1
            nill
            nill_
            nill1
            false
            false_
            false1
        )";
        // NOTE: NOTE THAT WE ARE NOT GETTING STRING LITERAL AS A TOKEN - I MADE THE DECISION TO PARSE THE STRING AT THE AstParser::AstGenerator LEVEL

        const Token expected_tokens[] = {
            Token("33", TokenType::CONST_NUM, 1, 3),

            Token("_a", TokenType::CONST_IDENF, 2, 15),
            Token("a", TokenType::CONST_IDENF, 3, 14),
            Token("bb", TokenType::CONST_IDENF, 4, 15),

            // Const Types
            Token("1", TokenType::CONST_NUM, 5, 14),

            Token("2", TokenType::CONST_NUM, 6, 14),

            // Operators
            Token("+", TokenType::OP_PLUS, 8, 13),
            Token("-", TokenType::OP_MINUS, 9, 13),

            Token("*", TokenType::OP_MULTIPLY, 10, 13),
            Token("/", TokenType::OP_DIVIDE, 11, 13),

            Token("!=", TokenType::OP_NOT_EQUAL_TO, 13, 13),
            Token("==", TokenType::OP_EQUAL_TO, 14, 13),

            Token("<=", TokenType::OP_EQ_LESS_THAN, 15, 13),
            Token(">=", TokenType::OP_EQ_GREATER_THAN, 16, 13),
            Token("<", TokenType::OP_LESS_THAN, 17, 13),
            Token(">", TokenType::OP_GREATER_THAN, 18, 13),

            // Keywords
            Token("while", TokenType::KEYWORD_WHILE, 20, 13),
            Token("while1", TokenType::CONST_IDENF, 21, 19),

            // Symbols
            Token("{", TokenType::SYM_OPEN_B, 23, 13),
            Token("}", TokenType::SYM_CLOSE_B, 25, 13),
            Token("(", TokenType::SYM_OPEN_P, 27, 13),
            Token(")", TokenType::SYM_CLOS_P, 29, 13),
            Token(";", TokenType::SEMI_COLON, 31, 13),

            // While Edge Cases
            Token("while_", TokenType::CONST_IDENF, 33, 19),
            Token("whilee", TokenType::CONST_IDENF, 34, 19),
            Token("whileP", TokenType::CONST_IDENF, 35, 19),

            Token("while", TokenType::KEYWORD_WHILE, 36, 13),
            Token("{", TokenType::SYM_OPEN_B, 36, 18),

            Token("while", TokenType::KEYWORD_WHILE, 37, 13),
            Token("(", TokenType::SYM_OPEN_P, 37, 18),


            Token("&&", TokenType::OP_LOGICAL_AND, 38, 13),
            Token("||", TokenType::OP_LOGICAL_OR, 39, 13),

            Token("=", TokenType::OP_ASSIGNMENT, 40, 13),

            Token(".", TokenType::SYM_DOT, 41, 13),
            Token("[", TokenType::SYM_OPEN_SQ_B, 42, 13),
            Token("]", TokenType::SYM_CLOSE_SQ_B, 43, 13),

            Token("\"", TokenType::SYM_DQUOTE, 44, 13),

            Token("func", TokenType::KEYWORD_FUNC, 45, 13),
            Token("func_", TokenType::CONST_IDENF, 46, 18),
            Token("func1", TokenType::CONST_IDENF, 47, 18),

            Token("if", TokenType::KEYWORD_IF, 48, 13),
            Token("if_", TokenType::CONST_IDENF, 49, 16),
            Token("if1", TokenType::CONST_IDENF, 50, 16),

            Token("return", TokenType::KEYWORD_RETURN, 51, 13),
            Token("return_", TokenType::CONST_IDENF, 52, 20),
            Token("return1", TokenType::CONST_IDENF, 53, 20),

            Token("decl", TokenType::KEYWORD_DECL, 54, 13),
            Token("decl_", TokenType::CONST_IDENF, 55, 18),
            Token("decl1", TokenType::CONST_IDENF, 56, 18),


            Token("!", TokenType::OP_LOGICAL_NOT, 57, 13),

            Token(",", TokenType::SYM_COMMA, 58, 13),

            Token("continue", TokenType::KEYWORD_CONTINUE, 59, 13),
            Token("continue_", TokenType::CONST_IDENF, 60, 22),
            Token("continue1", TokenType::CONST_IDENF, 61, 22),

            Token("break", TokenType::KEYWORD_BREAK, 62, 13),
            Token("break_", TokenType::CONST_IDENF, 63, 19),
            Token("break1", TokenType::CONST_IDENF, 64, 19),

            Token("true", TokenType::KEYWORD_TRUE, 65, 13),
            Token("true_", TokenType::CONST_IDENF, 66, 18),
            Token("true1", TokenType::CONST_IDENF, 67, 18),

            Token("nill", TokenType::KEYWORD_NILL, 68, 13),
            Token("nill_", TokenType::CONST_IDENF, 69, 18),
            Token("nill1", TokenType::CONST_IDENF, 70, 18),

            Token("false", TokenType::KEYWORD_FALSE, 71, 13),
            Token("false_", TokenType::CONST_IDENF, 72, 19),
            Token("false1", TokenType::CONST_IDENF, 73, 19),
        };

        std::vector<Token> tokens = Lexer::generate_tokens(source.c_str());

        assertm("EXPECTED NUMBER OF TOKENS PASS", tokens.size() == get_arr_size(expected_tokens));

        for(size_t i = 0; i < tokens.size(); i++) {
            // Token& token = tokens[i];
            Token token = tokens[i];

            Logger::info(token.to_string(), expected_tokens[i].to_string());

            assertm("Token Compare OK", compareTokens(token, expected_tokens[i]));
        }

    }

   }
    
   
   return 0;
}
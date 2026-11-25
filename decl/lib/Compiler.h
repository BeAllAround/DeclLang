
#ifndef DECL_COMPILER_H

#define DECL_COMPILER_H

#include <cctype> // isdigit, isalpha, isalnum // TODO: custom-implemention such as the following pseudo: isdigit: c >= '0' && c <= '9'

// Prefixed with the "is_" naming convention due to the conflict with <cctype>
#define is_digit(s) isdigit(*s)
#define is_plus(s) *s == '+'
#define is_multiply(s) *s == '*'
#define is_divide(s) *s == '/'
#define is_minus(s) *s == '-'

#define is_lessthan(s) *s == '<'
#define is_greaterthan(s) *s == '>'
// NOTE: Rely on "Short-circuit evaluation" so as not to advance if not necessary
#define is_eqgreaterthan(s) *s == '>' && *(s+1) == '='
#define is_eqlessthan(s) *s == '<' && *(s+1) == '='
#define is_equalto(s) *s == '=' && *(s+1) == '='
#define is_notequalto(s) *s == '!' && *(s+1) == '='

#define is_openingparenthesis(s) *s == '('
#define is_closingparenthesis(s) *s == ')'

#define LOOP for(;1;)



#define PROD_PREFIX "Decl::"
// NOTE: We have avoided overidding C++ operators on Value objects of Decl and decided on the more functional approach expect for the logical operators actually being macros in order to implement short-circut evaluation with the ternary conditional (?:)
// Arithmetic
#define DECL_PLUS "plus("
#define DECL_MINUS "minus("
#define DECL_MULTIPLY "multiply("
#define DECL_DIVIDE "divide("

// Relational
#define DECL_LESS_THAN "lessthan("
#define DECL_GREATER_THAN "greaterthan("
#define DECL_EQ_LESS_THAN "eqlessthan("
#define DECL_EQ_GREATER_THAN "eqgreaterthan("
#define DECL_EQUAL_TO "equalto("
#define DECL_NOT_EQUAL_TO "notequalto("

// #define PROD_DECL

// Target namespace (Source-to-Source(S2S))
namespace Target {

// This might not be the most effective approach but it is fine since we need to make sure that the function is ready for both testing and for production
#ifdef PROD_DECL

  const char* PLUS = PROD_PREFIX DECL_PLUS;
  const char* MINUS = PROD_PREFIX DECL_MINUS;
  const char* MULTIPLY = PROD_PREFIX DECL_MULTIPLY;
  const char* DIVIDE = PROD_PREFIX DECL_DIVIDE;

  // Relational
  const char* LESS_THAN = PROD_PREFIX DECL_LESS_THAN;
  const char* GREATER_THAN = PROD_PREFIX DECL_GREATER_THAN;
  const char* EQ_LESS_THAN = PROD_PREFIX DECL_EQ_LESS_THAN;
  const char* EQ_GREATER_THAN = PROD_PREFIX DECL_EQ_GREATER_THAN;

  const char* EQUAL_TO = PROD_PREFIX DECL_EQUAL_TO;
  const char* NOT_EQUAL_TO = PROD_PREFIX DECL_NOT_EQUAL_TO;

#else
  // NOTE: (const char* + std::string) works since the compiler turns the const char* into std::string
  const char* PLUS = DECL_PLUS;
  const char* MINUS = DECL_MINUS;
  const char* MULTIPLY = DECL_MULTIPLY;
  const char* DIVIDE = DECL_DIVIDE;

  // Relational
  const char* LESS_THAN = DECL_LESS_THAN;
  const char* GREATER_THAN = DECL_GREATER_THAN;
  const char* EQ_LESS_THAN = DECL_EQ_LESS_THAN;
  const char* EQ_GREATER_THAN = DECL_EQ_GREATER_THAN;

  const char* EQUAL_TO = DECL_EQUAL_TO;
  const char* NOT_EQUAL_TO = DECL_NOT_EQUAL_TO;


#endif

}

namespace Terminal {
  namespace Color {
    // Reference: Credit to https://stackoverflow.com/a/9158263

    const char* RESET = "\033[0m";
    const char* RED = "\033[31m";
    const char* GREEN = "\033[32m";
    const char* YELLOW = "\033[33m";
    const char* BLUE = "\033[34m";

    // bolded
    const char* BOLDWHITE = "\033[1m\033[37m";
    const char* BOLDRED = "\033[1m\033[31m";
  }
}

// Code Block
class Block {
  public:
    Block() = default;
};

// TODO: Intended for a Block
enum class Whence {
  FUNCTION,
  LAMBDA,
  WHILE,
  IF,
  ELSE_IF
};


// TODO: class Source;

// TODO: class Primary;
// maybe: class Primary : public Fragment {}; -> ? ->

class Fragment {
  public:
    Fragment() = default;

    Fragment(const std::string& source) : source{source} {};


    const std::string& get_source() const {
      return source;
    }

    void set_source(std::string& new_source) {
      source = new_source; // COPY
    }

    void set_source(std::string&& new_source) {
      source = std::move(new_source); // MOVE
    }

    /*
       POTENTIAL TODO: std::forward<T> ?
       void set_source(const std::string& new_source) {
       source = new_source;
       }
     */
  private:
    std::string source;
    std::string val;

};

class ExpressionFragment: public Fragment {
  public:
    enum class XValue {
      LVALUE,
      RVALUE
    };

    // Inherit constructors
    ExpressionFragment() : Fragment() {}

    ExpressionFragment(const std::string& source) : Fragment(source) {}

    const XValue& get_xvalue() {
      return xvalue;
    }

    void set_xvalue(XValue new_xvalue) {
      xvalue = new_xvalue;
    }

    void turn_lvalue() {
      set_xvalue(XValue::LVALUE);
    }

    void turn_rvalue() {
      set_xvalue(XValue::RVALUE);
    }

  private:
    XValue xvalue;
};

class BlockFragment: public Fragment {
  public:
    BlockFragment() : Fragment() {}

    BlockFragment(const std::string& source) : Fragment(source) {}
};




// TODO: Define child classes for sub-groups of tokens. For now, we have the namespaces workaround and groups for token peeking/look ahead.

enum class TokenType {
  // Consts
  NUM,
  ALPHA, // IDENTIFIER?
  STRING,

  // OPs
  PLUS,
  MINUS,
  MULTIPLY,
  DIVIDE,

  LESS_THAN,
  GREATER_THAN,
  EQ_LESS_THAN,
  EQ_GREATER_THAN,

  EQUAL_TO,
  NOT_EQUAL_TO,


  OPENING_P,
  CLOSING_P,


  IDENF,

  IF,
  WHILE,
  RETURN,
  LET,


  INVALID,
  NULL_CHAR,
};

enum class TokenGroup {
  KEYWORD,
  OPERATOR,
  SYMBOL,
  CONST, // CONSTANT
  IDENF, // IDENTIFIER
  STRING, // Not sure if we want to actually use this as a string is treated as CONSTANT in this language.

  ANY, // In the case of matching ANY token

};

namespace OP {
  constexpr TokenType PLUS = TokenType::PLUS;
  constexpr TokenType MINUS = TokenType::MINUS;
  constexpr TokenType MULTIPLY = TokenType::MULTIPLY;
  constexpr TokenType DIVIDE = TokenType::DIVIDE;


  constexpr TokenType LESS_THAN = TokenType::LESS_THAN;
  constexpr TokenType GREATER_THAN = TokenType::GREATER_THAN;
  constexpr TokenType EQ_LESS_THAN = TokenType::EQ_LESS_THAN;
  constexpr TokenType EQ_GREATER_THAN = TokenType::EQ_GREATER_THAN;

  constexpr TokenType EQUAL_TO = TokenType::EQUAL_TO;
  constexpr TokenType NOT_EQUAL_TO = TokenType::NOT_EQUAL_TO;
}

namespace CONST {
  constexpr TokenType NUM = TokenType::NUM;
  constexpr TokenType STRING = TokenType::STRING;
  constexpr TokenType ALPHA = TokenType::ALPHA;

}

namespace SYM {
  constexpr TokenType OPEN_P = TokenType::OPENING_P;
  constexpr TokenType CLOS_P = TokenType::CLOSING_P;
}

namespace KEYWORD {
  constexpr TokenType IF = TokenType::IF;
  constexpr TokenType WHILE = TokenType::WHILE;
  constexpr TokenType RETURN = TokenType::RETURN;
  constexpr TokenType LET = TokenType::LET;
}

class Token {
  private:
    std::string val;
    TokenType type;
    int line {0};

  public:
    // Token() = default;
    Token() {}

    /*
       Token(const std::string& val, TokenType type) :
       val{val}, type{type} {}

       Token(std::string&& val, TokenType type, int line) : 
       val{std::move(val)}, type{type}, line{line} {}
     */

    Token(const std::string& val, TokenType type, int line) : 
      val{val}, type{type}, line{line} {}

    Token(Token&& other) : val{std::move(other.val)}, type{ other.type}, line {other.line} {}

    Token(const Token& other): val{other.val}, type{ other.type}, line {other.line} {}


    TokenType get_type() const {
      return type;
    }

    const std::string& get_val() const {
      return val;
    }


    int get_line() const {
      return line;
    }
};


// Lexer holds the source code text pointer and gets tokens (and peeks, consumes, etc.(TODO) )
// It essentially serves as a TokenStream but it is indeed a program that "lexes"
class Lexer {
  public:

    // NOTE: The functionality of Stream, for now, is minimal thus being a struct
    struct Stream {
      const char* source;

      int current_line{1}; // row

      int column{0};

      /*
      // TODO: If needed
      // Pointing to start
      const char* source_start;
      // Pointing to end
      const char* source_end;
       */


      Stream(const char* source) : source{source} {}

      Stream(const char* source, int current_line) :
        source{source},
        current_line{current_line} {}

      Stream(const char* source, int current_line, int column) :
        source{source},
        current_line{current_line},
        column{column} {}

      // NOTE: These are these by default but let's have more control over them
      Stream(const Stream& other) {
        *this = other;
      }


      Stream&operator=(const Stream&other) {
        source = other.source;
        current_line = other.current_line;
        column = other.column;
        // source_start = other.source_start;
        // source_end = other.source_end;

        return *this;
      }

    };

    // NOTE: The personal preference of 'struct' is that I need direct(easy) access to the members without needing to add setters/getters and, additionally, add rvalue and lvalue for those members
    struct Peekable {
      Peekable(const Token& token, const Stream& stream) :
        token{token}, stream{stream} {}

      Peekable(Token&& token, const Stream& stream) :
        token{std::move(token)}, stream{stream} {}


      Token token;
      Stream stream;
    };


    /*
    // TODO: Proposed Design
    class Peekable : public Token {
    public:
    Stream stream;

    Peekable(Token&& token, const Stream& stream) : Token(token), stream{stream} {};
    };
     */


    Lexer() = default;

    explicit Lexer(const char* source) : 
      stream (source) {
      }

    void skip_space() {
      while(1) {
        switch(*stream.source) {
          case ' ': case '\t':
            stream.source++;
            break;
          case '\n':
            stream.source++;
            stream.current_line++;
            break;
          default:
            return;
        }
      }

    }


    void advance(int p) {
      for(int i = 0; i < p; i++) {
        stream.source++;
      }
    }

    bool is_over() {
      return *stream.source == 0;
    }

    void set_stream(Stream&& stream) {
      this->stream = std::move(stream);
    }

    void set_stream(const Stream& stream) {
      this->stream = stream;
    }

    Token get_token(TokenGroup t_group) {

      // skip ahead of the token - not after the token - also the reason to "peek"
      skip_space();

      const int current_line = stream.current_line;

      // NOTE: This has to be a block of if statements as opposed to if/else if due to the fact that the rest of the conditions(TokenGroup::ANY for example) become "unreachable" for a lack of a better word
      if(t_group == TokenGroup::CONST || t_group == TokenGroup::ANY) {
        if(is_digit(stream.source)) {
          std::string val;

          do {
            val += *stream.source;
            stream.source++;
          } while(is_digit(stream.source));

          return Token(std::move(val), CONST::NUM, current_line);
        }

      }

      if(t_group == TokenGroup::OPERATOR || t_group == TokenGroup::ANY) {
        if(is_plus(stream.source)) {
          return Token("+", OP::PLUS, current_line);
        } else if(is_multiply(stream.source)) {
          return Token("*", OP::MULTIPLY, current_line);
        } else if(is_divide(stream.source)) {
          return Token("/", OP::DIVIDE, current_line);
        } else if(is_minus(stream.source)) {
          return Token("-", OP::MINUS, current_line);

          // NOTE: takes priority over '<' and '>' to avoid conflict
        } else if(is_eqlessthan(stream.source)) {
          return Token("<=", OP::EQ_LESS_THAN, current_line);
        } else if(is_eqgreaterthan(stream.source)) {
          return Token(">=", OP::EQ_GREATER_THAN, current_line);
        } else if(is_equalto(stream.source)) {
          return Token("==", OP::EQUAL_TO, current_line);
        } else if(is_notequalto(stream.source)) {
          return Token("!=", OP::NOT_EQUAL_TO, current_line);
        } else if(is_lessthan(stream.source)) {
          return Token("<", OP::LESS_THAN, current_line);
        } else if(is_greaterthan(stream.source)) {
          return Token(">", OP::GREATER_THAN, current_line);
        }
      }

      if(t_group == TokenGroup::SYMBOL || t_group == TokenGroup::ANY) {
        if(is_openingparenthesis(stream.source)) {
          return Token("(", SYM::OPEN_P, current_line);
        } else if (is_closingparenthesis(stream.source)) {
          return Token(")", SYM::CLOS_P, current_line);
        }
      }

      // NOTE: never advance the null character
      if(*stream.source == 0) {
        std::string val;
        return Token(val, TokenType::NULL_CHAR, current_line);
      }

      // TODO: Don't use the scoped block
      {
        std::string val;
        val += *stream.source;

        /*
TODO: Only the case for TokenGroup::ANY
        // recognize that token until the end of the line/whitespace?
        while((*source != '\n' && *source != ' ' && *source != '\t')
        && *source != 0) {
        val += *source;
        source++;
        }
        // std::cout << "val: " << val << std::endl;
         */

        return Token(val, TokenType::INVALID, current_line);
      }

    }

    void eat_peekable(const Peekable& peekable) {

      // Swap/Get to the end of that stream from Peekable
      set_stream(peekable.stream);

      switch(peekable.token.get_type()) {
        // one-char (tokens) operators
        case OP::PLUS: case OP::MINUS: case OP::MULTIPLY: case OP::DIVIDE: case OP::LESS_THAN: case OP::GREATER_THAN: case SYM::OPEN_P: case SYM::CLOS_P:
          advance(1);
          break;
          // two-char (tokens) operators
        case OP::EQ_LESS_THAN: case OP::EQ_GREATER_THAN: case OP::EQUAL_TO: case OP::NOT_EQUAL_TO:
          advance(2);
          break;
        case TokenType::NULL_CHAR: case CONST::NUM:
          // NOTE: never advance the null character and all that is already consumed by pointer increment via stream.source
          return;
        default:
          // NOTE: advancing for one at the moment
          advance(1);
          break;
      }
    }

    Peekable peek_token(TokenGroup t_group) {
      // Copy temp of the stream that points to "before" getting the token
      Stream tmp_stream = stream;

      // std::cout << "current line tmp: " << current_line_tmp << std::endl;

      // std::cout << "start: '" << start << '\'' << std::endl;

      // Reuse get_token
      Token token = get_token(t_group);

      // NOTE: End of the stream for swapping(efficient token consumption)
      Stream end_of_stream = stream;

      // std::cout << "source: '" << source << '\'' << std::endl;

      // std::cout << "current line: " << current_line << std::endl;


      // NOTE: Reset the Stream as ONLY the consumed tokens have the permission to proceeed - that is why we peek for them first
      set_stream(tmp_stream);

      return Peekable(std::move(token), end_of_stream);

    }

  private:
    Stream stream;

};

class Status {
  public:
    Status() = default;

    Status(const std::string& description) : description{description} {}

  private:
    std::string description;
};

// TODO: Implement by returning a stream
class Logger {
  public:
    static void error(const std::string& message) {
      using namespace Terminal;

      // TODO: likely to be moved somewhere else but it is TBD
#ifdef DECL_COMPILER_VERBOSE

      std::cerr << Color::BOLDRED << "ERROR: " << Color::RESET;

      std::cerr << Color::BOLDWHITE << message
        << Color::RESET
        << std::endl;
#endif

    }
};

// enum class for the state of the compiler
class State {

  public:
    enum class Flag {
      SUCCESS,
      FAILURE,
    };

    // NOTE: Succuss by default
    State() : flag{Flag::SUCCESS} {};

    void set_flag(Flag new_flag) {
      flag = new_flag;
    }

    const Flag& get_flag() const {
      return flag;
    }

    void failure() {
      set_flag(State::Flag::FAILURE);
    }

    void print() const {
      // TODO: print the status description


      if(flag == Flag::FAILURE) {
        Logger::error("Compilation Failed");
      }

    }

  private:
    Flag flag;
    Status status;

};

class Compiler {
  std::vector<Block> blocks;

  Lexer lexer;

  State state;

#ifdef DECL_COMPILER_COUNT_INVALID_TOKENS
  int invalid_tokens{0};
#endif


  public:
  explicit Compiler(const char* decl_source) : lexer{decl_source} {}


  const State& get_state() const {
    return state;
  }

  // TODO: Turn this fragment into "class Source", BlockFragment or something as we will go beyond just "expressions" - breaks into the new line
  BlockFragment compile() {
    BlockFragment fragment;

    int delimiter = false;


    while(!lexer.is_over()) {
      ExpressionFragment expression_fragment = compile_expression();

      fragment.set_source(
          fragment.get_source() + 
          (delimiter ? "\n" : "") +
          expression_fragment.get_source()
          );

      // NOTE: don't pass on the xvalue type since it is a "block" of code
      // fragment.set_xvalue(expression_fragment.get_xvalue());


      if(!delimiter) {
        delimiter = true;
      }
      // expression.get_source()
      // expression().get_source()
    }


    // print out the status
    state.print();


    return fragment; // top processing function
  }

  ExpressionFragment compile_expression() {
    return expression();
  }

  ExpressionFragment expression() {
    return equality_expression();
  }

  ExpressionFragment equality_expression() {
    ExpressionFragment left = relational_expression();

    LOOP {
      Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);

      switch(op_token.token.get_type()) {
        case OP::EQUAL_TO:
          {
            lexer.eat_peekable(op_token);
            ExpressionFragment right = relational_expression();

            left.set_source(
                Target::EQUAL_TO +
                left.get_source() +
                ", " +
                right.get_source() +
                ')'
                );

            // NOTE: becomes rvalue
            left.turn_rvalue();
            break;

          }
        default:
          return left;
      }

    }

  }

  ExpressionFragment relational_expression() {
    ExpressionFragment left = additive_expression();

    LOOP {
      Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);

      switch(op_token.token.get_type()) {
        case OP::LESS_THAN:
          {
            lexer.eat_peekable(op_token);

            ExpressionFragment right = additive_expression();

            // mutate fragment
            left.set_source(
                Target::LESS_THAN +
                left.get_source() +
                ", " +
                right.get_source() +
                ')'
                );
            // NOTE: becomes rvalue
            left.turn_rvalue();

            break;

          }
        case OP::GREATER_THAN:
          {
            lexer.eat_peekable(op_token);

            ExpressionFragment right = additive_expression();

            // mutate fragment
            left.set_source(
                Target::GREATER_THAN +
                left.get_source() +
                ", " +
                right.get_source() +
                ')'
                );
            // NOTE: becomes rvalue
            left.turn_rvalue();

            break;
          }
        case OP::EQ_LESS_THAN:
          {
            lexer.eat_peekable(op_token);

            ExpressionFragment right = additive_expression();

            // mutate fragment
            left.set_source(
                Target::EQ_LESS_THAN +
                left.get_source() +
                ", " +
                right.get_source() +
                ')'
                );
            // NOTE: becomes rvalue
            left.turn_rvalue();

            break;
          }
        case OP::EQ_GREATER_THAN:
          {
            lexer.eat_peekable(op_token);

            ExpressionFragment right = additive_expression();

            // mutate fragment
            left.set_source(
                Target::EQ_GREATER_THAN +
                left.get_source() +
                ", " +
                right.get_source() +
                ')'
                );
            // NOTE: becomes rvalue
            left.turn_rvalue();

            break;
          }
        default:
          return left;

      }
    }

  }	

  ExpressionFragment additive_expression() {
    ExpressionFragment left = multiplicative_expression();

    LOOP {
      // TODO: Consider something that would work like this: peek_token(PLUS | MINUS)
      // The solution, for now, is to do TokenGroup::ANY for primary and have a switch case
      Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);

      // std::cout << (token.get_type() == TokenType::INVALID) << std::endl;

      switch(op_token.token.get_type()) {
        case OP::PLUS:
          {
            lexer.eat_peekable(op_token);

            ExpressionFragment right = multiplicative_expression();

            // mutate fragment
            left.set_source(
                Target::PLUS + 
                left.get_source() + 
                ", " + 
                right.get_source() +
                ')'
                );
            left.turn_rvalue();

            break;
          }
        case OP::MINUS:
          {
            lexer.eat_peekable(op_token);

            ExpressionFragment right = multiplicative_expression();

            // mutate fragment
            left.set_source(
                Target::MINUS +
                left.get_source() +
                ", " +
                right.get_source() +
                ')'
                );
            left.turn_rvalue();

            break;
          }
        default:
          return left;
      }
    }


    return left;
  }

  ExpressionFragment multiplicative_expression() {
    ExpressionFragment left = prim();

    LOOP {
      Lexer::Peekable op_token = lexer.peek_token(TokenGroup::OPERATOR);

      switch(op_token.token.get_type()) {
        case OP::MULTIPLY:
          {
            lexer.eat_peekable(op_token);

            ExpressionFragment right = prim();

            // mutate fragment
            left.set_source(
                Target::MULTIPLY +
                left.get_source() +
                ", " +
                right.get_source() +
                ')'
                );
            left.turn_rvalue();

            break;
          }
        case OP::DIVIDE:
          {
            lexer.eat_peekable(op_token);

            ExpressionFragment right = prim();

            // mutate fragment
            left.set_source(
                Target::DIVIDE +
                left.get_source() +
                ", " +
                right.get_source() +
                ')'
                );
            left.turn_rvalue();

            break;

          }
        default:
          return left;


      }
    }


  }


  // Primary
  ExpressionFragment prim() {
    // TODO: Refactor double-reading of a token O(2n): (peek(n) + get_token(n)): return the current pointer or something
    // NOTE: ANY for now as we need to match for the opening parenthesis, constants, etc.
    Lexer::Peekable p_token = lexer.peek_token(TokenGroup::ANY);

    switch(p_token.token.get_type()) {
      case CONST::NUM:
        {

          // REMOVED: Double lookup: O(2N) ExpressionFragment expression_fragment(lexer.get_token(TokenGroup::CONST).get_val());
          ExpressionFragment expression_fragment(p_token.token.get_val());

          lexer.eat_peekable(p_token);

          // all CONST Literals are RVALUE
          expression_fragment.turn_rvalue();

          return expression_fragment;
        }
      case SYM::OPEN_P:
        {
          // eat '('
          lexer.eat_peekable(p_token);

          ExpressionFragment expression_fragment = expression();

          Lexer::Peekable right = lexer.peek_token(TokenGroup::SYMBOL);


          if(right.token.get_type() != SYM::CLOS_P) {
            Logger::error("Missing ')'");
            // std::cout << (right.get_type() == TokenType::NULL_CHAR) << std::endl;
            state.failure();
            // NOTE: the default of this switch case will eat this token on next iteration.
          } else {
            // eat ')'
            lexer.eat_peekable(right);
          }

          return expression_fragment;
        }
      default:
        // NOTE: eat the peekable token by swapping the end stream
        lexer.eat_peekable(p_token);

        state.failure();

        if(p_token.token.get_type() == TokenType::NULL_CHAR) {
          Logger::error("Terminated: Unexpected null character at LINE: "
              + std::to_string(p_token.token.get_line()));
        } else {
          Logger::error("Invalid token: " + p_token.token.get_val() + " at LINE: " 
              + std::to_string(p_token.token.get_line()));
        }

        // throw "1";
        return ExpressionFragment();
    }
    // return Fragment(lexer.get_token().get_val());
  }

};

#endif

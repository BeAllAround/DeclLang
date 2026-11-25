
#ifndef HEADER_DECL_LEXER

#define HEADER_DECL_LEXER

#include <string>
#include <utility>

#include <cctype> // isdigit, isalpha, isalnum 
#include <cstring>
// TODO: custom-implemention such as the following pseudo: isdigit: c >= '0' && c <= '9'


#define LOOP for(;1;)

namespace LexAnalysis
{

  // NOTE: Prefixed with the "is_" naming convention due to the conflict with <cctype>
  inline bool is_alpha(const char *s)
  {
    return isalpha(*s);
  }

  inline bool is_alnum(const char *s)
  {
    return isalnum(*s);
  }

  inline bool is_digit(const char *s)
  {
    return isdigit(*s);
  }

  inline bool is_plus(const char *s)
  {
    return *s == '+';
  }

  inline bool is_multiply(const char *s)
  {
    return *s == '*';
  }

  inline bool is_divide(const char *s)
  {
    return *s == '/';
  }

  inline bool is_minus(const char *s)
  {
    return *s == '-';
  }

  inline bool is_lessthan(const char *s)
  {
    return *s == '<';
  }

  inline bool is_greaterthan(const char *s)
  {
    return *s == '>';
  }

  // NOTE: Rely on "Short-circuit evaluation" so as not to advance if not necessary
  inline bool is_eqgreaterthan(const char *s)
  {
    return *s == '>' && *(s + 1) == '=';
  }

  inline bool is_eqlessthan(const char *s)
  {
    return *s == '<' && *(s + 1) == '=';
  }

  inline bool is_equalto(const char *s)
  {
    return *s == '=' && *(s + 1) == '=';
  }

  inline bool is_notequalto(const char *s)
  {
    return *s == '!' && *(s + 1) == '=';
  }

  inline bool is_white_space(const char *s)
  {
    return (*s == ' ' || *s == '\n' || *s == '\t' || *s == '\r');
  }

  inline bool is_openingbrackets(const char *s)
  {
    return *s == '{';
  }

  inline bool is_closingbrackets(const char *s)
  {
    return *s == '}';
  }

  inline bool is_openingparenthesis(const char *s)
  {
    return *s == '(';
  }

  inline bool is_closingparenthesis(const char *s)
  {
    return *s == ')';
  }

  inline bool keyword_end(const char* s) {
    return *s != '_' && !is_alnum(s);
  }

  inline bool is_keyword_while(const char *s)
  {
    // See: TEST_BLOCK("Edge Cases | Keyword Tokens | While")
    // NOTE: while1, while_, while12, whileb are ALL CONST_IDEF
    // return (*s == 'w' && *(s+1) == 'h' && *(s+2) == 'i' && *(s+3) == 'l' && *(s+4) == 'e' && ( is_white_space(s+5) || *(s+5) == '(' || *(s+5) == 0 || (*(s+5) != '_'  && !is_alnum(s+5)) )  );
    return (*s == 'w' && *(s + 1) == 'h' && *(s + 2) == 'i' && *(s + 3) == 'l' && *(s + 4) == 'e' && keyword_end(s+5) );
  }

  inline bool is_keyword_func(const char* s) {
    return (*s == 'f' && *(s + 1) == 'u' && *(s + 2) == 'n' && *(s + 3) == 'c' && keyword_end(s+4) );
  }

  inline bool is_keyword_if(const char* s) {
    return *s == 'i' && *(s+1) == 'f' && keyword_end(s+2);
  }

  inline bool is_keyword_return(const char* s) {
    return *s == 'r' && *(s+1) == 'e' && *(s+2) == 't' && *(s+3) == 'u' && *(s+4) == 'r' && *(s+5) == 'n' && keyword_end(s+6);
  }

  inline bool is_keyword_decl(const char* s) {
    return *s == 'd' && *(s+1) == 'e' && *(s+2) == 'c' && *(s+3) == 'l' && keyword_end(s+4);
  }

  inline bool is_keyword_continue(const char* s) {
    return *s == 'c' && *(s+1) == 'o' && *(s+2) == 'n' && *(s+3) == 't' && *(s+4) == 'i' && *(s+5) == 'n' && *(s+6) == 'u' && *(s+7) == 'e' && keyword_end(s+8);
  }

  inline bool is_keyword_break(const char* s) {
    return *s == 'b' && *(s+1) == 'r' && *(s+2) == 'e' && *(s+3) == 'a' && *(s+4) == 'k' && keyword_end(s+5);
  }

  inline bool is_keyword_nill(const char* s) {
    return *s == 'n' && *(s+1) == 'i' && *(s+2) == 'l' && *(s+3) == 'l' && keyword_end(s+4);
  }

  inline bool is_keyword_true(const char* s) {
    return *s == 't' && *(s+1) == 'r' && *(s+2) == 'u' && *(s+3) == 'e' && keyword_end(s+4);
  }

  inline bool is_keyword_false(const char* s) {
    return *s == 'f' && *(s+1) == 'a' && *(s+2) == 'l' && *(s+3) == 's' && *(s+4) == 'e' && keyword_end(s+5);
  }

  inline bool is_semicolon(const char *s)
  {
    return *s == ';';
  }

  inline bool is_logical_and(const char *s)
  {
    return *s == '&' && *(s + 1) == '&';
  }

  inline bool is_logical_or(const char *s)
  {
    return *s == '|' && *(s + 1) == '|';
  }

  inline bool is_assignment(const char *s)
  {
    return *s == '=';
  }

  inline bool is_dot(const char *s)
  {
    return *s == '.';
  }

  inline bool is_openining_square_bracket(const char *s)
  {
    return *s == '[';
  }

  inline bool is_closing_square_bracket(const char *s)
  {
    return *s == ']';
  }

  inline bool is_double_quote(const char *s)
  {
    return *s == '"';
  }

  inline bool is_logical_not(const char* s) {
    return *s == '!';
  }

  inline bool is_comma(const char* s) {
    return *s == ',';
  }

  // NOTE: Using the prefix to represent the group of tokens. E.g. CONST_*, OP_*, SYM_*, etc.
  enum class TokenType
  {
    // Consts
    CONST_NUM,
    CONST_STRING,
    CONST_IDENF,

    // OPs
    OP_PLUS,
    OP_MINUS,
    OP_MULTIPLY,
    OP_DIVIDE,

    OP_LESS_THAN,
    OP_GREATER_THAN,
    OP_EQ_LESS_THAN,
    OP_EQ_GREATER_THAN,

    OP_EQUAL_TO,
    OP_NOT_EQUAL_TO,

    OP_LOGICAL_AND,
    OP_LOGICAL_OR,
    OP_LOGICAL_NOT,
    OP_ASSIGNMENT,

    // Symbols
    SYM_DOT,
    SYM_DQUOTE, // double quote

    SYM_OPEN_P,
    SYM_CLOS_P,
    SYM_OPEN_B,
    SYM_CLOSE_B,

    SYM_OPEN_SQ_B,
    SYM_CLOSE_SQ_B,
    SYM_COMMA,

    SEMI_COLON,

    // Keywords
    KEYWORD_FUNC,
    KEYWORD_IF,
    KEYWORD_WHILE,
    KEYWORD_RETURN,
    KEYWORD_DECL,
    KEYWORD_CONTINUE,
    KEYWORD_BREAK,
    KEYWORD_NILL,
    KEYWORD_TRUE,
    KEYWORD_FALSE,

    // Invalid
    INVALID,
    NULL_CHAR,
  };

  enum class TokenGroup
  {
    KEYWORD,
    OPERATOR,
    SYMBOL,
    CONST,  // CONSTANT
    IDENF,  // IDENTIFIER
    STRING, // Not sure if we want to actually use this as a string is treated as CONSTANT in this language.

    ANY, // In the case of matching ANY token

  };

  class Token
  {
  private:
    std::string val {""};
    TokenType type {TokenType::INVALID};
    size_t line{0};
    size_t column{0};

  public:
    Token() = default;
    // Token() {}

    /*
       Token(const std::string& val, TokenType type) :
       val{val}, type{type} {}

       Token(std::string&& val, TokenType type, size_t line) :
       val{std::move(val)}, type{type}, line{line} {}
     */
    // TODO: Maybe (std::string&& val) as well?

    Token(const std::string &val, TokenType type)
      : val{val},
      type{type} {}

    Token(const std::string &val, TokenType type, size_t line)
      : val{val},
        type{type},
        line{line} {}

    Token(const std::string &val, TokenType type, size_t line, size_t col)
      : val{val},
        type{type},
        line{line},
        column{col}
        {}


    // Token(Token &&other) : val{std::move(other.val)}, type{other.type}, line{other.line}, column{other.column} {}

    // Token(const Token &other) : val{other.val}, type{other.type}, line{other.line}, column{other.column} {}

    TokenType get_type() const
    {
      return type;
    }

    const std::string &get_val() const
    {
      // Returning const ref here as this is not considered to be Copy Elision. In fact, it is the string copy operation with the base type (std::string).
      return val;
    }

    size_t get_line() const
    {
      return line;
    }

    size_t get_col() const {
      return column;
    }

    std::string to_string() const {
      return "Token(val=\"" + val + "\",line=" + std::to_string(line) + ",col=" + std::to_string(column) + ")";
    }


  };

  // Lexer holds the source code text pointer and gets tokens (and peeks, consumes, etc.(TODO) )
  // It essentially serves as a TokenStream but it is indeed a program that "lexes"
  class Lexer
  {
  public:

    static std::vector<Token> generate_tokens(const char*);


    // NOTE: The functionality of Stream, for now, is minimal thus being a struct
    struct Stream
    {
      const char *source { nullptr };
      // Considered Immutable (read-only) pointer to source
      const char* source_start { nullptr };

      size_t current_line {1}; // row

      size_t column {1};

      // Zero-based index for the immutable source_start
      size_t source_c {0};

      /*
      // TODO: If needed
      // Pointing to start
      const char* source_start;
      // Pointing to end
      const char* source_end;
       */

      Stream() = default;

      Stream(const char *source)
        : source{source},
          source_start{source}
          {}

      Stream(const char *source, size_t current_line)
        : source {source},
          source_start{source},
        current_line{current_line} {}

      Stream(const char *source, size_t current_line, size_t column)
            : source{source},
              source_start{source},
              current_line{current_line},
              column{column} {}

      // NOTE: These are the following by default but let's have more control over them
      // No control needed especially if all of these are primitives.
      /*
      Stream(const Stream &other)
      {
        *this = other;
      }

      Stream &operator=(const Stream &other)
      {
        source = other.source;
        current_line = other.current_line;
        column = other.column;

        source_start = other.source_start;
        source_c = other.source_c;
        // source_start = other.source_start;
        // source_end = other.source_end;

        return *this;
      }
      */

      void skip_whitespace()
      {
        while (1)
        {
          switch (*source)
          {
          case ' ':
          case '\t':
          case '\r': {
            source++;
            column++; // COUNT COLUMN // TODO: Check if \t takes more column or NOT
            break;
          }
          case '\n': {
            source++;
            current_line++;
            // NOTE: COLUMN RESET ON NEW LINE
            column = 1;
            break;
          }
          default:
            return;
          }
        }
      }

      void skip_whitespace_except_newline()
      {
        while (1)
        {
          switch (*source)
          {
          case ' ':
          case '\t':
          case '\r': {
            source++;
            column++; // COUNT COLUMN // TODO: Check if \t takes more column or NOT
            break;
          }
          default:
            return;
          }
        }
      }

      void advance(size_t n)
      {
        for (size_t i = 0; i < n; i++)
        {
          // Safe advance
          if (*source != 0)
          {
            source++;
            column++;
          }
        }
      }

      char read_char() {
        return *source;
      }

      const char* read_as_pointer() {
        return source;
      }

      size_t get_current_line() const {
        return current_line;
      }

      size_t get_col() const {
        return column;
      }


    };

    // NOTE: The personal preference of 'struct' is that I need direct(easy) access to the members without needing to add setters/getters and, additionally, add rvalue and lvalue for those members
    struct Peekable
    {
      Peekable(const Token &token, const Stream &stream) : token{token}, stream{stream} {}

      Peekable(Token &&token, const Stream &stream) : token{std::move(token)}, stream{stream} {}

      // START: Inherit from Token, essentially
      TokenType get_type() const {
        return token.get_type();
      }

      size_t get_col() const {
        return token.get_col();
      }

      size_t get_line() const {
        return token.get_line();
      }

      const std::string& get_val() const {
        return token.get_val();
      }

      std::string to_string() const {
        return token.to_string();
      }
      // END: Inherit from Token, essentially

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

    explicit Lexer(const char *source) : stream(source)
    {
    }

    bool is_over()
    {
      return *stream.source == 0;
    }

    void set_stream(Stream &&stream)
    {
      this->stream = std::move(stream);
    }

    void set_stream(const Stream &stream)
    {
      this->stream = stream;
    }

    Stream& get_stream() {
      return stream;
    }

    // START: Inherit from Stream, essentially
    void skip_whitespace() {
      stream.skip_whitespace();
    }

    void skip_whitespace_except_newline() {
      stream.skip_whitespace_except_newline();
    }

    char read_char() {
      return stream.read_char();
    }

    size_t get_current_line() const {
      return stream.get_current_line();
    }

    size_t get_col() const {
      return stream.get_col();
    }
    // END: Inherit from Stream, essentially

    // TODO: REFACTOR THIS INTO A MACRO SO WE CAN DO SOMETHING LIKE THIS: GET_TOKEN(t_group == TokenGroup::CONST || t_group == TokenGroup::OPERATOR) ANYWHERE FROM THE CODE
    // WE CAN ALSO POSTFIX THE FUNCTIONS IN ACCORDANCE WITH THE TOKEN GROUP SUCH AS: get_token_keyword(), get_token_const(), get_token_operator(), get_token_any(). However, that could involve a lot of repetition in the code
    Token get_token(TokenGroup t_group)
    {
      #define constructToken(val, token_type) Token(val, token_type, current_line, stream.get_col())
      // #undef constructToken // NOTE: FOR AFTER LOCAL USAGE

      // NOTE: skips ahead of the token - not after the token - also the reason to "peek"
      // NOTE: we CANNOT consume the whitespace after the token by default. This is done manually via the line_defer below SINCE THE END OF AN EXPRESSION CAN BE '\n'
      skip_whitespace();

      const size_t current_line = stream.current_line;

      // NOTE: never advance the null character
      if (stream.read_char() == 0)
      {
        std::string val;
        return constructToken(val, TokenType::NULL_CHAR);
      }


      // NOTE: Keywords take priority over CONST_IDENF, etc. as the id can be matched to be "while" and similar
      if (t_group == TokenGroup::KEYWORD || t_group == TokenGroup::ANY) {

        if(is_keyword_while(stream.read_as_pointer())) {
          return constructToken("while", TokenType::KEYWORD_WHILE);
        }

        if(is_keyword_func(stream.read_as_pointer())) {
          return constructToken("func", TokenType::KEYWORD_FUNC);
        }

        if(is_keyword_if(stream.read_as_pointer())) {
          return constructToken("if", TokenType::KEYWORD_IF);
        }

        if(is_keyword_return(stream.read_as_pointer())) {
          return constructToken("return", TokenType::KEYWORD_RETURN);
        }

        if(is_keyword_decl(stream.read_as_pointer())) {
          return constructToken("decl", TokenType::KEYWORD_DECL);
        }

        if(is_keyword_break(stream.read_as_pointer())) {
          return constructToken("break", TokenType::KEYWORD_BREAK);
        }

        if(is_keyword_continue(stream.read_as_pointer())) {
          return constructToken("continue", TokenType::KEYWORD_CONTINUE);
        }

        if(is_keyword_true(stream.read_as_pointer())) {
          return constructToken("true", TokenType::KEYWORD_TRUE);
        }

        if(is_keyword_false(stream.read_as_pointer())) {
          return constructToken("false", TokenType::KEYWORD_FALSE);
        }

        if(is_keyword_nill(stream.read_as_pointer())) {
          return constructToken("nill", TokenType::KEYWORD_NILL);
        }

      }

      // NOTE: This has to be a block of if statements as opposed to if/else if due to the fact that the rest of the conditions(TokenGroup::ANY for example) become "unreachable" for a lack of a better word
      if (t_group == TokenGroup::CONST || t_group == TokenGroup::ANY)
      {
        if (is_digit(stream.read_as_pointer()))
        {
          std::string val;

          do
          {
            val += stream.read_char();
            stream.advance(1);
          } while (is_digit(stream.read_as_pointer()));

          return constructToken(std::move(val), TokenType::CONST_NUM);
        }
      }

      if(t_group == TokenGroup::IDENF || t_group == TokenGroup::ANY) {
        if(is_alpha(stream.read_as_pointer()) || stream.read_char() == '_') {
          std::string val;

          do {
            val += stream.read_char();
            stream.advance(1);
          } while(is_alnum(stream.read_as_pointer()) || stream.read_char() == '_');

          return constructToken(std::move(val), TokenType::CONST_IDENF);
        }
      }




      if (t_group == TokenGroup::OPERATOR || t_group == TokenGroup::ANY)
      {
        // NOTE: takes priority over '<' and '>' in order to avoid conflict
        // Tokens that are greater in length take precedence over others
        // NOTE: No point of the else if here as we "return" anyways
        if (is_eqlessthan(stream.read_as_pointer()))
        {
          return constructToken("<=", TokenType::OP_EQ_LESS_THAN);
        }
        else if (is_eqgreaterthan(stream.read_as_pointer()))
        {
          return constructToken(">=", TokenType::OP_EQ_GREATER_THAN);
        }
        else if (is_equalto(stream.read_as_pointer()))
        {
          return constructToken("==", TokenType::OP_EQUAL_TO);
        }
        else if (is_notequalto(stream.read_as_pointer()))
        {
          return constructToken("!=", TokenType::OP_NOT_EQUAL_TO);
        }


        if(is_logical_and(stream.read_as_pointer())) {
          return constructToken("&&", TokenType::OP_LOGICAL_AND);
        }
        else if(is_logical_or(stream.read_as_pointer())) {
          return constructToken("||", TokenType::OP_LOGICAL_OR);
        }


        if(is_assignment(stream.read_as_pointer())) {
          return constructToken("=", TokenType::OP_ASSIGNMENT);
        }

        if(is_logical_not(stream.read_as_pointer())) {
          return constructToken("!", TokenType::OP_LOGICAL_NOT);
        }



        if (is_plus(stream.read_as_pointer()))
        {
          return constructToken("+", TokenType::OP_PLUS);
        }
        else if (is_multiply(stream.read_as_pointer()))
        {
          return constructToken("*", TokenType::OP_MULTIPLY);
        }
        else if (is_divide(stream.read_as_pointer()))
        {
          return constructToken("/", TokenType::OP_DIVIDE);
        }
        else if (is_minus(stream.read_as_pointer()))
        {
          return constructToken("-", TokenType::OP_MINUS);

        }

        if (is_lessthan(stream.read_as_pointer()))
        {
          return constructToken("<", TokenType::OP_LESS_THAN);
        }
        else if (is_greaterthan(stream.read_as_pointer()))
        {
          return constructToken(">", TokenType::OP_GREATER_THAN);
        }

      }




      if (t_group == TokenGroup::SYMBOL || t_group == TokenGroup::ANY)
      {
        if (is_semicolon(stream.read_as_pointer())){
          return constructToken(";", TokenType::SEMI_COLON);
        }
        else if (is_openingparenthesis(stream.read_as_pointer()))
        {
          return constructToken("(", TokenType::SYM_OPEN_P);
        }
        else if (is_closingparenthesis(stream.read_as_pointer()))
        {
          return constructToken(")", TokenType::SYM_CLOS_P);
        }
        else if(is_openingbrackets(stream.read_as_pointer())) {
          return constructToken("{", TokenType::SYM_OPEN_B);
        }
        else if(is_closingbrackets(stream.read_as_pointer())) {
          return constructToken("}", TokenType::SYM_CLOSE_B);
        }
        
        if(is_dot(stream.read_as_pointer())) {
          return constructToken(".", TokenType::SYM_DOT);
        }

        if(is_openining_square_bracket(stream.read_as_pointer())) {
          return constructToken("[", TokenType::SYM_OPEN_SQ_B);
        }

        if(is_closing_square_bracket(stream.read_as_pointer())) {
          return constructToken("]", TokenType::SYM_CLOSE_SQ_B);
        }

        if(is_double_quote(stream.read_as_pointer())) {
          return constructToken("\"", TokenType::SYM_DQUOTE);
        }

        if(is_comma(stream.read_as_pointer())) {
          return constructToken(",", TokenType::SYM_COMMA);
        }

      }



      // TODO: Don't use the scoped block
      {
        std::string val;
        val += stream.read_char();

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

        return constructToken(std::move(val), TokenType::INVALID);
      }
    }

    void eat_peekable(const Peekable &peekable)
    {

      // Swap/Get to the end of that stream from Peekable
      set_stream(peekable.stream);

      switch (peekable.get_type())
      {
        // one-char (tokens)
        case TokenType::SEMI_COLON:
        case TokenType::OP_PLUS:
        case TokenType::OP_MINUS:
        case TokenType::OP_MULTIPLY:
        case TokenType::OP_DIVIDE:
        case TokenType::OP_LESS_THAN:
        case TokenType::OP_GREATER_THAN:
        case TokenType::SYM_OPEN_P:
        case TokenType::SYM_CLOS_P:
        case TokenType::SYM_OPEN_B:
        case TokenType::SYM_CLOSE_B:
        case TokenType::OP_ASSIGNMENT:
        case TokenType::SYM_DOT:
        case TokenType::SYM_DQUOTE:
        case TokenType::SYM_OPEN_SQ_B:
        case TokenType::SYM_CLOSE_SQ_B:
        case TokenType::SYM_COMMA:
        case TokenType::OP_LOGICAL_NOT:
        {
          stream.advance(1);
          break;
        }
        // two-char (tokens) operators
        case TokenType::OP_EQ_LESS_THAN:
        case TokenType::OP_EQ_GREATER_THAN:
        case TokenType::OP_EQUAL_TO:
        case TokenType::OP_NOT_EQUAL_TO:
        case TokenType::OP_LOGICAL_AND:
        case TokenType::OP_LOGICAL_OR:
        {
          stream.advance(2);
          break;
        }
        // These already advanced while "get_token" so set_stream takes care of it
        case TokenType::CONST_NUM:
        case TokenType::CONST_IDENF:
        case TokenType::NULL_CHAR:
          // NOTE: never advance the null character and all that is already consumed by pointer increment via stream.source -> stream.advance(size_t)
          return;
        case TokenType::KEYWORD_WHILE: {
          // stream.advance(5);
          stream.advance(strlen("while"));
          break;
        }
        case TokenType::KEYWORD_FUNC: {
          stream.advance(strlen("func"));
          break;
        }
        case TokenType::KEYWORD_IF: {
          stream.advance(strlen("if"));
          break;
        }
        case TokenType::KEYWORD_RETURN: {
          stream.advance(strlen("return"));
          break;
        }
        case TokenType::KEYWORD_DECL: {
          stream.advance(strlen("decl"));
          break;
        }
        case TokenType::KEYWORD_BREAK: {
          stream.advance(strlen("break"));
          break;
        }
        case TokenType::KEYWORD_CONTINUE: {
          stream.advance(strlen("continue"));
          break;
        }
        case TokenType::KEYWORD_FALSE: {
          stream.advance(strlen("false"));
          break;
        }
        case TokenType::KEYWORD_TRUE: {
          stream.advance(strlen("true"));
          break;
        }
        case TokenType::KEYWORD_NILL: {
          stream.advance(strlen("nill"));
          break;
        }
        default:
        {
          // NOTE: advancing for one at the moment
          stream.advance(1);
          break;
        }
      }

    }

    Peekable peek_token(TokenGroup t_group)
    {
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


  std::vector<Token> Lexer::generate_tokens(const char* source) {
    std::vector<Token> tokens;

    Lexer lexer;

    lexer.set_stream(source);

    while(!lexer.is_over()) {
      Peekable token = lexer.peek_token(TokenGroup::ANY);

      lexer.eat_peekable(token);
      tokens.push_back(std::move(token.token));

      // NOTE: The lexer stream is not OVER in this case since there is remaining white space at the end
      // NOTE: This was done on purpose so that we can detect new line at the end of expressions while parsing
      line_defer: __attribute__((cold, unused)); {
        lexer.skip_whitespace();
      }

    }

    return tokens;

  }

  bool compareTokens(const Token& a_token, const Token& b_token) {

    return a_token.get_type() == b_token.get_type() &&
             a_token.get_val() == b_token.get_val() &&
              a_token.get_line() == b_token.get_line() &&
                a_token.get_col() == b_token.get_col();

  }

} // namespace LexAnalysis

#endif
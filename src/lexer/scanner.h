#pragma once

#include "token.h"
#include <cstddef>
#include <cstdint>
#include <queue>
#include <string_view>
#include <vector>

struct Scan {
  std::vector<Token> tokens;
  std::vector<std::string> errors;
};

class Scanner {
  Scanner(std::string_view source);
  ~Scanner();

private:
  std::string_view source;
  size_t current{0};
  Pos pos{1, 1};
  std::vector<Token> tokens;
  std::vector<std::string> errors;
  int line, column;

  char32_t peek();
  char32_t peekNext();
  char32_t advance();
  bool isAtEnd();
  bool match(TokenType type);
  void scanToken();
  void emit(TokenType type, std::string_view lexeme, Pos pos);
  void emit(TokenType type, std::string_view lexeme, Pos pos, double number);
  void emit(TokenType type, std::string_view lexeme, Pos pos,
            std::string value);
  void addError(std::string msg);
  void emitToken(TokenType type);
  void scanBlockComment();
  void scanString();
  void scanRawString();
  void scanFmtString();
  void scanIdent(char32_t c);
  bool isDigit(char32_t c);
  bool isAlpha(char32_t c);
  bool isAlphaNumeric(char32_t c);
  void scanNumber(char32_t c);

public:
  Scan scan();
};

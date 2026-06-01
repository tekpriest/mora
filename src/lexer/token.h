#pragma once

#include <string>
#include <string_view>

enum class TokenType {
  // Special
  END_OF_FILE,
  ILLEGAL,

  // Literals
  IDENT,
  INT,
  FLOAT,
  STRING,
  RAWSTR,
  FMTSTR,
  BOOL,

  // Keywords
  LET,
  FN,
  IF,
  ELSE,
  MATCH,
  STRUCT,
  ENUM,
  TRAIT,
  IMPL,
  FOR,
  IN,
  RETURN,
  USE,
  WITH,
  MACRO,

  // Operators
  PLUS,
  MINUS,
  STAR,
  SLASH,
  PERCENT,
  BANG,
  AMP,
  PIPE,
  CARET,

  // Comparison
  EQ,
  NEQ,
  LT,
  GT,
  LTE,
  GTE,

  // Assignment
  ASSIGN,
  PLUS_ASSIGN,
  MINUS_ASSIGN,

  // Delimiters
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LBRACKET,
  RBRACKET,

  // Punctuation
  COMMA,
  COLON,
  DCOLON,
  SEMICOLON,
  DOT,
  DOTDOT,
  QUESTION,
  AT,
  HASH,
  DOLLAR,

  // Mora-specific
  ARROW,     // ->
  FAT_ARROW, // =>
  PIPE_OP,   // |>
  LARROW,    // <-
};

struct Pos {
  int line, column;
};

struct Token {
  TokenType type;
  std::string_view lexeme; // the raw source text
  Pos pos;
  // value storage (one of these is active depending on type)
  double number_value;
  std::string string_value;
};

TokenType lookup_ident(std::string_view s);
std::string_view to_string(TokenType type);

#include "token.h"
#include <string_view>
#include <unordered_map>

TokenType lookupIdent(std::string_view s) {
  static const std::unordered_map<std::string_view, TokenType> kw = {
      {"let", TokenType::LET},     {"fn", TokenType::FN},
      {"if", TokenType::IF},       {"else", TokenType::ELSE},
      {"match", TokenType::MATCH}, {"struct", TokenType::STRUCT},
      {"enum", TokenType::ENUM},   {"trait", TokenType::TRAIT},
      {"impl", TokenType::IMPL},   {"for", TokenType::FOR},
      {"in", TokenType::IN},       {"return", TokenType::RETURN},
      {"use", TokenType::USE},     {"with", TokenType::WITH},
      {"macro", TokenType::MACRO}, {"true", TokenType::BOOL},
      {"false", TokenType::BOOL}};
  auto it = kw.find(s);
  return it != kw.end() ? it->second : TokenType::IDENT;
};

std::string toString(TokenType type) {
  switch (type) {
  case TokenType::END_OF_FILE:
    return "EOF";
  case TokenType::ILLEGAL:
    return "ILLEGAL";
  case TokenType::IDENT:
    return "IDENT";
  case TokenType::INT:
    return "INT";
  case TokenType::FLOAT:
    return "FLOAT";
  case TokenType::STRING:
    return "STRING";
  case TokenType::RAWSTR:
    return "RAWSTR";
  case TokenType::FMTSTR:
    return "FMTSTR";
  case TokenType::BOOL:
    return "BOOL";
  case TokenType::LET:
    return "LET";
  case TokenType::FN:
    return "FN";
  case TokenType::IF:
    return "IF";
  case TokenType::ELSE:
    return "ELSE";
  case TokenType::MATCH:
    return "MATCH";
  case TokenType::STRUCT:
    return "STRUCT";
  case TokenType::ENUM:
    return "ENUM";
  case TokenType::TRAIT:
    return "TRAIT";
  case TokenType::IMPL:
    return "IMPL";
  case TokenType::FOR:
    return "FOR";
  case TokenType::IN:
    return "IN";
  case TokenType::RETURN:
    return "RETURN";
  case TokenType::USE:
    return "USE";
  case TokenType::WITH:
    return "WITH";
  case TokenType::MACRO:
    return "MACRO";
  case TokenType::PLUS:
    return "+";
  case TokenType::MINUS:
    return "-";
  case TokenType::STAR:
    return "*";
  case TokenType::SLASH:
    return "/";
  case TokenType::PERCENT:
    return "%";
  case TokenType::BANG:
    return "!";
  case TokenType::AMP:
    return "&";
  case TokenType::PIPE:
    return "|";
  case TokenType::CARET:
    return "^";
  case TokenType::EQ:
    return "==";
  case TokenType::NEQ:
    return "!=";
  case TokenType::LT:
    return "<";
  case TokenType::GT:
    return ">";
  case TokenType::LTE:
    return "<=";
  case TokenType::GTE:
    return ">=";
  case TokenType::ASSIGN:
    return "=";
  case TokenType::PLUS_ASSIGN:
    return "+=";
  case TokenType::MINUS_ASSIGN:
    return "-=";
  case TokenType::LPAREN:
    return "(";
  case TokenType::RPAREN:
    return ")";
  case TokenType::LBRACE:
    return "{";
  case TokenType::RBRACE:
    return "}";
  case TokenType::LBRACKET:
    return "[";
  case TokenType::RBRACKET:
    return "]";
  case TokenType::COMMA:
    return ",";
  case TokenType::COLON:
    return ":";
  case TokenType::DCOLON:
    return "::";
  case TokenType::SEMICOLON:
    return ";";
  case TokenType::DOT:
    return ".";
  case TokenType::DOTDOT:
    return "..";
  case TokenType::QUESTION:
    return "?";
  case TokenType::AT:
    return "@";
  case TokenType::HASH:
    return "#";
  case TokenType::DOLLAR:
    return "$";
  case TokenType::ARROW:
    return "->";
  case TokenType::FAT_ARROW:
    return "=>";
  case TokenType::PIPE_OP:
    return "|>";
  case TokenType::LARROW:
    return "<-";
  }

  return "UNKNOWN";
}

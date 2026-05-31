#include "scanner.h"
#include "token.h"
#include <string_view>

Scanner::Scanner(std::string_view source) {
  source = source;
  current = 0;
  pos = Pos{line = 1, column = 1};
}

Scan Scanner::scan() {
  if (!isAtEnd()) {
    scanToken();
  }
  emit(TokenType::END_OF_FILE, "", pos);

  return Scan{.tokens = tokens, .errors = errors};
}

void Scanner::scanToken() {
  pos = Pos{.line = line, .column = column};
  char32_t c = advance();

  switch (c) {
  case '(':
    emitToken(TokenType::LPAREN);
  case ')':
    emitToken(TokenType::RPAREN);
  case '{':
    emitToken(TokenType::LBRACE);
  case '}':
    emitToken(TokenType::RBRACE);
  case '[':
    emitToken(TokenType::LBRACKET);
  case ']':
    emitToken(TokenType::RBRACKET);
  case ',':
    emitToken(TokenType::COMMA);
  case ';':
    emitToken(TokenType::SEMICOLON);
  case '%':
    emitToken(TokenType::PERCENT);
  case '^':
    emitToken(TokenType::CARET);
  case '@':
    emitToken(TokenType::AT);
  case '?':
    emitToken(TokenType::QUESTION);
  case '&':
    emitToken(TokenType::AMP);
  case '+':
    match(TokenType::EQ) ? emitToken(TokenType::PLUS_ASSIGN)
                         : emitToken(TokenType::PLUS);
  case '-':
    if (match(TokenType::GT)) {
      emitToken(TokenType::ARROW);
    } else if (match(TokenType::EQ)) {
      emitToken(TokenType::MINUS_ASSIGN);
    } else {
      emitToken(TokenType::MINUS);
    }
  case '*':
    emitToken(TokenType::STAR);
  case '!':
    match(TokenType::EQ) ? emitToken(TokenType::NEQ)
                         : emitToken(TokenType::BANG);
  case '=':
    if (match(TokenType::EQ)) {
      emitToken(TokenType::EQ);
    } else if (match(TokenType::GT)) {
      emitToken(TokenType::FAT_ARROW);
    } else {
      emitToken(TokenType::ASSIGN);
    }

  case '<':
    if (match(TokenType::ASSIGN)) {
      emitToken(TokenType::LTE);
    } else if (match(TokenType::MINUS)) {
      emitToken(TokenType::LARROW);
    } else {
      emitToken(TokenType::LT);
    }

  case '>':
    match(TokenType::ASSIGN) ? emitToken(TokenType::GTE)
                             : emitToken(TokenType::GT);

  case '|':
    match(TokenType::GT) ? emitToken(TokenType::PIPE_OP)
                         : emitToken(TokenType::PIPE);

  case ':':
    match(TokenType::COLON) ? emitToken(TokenType::DCOLON)
                            : emitToken(TokenType::COLON);

  case '.':
    match(TokenType::DOT) ? emitToken(TokenType::DOTDOT)
                          : emitToken(TokenType::DOT);
  case '$':
    emitToken(TokenType::DOLLAR);
  // comments
  case '/':
    if (match(TokenType::SLASH)) {
      // single line comment - consume to end of line
      while (!isAtEnd() && peek() != '\n') {
        advance();
      }
    } else if (match(TokenType::STAR)) {
      scanBlockComment();
    } else {
      emitToken(TokenType::SLASH);
    }

  case ' ':
  case '\t':
  case '\r':
  case '\n':

  case '"':
    scanString();
  case 'r':
    if (peek() == '#' && peekNext() == '"') {
      advance(); // #
      advance(); // "
      scanRawString();
    } else {
      scanIdent('r');
    }

  case 'f':
    if (peek() == '#' && peekNext() == '"') {
      advance(); // #
      scanFmtString();
    } else {
      scanIdent('f');
    }

  default:
    if (isDigit(c)) {
      scanNumber(c);
    }
    if (isAlpha(c)) {
      scanIdent(c);
    }
    addError(&"unexpected character "[c]);
    emitToken(TokenType::ILLEGAL);
  }
}

void Scanner::emitToken(TokenType type) {
  std::string_view tokenString = to_string(type);
  emit(type, tokenString, pos);
}

bool Scanner::isAtEnd() { return current >= source.size(); }

bool Scanner::match(TokenType type) {}
void Scanner::addError(std::string msg) {}
char32_t Scanner::peek() {}
char32_t Scanner::peekNext() {}
char32_t Scanner::advance() {}
void Scanner::emit(TokenType type, std::string_view lexeme, Pos pos) {}
void emit(TokenType type, std::string_view lexeme, Pos pos, double number) {}
void emit(TokenType type, std::string_view lexeme, Pos pos, std::string value) {
}
void Scanner::scanIdent(char32_t c) {}
void Scanner::scanBlockComment() {}
void Scanner::scanString() {}
void Scanner::scanRawString() {}
void Scanner::scanFmtString() {}
bool Scanner::isDigit(char32_t c) {}
bool Scanner::isAlpha(char32_t c) {}
bool Scanner::isAlphaNumeric(char32_t c) {}
void Scanner::scanNumber(char32_t c) {}

#include "scanner.h"
#include "token.h"
#include <string>
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
    match('=') ? emitToken(TokenType::PLUS_ASSIGN) : emitToken(TokenType::PLUS);
  case '-':
    if (match('>')) {
      emitToken(TokenType::ARROW);
    } else if (match('=')) {
      emitToken(TokenType::MINUS_ASSIGN);
    } else {
      emitToken(TokenType::MINUS);
    }
  case '*':
    emitToken(TokenType::STAR);
  case '!':
    match('=') ? emitToken(TokenType::NEQ) : emitToken(TokenType::BANG);
  case '=':
    if (match('=')) {
      emitToken(TokenType::EQ);
    } else if (match('>')) {
      emitToken(TokenType::FAT_ARROW);
    } else {
      emitToken(TokenType::ASSIGN);
    }

  case '<':
    if (match('=')) {
      emitToken(TokenType::LTE);
    } else if (match('-')) {
      emitToken(TokenType::LARROW);
    } else {
      emitToken(TokenType::LT);
    }

  case '>':
    match('=') ? emitToken(TokenType::GTE) : emitToken(TokenType::GT);

  case '|':
    match('>') ? emitToken(TokenType::PIPE_OP) : emitToken(TokenType::PIPE);

  case ':':
    match(':') ? emitToken(TokenType::DCOLON) : emitToken(TokenType::COLON);

  case '.':
    match('.') ? emitToken(TokenType::DOTDOT) : emitToken(TokenType::DOT);
  case '$':
    emitToken(TokenType::DOLLAR);
  // comments
  case '/':
    if (match('/')) {
      // single line comment - consume to end of line
      while (!isAtEnd() && peek() != '\n') {
        advance();
      }
    } else if (match('*')) {
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
      scanRawString(pos);
    } else {
      scanIdent('r');
    }

  case 'f':
    if (peek() == '#' && peekNext() == '"') {
      advance(); // #
      scanFmtString(pos);
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

bool Scanner::match(char32_t c) {
  char s = source[current];
  if (isAtEnd() || source[current] != c) {
    return false;
  }
  advance();
  return true;
}
void Scanner::addError(std::string msg) { errors.push_back(msg); }
char32_t Scanner::peek() { return isAtEnd() ? 0 : source[current]; }
char32_t Scanner::peekNext() {
  if (current + 1 >= source.size()) {
    return 0;
  }

  return source[current + 1];
}
char32_t Scanner::advance() {
  char32_t ch = source[current];
  current++;
  if (ch == '\n') {
    line++;
    column = 1;
  } else {
    column++;
  }

  return ch;
}
void Scanner::emit(TokenType type, std::string_view lexeme, Pos pos) {
  tokens.push_back(Token{.type = type, .lexeme = lexeme, .pos = pos});
}
void Scanner::emit(TokenType type, std::string_view lexeme, Pos pos,
                   double number) {
  Token token =
      Token{.type = type, .lexeme = lexeme, .pos = pos, .number_value = number};
  tokens.push_back(token);
}
void Scanner::emit(TokenType type, std::string_view lexeme, Pos pos,
                   std::string value) {
  Token token =
      Token{.type = type, .lexeme = lexeme, .pos = pos, .string_value = value};
  tokens.push_back(token);
}
void Scanner::scanIdent(char32_t c) {
  // TODO: implement scan ident
  std::string word;
  word += c;
  while (!isAtEnd() && isAlphaNumeric(peek())) {
    word += advance();
  }
  TokenType type = lookup_indent(word);
  emit(type, word, pos);
}
void Scanner::scanBlockComment() {
  int depth{1};
  while (!isAtEnd() && depth > 0) {
    if (peek() == '/' && peekNext() == '*') {
      advance();
      advance();
      depth++;
    } else if (peek() == '*' && peekNext() == '/') {
      advance();
      advance();
      depth--;
    } else {
      advance();
    }
  }

  if (depth > 0) {
    addError("unterminated block comment");
  }
}
void Scanner::scanString() {
  std::string word;
  while (!isAtEnd() && peek() != '"') {
    char32_t ch = advance();
    if (ch == '\\') {
      if (isAtEnd()) {
        addError("unterminated string");
        return;
      }

      switch (advance()) {
      case 'n':
        word += '\n';
      case 't':
        word += '\t';
      case '\\':
        word += '\\';
      case '"':
        word += '"';
      case 'r':
        word += '\r';
      default:
        word += ch;
      }
    } else {
      word += ch;
    }
  }
  if (isAtEnd()) {
    addError("unterminated string");
    return;
  }
  advance();
  emit(TokenType::STRING, word, pos);
}
void Scanner::scanRawString(Pos pos) {
  std::string word;
  while (!isAtEnd()) {
    if (peek() == '"' && peekNext() == '#') {
      advance();
      advance();
      emit(TokenType::RAWSTR, word, pos);
      return;
    }
    word += advance();
  }
  addError("unterminated raw string");
}
void Scanner::scanFmtString(Pos pos) {
  std::string word;
  while (!isAtEnd() && peek() != '"') {
    word += advance();
  }
  if (isAtEnd()) {
    addError("unterminated format string");
    return;
  }
  advance();
  emit(TokenType::FMTSTR, word, pos);
}
bool Scanner::isDigit(char32_t c) { return c >= '0' && c <= '9'; }
bool Scanner::isAlpha(char32_t c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
bool Scanner::isAlphaNumeric(char32_t c) { return isAlpha(c) || isDigit(c); }
void Scanner::scanNumber(char32_t c) {
  std::string word;
  word += c;

  while (!isAtEnd() && (isDigit(peek()) || peek() == '_')) {
    word += advance();
  }

  // check for float
  if (!isAtEnd() && peek() == '.' && isDigit(peekNext())) {
    word += advance(); // consume '.'
    while (!isAtEnd() && isDigit(peek())) {
      word += advance();
    }
    emit(TokenType::FLOAT, word, pos);
    return;
  }
  emit(TokenType::INT, word, pos);
}

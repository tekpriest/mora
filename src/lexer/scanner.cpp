#include "scanner.h"
#include "token.h"
#include <string>
#include <string_view>

Scanner::Scanner(std::string_view source)
    : source(source), cursor(0), pos{1, 0} {}

Scanner::~Scanner() = default;

Scan Scanner::scan() {
  while (!isAtEnd()) {
    scanToken();
  }
  emit(TokenType::END_OF_FILE, "", pos);

  return Scan{.tokens = tokens, .errors = errors};
}

void Scanner::scanToken() {
  Pos start = pos;
  char32_t c = advance();

  switch (c) {
  case '(':
    emitToken(TokenType::LPAREN, start);
    break;
  case ')':
    emitToken(TokenType::RPAREN, start);
    break;
  case '{':
    emitToken(TokenType::LBRACE, start);
    break;
  case '}':
    emitToken(TokenType::RBRACE, start);
    break;
  case '[':
    emitToken(TokenType::LBRACKET, start);
    break;
  case ']':
    emitToken(TokenType::RBRACKET, start);
    break;
  case ',':
    emitToken(TokenType::COMMA, start);
    break;
  case ';':
    emitToken(TokenType::SEMICOLON, start);
    break;
  case '%':
    emitToken(TokenType::PERCENT, start);
    break;
  case '^':
    emitToken(TokenType::CARET, start);
    break;
  case '@':
    emitToken(TokenType::AT, start);
    break;
  case '?':
    emitToken(TokenType::QUESTION, start);
    break;
  case '&':
    emitToken(TokenType::AMP, start);
    break;
  case '+':
    match('=') ? emitToken(TokenType::PLUS_ASSIGN, start)
               : emitToken(TokenType::PLUS, start);
    break;
  case '-':
    if (match('>')) {
      emitToken(TokenType::ARROW, start);
    } else if (match('=')) {
      emitToken(TokenType::MINUS_ASSIGN, start);
    } else {
      emitToken(TokenType::MINUS, start);
    }
    break;
  case '*':
    emitToken(TokenType::STAR, start);
    break;
  case '!':
    match('=') ? emitToken(TokenType::NEQ, start)
               : emitToken(TokenType::BANG, start);
    break;
  case '=':
    if (match('=')) {
      emitToken(TokenType::EQ, start);
    } else if (match('>')) {
      emitToken(TokenType::FAT_ARROW, start);
    } else {
      emitToken(TokenType::ASSIGN, start);
    }
    break;

  case '<':
    if (match('=')) {
      emitToken(TokenType::LTE, start);
    } else if (match('-')) {
      emitToken(TokenType::LARROW, start);
    } else {
      emitToken(TokenType::LT, start);
    }
    break;

  case '>':
    match('=') ? emitToken(TokenType::GTE, start)
               : emitToken(TokenType::GT, start);
    break;

  case '|':
    match('>') ? emitToken(TokenType::PIPE_OP, start)
               : emitToken(TokenType::PIPE, start);
    break;

  case ':':
    match(':') ? emitToken(TokenType::DCOLON, start)
               : emitToken(TokenType::COLON, start);
    break;

  case '.':
    match('.') ? emitToken(TokenType::DOTDOT, start)
               : emitToken(TokenType::DOT, start);
    break;
  case '$':
    emitToken(TokenType::DOLLAR, start);
    break;
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
      emitToken(TokenType::SLASH, start);
    }
    break;

  case ' ':
    break;
  case '\t':
    break;
  case '\r':
    break;
  case '\n':
    pos.line++;
    pos.column = 0;
    break;

  case '"':
    scanString();
    break;
  case 'r':
    if (peek() == '#' && peekNext() == '"') {
      advance(); // #
      advance(); // "
      scanRawString(pos);
    } else {
      scanIdent('r');
    }
    break;

  case 'f':
    if (peek() == '#' && peekNext() == '"') {
      advance(); // #
      scanFmtString(pos);
    } else {
      scanIdent('f');
    }
    break;

  default:
    if (isDigit(c)) {
      scanNumber(c);
    } else if (isAlpha(c)) {
      scanIdent(c);
    } else {
      addError("unexpected character: " + std::string(1, static_cast<char>(c)));
      emitToken(TokenType::ILLEGAL, start);
    }
  }
}

void Scanner::emitToken(TokenType type, Pos pos) {
  std::string tokenString = to_string(type);
  emit(type, tokenString, pos);
}

bool Scanner::isAtEnd() { return cursor >= source.size(); }

bool Scanner::match(char32_t c) {
  auto s = static_cast<char32_t>(source[cursor]);

  if (isAtEnd() || s != c) {
    return false;
  }
  advance();
  return true;
}
void Scanner::addError(std::string msg) { errors.push_back(msg); }
char32_t Scanner::peek() { return isAtEnd() ? 0 : source[cursor]; }
char32_t Scanner::peekNext() {
  if (cursor + 1 >= source.size()) {
    return 0;
  }

  return source[cursor + 1];
}

char32_t Scanner::advance() {
  char32_t ch = source[cursor];
  cursor++;
  if (ch == '\n') {
    pos.line++;
    pos.column = 0;
  } else {
    pos.column++;
  }

  return ch;
}
void Scanner::emit(TokenType type, std::string lexeme, Pos pos) {
  tokens.push_back(Token{.type = type, .lexeme = lexeme, .pos = pos});
}
void Scanner::emit(TokenType type, std::string lexeme, Pos pos, double number) {
  Token token =
      Token{.type = type, .lexeme = lexeme, .pos = pos, .number_value = number};
  tokens.push_back(token);
}
void Scanner::emit(TokenType type, std::string lexeme, Pos pos,
                   std::string value) {
  Token token =
      Token{.type = type, .lexeme = lexeme, .pos = pos, .string_value = value};
  tokens.push_back(token);
}

void Scanner::scanIdent(char32_t c) {
  std::string word;
  word += c;
  while (!isAtEnd() && isAlphaNumeric(peek())) {
    word += advance();
  }
  TokenType type = lookup_ident(word);
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

      char32_t escaped = advance();
      switch (escaped) {
      case 'n':
        word += '\n';
        break;
      case 't':
        word += '\t';
        break;
      case '\\':
        word += '\\';
        break;
      case '"':
        word += '"';
        break;
      case 'r':
        word += '\r';
        break;
      default:
        word += escaped;
        break;
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

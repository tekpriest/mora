#include "lexer/scanner.h"
#include "lexer/token.h"
#include <catch2/catch_test_macros.hpp>
#include <initializer_list>
#include <iostream>
#include <string_view>

// --- Helpers -------------------------
static Scan scan_source(std::string_view source) {
  Scanner scanner(source);
  return scanner.scan();
}

// Check token sequence by type only
static void expect_types(const std::vector<Token> &tokens,
                         std::initializer_list<TokenType> expected) {
  REQUIRE(tokens.size() == expected.size());
  size_t i = 0;
  for (auto type : expected) {
    INFO("Token index " << i);
    REQUIRE(tokens[i].type == type);
    i++;
  }
}

// --- Single character tokens ---------
TEST_CASE("Scanner: single char tokens", "[scanner]") {
  auto result = scan_source("(){},;");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens,
               {TokenType::LPAREN, TokenType::RPAREN, TokenType::LBRACE,
                TokenType::RBRACE, TokenType::COMMA, TokenType::SEMICOLON,
                TokenType::END_OF_FILE});
}

TEST_CASE("Scanner: two-char operators and Mora-specific", "[scanner]") {
  auto result = scan_source("-> => |> <- == != <= >= += -=");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens,
               {TokenType::ARROW, TokenType::FAT_ARROW, TokenType::PIPE_OP,
                TokenType::LARROW, TokenType::EQ, TokenType::NEQ,
                TokenType::LTE, TokenType::GTE, TokenType::PLUS_ASSIGN,
                TokenType::MINUS_ASSIGN, TokenType::END_OF_FILE});
};

TEST_CASE("Scanner: := vs : vs ::", "[scanner]") {
  auto result = scan_source(": ::");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens,
               {TokenType::COLON, TokenType::DCOLON, TokenType::END_OF_FILE});
}

// ── Keywords and identifiers ───────────────────────────

TEST_CASE("Scanner: keywords", "[scanner]") {
  auto result = scan_source("let fn if else match struct enum return");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens,
               {TokenType::LET, TokenType::FN, TokenType::IF, TokenType::ELSE,
                TokenType::MATCH, TokenType::STRUCT, TokenType::ENUM,
                TokenType::RETURN, TokenType::END_OF_FILE});
}

TEST_CASE("Scanner: identifiers", "[scanner]") {
  auto result = scan_source("foo bar_baz x1 _private");
  REQUIRE(result.errors.empty());
  for (size_t i = 0; i < 4; i++) {
    REQUIRE(result.tokens[i].type == TokenType::IDENT);
  }
  REQUIRE(result.tokens[0].lexeme == "foo");
  REQUIRE(result.tokens[1].lexeme == "bar_baz");
  REQUIRE(result.tokens[2].lexeme == "x1");
  REQUIRE(result.tokens[3].lexeme == "_private");
}

TEST_CASE("Scanner: bool literals", "[scanner]") {
  auto result = scan_source("true false");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens,
               {TokenType::BOOL, TokenType::BOOL, TokenType::END_OF_FILE});
  REQUIRE(result.tokens[0].lexeme == "true");
  REQUIRE(result.tokens[1].lexeme == "false");
}

// ── Numbers ─────────────────────────────────────────────

TEST_CASE("Scanner: integers", "[scanner]") {
  auto result = scan_source("42 0 1_000_000");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens, {TokenType::INT, TokenType::INT, TokenType::INT,
                               TokenType::END_OF_FILE});
  REQUIRE(result.tokens[0].lexeme == "42");
  REQUIRE(result.tokens[2].lexeme == "1_000_000");
}

TEST_CASE("Scanner: floats", "[scanner]") {
  auto result = scan_source("3.14 0.5");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens,
               {TokenType::FLOAT, TokenType::FLOAT, TokenType::END_OF_FILE});
  REQUIRE(result.tokens[0].lexeme == "3.14");
}

// ── Strings ─────────────────────────────────────────────

TEST_CASE("Scanner: regular strings", "[scanner]") {
  auto result = scan_source("\"hello\" \"world\"");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens,
               {TokenType::STRING, TokenType::STRING, TokenType::END_OF_FILE});
}

TEST_CASE("Scanner: string escapes", "[scanner]") {
  auto result = scan_source("\"a\\nb\\tc\"");
  REQUIRE(result.errors.empty());
  REQUIRE(result.tokens[0].type == TokenType::STRING);
  REQUIRE(result.tokens[0].lexeme == "a\nb\tc");
}

TEST_CASE("Scanner: raw strings", "[scanner]") {
  auto result = scan_source("r#\"raw\\no_escape\"#");
  REQUIRE(result.errors.empty());
  REQUIRE(result.tokens[0].type == TokenType::RAWSTR);
  REQUIRE(result.tokens[0].lexeme == "raw\\no_escape");
}

TEST_CASE("Scanner: format strings", "[scanner]") {
  auto result = scan_source("f#\"hello {name}\"");
  REQUIRE(result.errors.empty());
  REQUIRE(result.tokens[0].type == TokenType::FMTSTR);
  REQUIRE(result.tokens[0].lexeme == "hello {name}");
}

// ── Comments ────────────────────────────────────────────

TEST_CASE("Scanner: line comments are ignored", "[scanner]") {
  auto result = scan_source("// this is a comment\nlet");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens, {TokenType::LET, TokenType::END_OF_FILE});
}

TEST_CASE("Scanner: block comments", "[scanner]") {
  auto result = scan_source("/* nested /* comment */ here */let");
  REQUIRE(result.errors.empty());
  expect_types(result.tokens, {TokenType::LET, TokenType::END_OF_FILE});
}

TEST_CASE("Scanner: unterminated block comment", "[scanner]") {
  auto result = scan_source("/* oops");
  REQUIRE(!result.errors.empty());
  REQUIRE(result.errors[0].find("unterminated") != std::string::npos);
}

// ── Error handling ──────────────────────────────────────

TEST_CASE("Scanner: illegal character", "[scanner]") {
  auto result = scan_source("~");
  REQUIRE(result.tokens[0].type == TokenType::ILLEGAL);
  REQUIRE(!result.errors.empty());
}

// ── Position tracking ───────────────────────────────────

TEST_CASE("Scanner: line and column tracking", "[scanner]") {
  auto result = scan_source("let\n  x");
  REQUIRE(result.errors.empty());
  REQUIRE(result.tokens[0].type == TokenType::LET);
  REQUIRE(result.tokens[0].pos.line == 1);
  REQUIRE(result.tokens[0].pos.column == 0); // column at token start

  REQUIRE(result.tokens[1].type == TokenType::IDENT);
  REQUIRE(result.tokens[1].pos.line == 2);
  REQUIRE(result.tokens[1].pos.column == 2);
}

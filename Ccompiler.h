#pragma once

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

enum TokenKind
{
    TK_RESERVED, // 記号
    TK_WORD,     // 予約語
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
};

// トークン型
struct Token
{
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;
    Token(TokenKind kind, Token *cur, char *str,int len);
    Token();
};
struct Node;

// 入力プログラム
extern char *user_input;
// 現在着目しているトークン
extern Token *token;
// コード
extern Node *code[100];

// Perse
bool consume(const char *op);
void expect(const char *op);
int expect_number();
Token *tokenize(char *p);
bool at_eof();
Token *consume_ident();
void error(const char *str,...);
bool consume_same_kind(TokenKind kind);

// codegen
void gen(Node *node);
Node* expr();
void program();
void error(const char *loc, const char *fmt, ...);

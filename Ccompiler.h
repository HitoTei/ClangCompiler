#pragma once

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

struct Token;
struct Node;

// 入力プログラム
extern char *user_input;
// 現在着目しているトークン
extern Token *token;


// Perse
bool consume(const char *op);
void expect(const char *op);
int expect_number();
Token *tokenize(char *p);

// codegen
void gen(Node *node);
Node* expr();

void error(const char *loc, const char *fmt, ...);

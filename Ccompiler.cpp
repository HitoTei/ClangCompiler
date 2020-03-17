#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

using namespace std;
// トークンの種類
enum TokenKind
{
    TK_RESERVED, // 記号
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

    // 新しいトークンを作成してcurに繋げる
    Token(TokenKind kind, Token *cur, char *str,int len)
    {
        this->kind = kind;
        this->str = str;
        cur->next = this;
        this->len = len;
    }
    Token(){};
};

// 現在着目しているトークン
Token *token;
// 入力プログラム
char *user_input;

// エラー箇所を報告する
void error(const char *loc, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(const char *op)
{
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str,op,token->len))
        return false;

    token = token->next;
    return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(const char *op)
{
    if (token->kind != TK_RESERVED || 
        token->len != strlen(op) ||
        memcmp(token->str,op,token->len))
        error(token->str, "'%s'は'%s'ではありません", token->str, op);
    
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number()
{
    if (token->kind != TK_NUM)
        error(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

bool startswith(const char*p, const char *q){
  return !memcmp(p,q,strlen(q));
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        // 空白文字をスキップ
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if(startswith(p,"==") ||
           startswith(p,"!=") ||
           startswith(p,"<=") ||
           startswith(p,">=")){
             cur = new Token(TK_RESERVED,cur,p,2);
             p+=2;
             continue;
           }

        if (strchr("+-*/()<>",*p))
        {
            cur = new Token(TK_RESERVED, cur, p++,1);
            continue;
        }

        if (isdigit(*p))
        {
            cur = new Token(TK_NUM, cur, p,0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p-q;
            continue;
        }

        error(token->str, "トークナイズできません");
    }

    new Token(TK_EOF, cur, p,0);
    return head.next;
}

// 抽象構文木のノードの種類
enum NodeKind
{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LT,  // <
    ND_LE,  // <=
    ND_NUM, // 整数
};

// 抽象構文木のノードの型
struct Node
{
    NodeKind kind; // ノードの型
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    int val;       // kindがND_NUMの場合のみ使う
    Node() {}
    Node(NodeKind kind, Node *lhs, Node *rhs)
    {
        this->kind = kind;
        this->lhs = lhs;
        this->rhs = rhs;
    }
    Node(int val){
        this->kind = ND_NUM;
        this->val = val;
    }
};

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();


Node *expr() {
  return equality();
}
Node *equality(){
  Node *node = relational();
  while(true){
    if(consume("=="))
      node = new Node(ND_EQ,node,relational());
    else if(consume("!="))
      node = new Node(ND_NE,node,relational());
    else return node;
  }
}

Node *relational(){
  Node *node = add();

  while(true){ // 基本は<,<=. >,>=は両辺を入れ替える
    if(consume("<"))
      node = new Node(ND_LT,node,add());
    else if (consume("<="))
      node = new Node(ND_LE, node, add());
    else if (consume(">"))
      node = new Node(ND_LT, add(), node);
    else if (consume(">="))
      node = new Node(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add(){
  Node *node = mul();
  while(true){
    if(consume("+"))
      node = new Node(ND_ADD,node,mul());
    else if(consume("-"))
      node = new Node(ND_SUB,node,mul());
    else return node;
  }
}

Node *mul() {
  Node *node = unary();

  while(true) {
    if (consume("*"))
      node = new Node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new Node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary(){
  if(consume("+"))
    return unary();
  if(consume("-"))
    return new Node(ND_SUB,new Node(0),unary());
  return primary();
}

Node *primary() {

  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  // そうでなければ数値のはず
  return new Node(expect_number());
}


void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD: // +
    printf("  add rax, rdi\n");
    break;
  case ND_SUB: // -
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL: // *
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV: // /
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ: // ==
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE: // !=
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT: // <
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE: // <=
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}
int main(int argc, char **argv) {
  if (argc != 2) {
    error(0,"引数の個数が正しくありません");
    return 1;
  }

  // トークナイズしてパースする
  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expr();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 抽象構文木を下りながらコード生成
  gen(node);

  // スタックトップに式全体の値が残っているはずなので
  // それをRAXにロードして関数からの返り値とする
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
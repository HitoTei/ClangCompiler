#include "Ccompiler.h"

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
    Node(NodeKind kind, Node *lhs, Node *rhs){
        this->kind = kind;
        this->lhs = lhs;
        this->rhs = rhs;
    }
    Node(int val){
        this->kind = ND_NUM;
        this->val = val;
    }
};


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
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
    ND_ASSIGN, // =
    ND_RETURN, // リターン
    ND_IF,     // if
    ND_WHILE,  // while
    ND_FOR,    // for
    ND_BLOCK,  // ブロック{}
    ND_FUNCALL,// 関数
    ND_LVAR    // ローカル変数
};

// 抽象構文木のノードの型
struct Node
{
    NodeKind kind; // ノードの型
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
  
    // "if" statement
    Node *cond;
    Node *then;
    Node *els = NULL;
    
    // "for"
    Node *init;
    Node *inc;

    // "block"
    Node *body;
    Node *next;

    // "funcall"
    std::string funcname;

    int val;       // kindがND_NUMの場合のみ使う
    int offset;    // kindがND_LVARの場合のみ使う
    
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

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR){
    error("代入の左辺値が変数ではありません");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  
  static int labelseq = 1;
  switch (node->kind) // 予約語
  {
   case ND_RETURN: {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }
  case ND_FUNCALL:
  printf("  call %s\n", (node->funcname).c_str());
  printf("  push rax\n");
  return;
  case ND_IF: {
    int seq = labelseq++;
    if (node->els) {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .L.else.%d\n", seq);
      gen(node->then);
      printf("  jmp .L.end.%d\n", seq);
      printf(".L.else.%d:\n", seq);
      gen(node->els);
      printf(".L.end.%d:\n", seq);
    } else {

      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .L.end.%d\n", seq);
      gen(node->then);
      printf(".L.end.%d:\n", seq);
    }
    return;
  }
  case ND_WHILE:{
    int seq = labelseq++;
    printf(".L.begin.%d:\n", seq);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .L.end.%d\n", seq);
    gen(node->then);
    printf("  jmp .L.begin.%d\n", seq);
    printf(".L.end.%d:\n", seq);
    return;
  }
  case ND_FOR:{
    int seq = labelseq++;
    if (node->init)
      gen(node->init);
    printf(".L.begin.%d:\n", seq);
    if (node->cond) {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .L.end.%d\n", seq);
    }
    gen(node->then);
    if (node->inc)
      gen(node->inc);
    printf("  jmp .L.begin.%d\n", seq);
    printf(".L.end.%d:\n", seq);
    return;
  }
  case ND_BLOCK: {
    for(Node *n = node->body;n;n = n->next)gen(n);
    return;
  }
  }

  switch (node->kind) { // 数字や変数や代入
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  // 演算
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

void  program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *code[100];

void program(){
  int i = 0;
  while(!at_eof())code[i++] = stmt();
  code[i] = NULL;
}


Node *stmt(){
  Node *node;
  if(consume("{")){
    Node head = {};
    Node *cur = &head;
    while(!consume("}")){
      cur->next = stmt();
      cur = cur->next; // next -> lhs
    }

    Node *node = new Node();
    node->kind = ND_BLOCK;
    node->body = head.next;

    return node;
  }



  if(consume("return")){
    node = new Node();
    node->kind = ND_RETURN;
    node->lhs  = expr();
  }else if(consume("if")){
    node = new Node();
    node->kind = ND_IF;
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if(consume("else"))
      node->els = stmt();
    return node;
  }else if(consume("while")){
    node = new Node();
    node->kind = ND_WHILE;
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  }else if(consume("for")){
    node = new Node();
    node ->kind = ND_FOR;
    expect("(");
    if(!consume(";")){
      node->init = stmt();
    }
    if(!consume(";")){
      node->cond = expr();
      expect(";");
    }
    if(!consume(")")){
      node->inc = expr();
      expect(")");
    }
    node->then = stmt();
    return node;
  }else{
    node = expr();
  }
  expect(";");

  return node;
}

Node *assign(){
  Node *node = equality();
  if(consume("="))
    node = new Node(ND_ASSIGN,node,assign());
  return node;
}

Node *expr() {
  return assign();
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

#include <map>
using std::map;
using std::string;
map<string,int> locals; // ローカル変数<名前,RBPからのオフセット>


/* primary  = num 
            | ident ("(" ")")? 
            | "(" expr ")"
*/
Node *primary() {

  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }


  Token *tok = consume_ident();
  if (tok) {
  
    string str = string(tok->str);
    str = str.substr(0,tok->len);

    token = tok->next;
    if(consume("(")){

      expect(")");
      Node *node = new Node();
      node->kind = ND_FUNCALL;


      node->funcname = str;
      return node;
    }



    Node *node = new Node();
    node->kind = ND_LVAR;
    
    int lvar = locals[str];

    if(lvar != 0){
      node->offset = lvar;
    }else{
      int offset = (locals.size()+1) * 8;
      node->offset = offset;
      locals[str]  = offset;
    }

    return node;
  }

  // そうでなければ数値のはず
  return new Node(expect_number());
}

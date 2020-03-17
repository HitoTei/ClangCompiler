#include "Ccompiler.h"


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

bool at_eof(){
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

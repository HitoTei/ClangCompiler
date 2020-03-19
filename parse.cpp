#include "Ccompiler.h"
#include <string>
#include <map>

// 新しいトークンを作成してcurに繋げる
Token::Token(TokenKind kind, Token *cur, char *str,int len)
{
    this->kind = kind;
    this->str = str;
    cur->next = this;
    this->len = len;
}
Token::Token(){};



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
void error(const char *str,...){
    fprintf(stderr, "%s\n",str);
}


// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(const char *op)
{
    if (!(token->kind == TK_RESERVED || token->kind == TK_WORD)||
        strlen(op) != token->len ||
        memcmp(token->str,op,token->len))
        return false;

    token = token->next;
    return true;
}

bool consume_same_kind(TokenKind kind){
    if(kind == token->kind){
        // token = token->next;
        return true;
    }
    return false;
}

Token *consume_ident(){
    if(token->kind == TK_IDENT)
        return token;
    else return NULL;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(const char *op)
{
    if (token->kind != TK_RESERVED || 
        token->len != strlen(op) ||
        memcmp(token->str,op,token->len)){
        char str[100];
        strncpy(str,token->str,token->len);
        str[token->len] = '\0';
        error(token->str, "'%s'は'%s'ではありません", str, op);
    }
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

// pとqが同じ文字列か
bool startswith(const char*p, const char *q){
  return !memcmp(p,q,strlen(q));
}

bool canBeVariable(char p){
    return 'A' <= p && p <= 'z' || p == '_';
}

// 予約語ならそれを返す。そうでないならNULLを返す
const char* starts_with_word(const char *p){
    constexpr char const *const reserved[] = {"return","if","else"};
    for(auto&& key : reserved){
        int len = strlen(key);
        if(startswith(p,key) && !canBeVariable(p[len]))
            return key;
    }
    return NULL;
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

        if (strchr("+-*/()<>;=",*p))
        {
            cur = new Token(TK_RESERVED, cur, p++,1);
            continue;
        }

        if(const char *str = starts_with_word(p)){
            int len = strlen(str);
            cur = new Token(TK_WORD,cur,p,len);
            p += len;
            continue;
        }

        if(canBeVariable(*p)){
            int cnt = 0;
            while(canBeVariable(p[cnt]) || isdigit(*(p+cnt)))cnt++;
            cur = new Token(TK_IDENT,cur,p,cnt);
            p += cnt;
            cur->len = 1;
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

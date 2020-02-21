#include <iostream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <vector>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#define rep(i,n) for(int (i) = 0;(i) < (n);++(i))
#define all(v) v.begin(),v.end()

using namespace std;

void error(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}


enum TokenKind{
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
};

class Token{
    public:

    TokenKind kind;
    int val;
    string str; 

    Token(TokenKind kind,string str){
        this->kind = kind;
        this->str  = str;
    }

    bool consume(char op){
        return (this->kind == TK_RESERVED && str[0] == op);
    }
    
    void expect(char op){
        if(this->kind != TK_RESERVED || str[0] != op)
            error("'%c'ではありません",op);
    }

    int expect_number(){
        if(this->kind != TK_NUM)error("数ではありません");
        return this->val;
    }

    static vector<Token> tokenize(string str){
        vector<Token> tokens;
        rep(i,str.size()){
            char c = str.at(i);
            
            if(isspace(c))continue;
            if(c == '+' || c == '-'){
                tokens.push_back(Token(TK_RESERVED,((string)""+c)));
            }else if(isdigit(c)){
                int size = 0;
                while(i + size < str.size() && isdigit(str.at(i+size)))size++;                
                string num = str.substr(i,size);

                auto token = Token(TK_NUM,num);
                token.val = stoi(num);
                tokens.push_back(token);
                i += size-1;
            }else{
                error("トークナイズできません");
            }
        }
        return tokens;
    }
};

// Token *token;
int main(int argc,char **argv){

    if(argc != 2){
        fprintf(stderr,"error\n");
        return 1;
    }

    cout<<(".intel_syntax noprefix\n");
    cout<<(".global main\n");
    cout<<("main:\n");
    
    auto tokens = Token::tokenize(argv[1]);
    int now = 0;
    cout<<"  mov rax, "<<tokens[now++].expect_number()<<endl;

    for(int i = 1;i < tokens.size();++i){
        if(tokens[i].consume('+')){
            cout<<"  add rax, "<<tokens[++i].expect_number()<<endl;
        }else{
            tokens[i].expect('-');
            cout<<"  sub rax, "<<tokens[++i].expect_number()<<endl;
        }
    }

    cout<<("  ret\n");
    return 0;
}
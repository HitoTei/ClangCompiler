#include <iostream>
#include <algorithm>
#include <string>
#include <cstdlib>

#define rep(i,n) for(int (i) = 0;(i) < (n);++(i))
#define all(v) v.begin(),v.end()


using namespace std;
int main(int argc,char **argv){
    if(argc != 2){
        fprintf(stderr,"error\n");
        return 1;
    }

    cout<<(".intel_syntax noprefix\n");
    cout<<(".global main\n");
    cout<<("main:\n");
    cout<<"  mov rax," <<atoi(argv[1])<<"\n";
    cout<<("  ret\n");
    return 0;
}
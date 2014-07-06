
/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
------------------------------------------------------------------------------
错误恢复的范围是向前向后看一个字符，所以能力有限。除非遇到可能发生混乱的
情况，程序持续分析直到文件结束
（需要标识符时找不到，如果在向后看一个字符找到了，就不会出错；
如果没有定义，可以恢复定义）
============================================================================= */

#include "lex.h"
#include "code.h"

/** 调试信息输出和写入文件 ---------------------------------------------------------------- */

#include <windows.h>
#define COLOR_LEX   FOREGROUND_INTENSITY|FOREGROUND_GREEN
#define COLOR_PARSE FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE
#define COLOR_CODE  FOREGROUND_INTENSITY|FOREGROUND_BLUE

static int pos=0;
#define _MOVE_POS                    do{int p=pos;while(p){printf(" ");p--;}}while(0)
#define _MOVE_NEXT_POS               do{_MOVE_POS;pos++;}while(0)
#define _MOVE_BACK_POS               do{pos--;_MOVE_POS;}while(0)

static FILE* lexFile = NULL;
static FILE* parseFile = NULL;
static FILE* codeFile = NULL;
#define Out_LexToken(tk)             do{Format_setColor(COLOR_LEX);Lex_dispToken(tk);Lex_writeToken(tk,lexFile);}while(0) // _MOVE_POS;
#define Out_parseBeginMark()         do{Format_setColor(COLOR_PARSE);_MOVE_NEXT_POS;printf("<%s>{\n",__FUNCTION__);fprintf(parseFile,"<%s>{\n",__FUNCTION__);}while(0)
#define Out_parseEndMark()           do{Format_setColor(COLOR_PARSE);_MOVE_BACK_POS;printf("<%s>}\n",__FUNCTION__);fprintf(parseFile,"<%s>}\n",__FUNCTION__);}while(0)

static int nextStat = 0;
#define Out_CodeEmit(x1,x2,x3,x4)    do{Format_setColor(COLOR_CODE);printf("%04d (%8s %8s %8s %8s)\n",nextStat,x1,x2,x3,x4);fprintf(codeFile,"%04d (%8s %8s %8s %8s)\n",nextStat,x1,x2,x3,x4);nextStat++;}while(0)
#define Out_SetLabel(label)          do{Format_setColor(COLOR_CODE);printf("%-4s:\n",label);fprintf(codeFile,"%-4s:\n",label);}while(0)


/** 内部变量声明 ---------------------------------------------------------------- */

static Token* g_lastToken=NULL;// 需要保存上一个token
static Token* g_thisToken=NULL;
static Token* g_nextToken=NULL;
// 再定义一个g_thisToken 比较方便
#define g_sym   (g_thisToken->type)
#define g_value (g_thisToken->value)
//static sym_e g_sym;
//static char* g_value;

/** 内部函数声明 ---------------------------------------------------------------- */
static void getNextSym();
static void P();
static void D();
static void S();
static void L();
#ifndef REVISE
static void Lprime();
#endif

static char* B();
static char* Tprime();
static char* Fprime();

static char* E();
static char* T();
static char* F();


/** 错误处理 ---------------------------------------------------------------- */
static FILE* errFile = NULL;

// 非法字符直接在getSym时跳过，不再在这里处理_Error_print("UNDEF CHAR: %s",g_value);}
#define _Error_print(FORMAT,...)        do{Format_setColor(COLOR_ERROR);printf("[%04d] " FORMAT "\n",curLine,##__VA_ARGS__);fprintf(errFile,"[%04d] " FORMAT "\n",curLine,##__VA_ARGS__);}while(0)
#define Error_parseFail()               do{_Error_print("Fail in parsing '%s'.Fatal error, analysis stoped.",__FUNCTION__);exit(-1);}while(0)
#define Error_notEof()                  _Error_print("Expect EOF.")

static int tokenBackNum = 0;
static void Error_expect(sym_e sym){
Token* lastToken = NULL;
// 保存现场环境
    lastToken  = g_lastToken;

    if(g_sym!=sym){
        getNextSym();
        if(g_sym == sym){// 多了一个字符
            _Error_print("I am quite sure you make a mistake, so I repair it.");
        }
        else { // 丢了 mising
            if( sym == TK_IDENT ){
                _Error_print("Expect '%s'. after '%s'.",tokenName[(int)sym],tokenName[g_lastToken->type]);
                _Error_print("Fatal error, analysis stoped."); // 要求是标识符时，找不到标识符为致命错误
                exit(0); //exit(-1);
            }
            // 还要确保getNextToken弹回。backNextToken
            tokenBackNum = 2;
            // 恢复现场环境
            g_nextToken = g_thisToken;
            g_thisToken = g_lastToken;
            g_lastToken = lastToken;
            _Error_print("Missing '%s' have been fixed.",tokenName[(int)sym]);
        }
    }
    // 并不终止程序，继续执行
}
static void Error_ifIdentNotDefined(char* id){
    // 进行定义工作
    if(symTab_lookup(id) == NULL){
        _Error_print("not defined IDENT symbol: %s",id);
        if(id!=NULL){
            symTab_insert(TK_INT,id);
        }
        else {
            _Error_print("Fatal error, analysis stoped."); // 需要是标识符才行
            exit(-1);
        }
    }
}


/** 内部函数定义 ---------------------------------------------------------------- */
static void getNextSym(){
// 未定义的token不会影响程序
// 进入下一个token 打印上一个token
    ASSERT(g_lastToken!=NULL);   // 初始化中读入了一个token

    if(tokenBackNum == 2 ){
        tokenBackNum --;
    }
    else if(tokenBackNum == 1){
        tokenBackNum --;
        g_lastToken = g_thisToken;
        g_thisToken = g_nextToken;
    }
    else{
        g_lastToken = g_thisToken;
        g_thisToken = Lex_getNextToken();
        /** 非法字符处理 ---------------------------------- */
        while(g_thisToken->type == TK_UNDEF){
            _Error_print("I found a UNDEF char: '%s'",g_thisToken->value);
            Out_LexToken(g_thisToken);
            g_thisToken = Lex_getNextToken();
        }
        /** ------------------------------------------------ */
        Out_LexToken(g_lastToken);
    }
}

void Gen_run(char* filename){
    Lex_selectFile(filename);

    // 先读入一个字符,给g_sym，g_value,tk赋予初值
    g_thisToken = g_lastToken = Lex_getNextToken();

    lexFile = fopen("lex.txt","w");
    parseFile = fopen("parse.txt","w");
    codeFile = fopen("code.txt","w");
    errFile = fopen("err.txt","w");

    P();

    while(g_sym!=TK_EOF){
        Error_notEof();
        P();
    }
}

// P →{DS}
void P(){
    Out_parseBeginMark();
    Error_expect(TK_LBRACE);
    getNextSym();
    D();
    S();
    Error_expect(TK_RBRACE);
    getNextSym();
    Out_parseEndMark();
}

//D →int ID ;{int ID;}  D->int ID ; D' D'->D|null
// 声明语句的翻译
void D(){
    Out_parseBeginMark();
    do{
        Error_expect( TK_INT );
        getNextSym();
            Error_expect( TK_IDENT );
            symTab_insert(TK_INT,g_value);
            getNextSym();
                Error_expect( TK_SEMICOLON );
                getNextSym();
    }while(  g_sym == TK_INT/*in First(D)*/ );
    Out_parseEndMark();
}

/**
S→if (B) then S [else S ] | while (B) do S | { L } | ID=E
       | write (E); | read ID;
*/
void S(){
char* label1 =NULL;
char* label2 =NULL;
char* place =NULL;
    Out_parseBeginMark();
    if( g_sym == TK_IF ){
        getNextSym();
        Error_expect( TK_LPAREN );
            getNextSym();
            place = B();
            Error_expect( TK_RPAREN );
                getNextSym();

                label1=getLabel();
                Out_CodeEmit("jz",place,"_",label1);

                Error_expect( TK_THEN );
                    getNextSym();
                    S();

                    label2=getLabel();
                    Out_CodeEmit("jp","_","_",label2);
                    Out_SetLabel(label1);

                    if( g_sym == TK_ELSE ){
                        getNextSym();
                        S();
                        Out_SetLabel(label2);
                    }
                    else { /*null*/ }
    }
    else if( g_sym == TK_WHILE ){
        label1 = getLabel();
        Out_SetLabel(label1);

        getNextSym();
        Error_expect( TK_LPAREN );
            getNextSym();
            place = B();
            Error_expect( TK_RPAREN );

                label2 = getLabel();
                Out_CodeEmit("jz",place,"_",label2);

                getNextSym();
                Error_expect( TK_DO );
                    getNextSym();
                    S();

                    Out_CodeEmit("jp","_","_",label1);
                    Out_SetLabel(label2);
    }
    else if( g_sym == TK_LBRACE ){
        getNextSym();
        L();
        Error_expect( TK_RBRACE );
            getNextSym();
    }
    /* ID=E ------------------------------- */
    else if( g_sym == TK_IDENT ){
    char* n;
        n = g_value;
        getNextSym();
        Error_expect( TK_ASSIGN );
        char* x = NULL;
            getNextSym();
            x = E();
#ifdef REVISE
            Error_expect(TK_SEMICOLON );
                getNextSym();
#endif // REVISE
            Error_ifIdentNotDefined(n);
            Out_CodeEmit(":=",x,"_",n);
    }
    /* write <算术表达式>; ----------------- */
    else if( g_sym == TK_WRITE ){
    char* place=NULL;
        getNextSym();
        Error_expect( TK_LPAREN );
            getNextSym();
            place = E();
            Out_CodeEmit("out",place,"_","std::out");
            Error_expect( TK_RPAREN );
                getNextSym();
                Error_expect( TK_SEMICOLON );
                    getNextSym();

    }
    /* read ID; ---------------------------- */
    else if( g_sym == TK_READ ){
        getNextSym();
        Error_expect(TK_IDENT);
            Error_ifIdentNotDefined(g_value);
            Out_CodeEmit("in","std::in","_",g_value);
            getNextSym();
            Error_expect( TK_SEMICOLON );
                getNextSym();
    }
    else { Error_parseFail(); }
    Out_parseEndMark();
}
// L→SL’

void L(){
    Out_parseBeginMark();

#ifdef REVISE
    while( g_sym == TK_IF || g_sym == TK_WHILE || g_sym == TK_IDENT || g_sym == TK_WRITE || g_sym == TK_READ ){
        S();
    }
#else
    S();
    Lprime();
#endif // REVISE
    Out_parseEndMark();
}

#ifndef REVISE
// L’ →; L | null
void Lprime(){
    Out_parseBeginMark();
    if( g_sym == TK_SEMICOLON ){
        getNextSym();
        L();
    }
    else { /*null*/ }
    Out_parseEndMark();
}
#endif
// B→T’ { |T’} 可出现多次的话可采用while
// 这种方式比较麻烦 B→ T' || B | T'

// 布尔表达式的翻译

char* B(){
char* x =NULL;
char* q =NULL;
char* r =NULL;
    Out_parseBeginMark();
    x = q = Tprime();
    while( g_sym == TK_LOG_OR ){
        getNextSym();
        r = Tprime();
        x = newTemp();
        Out_CodeEmit("or",q,r,x);
    }
    Out_parseEndMark();
    return x;
}
// T’ →F’ { & F’ }
char* Tprime(){
char* x =NULL;
char* q =NULL;
char* r =NULL;
    Out_parseBeginMark();
    x = q = Fprime();
     while( g_sym == TK_LOG_AND ){
        getNextSym();
        r = Fprime();
        x = newTemp();
        Out_CodeEmit("and",q,r,x);
    }
    Out_parseEndMark();
    return x;
}
// F’ →ID relop ID | ID
char* Fprime(){
char* x =NULL;
char* n =NULL;
char* q =NULL;
    Out_parseBeginMark();
    Error_expect( TK_IDENT );
        n = g_value;
        Error_ifIdentNotDefined(g_value);
        x = newTemp();
        Out_CodeEmit("j!=",n,"0",itos(nextStat+3));
        Out_CodeEmit(":=","0","_",x);
        Out_CodeEmit("jp","_","_",itos(nextStat+2));
        Out_CodeEmit(":=","1","_",x);

        getNextSym();
        if ( g_sym == TK_RELOP ){
        char* m = g_value;
            getNextSym();
            Error_expect( TK_IDENT );
                q = g_value;
                Error_ifIdentNotDefined(g_value);
                x = newTemp();
                char s[4] = "";
                s[0]='j';
                strcpy(s+1,m);
                Out_CodeEmit(s,n,q,itos(nextStat+3));
                Out_CodeEmit(":=","0","_",x);
                Out_CodeEmit("jp","_","_",itos(nextStat+2));
                Out_CodeEmit(":=","1","_",x);

                getNextSym();
        }
        else { /*null*/ }
    Out_parseEndMark();
    return x;
}
// E→T{+T| -T}
char* E(){
char* q =NULL;
char* r =NULL;
char* x =NULL;
    Out_parseBeginMark();
     x = q = T(); // 这里x =也是处理单个项的情况
     while( 1 ){
        if( g_sym == TK_ADD ){
            getNextSym();
            r = T();
            x = newTemp();
            Out_CodeEmit("+",q,r,x);
        }
        else if( g_sym == TK_SUB ){
            getNextSym();
            r = T();
            x = newTemp();
            Out_CodeEmit("-",q,r,x);
        }
        else {/*null*/ break; }
    }
    Out_parseEndMark();
    return x;
}
//T→F{ * F | /F }
char* T(){
char* q =NULL;
char* r =NULL;
char* x =NULL;
    Out_parseBeginMark();
     x = q = F(); // 注意这里 x =  需要给x赋值，防止单独因子的情况 T->F
     while( 1 ){
        if( g_sym == TK_MUL ){
            getNextSym();
            r = F();
            x = newTemp();
            Out_CodeEmit("*",q,r,x);
        }
        else if( g_sym == TK_DIV ){
            getNextSym();
            r = F();
            x = newTemp();
            Out_CodeEmit("/",q,r,x);
        }
        else {/*null*/break;}
    }
    Out_parseEndMark();
    return x;
}
// F→ (E) | NUM | ID
// 在语法分析的基础上，每个识别过程添加一个返回值，用来传递属性
char* F(){
char* x =NULL;
    Out_parseBeginMark();
    if( g_sym == TK_LPAREN ){
        getNextSym();
        char* q = E();
        Error_expect( TK_RPAREN );
            getNextSym();
            x = q;
    }
    else if( g_sym == TK_NUM ){
    char* lexval = g_value;
    //ConstSym* p =
        constTab_lookInt(lexval);
        x = lexval;
        getNextSym();
    }
    else if( g_sym == TK_IDENT ){
    char* n = g_value;
        Error_ifIdentNotDefined(g_value);
        x = n ;
        getNextSym();
    }
    else { Error_parseFail(); }
    Out_parseEndMark();
    return x;
}



/**
#ifdef TEST
void boolExpression_test();
void simpleAssignStatement_test();
void ifwhileStatement_test();
#endif
void boolExpression_test(){
}

void simpleAssignStatement_test(){
}
//文件打开失败的原因，隐藏后缀名了！！ example2.h.h
first follow

FirstA[] = {} ;
isElemInGroup(SYM,FirstA)
bool isElemInGroup(char elem, char Group[]){
}
Expect follow

计算first follow集

数字也是按字符串存储的，最后转换为数字，不建议
数字常量为int型，没有大数
由于各个过程返回都是char* 因此不适合存储数字
可以采用公用体，但比较麻烦
返回的为地址

注意返回值为字符串 是变量名
*/

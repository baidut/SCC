
/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
------------------------------------------------------------------------------
����ָ��ķ�Χ����ǰ���һ���ַ��������������ޡ������������ܷ������ҵ�
����������������ֱ���ļ�����
����Ҫ��ʶ��ʱ�Ҳ�������������һ���ַ��ҵ��ˣ��Ͳ������
���û�ж��壬���Իָ����壩
============================================================================= */

#include "lex.h"
#include "code.h"

/** ������Ϣ�����д���ļ� ---------------------------------------------------------------- */

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


/** �ڲ��������� ---------------------------------------------------------------- */

static Token* g_lastToken=NULL;// ��Ҫ������һ��token
static Token* g_thisToken=NULL;
static Token* g_nextToken=NULL;
// �ٶ���һ��g_thisToken �ȽϷ���
#define g_sym   (g_thisToken->type)
#define g_value (g_thisToken->value)
//static sym_e g_sym;
//static char* g_value;

/** �ڲ��������� ---------------------------------------------------------------- */
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


/** ������ ---------------------------------------------------------------- */
static FILE* errFile = NULL;

// �Ƿ��ַ�ֱ����getSymʱ���������������ﴦ��_Error_print("UNDEF CHAR: %s",g_value);}
#define _Error_print(FORMAT,...)        do{Format_setColor(COLOR_ERROR);printf("[%04d] " FORMAT "\n",curLine,##__VA_ARGS__);fprintf(errFile,"[%04d] " FORMAT "\n",curLine,##__VA_ARGS__);}while(0)
#define Error_parseFail()               do{_Error_print("Fail in parsing '%s'.Fatal error, analysis stoped.",__FUNCTION__);exit(-1);}while(0)
#define Error_notEof()                  _Error_print("Expect EOF.")

static int tokenBackNum = 0;
static void Error_expect(sym_e sym){
Token* lastToken = NULL;
// �����ֳ�����
    lastToken  = g_lastToken;

    if(g_sym!=sym){
        getNextSym();
        if(g_sym == sym){// ����һ���ַ�
            _Error_print("I am quite sure you make a mistake, so I repair it.");
        }
        else { // ���� mising
            if( sym == TK_IDENT ){
                _Error_print("Expect '%s'. after '%s'.",tokenName[(int)sym],tokenName[g_lastToken->type]);
                _Error_print("Fatal error, analysis stoped."); // Ҫ���Ǳ�ʶ��ʱ���Ҳ�����ʶ��Ϊ��������
                exit(0); //exit(-1);
            }
            // ��Ҫȷ��getNextToken���ء�backNextToken
            tokenBackNum = 2;
            // �ָ��ֳ�����
            g_nextToken = g_thisToken;
            g_thisToken = g_lastToken;
            g_lastToken = lastToken;
            _Error_print("Missing '%s' have been fixed.",tokenName[(int)sym]);
        }
    }
    // ������ֹ���򣬼���ִ��
}
static void Error_ifIdentNotDefined(char* id){
    // ���ж��幤��
    if(symTab_lookup(id) == NULL){
        _Error_print("not defined IDENT symbol: %s",id);
        if(id!=NULL){
            symTab_insert(TK_INT,id);
        }
        else {
            _Error_print("Fatal error, analysis stoped."); // ��Ҫ�Ǳ�ʶ������
            exit(-1);
        }
    }
}


/** �ڲ��������� ---------------------------------------------------------------- */
static void getNextSym(){
// δ�����token����Ӱ�����
// ������һ��token ��ӡ��һ��token
    ASSERT(g_lastToken!=NULL);   // ��ʼ���ж�����һ��token

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
        /** �Ƿ��ַ����� ---------------------------------- */
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

    // �ȶ���һ���ַ�,��g_sym��g_value,tk�����ֵ
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

// P ��{DS}
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

//D ��int ID ;{int ID;}  D->int ID ; D' D'->D|null
// �������ķ���
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
S��if (B) then S [else S ] | while (B) do S | { L } | ID=E
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
    /* write <�������ʽ>; ----------------- */
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
// L��SL��

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
// L�� ��; L | null
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
// B��T�� { |T��} �ɳ��ֶ�εĻ��ɲ���while
// ���ַ�ʽ�Ƚ��鷳 B�� T' || B | T'

// �������ʽ�ķ���

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
// T�� ��F�� { & F�� }
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
// F�� ��ID relop ID | ID
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
// E��T{+T| -T}
char* E(){
char* q =NULL;
char* r =NULL;
char* x =NULL;
    Out_parseBeginMark();
     x = q = T(); // ����x =Ҳ�Ǵ�����������
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
//T��F{ * F | /F }
char* T(){
char* q =NULL;
char* r =NULL;
char* x =NULL;
    Out_parseBeginMark();
     x = q = F(); // ע������ x =  ��Ҫ��x��ֵ����ֹ�������ӵ���� T->F
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
// F�� (E) | NUM | ID
// ���﷨�����Ļ����ϣ�ÿ��ʶ��������һ������ֵ��������������
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
//�ļ���ʧ�ܵ�ԭ�����غ�׺���ˣ��� example2.h.h
first follow

FirstA[] = {} ;
isElemInGroup(SYM,FirstA)
bool isElemInGroup(char elem, char Group[]){
}
Expect follow

����first follow��

����Ҳ�ǰ��ַ����洢�ģ����ת��Ϊ���֣�������
���ֳ���Ϊint�ͣ�û�д���
���ڸ������̷��ض���char* ��˲��ʺϴ洢����
���Բ��ù����壬���Ƚ��鷳
���ص�Ϊ��ַ

ע�ⷵ��ֵΪ�ַ��� �Ǳ�����
*/

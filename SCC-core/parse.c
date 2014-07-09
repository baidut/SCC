// ����ֻ֧������id�ıȽ�
#include "parse.h"

#include "lex.h"

typedef tk_e sym_e;

static sym_e g_sym;
static char* g_value;

#define DEBUG

#ifdef DEBUG

static int pos=0;

#define MOVE_NEXT_POS   do{MOVE_POS;pos++;}while(0)
#define MOVE_BACK_POS   do{pos--;MOVE_POS;}while(0)
#define MOVE_POS        do{int p=pos;while(p){DOUT(" ");p--;}}while(0)

#define BEGIN       MOVE_NEXT_POS;DOUT("<%s>{\n",__FUNCTION__);
#define END         MOVE_BACK_POS;DOUT("<%s>}\n",__FUNCTION__);

#else
#define BEGIN
#define END

#endif // DEBUG

#define error(s)    printf("%s",s);CHECKPOINT();exit(1)
//inline

static Token* g_lastToken=NULL;// ��Ҫ������һ��token

static void getNextSym(){
// ������һ��token ��ӡ��һ��token
    ASSERT(g_lastToken!=NULL);   // ��ʼ���ж�����һ��token

    MOVE_POS;
    Lex_dispToken(g_lastToken); // ���������һ��token ��ӡ��һ��token

    g_lastToken = Lex_getNextToken();
    g_sym = g_lastToken -> type;
    g_value = g_lastToken -> value;
}

static void P();
static void D();
static void S();
static void L();
static void Lprime();
static void B();
static void Tprime();
static void Fprime();
static void E();
static void T();
static void F();

void Parse_test(){
//char * s;
    //scanf("%s",s);

    Lex_selectFile("example.h");

    Parse_init();
    P();
}


void Parse_init(){
// �ȶ���һ���ַ�,��g_sym��g_value,tk�����ֵ
    g_lastToken = Lex_getNextToken();
    g_sym = g_lastToken -> type;
    g_value = g_lastToken -> value;
}

// P ��{DS}
void P(){
BEGIN
    while( g_sym == TK_INT ){
        D();S();
    }
END
}

//D ��int ID ;{int ID;}  D->int ID ; D' D'->D|null
void D(){
BEGIN
    do{
        if( g_sym == TK_INT ){
            getNextSym();
            ASSERT(g_sym!=TK_INT);
            if( g_sym == TK_IDENT ){
                getNextSym();
                if( g_sym == TK_SEMICOLON ){
                    getNextSym();
                }
                else { error("");}
            }
            else { error("");}
        }
        else { error("");}
    }while(  g_sym == TK_INT/*in First(D)*/ );
END
}

/**
S��if (B) then S [else S ] | while (B) do S | { L } | ID=E
       | write <�������ʽ>; | read ID;
*/
void S(){
BEGIN
    if( g_sym == TK_IF ){
        getNextSym();
        if( g_sym == TK_LPAREN ){
            getNextSym();
            B();
            if( g_sym == TK_RPAREN ){
                getNextSym();
                if( g_sym == TK_THEN ){
                    getNextSym();
                    S();
                    if( g_sym == TK_ELSE ){
                        getNextSym();
                        S();
                    }
                    else { /*null*/ }
                }
                else { error("Expect then");}
            }
            else { error("Expect )"); }
        }
        else { error("Expect ("); }
    }
    else if( g_sym == TK_WHILE ){
        getNextSym();
        if( g_sym == TK_LPAREN ){
            getNextSym();
            B();
            if( g_sym == TK_RPAREN ){
                getNextSym();
                if( g_sym == TK_DO ){
                    getNextSym();
                    S();
                }
                else { error("Expect do");}
            }
            else { error("Expect )"); }
        }
        else { error("Expect ("); }
    }
    else if( g_sym == TK_LBRACE ){
        getNextSym();
        L();
        if ( g_sym == TK_RBRACE ){
            getNextSym();
        }
        else { error("Expect }"); }
    }
    /* ID=E ------------------------------- */
    else if( g_sym == TK_IDENT ){
        getNextSym();
        if( g_sym == TK_ASSIGN ){
            getNextSym();
            E();
#ifdef REVISE
            if( g_sym == TK_SEMICOLON ){
                getNextSym();
            }
            else { error(""); }
#endif // REVISE
        }
        else {error("");}
    }
    /* write <�������ʽ>; ----------------- */
    else if( g_sym == TK_WRITE ){
        getNextSym();
        E();
        if( g_sym == TK_SEMICOLON ){
            getNextSym();
        }
        else { error(""); }
    }
    /* read ID; ---------------------------- */
    else if( g_sym == TK_READ ){
        getNextSym();
        if( g_sym == TK_IDENT ){
            getNextSym();
            if( g_sym == TK_SEMICOLON ){
                getNextSym();
            }
            else { error(""); }
        }
        else { error(""); }
    }
    else { error(""); }
END
}
// L��SL��

void L(){
BEGIN

#ifdef REVISE
    while( g_sym == TK_IF || g_sym == TK_WHILE || g_sym == TK_IDENT || g_sym == TK_WRITE || g_sym == TK_READ ){
        S();
    }
#else
    S();
    Lprime();
#endif // REVISE
END
}
// L�� ��; L | null
void Lprime(){
BEGIN
    if( g_sym == TK_SEMICOLON ){
        getNextSym();
        L();
    }
    else { /*null*/ }
END
}
// B��T�� { |T��} �ɳ��ֶ�εĻ��ɲ���while
// ���ַ�ʽ�Ƚ��鷳 B�� T' || B | T'

// �������ʽ�ķ���

void B(){
BEGIN
    Tprime();
    while( g_sym == TK_LOG_OR ){
        getNextSym();
        Tprime();
    }
END
}
// T�� ��F�� { & F�� }
void Tprime(){
BEGIN
    Fprime();
     while( g_sym == TK_LOG_AND ){
        getNextSym();
        Fprime();
    }
END
}
// F�� ��ID relop ID | ID
void Fprime(){
BEGIN
    if( g_sym == TK_IDENT ){
        getNextSym();
        if ( g_sym == TK_RELOP ){
            getNextSym();
            if ( g_sym == TK_IDENT ){
                getNextSym();
            }
            else { error(""); }
        }
        else { /*null*/ }
    }
    else { error(""); }
END
}
// E��T{+T| -T}
void E(){
BEGIN
    T();
     while( 1 ){
        if( g_sym == TK_ADD ){
            getNextSym();
            T();
        }
        else if( g_sym == TK_SUB ){
            getNextSym();
            T();
        }
        else {/*null*/ break; }
    }
END
}
//T��F{ * F | /F }
void T(){
BEGIN
    F();
     while( 1 ){
        if( g_sym == TK_MUL ){
            getNextSym();
            F();
        }
        else if( g_sym == TK_DIV ){
            getNextSym();
            F();
        }
        else {/*null*/break;}
    }
END
}
// F�� (E) | NUM | ID
// ���﷨�����Ļ����ϣ�ÿ��ʶ��������һ������ֵ��������������
void F(){
BEGIN
    if( g_sym == TK_LPAREN ){
        getNextSym();
        E();
        if( g_sym == TK_RPAREN ){
            getNextSym();
        }
        else { error(""); }
    }
    else if( g_sym == TK_NUM ){
        getNextSym();
    }
    else if( g_sym == TK_IDENT ){
        getNextSym();
    }
    else { error(""); }
END
}

/** first follow

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

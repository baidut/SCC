
/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
------------------------------------------------------------------------------
������
Lex_run("src.c","src.o");
�������
��ʼ��ѡ��������ļ�Lex_selectFile("src.c");
ȡ��һ������tk = Lex_getNextToken(); ����ļ������򷵻�TK_EOF
�������ĻLex_dispToken(tk);
���д���ļ�Lex_writeToken(tk,fout);
============================================================================= */

#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

#include "common.h"

typedef struct {
  tk_e  type;
  char* value;
}Token;

extern const char* keywords[];
extern const char* tokenName[];

extern int curLine;

void    Lex_run(const char* inFilename,const char* outFilename);

void    Lex_selectFile(const char* filename);
Token*  Lex_getNextToken();
void    Lex_dispToken(Token* tk);
void    Lex_writeToken(Token* tk,FILE* fout);


#endif // LEX_H_INCLUDED

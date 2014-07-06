
/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
------------------------------------------------------------------------------
多遍编译
Lex_run("src.c","src.o");
单遍编译
初始化选择分析的文件Lex_selectFile("src.c");
取下一个单词tk = Lex_getNextToken(); 如果文件结束则返回TK_EOF
输出到屏幕Lex_dispToken(tk);
输出写入文件Lex_writeToken(tk,fout);
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

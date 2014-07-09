
/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
------------------------------------------------------------------------------
BUG1 解决< > 识别成UNDEF的错误
============================================================================= */

#include "lex.h"

FILE* fin = NULL;
FILE* fout = NULL;
char* filename = NULL;

void Lex_selectFile(const char* f){
  if(fin!=NULL)fclose(fin);

  fin=fopen(f,"rt");

  if(fin==NULL)printf("error in fopen %s\n",f);
  ASSERT(fin!=NULL);

  filename = (char*)f;
  curLine = 1;
}

char currentChar = ' ';// 空格会继续读入 由于存储当前指向的位置，所以必须为全局量
char* pString = NULL;// 采用fscanf可以自动过滤空格，但是不能计量行号，故不采用

#define CURRENT_CHAR      (currentChar)
#define MOVE_NEXT_CHAR    if( EOF == (currentChar=fgetc(fin)) ){currentChar = EOF;}

#define isSpace(c)        (c == '\t' || c == ' ' || c == '\n')
#define isDigit(c)        (c >= '0' && c <= '9')
#define isAlpha(c)        ((c >= 'a' && c <= 'z') || (c == '_') || (c >= 'A' && c <= 'Z'))
#define isRelop(c)        (c == '!' || c == '<' || c == '>' )

#define error()           return tk/*exit(1)*/

const char* keywords[]={
#define TOKEN(k, s) s,
#include "token_keywords.h"
#undef  TOKEN
};
#define KEYWORDS_NUM sizeof(keywords)/sizeof(keywords[0])

const char* tokenName[]={
#define TOKEN(k, s) s,
#include "token.h"
#undef  TOKEN
};

int curLine = 0;

#define MAX_STR_LEN       40 // 支持的最长的存储值的字符串
#define NEW_STRING        pString = (char*) malloc ( MAX_STR_LEN * sizeof(char))
#define APPEND_CHAR(c)    *pString = c; pString++;

// 设已经弹入一个字符，CURRENT_CHAR指向当前待分析的字符（由于可能需要向后看一个字符）
// 试探下一个字符，若是自己的一部分，则读入下一个字符再返回，否则直接返回
Token* Lex_getNextToken(){
  Token* tk = (Token*) malloc (sizeof(Token));
  tk -> type = TK_UNDEF;
  tk ->value = NULL;

  while(isSpace(CURRENT_CHAR)){
    if(CURRENT_CHAR == '\n')curLine++;
    MOVE_NEXT_CHAR;
  }
  if(CURRENT_CHAR == EOF ){ // 注意先跳过格式字符再看是否EOF，否则文件最后的空格会识别错误
    tk -> type = TK_EOF;
    // 删除：curLine=0; 原因：造成最后一个错误报告行为0
    return tk;
  }
  if(isAlpha(CURRENT_CHAR)){
  int i = 0;

    tk -> value = NEW_STRING;
    while(isAlpha(CURRENT_CHAR)||isDigit(CURRENT_CHAR)){
      APPEND_CHAR(CURRENT_CHAR);
      MOVE_NEXT_CHAR;
    }
    APPEND_CHAR('\0');
    // 保留字和标志符的识别
    for(i=0;i< KEYWORDS_NUM ;i++){
      if(0==strcmp(tk -> value ,keywords[i])){
        tk -> type =  i; //(tk_e)
        free(tk -> value);// 如果是关键字，就释放存储值的空间
        return tk;
      }
    }
    tk -> type = TK_IDENT;
    return tk;
  }
  else if(isDigit(CURRENT_CHAR)){ // 无符号整数识别
    tk -> value = NEW_STRING;
    while(isDigit(CURRENT_CHAR)){
      APPEND_CHAR(CURRENT_CHAR);
      MOVE_NEXT_CHAR;
    }
    APPEND_CHAR('\0');
    tk -> type = TK_NUM;
    return tk;
  }
  else if(isRelop(CURRENT_CHAR)){
    tk -> value = NEW_STRING;
    APPEND_CHAR(CURRENT_CHAR);
    MOVE_NEXT_CHAR;
    if('=' == CURRENT_CHAR){
      if(CURRENT_CHAR == '!'){
        tk -> type = TK_UNDEF;
      }
      else {
        APPEND_CHAR(CURRENT_CHAR);
        MOVE_NEXT_CHAR;
        tk -> type = TK_RELOP;
      }
    }
    else {
        tk -> type = TK_RELOP; // 2014-7-6 Fixed BUG1
    }
    APPEND_CHAR('\0');
    return tk;
  }
  else if( '=' == CURRENT_CHAR ){
    tk -> value = NEW_STRING;
    APPEND_CHAR(CURRENT_CHAR);
    MOVE_NEXT_CHAR;
    if('=' == CURRENT_CHAR){
      APPEND_CHAR(CURRENT_CHAR);
      MOVE_NEXT_CHAR;
      tk -> type = TK_RELOP;
    }
    else{
      tk -> type = TK_ASSIGN;
    }
    APPEND_CHAR('\0');
    return tk;
  }
  else {
    switch(CURRENT_CHAR){
      case '+':tk -> type = TK_ADD;break;
      case '-':tk -> type = TK_SUB;break;
      case '*':tk -> type = TK_MUL;break;
      case '/':tk -> type = TK_DIV;break;
      case '{':tk -> type = TK_LBRACE;break;
      case '}':tk -> type = TK_RBRACE;break;
      case '(':tk -> type = TK_LPAREN;break;
      case ')':tk -> type = TK_RPAREN;break;
      case ';':tk -> type = TK_SEMICOLON;break;
      case '|':tk -> type = TK_LOG_OR;break;
      case '&':tk -> type = TK_LOG_AND;break;
      default:
        tk -> type = TK_UNDEF;
        tk -> value = NEW_STRING;
        APPEND_CHAR(CURRENT_CHAR);
        APPEND_CHAR('\0');
        break;
    }
    MOVE_NEXT_CHAR;
    return tk;
  }
}
#define OUT_TOKEN(tk)                                       \
do{															\
	_OUT("%10s",tokenName[(unsigned int)(tk->type)]);		\
	_OUT(" ");                                              \
	switch(tk->type){                                       \
		case TK_IDENT:                                      \
		case TK_RELOP:                                      \
		case TK_NUM:                                        \
		case TK_UNDEF:                                      \
		_OUT("\"%s\"",tk->value);                           \
		break;                                              \
		default:                                            \
		_OUT("_");break;                                    \
	}                                                       \
	_OUT("\n");                                             \
}while(0)
// 用tab不整齐，改为空格间隔
void Lex_dispToken(Token* tk){
#define _OUT(FORMAT,...) printf(FORMAT,##__VA_ARGS__)
  OUT_TOKEN(tk);
#undef _OUT
}
void Lex_writeToken(Token* tk,FILE* fout){
#define _OUT(FORMAT,...) fprintf(fout,FORMAT,##__VA_ARGS__)
  OUT_TOKEN(tk);
#undef _OUT
}
void Lex_run(const char* inFilename,const char* outFilename){
Token* tk;

    Lex_selectFile(inFilename);
    fout = fopen( outFilename ,"w");
    ASSERT(fout!=NULL);
    do{
        tk = Lex_getNextToken();
        Lex_dispToken(tk);
        Lex_writeToken(tk,fout);
    }while(tk->type!=TK_EOF);
    fclose(fout);
}

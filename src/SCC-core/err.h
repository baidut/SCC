#ifndef ERR_H_INCLUDED
#define ERR_H_INCLUDED

// 2014-7-9 建议将错误处理分离到一个处理模块下，现在是全局可以访问，不利于维护
// 建议warning和error都列举出来，建立错误编号!
#define _Error_print(FORMAT,...)        do{Format_setColor(COLOR_ERROR);printf("[%04d] " FORMAT "\n",curLine,##__VA_ARGS__);fprintf(errFile,"[%04d] " FORMAT "\n",curLine,##__VA_ARGS__);}while(0)
extern int curLine;
extern FILE* errFile;

#endif // ERR_H_INCLUDED

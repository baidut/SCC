
#include "debug.h"

#ifndef NO_CFG

#include <windows.h>

// 输出配置可以同时输出到屏幕和文件
// 文件fout指针的问题 需要提前定义好
// 输出函数不是printf
// 初始化一个输出

// out 按照当前输出配置进行处理

#define OUT_TO_FILE     0x01
#define OUT_TO_SCREEN   0x02

void Out_setOutFile(FILE* fout);
void Out_setTarget();
void Out_setColor();

// 每次都要切换颜色，文件指针，太麻烦了。。。。
// 如果只输入到屏幕，直接printf就行了，如果都要输出，就用OUT(OUT2FILE|OUT2SCREEN)
// 配置结构体把问题弄复杂了，直接提供fout
// 可以同时输出到文件和屏幕



bool isOut2File(){
    return ;
}
bool isOut2Screen(){
    return ;
}

void DEBUG_Out(){
    switch(OUT_POS)
}

void CFG_OUT(DEBUG_CFG_e cfg){
    switch(cfg){
    case CFG_ASSERT:

    case CFG_HINT,
        CFG_CHECKPOINT,
        CFG_WATCH,
        CFG_TRACK,
        CFG_LOG
    }
}
#endif

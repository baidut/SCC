
#include "debug.h"

#ifndef NO_CFG

#include <windows.h>

// ������ÿ���ͬʱ�������Ļ���ļ�
// �ļ�foutָ������� ��Ҫ��ǰ�����
// �����������printf
// ��ʼ��һ�����

// out ���յ�ǰ������ý��д���

#define OUT_TO_FILE     0x01
#define OUT_TO_SCREEN   0x02

void Out_setOutFile(FILE* fout);
void Out_setTarget();
void Out_setColor();

// ÿ�ζ�Ҫ�л���ɫ���ļ�ָ�룬̫�鷳�ˡ�������
// ���ֻ���뵽��Ļ��ֱ��printf�����ˣ������Ҫ���������OUT(OUT2FILE|OUT2SCREEN)
// ���ýṹ�������Ū�����ˣ�ֱ���ṩfout
// ����ͬʱ������ļ�����Ļ



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

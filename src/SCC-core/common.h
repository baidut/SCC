#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h> // getch

#include "debug.h"

#define REVISE
#define MAX_SYM_NUM 80

#define GEN_CODE

typedef enum{
    false = 0,
    true = 1,
}bool;

typedef enum{
// ȡtoken.h�ļ�������һ������tkö��ֵ
#define TOKEN(k, s) k,
#include "token.h"
#undef  TOKEN
}tk_e;

char* itos(int n);

#include "err.h"

#endif // COMMON_H_INCLUDED

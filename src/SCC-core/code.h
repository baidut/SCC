#ifndef CODE_H_INCLUDED
#define CODE_H_INCLUDED

#include "common.h"

typedef tk_e sym_e;
typedef struct{
    sym_e type;
    char* value;
}IdentSym;

typedef struct{
    char* value;
}ConstSym;

char*       newTemp();
char*       getLabel();
void        symTab_insert(sym_e type, char* value);
IdentSym*   symTab_lookup(char* value);
ConstSym*   constTab_lookInt(char* value);

#endif // CODE_H_INCLUDED

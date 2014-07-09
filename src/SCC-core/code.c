
#include "code.h"
#include <windows.h>

static IdentSym symTab[MAX_SYM_NUM];
static ConstSym constTab[MAX_SYM_NUM];
static int pos_id = 0;
static int pos_const = 0;

void symTab_insert(sym_e type, char* value){
/// 2014-7-9 添加重复定义提示出错
    if(symTab_lookup(value)!=NULL){
        _Error_print("redefined IDENT symbol %s",value);
        return; // 不再插入符号表
    }
/// 2014-7-9 添加重复定义提示出错
    symTab[pos_id].type = type;
    ASSERT(value!=0);
    symTab[pos_id].value = value;
    ASSERT(pos_id<MAX_SYM_NUM);
    pos_id ++;
}
IdentSym* symTab_lookup(char* value){
int i;
    for(i=0;i<pos_id;i++){
        if(0==strcmp(symTab[i].value,value)){
            return symTab+i;
        }
    }
    return NULL;
}
ConstSym* constTab_lookInt(char* value){
int i;
    for(i=0;i<pos_const;i++){
        if(0==strcmp(constTab[i].value,value)){
            return constTab+i;
        }
    }
    if(i==pos_const){
        constTab[pos_const].value = value;
        ASSERT(pos_const<MAX_SYM_NUM);
        pos_const ++;
    }
    return constTab+pos_const;
}
//函数用来产生临时变量, 如产生临时变量t1 :
static int tempNum=0;
char* newTemp(){
  //N=N+1;
  //“t”|| ITOS(N) ;
  tempNum ++ ;
  char* tempName = (char*) malloc(8*sizeof(char));
  *tempName='t';
  strcpy(tempName+1,itos(tempNum) );
  return tempName;
}

static int labelNum=0;
char* getLabel(){
  //N=N+1;
  //“t”|| ITOS(N) ;
  labelNum ++ ;
  char* labelName = (char*) malloc(8*sizeof(char));
  strcpy(labelName,"label");
  strcpy(labelName+5,itos(labelNum) );
  return labelName;
}
char* itos(int n) {
    char* str= (char*)malloc(10*sizeof(char));
    int radix=10;

    int i = 0;
    int m = n;
    int f = 0;
    if (n == 0) {  //如果是0，直接赋值
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
    else if (n < 0){
        str[0] = '-';
        n = -n;
        f = 1;
    }
    while (m){
        m /= radix;
        i++;
    }
    str[i + f] = '\0';
    i--;
    while (n){
        str[i + f] = n % radix;
        if (str[i + f] < 10){
            str[i + f] += '0';
        }
        else{
            str[i + f] += ('a' - 10);
        }
        n /= radix;
        i--;
    }
    return str;
}

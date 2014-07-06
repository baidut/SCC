
/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
@version v3.0
------------------------------------------------------------------------------
调试库
============================================================================= */

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "debug_settings.h"

#ifdef __cplusplus
extern "C"{
#endif

#define COMMENT_BEGIN    while(0){
#define COMMENT_END      }

// 调试信息属性设置
typedef enum{
	CFG_ASSERT,
	CFG_HINT,
	CFG_CHECKPOINT,
	CFG_WATCH,
	CFG_TRACK,
	CFG_LOG
}DEBUG_CFG_e;

#ifdef	NO_CFG
	#define CFG_OUT(x)
#else
	extern CFG_OUT(DEBUG_CFG_e);
#endif

// 禁用调试 ====================================================================
#ifndef DEBUG
	#define DEBUG_Init()
	#define DOUT					while(0)printf // 旧编译器不支持可变参数宏
#else
    #ifndef DOUT // 允许在debug_settings.h中自定义DOUT
    #define DOUT(FORMAT,...)	\
    do{\
        if(isOut2File()){\
            fprintf(fout,FORMAT,__VA_ARGS__);\
        }\
        if(isOut2Screen()){\
            printf(FORMAT,__VA_ARGS__);\
        }\
    }while(0)
    #endif
#endif // DEBUG

// 排版控制 ====================================================================
#define BR              	OUT("\n")

// 变量按格式输出 ==============================================================
#define OUT_DATETIME()		OUT(" DATE: %s TIME: %s ",\
								__DATE__ , __TIME__)

#define OUT_STR(var)        DOUT(" %s",(char*)var)
#define OUT_CHAR(var)       DOUT(" %c",(char)var)

#define OUT_DEC(var)        DOUT(" %d",(int)var)
#define OUT_HEX(var)        DOUT(" %X",(int)var)
#define OUT_OCT(var)        DOUT(" %o",(int)var)
#define OUT_BIN(var)  		\
do{							\
    int d=var;	            \
    long long int b=0;	    \
    long long int times=1;	\
    while(d!=0){	        \
        b+=(d%2)*times;	    \
        d/=2;	            \
        times*=10;	        \
    }	                    \
    DEBUG_OUT(" %I64d",b);	\
}while(0)

// 可变参宏的参数遍历 ----------------------------------------------------------------------
#define STRINGIZE(arg)  STRINGIZE1(arg)
#define STRINGIZE1(arg) STRINGIZE2(arg)
#define STRINGIZE2(arg) #arg

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define FOR_EACH_1(what, x, ...) what(x)
#define FOR_EACH_2(what, x, ...)\
  what(x);\
  FOR_EACH_1(what,  __VA_ARGS__);
#define FOR_EACH_3(what, x, ...)\
  what(x);\
  FOR_EACH_2(what, __VA_ARGS__);
#define FOR_EACH_4(what, x, ...)\
  what(x);\
  FOR_EACH_3(what,  __VA_ARGS__);
#define FOR_EACH_5(what, x, ...)\
  what(x);\
 FOR_EACH_4(what,  __VA_ARGS__);
#define FOR_EACH_6(what, x, ...)\
  what(x);\
  FOR_EACH_5(what,  __VA_ARGS__);
#define FOR_EACH_7(what, x, ...)\
  what(x);\
  FOR_EACH_6(what,  __VA_ARGS__);
#define FOR_EACH_8(what, x, ...)\
  what(x);\
  FOR_EACH_7(what,  __VA_ARGS__);

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) FOR_EACH_ARG_N(__VA_ARGS__)
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH_(N, what, x, ...) CONCATENATE(FOR_EACH_, N)(what, x, __VA_ARGS__)
#define FOR_EACH(what, x, ...) FOR_EACH_(FOR_EACH_NARG(x, __VA_ARGS__), what, x, __VA_ARGS__)

#define DEBUG_OUT_ENUM(var,...)   						\
	do{			                                		\
		switch(var){                            		\
		FOR_EACH(_CASE,##__VA_ARGS__)           		\
		default:break;                          		\
		}                                       		\
	}while(0)
	#define _CASE(i)    case i:OUT(" "#i);break;

// 特殊信息输出 ================================================================

// -----------------------------------------------------------------------------
#ifdef DEBUG_CHECKPOINT
    #define CHECKPOINT() 						\
	do{                                         \
		CFG_OUT(CFG_CHECKPOINT);				\
		DOUT("\r[%s] File:%s Line:%d\n",		\
			__FUNCTION__ ,__FILE__, __LINE__);	\
	}while(0)
#else
    #define CHECKPOINT()
#endif // DEBUG_CHECKPOINT
// -----------------------------------------------------------------------------
#ifdef DEBUG_WATCH
	#define WATCH(var,TYPE)  					\
	do{                                         \
		CFG_OUT(CFG_WATCH);                     \
		OUT(" $ %s = ",#var);                   \
		OUT_##TYPE (var);BR;                    \
	}while(0)
	#else
    #define	WATCH(var,TYPE)
#endif // DEBUG_WATCH
// -----------------------------------------------------------------------------
#ifdef DEBUG_TRACK
    #define TRACK(var,TYPE)  					\
    do{                                         \
        static void *p=NULL;                    \
        void *q=NULL;                           \
        void *tmp=NULL;                         \
        q=&(var);                               \
        if(p==NULL){                            \
            p=malloc(sizeof(var));              \
            memcpy(p,q,sizeof(var));            \
        }                                       \
        if(0!=memcmp(p,q,sizeof(var))){         \
            tmp=malloc(sizeof(var));            \
            memcpy(tmp,q,sizeof(var));          \
            memcpy(q,p,sizeof(var));            \
			CFG_OUT(CFG_TRACK);              	\
            DOUT(" # %s CHANGED FROM ",#var);   \
            OUT_##TYPE(var);                    \
            DOUT(" TO ");                       \
            memcpy(q,tmp,sizeof(var));          \
            OUT_##TYPE(var);                    \
            BR;                                 \
            memcpy(p,q,sizeof(var));            \
            free(tmp);                          \
        }                                       \
    }while(0)
#else
    #define TRACK(var,TYPE)
#endif // DEBUG_TRACK


// 不同类型信息的输出
// -----------------------------------------------------------------------------
#ifdef DEBUG_HINT
    #define HINT		CFG_OUT(CFG_HINT);
    #define LOG_END		CFG_OUT(CFG_DEFAULT);
#else
	#define HINT		COMMENT_BEGIN
    #define END_HINT	COMMENT_END
#endif // DEBUG_HINT
// -----------------------------------------------------------------------------
#ifdef DEBUG_LOG
    #define LOG   								\
    do{											\
        CFG_OUT(CFG_LOG);						\
        OUT_DATETIME();							\
    }while(0);
    #define LOG_END		CFG_OUT(CFG_DEFAULT);
#else
    #define LOG			COMMENT_BEGIN
    #define END_LOG		COMMENT_END
#endif // DEBUG_LOG

// -----------------------------------------------------------------------------
//控制
#ifdef DEBUG_ASSERT
    #define ASSERT(expr) 								 \
    do{                                                  \
	  if (expr) { ; }                                    \
	  else  {                                            \
		CFG_OUT(CFG_ASSERT);                          	 \
		DOUT("Assert failed: " #expr "                   \
		(function %s file %s line %d)n",                 \
		__FUNCTION__,__FILE__, (int) __LINE__ );         \
		exit(1);/*while (1);*/                           \
	  }                                                  \
    }while(0)
#else
    #define	ASSERT(expr)
#endif
#ifdef __cplusplus
}
#endif


/** 控制台颜色 ----------------------------------------------------------------- */


#define COLOR_DEFAULT   (FOREGROUND_RED |FOREGROUND_GREEN |FOREGROUND_BLUE)//WHITE
#define COLOR_ERROR     (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define COLOR_WARNING   (FOREGROUND_RED |FOREGROUND_INTENSITY|FOREGROUND_GREEN)

#define COLOR_NORMAL    COLOR_DEFAULT
#define COLOR_ASSERT    COLOR_ERROR
#define COLOR_TRACK     COLOR_WARNING
#define COLOR_DEBUG     FOREGROUND_GREEN
#define COLOR_HINT      FOREGROUND_BLUE | FOREGROUND_RED |  FOREGROUND_INTENSITY
#define COLOR_WATCH     FOREGROUND_BLUE | FOREGROUND_INTENSITY

#define Format_setColor(x)  do{SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),(x));}while(0)
/**  ----------------------------------------------------------------- */

#endif  // DEBUG_H_INCLUDED

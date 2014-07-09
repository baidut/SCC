/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
------------------------------------------------------------------------------
注意这不是头文件，而是数据配置文件，可以重复包含
务必在最前面包含#include "token_keywords.h"否则keywords[]不能正确匹配
关系运算符对语法分析无区别，改为一类
// TOKEN(TK_GREAT,         ">")
// TOKEN(TK_LESS,          "<")
// TOKEN(TK_GREAT_EQ,      ">=")
// TOKEN(TK_LESS_EQ,       "<=")
// TOKEN(TK_EQUAL,         "==")
// TOKEN(TK_UNEQUAL,       "!=")
============================================================================= */

//保留字
#include "token_keywords.h" // 注意务必在最前面
//标志符
TOKEN(TK_IDENT,         "IDENT")
//常数
TOKEN(TK_NUM,           "NUM")
//算术运算符
TOKEN(TK_ADD,           "+")
TOKEN(TK_SUB,           "-")
TOKEN(TK_MUL,           "*")
TOKEN(TK_DIV,           "/")
//关系运算符
TOKEN(TK_RELOP,         "RELOP")
//逻辑运算符
TOKEN(TK_LOG_OR,        "|")
TOKEN(TK_LOG_AND,       "&")
//其他运算符
TOKEN(TK_ASSIGN,        "=")
//分隔符
TOKEN(TK_LBRACE,        "{")
TOKEN(TK_RBRACE,        "}")
TOKEN(TK_SEMICOLON,     ";")
TOKEN(TK_LPAREN,        "(")
TOKEN(TK_RPAREN,        ")")
//特殊符号
TOKEN(TK_UNDEF,         "UNDEF")
TOKEN(TK_EOF,           "EOF")

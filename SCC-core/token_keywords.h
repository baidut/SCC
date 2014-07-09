/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
@brief 样本S语言关键字
------------------------------------------------------------------------------
采用单遍编译，为了提高效率，传递的是枚举值，而在需要调试输出时打印对应
的字符串
============================================================================= */

#ifndef TOKEN
#error "You must define TOKEN macro before include this file"
#endif

TOKEN(TK_INT,           "int")
TOKEN(TK_IF,            "if")
TOKEN(TK_THEN,          "then")
TOKEN(TK_ELSE,          "else")
TOKEN(TK_WHILE,         "while")
TOKEN(TK_DO,            "do")
TOKEN(TK_WRITE,         "write")
TOKEN(TK_READ,          "read")

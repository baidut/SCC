/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
------------------------------------------------------------------------------
ע���ⲻ��ͷ�ļ����������������ļ��������ظ�����
�������ǰ�����#include "token_keywords.h"����keywords[]������ȷƥ��
��ϵ��������﷨���������𣬸�Ϊһ��
// TOKEN(TK_GREAT,         ">")
// TOKEN(TK_LESS,          "<")
// TOKEN(TK_GREAT_EQ,      ">=")
// TOKEN(TK_LESS_EQ,       "<=")
// TOKEN(TK_EQUAL,         "==")
// TOKEN(TK_UNEQUAL,       "!=")
============================================================================= */

//������
#include "token_keywords.h" // ע���������ǰ��
//��־��
TOKEN(TK_IDENT,         "IDENT")
//����
TOKEN(TK_NUM,           "NUM")
//���������
TOKEN(TK_ADD,           "+")
TOKEN(TK_SUB,           "-")
TOKEN(TK_MUL,           "*")
TOKEN(TK_DIV,           "/")
//��ϵ�����
TOKEN(TK_RELOP,         "RELOP")
//�߼������
TOKEN(TK_LOG_OR,        "|")
TOKEN(TK_LOG_AND,       "&")
//���������
TOKEN(TK_ASSIGN,        "=")
//�ָ���
TOKEN(TK_LBRACE,        "{")
TOKEN(TK_RBRACE,        "}")
TOKEN(TK_SEMICOLON,     ";")
TOKEN(TK_LPAREN,        "(")
TOKEN(TK_RPAREN,        ")")
//�������
TOKEN(TK_UNDEF,         "UNDEF")
TOKEN(TK_EOF,           "EOF")

#ifndef ERR_H_INCLUDED
#define ERR_H_INCLUDED

// 2014-7-9 ���齫��������뵽һ������ģ���£�������ȫ�ֿ��Է��ʣ�������ά��
// ����warning��error���оٳ���������������!
#define _Error_print(FORMAT,...)        do{Format_setColor(COLOR_ERROR);printf("[%04d] " FORMAT "\n",curLine,##__VA_ARGS__);fprintf(errFile,"[%04d] " FORMAT "\n",curLine,##__VA_ARGS__);}while(0)
extern int curLine;
extern FILE* errFile;

#endif // ERR_H_INCLUDED

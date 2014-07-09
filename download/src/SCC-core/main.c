
/** ==========================================================================
@author YingZhenqiang  yingzhenqiang@gmail.com
------------------------------------------------------------------------------
����ǵ��exe�����ģ�����Ҫ�ڴ�ӡ�������ȴ��û�������ڴ���һ���������Ѻã�
������ִ�����Ҳ��Ҫ�ȴ������Դ��󲻻ᷢ���������ٶ�������ӵȴ���
��������λ������ʱ�������⣬���ô��봮�ķ�ʽ���У������ô�������ķ�ʽ��
�൱���û������ˡ�

TODO
��ɶ����벿��
MAYBE SOMEDAY
���������ù��ܡ���������������ļ���������ݡ�����ѡ���

codeblock���Զϵ粻ͣ������ԭ��
����·��
���ļ��ƶ������ѣ���Ҫ���ƶ��ļ���.......
============================================================================= */

#include "lex.h"
#include "parse.h"
#include "gen.h"

char filename[40];
bool isSinglePass;
bool isClickExe = false ;

static void ParseCmdLine(int argc, char *argv[]){
    ASSERT(argc==1);
    strcpy(filename,argv[0]);
}

int main(int argc, char *argv[]) {
 isClickExe = false;
    if (argc <= 1){
		//ShowHelp();
		isClickExe = true;
		printf("Input filename:");
		scanf("%s",filename);
	}
	else ParseCmdLine(--argc, ++argv);

    isSinglePass =true;//false;//

    if(isSinglePass){
        Gen_run(filename);
    }
    else{
        // �������������໥������ģ����ɣ������ļ����������ļ� ����û�б�д���ù��ܣ�����δ��д��鲿��
        Lex_run(filename,"lex.txt");
        //Parse_run("lex.txt","code.txt");
    }
    /**/if(isClickExe){
        printf("�����ɣ���������˳�");
        getch();
    }
    return 0;
}

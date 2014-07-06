
// test�ײ�Դ���������

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

int main(int argc, char *argv[]) { // �����л�ȡ�ļ�����������������ļ���

// ����ǵ��exe�����ģ�����Ҫ�ڴ�ӡ�������ȴ��û�������ڴ���һ���������Ѻã�������ִ�����Ҳ��Ҫ�ȴ������Դ��󲻻ᷢ���������ٶ�������ӵȴ���
    isClickExe = false;
    if (argc <= 1){
		//ShowHelp();
		isClickExe = true;
		printf("Input filename:");
		scanf("%s",filename);
	}
	else ParseCmdLine(--argc, ++argv);

    isSinglePass = true;//false;

    if(isSinglePass){
        Gen_run(filename);
    }
    else{
        // �������������໥������ģ����ɣ������ļ����������ļ� ����û�б�д���ù��ܣ�����δ��д��鲿��
        Lex_run(filename,"lex.txt");
        //Parse_run("lex.txt","code.txt");
    }
    /*if(isClickExe){
        printf("�����ɣ���������˳�");
        getch();
    }*/
    return 0;
}

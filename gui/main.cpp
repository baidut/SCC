#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QFont font  = a .font();
    //font.setPointSize(12);
    font.setFamily("微软雅黑");//Courier New
    a.setFont(font);


    MainWindow w;
    w.show();

    return a.exec();
}

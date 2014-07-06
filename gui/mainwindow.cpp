
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->action_new,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(ui->action_open,SIGNAL(triggered()),this,SLOT(open()));
    connect(ui->action_save,SIGNAL(triggered()),this,SLOT(save()));
    connect(ui->action_saveAs,SIGNAL(triggered()),this,SLOT(saveAs()));
    connect(ui->action_exit,SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    ui->plainTextEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}
void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ui->plainTextEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}
bool MainWindow::maybeSave()
{
    if (ui->plainTextEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}
void MainWindow::newFile()
{
    if (maybeSave()) {
        ui->plainTextEdit->clear();
        setCurrentFile("");
    }
}
bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}
bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}
bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << ui->plainTextEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
void MainWindow::showOutputFile(QString filename,QTextBrowser* browser)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(filename)
                             .arg(file.errorString()));
        return;
    }
    QTextStream in(&file);
    browser->setPlainText(in.readAll());
}
bool MainWindow::run()
{
    if(curFile.isEmpty()){
        QMessageBox::warning(this, tr("Application"),
                             tr("你还没有打开文件哩."));
        return false;
    }
    maybeSave();
    QProcess process;
    process.start("scc", QStringList()<<curFile);
    if (!process.waitForStarted()){
        ui->statusBar->showMessage("启动失败,请检查scc程序是否在当前目录下.");
        return false;
    }
     process.closeWriteChannel();
     if (!process.waitForFinished(8000)){
         ui->statusBar->showMessage("结束失败");
         return false;
     }
     QByteArray result = process.readAll();
     ui->statusBar->showMessage("已经执行完毕");
     ui->textBrowser_msg->setPlainText(result);
     return true;
}

void MainWindow::on_action_show_triggered()
{
    if(run()){
        showOutputFile("lex.txt",ui->textBrowser_lex);
        showOutputFile("parse.txt",ui->textBrowser_parse);
        showOutputFile("code.txt",ui->textBrowser_code);
        showOutputFile("err.txt",ui->textBrowser_err);
        ui->tabWidget_info->setCurrentIndex(3);
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}
void MainWindow::on_action_lex_triggered()
{
    if(run()){
        showOutputFile("lex.txt",ui->textBrowser_lex);
        ui->tabWidget_info->setCurrentIndex(0);
    }
}
void MainWindow::on_action_parse_triggered()
{
    if(run()){
        showOutputFile("parse.txt",ui->textBrowser_parse);
        ui->tabWidget_info->setCurrentIndex(1);
    }
}
void MainWindow::on_action_code_triggered()
{
    if(run()){
        showOutputFile("code.txt",ui->textBrowser_code);
        ui->tabWidget_info->setCurrentIndex(2);
    }
}

#include <QDesktopServices>
#include <QUrl>

void MainWindow::on_action_about_triggered()
{
    QMessageBox::about(this, tr("关于这个玩意"),
             tr("<h3>样本S语言编译器</h3>"
                "编译原理课程设计作品<br>学生：应振强 班级 电计1101 学号201181086"
                "<hr>联系方式 <u>13591120447@163.com</u>"
                "功能有待完善，你可以从github上克隆这个项目，项目地址："));
}
void MainWindow::on_action_seeReport_triggered()
{
    QDesktopServices::openUrl(QUrl("readMe.pdf"));
}

void MainWindow::on_action_seeDoc_triggered()
{
    QDesktopServices::openUrl(QUrl("Developer Manual.html"));
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    //void about();
    //void documentWasModified();
    void on_action_show_triggered();

    void on_action_parse_triggered();

    void on_action_lex_triggered();

    void on_action_code_triggered();

    void on_action_about_triggered();

    void on_action_seeReport_triggered();

    void on_action_seeDoc_triggered();

private:
    void setCurrentFile(const QString &fileName);
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    bool maybeSave();
    bool run();

private:
    Ui::MainWindow *ui;

    QString curFile;

    void showOutputFile(QString filename,QTextBrowser* browser);
};

#endif // MAINWINDOW_H

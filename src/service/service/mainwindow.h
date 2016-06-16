#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <iostream>
#include <QListWidget>
#include <QLineEdit>
#include "my_search.h"
#include "set_ie.h"
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <QFileInfo>
#include <QMessageBox>
#include <net_work.h>
#include <pthread.h>
#include <QFileInfoList>



using namespace std;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    char* find_path(char*);
    void my_send(char *);

    char *path;    //歌曲路径
    QListWidget *musicnames;//歌曲列表
    QString dir_path;
    QString duankou;    //端口号
    pthread_t net_tid;  //服务器线程ID
    NET_work *net_work; //网络类对象

public slots:
    void start();
    void add();
    void search();
    void my_delete();
    void my_set();
    void add_ip();
    void shut_ip();
    void get_names();
private:
    Ui::MainWindow *ui;
    My_Search *my_search;//搜索窗口
    Set_IE *set_ie;      //设置窗口

};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "music.h"
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "net_work.h"
#include <QDir>
#include<QByteArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QTime>
#include <QApplication>
#include <QStyle>
#include <stdio.h>
#include <QStringList>
#include <QDataStream>
#include <QIODevice>
#include <QTextStream>
#include <QTextCodec>


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
    void send_next();
    void set_dir();
    void add_song(char *);
    void my_select();
    char* my_get_row();
    void accept_music(QString);
    void set_fulltime(long int);
    void add_second();
    void move_line(int );
    void set_line(int ,int);
    void set_time_now();
    void my_words();
    void word_move();
    void send_now();



    pthread_t download_tid;//下载线程ID
    int row;               //被选行的行号
    QByteArray mi;         //歌曲名由qstring到char的中间变量
    QDir dir;              //存储默认路径
    NET_work *net;         //网络类的对象
    music *my_music;       //音乐类的对象
    pthread_t net_tid;     //网络线程ID
    int music_fd;          //打开dsp的fd
    int danqu;
    int j;

signals:
    void nextmusic();      //下一曲信号
    void thismusic();      //单曲信号

public slots:
    void my_denglu();
    void my_play();
    void my_sus();
    void my_shut();
    void my_next();
    void my_last();
    void play_music();
    void close_client();
    void add_music();
    void my_download();
    void my_jump(int);
    void re_set();
    void my_delete();
    void double_click();
    void change_vol();
    void show_word();
    void my_style();
    void change_background(QString);
    void bofang();
    void only_music();


private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H

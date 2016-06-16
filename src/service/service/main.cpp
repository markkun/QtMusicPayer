#include<iostream>
#include"mainwindow.h"
#include<QApplication>
#include"my_search.h"
using namespace std;
int main(int argc,char **argv)
{
    QApplication app(argc,argv);
    MainWindow mainwindow;
    mainwindow.show();
    app.exec();
}

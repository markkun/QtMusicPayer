#include "mainwindow.h"
#include <QApplication>
#include <iostream>

using namespace std;
int main(int argc,char **argv)
{
    QApplication app(argc,argv);
    MainWindow main;
    main.show();
    app.exec();

}

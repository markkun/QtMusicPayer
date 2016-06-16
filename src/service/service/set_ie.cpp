#include "set_ie.h"
#include "ui_set_ie.h"
#include<iostream>
using namespace std;

Set_IE::Set_IE(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Set_IE)
{
    ui->setupUi(this);
    //OK键
    connect(this->ui->ok_butten,SIGNAL(clicked()),this,SLOT(my_ok()));
    //关闭键
    connect(this->ui->clos_butten,SIGNAL(clicked()),this,SLOT(close()));
}

void Set_IE::my_ok()//按OK获取所输入端口号
{
    this->mo=this->ui->lineEdit_2->text();
    this->accept();
}

void Set_IE::set(QString str)//自动设置端口号
{
    this->ui->lineEdit_2->setText(str);
}

Set_IE::~Set_IE()
{
    delete ui;
}

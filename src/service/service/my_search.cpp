#include "my_search.h"
#include "ui_my_search.h"

My_Search::My_Search(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::My_Search)
{
    ui->setupUi(this);
    //OK建
    connect(this->ui->ok_butten,SIGNAL(clicked()),this,SLOT(my_ok()));
    //关闭建
    connect(this->ui->clos_butten,SIGNAL(clicked()),this,SLOT(close()));
}

void My_Search::my_ok()//单击OK，获取输入内容
{

    search_filename=this->ui->lineEdit->text();

    this->accept();
}

My_Search::~My_Search()
{
    delete ui;
}

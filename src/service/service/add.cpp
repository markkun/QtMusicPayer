#include "add.h"
#include "ui_add.h"

add::add(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add)
{
    ui->setupUi(this);
    connect(this->ui->ok_butten,SIGNAL(clicked()),this,SLOT(ok()));
    connect(this->ui->cancel_butten,SIGNAL(clicked()),this,SLOT(close()));
}

void add::ok()
{
    QString filename;
    filename=this->ui->lineEdit->text().fromUtf8().data();
}
add::~add()
{
    delete ui;
}

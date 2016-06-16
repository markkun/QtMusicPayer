#include "mainwindow.h"
#include "ui_mainwindow.h"
int start_i=1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //开启键
    connect(this->ui->start,SIGNAL(clicked()),this,SLOT(start()));
    //添加建
    connect(this->ui->add,SIGNAL(clicked()),this,SLOT(add()));
    //查找建
    connect(this->ui->find,SIGNAL(clicked()),this,SLOT(search()));
    //删除键
    connect(this->ui->delete_2,SIGNAL(clicked()),this,SLOT(my_delete()));
    //设置建
    connect(this->ui->set,SIGNAL(clicked()),this,SLOT(my_set()));
    //断开建
    connect(this->ui->shutdown,SIGNAL(clicked()),this,SLOT(shut_ip()));
}

void *my_start(void*arg) //创建服务器并读取播放列表等待下载
{
    MainWindow *all_include=(MainWindow*)arg;
    cout<<"set "<<ntohs(all_include->net_work->ser_addr.sin_port)<<endl;
    //开启服务器
    all_include->net_work->ser_run();
    cout<<all_include->net_work->ip<<endl;
    //添加已链接客户端的IP
    all_include->add_ip();
    all_include->get_names();
    char buf[128];
    int i;
    //传送播放列表
    for(i=0;i<all_include->musicnames->count();i++)
    {
        cout<<i<<endl;
        QFileInfo fi(all_include->musicnames->item(i)->text());
        strcpy(buf,fi.fileName().toUtf8().data());
        //strcat(buf,"\n");
        cout<<buf<<endl;
        if(write(all_include->net_work->cli_fd,buf,sizeof(buf))<0)
        {
            perror("write err");
        }
    }
    //等待下载歌曲
    while(1)
    {
        if(read(all_include->net_work->cli_fd,buf,sizeof(buf))>0)
        {
            cout<<"read end"<<buf<<endl;
            all_include->my_send(buf);
        }
    }

 }

void MainWindow::start()//开启/关闭 建
{

    start_i++;
    if(start_i%2==0)
    {
        this->ui->start->setText(QString::fromUtf8("关闭"));
        this->net_work->set_addr(this->duankou);
        net_work->ser_addr.sin_port=htons(atoi(this->duankou.toAscii().data()));
        pthread_create(&net_tid,NULL,my_start,this);
    }
    else
    {
        net_work->my_close();
        pthread_cancel(net_tid);
        this->ui->start->setText(QString::fromUtf8("开启"));
    }
}

void MainWindow::add()//向列表内添加歌曲
{

    QStringList filenames=QFileDialog::getOpenFileNames();
    this->ui->list_song->addItems(filenames);
}

void MainWindow::get_names()
{
  musicnames=this->ui->list_song;
}

void MainWindow::my_delete()//从列表内移除歌曲
{
  this->ui->list_song->takeItem(this->ui->list_song->currentRow());

}

void MainWindow::search()//在列表内查找歌曲
{
    int i;
    this->my_search = new My_Search;
    this->my_search->show();
    if(this->my_search->exec()==1)
    {
        for(i=0;i<this->ui->list_song->count();i++)
        {

           if(this->ui->list_song->item(i)
                    ->text().contains(this->my_search
                                      ->search_filename.toUtf8().data(),Qt::CaseInsensitive))
           {
               this->ui->list_song->setCurrentRow(i);

           }
        }

    }

}

void MainWindow::my_set()//设置端口号
{

    this->set_ie = new Set_IE;
    this->set_ie->mo=this->duankou;
    this->set_ie->show();
    this->set_ie->set(this->duankou);
    if(this->set_ie->exec() == 1)
       this->duankou=set_ie->mo;
}

void MainWindow::add_ip()//添加已链接用户的IP
{
    cout<<"1"<<endl;
    this->ui->list_ip->addItem(QString::fromUtf8(this->net_work->ip));
}

void MainWindow::shut_ip()//断开所选IP
{
    this->ui->list_ip->takeItem(this->ui->list_ip->currentRow());
    this->net_work->my_close();
}

char* MainWindow::find_path(char *ptr)//获取歌曲路径
{
    int i;
    for(i=0;i<this->ui->list_song->count();i++)
    {

       if(this->ui->list_song->item(i)
                ->text().contains(
                        QString::fromUtf8(ptr),Qt::CaseInsensitive))
       {
           return this->ui->list_song->item(i)->text().toUtf8().data();

       }
    }
}

void *send_music(void *arg)//发送歌曲
{
    char buf[1024];
    int fd,lenth;
    MainWindow *main=(MainWindow*)arg;
    cout<<main->path<<endl;
    if((fd=open(main->path,O_RDONLY))<0)
    {
        perror("open err");
    }
    while(1)
    {
        lenth=read(fd,buf,sizeof(buf));
        if(lenth<=0)
        {
            write(main->net_work->cli_fd,"over",sizeof(buf));
            cout<<"send over "<<endl;
            break;
        }
        lenth=write(main->net_work->cli_fd,buf,sizeof(buf));
        cout<<"lenth:"<<lenth<<endl;
    }
    close(fd);
    cout<<"1"<<endl;
    pthread_exit(0);
}

void MainWindow::my_send(char *buf)//创建发送歌曲线程
{
    //char buf[128];
    path=this->find_path(buf);
    pthread_t tid;
    if(pthread_create(&tid,NULL,send_music,this)<0)
    {
        perror("create tid err");
    }

}


MainWindow::~MainWindow()
{
    delete ui;
}

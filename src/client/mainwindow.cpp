#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColor>
#include <stdio.h>

extern pthread_mutex_t my_mutex; //互斥锁，用于暂停
extern QTime current_time;


int lock_i=0;                   //统计点击播放键的次数

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    row=-1;

    ui->setupUi(this);
    //播放键
    connect(this->ui->butn_play,SIGNAL(clicked()),this,SLOT(my_play()));
    //停止键
    connect(this->ui->butn_shut,SIGNAL(clicked()),this,SLOT(my_shut()));
    //下一首
    connect(this->ui->butn_next,SIGNAL(clicked()),this,SLOT(my_next()));
    //上一首
    connect(this->ui->butn_last,SIGNAL(clicked()),this,SLOT(my_last()));
    //信号next
    connect(this,SIGNAL(nextmusic()),this,SLOT(my_next()));
    //登录建
    connect(this->ui->bunt_denglu,SIGNAL(clicked()),this,SLOT(my_denglu()));
    //断开建
    connect(this->ui->butn_duankai,SIGNAL(clicked()),this,SLOT(close_client()));
    //下载建
    connect(this->ui->butn_xiazai,SIGNAL(clicked()),this,SLOT(my_download()));
    //进度条
    connect(this->ui->jindu,SIGNAL(sliderMoved(int)),this,SLOT(my_jump(int)));
    //刷新播放列表
    connect(this->ui->butn_reset,SIGNAL(clicked()),this,SLOT(re_set()));
    //从列表删除
    connect(this->ui->butn_delete,SIGNAL(clicked()),this,SLOT(my_delete()));
    //双击播放
    connect(this->ui->song_here,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(double_click()));
    //修改音量
    connect(this->ui->vol_length,SIGNAL(sliderMoved(int)),this,SLOT(change_vol()));
    //打开歌词
    connect(this->ui->butn_geci,SIGNAL(clicked()),this,SLOT(show_word()));
    //添加/修改主题
    connect(this->ui->add_music,SIGNAL(clicked()),this,SLOT(my_style()));
    //修改背景
    connect(this->ui->comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(change_background(QString)));

    connect(this->ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(bofang()));

    connect(this,SIGNAL(thismusic()),this,SLOT(only_music()));

    //默认隐藏歌词列表
    this->ui->list_word->hide();

    this->set_dir();        //设置默认存储文件夹

    this->re_set();

    this->ui->line_duankou->setText(QString::fromUtf8("65530"));     //设置默认端口

    this->ui->line_ip->setText(QString::fromUtf8("192.168.1.201"));  //设置默认IP

    this->ui->vol_length->setRange(80,100);

    this->ui->vol_length->setValue(90);

}

void *net_go(void *arg)  //运行客户端网络套接字并connect获取列表
{
    char buf[128];
    int len;
    MainWindow *main=(MainWindow*)arg;
    main->net=new NET_work;
    main->net->work_start();
    while(1)
    {
        len=main->net->my_read(buf);
        main->add_song(buf);
        if(len<=0)
        {
            cout<<"1"<<endl;
            break;
        }
    }

}

void MainWindow::my_denglu() //创建网络线程
{
    this->net->my_get(atoi(this->ui->line_duankou->text().toUtf8().data()),
                      this->ui->line_ip->text().toUtf8().data());
    if(pthread_create(&net_tid,NULL,net_go,this)<0)
    {
        perror("create net err");
    }

}

void MainWindow::change_background(QString name)//修改背景
{
    QString path("background-image: url(:/new/prefix1/);");
    path.insert(36,name);
    cout<<path.toUtf8().data()<<endl;
    this->ui->centralwidget->setStyleSheet(path.toLatin1());
}

void MainWindow::show_word()//显示歌词
{
    QSize size(15,25);
    int i;

    this->ui->list_word->show();
    if(lock_i>0)
    {

        this->ui->list_word->setGridSize(size);
        for(i=0;i<this->ui->list_word->count();i++)
        {
            this->ui->list_word->item(i)->setTextAlignment(Qt::AlignHCenter);
        }
    }
}

void MainWindow::word_move()//点亮当前行并使界面移动
{
    int i;
    QColor color2(19,20,20);
    QString time;
    QColor color1(93,7,77);

    for(i=0;i<this->my_music->word_time.count();i++)
    {
        time=this->my_music->word_time.at(i);
        if(QString::compare(time,this->ui->time_now->text())==0)
        {

            if(this->ui->list_word->currentRow()>1)
            {
                this->ui->list_word->item(j)->setTextColor(color2);
            }
            j=i;
            this->ui->list_word->item(i)->setTextColor(color1);
            this->ui->list_word->setCurrentRow(i+2);
            //this->ui->list_word->setItemHidden(this->ui->list_word->item(i-3),true);
        }
    }
}

void MainWindow::my_words()//添加歌词
{
    this->ui->list_word->clear();
    this->my_music->word_time.clear();
    char word[128];
    strcpy(word,this->my_music->music_name);

    word[strlen(word)-1]='c';
    word[strlen(word)-2]='r';
    word[strlen(word)-3]='l';

    if(access(word,R_OK))
    {
        this->ui->list_word->addItem(QString::fromUtf8("没有相应歌词"));
    }
    else
    {
        QFile inputFile(QString::fromUtf8(word));

        QTextStream inputStream(&inputFile);
        inputStream.setCodec(QTextCodec::codecForName("GB2312"));
        inputFile.open(QIODevice::ReadOnly);

        while(!inputStream.atEnd())
        {
            QString line=inputStream.readLine();

            if(line.indexOf("]")==9)
            {
                this->my_music->word_time<<line.mid(1,line.indexOf("]")-4);

                this->ui->list_word->addItem(line.right(line.count()-line.indexOf("]")-1));
            }
        }

    }
}

void MainWindow::my_style()//切换主题
{
    this->ui->comboBox->setCurrentIndex(0);
    QString my_path;
    my_path=QFileDialog::getOpenFileName(this,tr("Open File"),"../style",tr("text(*.qss)"));
    QFile file(my_path);
    file.open(QIODevice::ReadOnly);
    QString the_style=file.readAll();
    this->setStyleSheet(the_style.toLatin1());
}

void MainWindow::re_set()//刷新播放列表
{
    this->ui->song_here->clear();
    QStringList filters;
     filters << "*.wav";
     dir.setNameFilters(filters);
    this->ui->song_here->addItems(this->dir.entryList(filters,QDir::Files,QDir::Name));//读取默认文件夹内的内容
}



void MainWindow::send_next()//当一首歌结束时发送信号给主界面调用下一首
{
    emit this->nextmusic();
}

void *my_run(void *arg)//播放线程
{
    MainWindow *mine=(MainWindow*)arg;
    long int a;
    if((mine->my_music->dsp_fd=open("/dev/dsp",O_WRONLY))<0)
    {
        perror("open err");
    }
    mine->my_music->music_open();
    a=(mine->my_music->head.Subchunk2Size/mine->my_music->head.ByteRate);
    mine->set_fulltime(a);
    mine->my_music->play_music();
    cout<<"danqu"<<mine->danqu<<endl;
    if(mine->danqu==1)
    {
        mine->send_now();

    }
    else
        mine->send_next();
}

void MainWindow::play_music()//播放
{

    int err;
    mi=this->dir.absoluteFilePath(
            this->ui->song_here->item(row)->text()).toUtf8();
    if(access(mi.data(),R_OK))//列表内播放的音乐是否存在
    {
        QMessageBox msg;
        msg.setText(QString::fromUtf8("歌曲不存在,请下载"));
        msg.show();
        msg.exec();
    }

    else
    {

        this->my_music=new music(this);
        lock_i++;
        this->ui->butn_play->setStyleSheet("\
              QPushButton\
              {\
                border-image: url(:/new/prefix1/2007654410427.png);\
              }\
            ");
        this->ui->song_here->setCurrentRow(row);//点亮需要播放的音乐的文件名

        this->ui->my_music->setText(this->ui->song_here->currentItem()->text());

        this->my_music->music_name=mi.data();

        if((err=pthread_create(&this->my_music->music_tid,NULL,my_run,this))!=0)//创建播放线程
        {
            perror("create tid err");
        }
    }
}

void MainWindow::my_select()//获取选中行行号
{
    if(this->ui->song_here->currentRow()==-1)//如果未选择，则默认选中第一行
       this->row=0;
    else
        row=this->ui->song_here->currentRow();
}

void MainWindow::send_now()
{
    emit thismusic();
}

void MainWindow::my_play()//播放按键操作
{

    if(this->ui->song_here->count()!=0)
    {    if(lock_i==0)           //若lock_i=0,播放
        {

            this->my_select();

            this->play_music();
        }
        else                   //若lock_i！=0,播放或者暂停
            this->my_sus();
    }

}

void MainWindow::my_sus()//暂停/继续
{


    if(lock_i%2==0)
    {
        pthread_mutex_unlock(&my_mutex);
        this->ui->butn_play->setStyleSheet("\
              QPushButton\
              {\
                border-image: url(:/new/prefix1/2007654410427.png);\
              }\
            ");
    }
    if(lock_i%2!=0)
    {
        pthread_mutex_lock(&my_mutex);
        this->ui->butn_play->setStyleSheet("\
              QPushButton\
              {\
                border-image: url(:/new/prefix1/2007654416100.png);\
              }\
            ");
    }
    lock_i++;
}

void *close_dsp(void *arg)//关闭声卡及相应播放线程
{
    music *my_music=(music*)arg;
    pthread_cancel(my_music->music_tid);

    dup2(1,my_music->dsp_fd);
    pthread_exit(0);
}

void MainWindow::my_shut()//停止播放
{
    if(lock_i!=0)
    {
        this->ui->list_word->clear();
        pthread_mutex_unlock(&my_mutex); //解除所有锁
        pthread_t tid;
        pthread_create(&tid,NULL,close_dsp,my_music); //创建线程
        this->ui->jindu->setValue(0);
        this->ui->full_time->setText(QString::fromUtf8("00:00"));
        this->ui->time_now->setText(QString::fromUtf8("00:00"));
        lock_i=0;                           //lock归零，再点播放时创建线程播放音乐
        this->ui->butn_play->setStyleSheet("\
              QPushButton\
              {\
                border-image: url(:/new/prefix1/2007654416100.png);\
              }\
            ");
    }

}

void MainWindow::add_song(char *str)//向歌曲列表内添加歌曲
{
    this->ui->song_net->addItem(QString::fromUtf8(str));
}


void MainWindow::set_dir()//设置默认存储文件夹
{
    mkdir("../style",0777);
    mkdir("../my_music",0777);
    this->dir.setPath(QString::fromUtf8("../my_music"));
}

void MainWindow::my_next()//下一首
{

    if(lock_i!=0) //若正在播放
    {
        this->my_shut();//关闭DSP
        sleep(1);
        this->ui->butn_play->setStyleSheet("\
              QPushButton\
              {\
                border-image: url(:/new/prefix1/2007654410427.png);\
              }\
            ");
        row++;          //光标移至下一行
        if(row==this->ui->song_here->count())
            row=0;      //如果到最后一行返回第一行
        this->play_music();
    }
}

void MainWindow::my_last()//上一首
{

    if(lock_i!=0)
    {
        this->my_shut();
        sleep(1);
        this->ui->butn_play->setStyleSheet("\
              QPushButton\
              {\
                border-image: url(:/new/prefix1/2007654410427.png);\
              }\
            ");
        row--;
        if(row<0)
            row=this->ui->song_here->count()-1;
        this->play_music();
    }
}

void MainWindow::close_client()//关闭套接字和我网络线程
{
    if(net_tid!=0)
    {
        dup2(1,this->net->sockfd);
        pthread_cancel(net_tid);
        net_tid=0;
    }
}

void MainWindow::add_music()//添加本地歌曲
{
    QMessageBox msg;
    msg.setText(QString::fromUtf8("待定"));
    msg.show();
    msg.exec();
}

char* MainWindow::my_get_row()//获取已选中行的内容
{
    return this->ui->song_net->currentItem()->text().toUtf8().data();
}

void *net_download(void *arg)//创建线程下载歌曲
{
    MainWindow *main=(MainWindow*)arg;
    char *name;
    name=main->my_get_row();
    if(write(main->net->sockfd,name,128)<0)
    {
        perror("write err");
    }
    cout<<"write over "<<name<<endl;

    main->accept_music(QString::fromUtf8(name));
    cout<<"1"<<endl;
    pthread_exit(0);

}

void MainWindow::accept_music(QString name)//在默认文件夹内创建同名文件并接受
{
   this->ui->butn_xiazai->setEnabled(false);

    pthread_cancel(this->net_tid);
    char path[128];
    char buf[1024];
    char name1[128];
    strcpy(name1,name.toUtf8().data());

    int fd,len;
    strcpy(path,this->dir.path().toUtf8().data());
    cout<<"name:"<<name.toUtf8().data()<<endl;
    cout<<"name1:"<<name1<<endl;
    strcat(path,"/");
    strcat(path,name1);
    cout<<"path"<<path<<endl;
    if((fd=open(path,O_CREAT|O_EXCL|O_WRONLY,0777))<0)
    {
        perror("open err");

    }
    cout<<"fd1 "<<fd<<endl;
    while(1)
    {
        len=read(this->net->sockfd,buf,sizeof(buf));
        if(strcmp(buf,"over")==0)
        {
            cout<<"read over"<<endl;
            break;
        }
        cout<<"read"<<len<<endl;
        if((write(fd,buf,len))<0)
        {
            perror("write err");
        }
        cout<<"fd2 "<<fd<<endl;
    }
    this->ui->butn_xiazai->setEnabled(true);
    ::close(fd);

}

void MainWindow::my_download()//创建下载线程
{
    if(pthread_create(&this->download_tid,NULL,net_download,this)<0)
    {
        perror("create tid2 err");
    }
}

void MainWindow::set_fulltime(long int a)
{
    QString time_s;
    QTime time(0,a/60,a%60,0);
    time_s=time.toString("mm:ss");
    this->ui->full_time->setText(time_s);
}

void MainWindow::add_second()//当前播放时间增加
{
   QTime time1,time2;
   time1=QTime::fromString(this->ui->time_now->text(),"mm:ss");
   time2=time1.addSecs(1);
   this->ui->time_now->setText(time2.toString("mm:ss"));

}

void MainWindow::set_line(int max,int min)//设置进度条最大值和最小值
{
    this->ui->jindu->setRange(min,max);
}

void MainWindow::move_line(int i)//移动进度条
{
    this->ui->jindu->setValue(i);
}

void MainWindow::set_time_now()//播放时间与进度条进度同步
{
    int a;
    float b;
    b=(float)ftell(this->my_music->music_fp)/(float)(this->my_music->head.ChunkSize+8);
    a=b*(this->my_music->head.Subchunk2Size/this->my_music->head.ByteRate);
    cout<<"ftell:"<<ftell(this->my_music->music_fp)<<endl;
    cout<<"a:"<<a<<endl;
    QString time_s;
    QTime time(0,a/60,a%60,0);
    time_s=time.toString("mm:ss");
    this->ui->time_now->setText(time_s);
}

void MainWindow::my_jump(int now)//进度条移动
{
    int i;
    i=this->ui->jindu->sliderPosition();
    cout<<"jump:"<<i<<endl;
    i=(i/176400*176400);
    fseek(this->my_music->music_fp,i,SEEK_SET);
    cout<<"jump end:"<<ftell(this->my_music->music_fp)<<endl;
    this->set_time_now();
}

void MainWindow::my_delete()//删除所选列表内的所选内容
{

    int i;
    i=this->ui->tabWidget->currentIndex();
    if(i==0)
    {
        if(this->ui->song_here->count()!=-1)
        {
            if(this->ui->song_here->currentRow()!=-1)
            {
                this->ui->song_here->takeItem(this->ui->song_here->currentRow());
                if(!QString::fromUtf8(this->my_music->music_name).contains(this->ui->song_here->currentItem()->text()))
                    this->my_shut();
            }
            else
                this->ui->song_here->takeItem(0);
        }
    }
    if(i==1)
    {
        if(this->ui->song_net->count()!=0)
            this->ui->song_net->takeItem(this->ui->song_net->currentRow());
    }

}

void MainWindow::double_click()//双击播放
{
    this->my_shut();
    usleep(1500000);
    this->my_play();
}

void MainWindow::change_vol()//改变音量
{
    this->my_music->vol=this->ui->vol_length->value();
    cout<<this->my_music->vol<<endl;
}

void MainWindow::bofang()
{
    this->danqu=this->ui->comboBox_2->currentIndex();
    cout<<this->danqu<<endl;
}

void MainWindow::only_music()
{
    if(lock_i!=0) //若正在播放
    {
        this->my_shut();//关闭DSP
        sleep(1);
        this->ui->butn_play->setStyleSheet("\
              QPushButton\
              {\
                border-image: url(:/new/prefix1/2007654410427.png);\
              }\
            ");
       // row++;          //光标移至下一行

        this->play_music();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

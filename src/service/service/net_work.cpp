#include "net_work.h"
#include <iostream>
using namespace std;


NET_work::NET_work(QObject *parent)
    :QObject(parent)
{
    cli_fd=0;

}
int NET_work::ser_run()//创建服务器并等待客户端链接
{
    struct sockaddr_in cli_addr;
    int clifd;
    socklen_t len;
    len=sizeof(struct sockaddr);
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("sock err");
        exit(0);
    }
    if(bind(sockfd,(struct sockaddr*)&ser_addr,sizeof(struct sockaddr))<0)
    {
        perror("bind err");
        exit(0);
    }
    if(listen(sockfd,5)<0)
    {
        perror("listen err");
        return -1;
    }
    bzero(&cli_addr,sizeof(cli_addr));
    cout<<"accepting"<<endl;
    if((clifd=accept(sockfd,(struct sockaddr*)&cli_addr,&len))<0)
    {
        perror("accept err");
        return -1;
    }
    cout<<"connected"<<endl;
    char *buf;
    inet_ntop(AF_INET,(void*)&cli_addr,buf,len);
    ip=buf;
    cout<<buf<<endl;
    this->cli_fd=clifd;
}

int NET_work::set_addr(QString str) //设置地址
{
    bzero(&ser_addr,sizeof(struct sockaddr));
    ser_addr.sin_family=AF_INET;

    ser_addr.sin_port=htons(atoi(str.toAscii().data()));
    cout<<ntohs(ser_addr.sin_port)<<endl;
    ser_addr.sin_addr.s_addr=INADDR_ANY;
}

void NET_work::tell()
{
    emit this->intsert();
}

void NET_work::my_close()//关闭服务器
{
    if(cli_fd!=0)
    {
        close(this->sockfd);
        close(this->cli_fd);
        cli_fd=0;
        sockfd=0;
    }
}

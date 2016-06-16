#ifndef NET_WORK_H
#define NET_WORK_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <QString>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <QObject>

class NET_work:QObject
{
Q_OBJECT
public:
    NET_work(QObject *parent = 0);
    NET_work();
    int cli_fd;//服务器套接字
    char *ip;  //客户端IP
    int sockfd;
    struct sockaddr_in ser_addr; //地址
    int ser_run(void);
    int set_addr(QString );
    void my_close();    //关闭套接字
    void tell();
signals:
    void intsert();
public slots:


};

#endif // NET_WORK_H

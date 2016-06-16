#include "net_work.h"
char my_ip[100];
long int my_port;
NET_work::NET_work()
{
    this->sockfd=0;
}

void NET_work::my_get(long int num,char *ptr)//获取输入的IP和端口
{
    char *ip;
    long int port;
    ip=ptr;
    port=num;
    cout<<num<<endl;
    cout<<ptr<<endl;
    strcpy(my_ip,ip);
    my_port=port;
}

int NET_work::work_start()//创建客户端并链接服务器
{


    struct sockaddr_in addr;
    int fd;


    fd=socket(AF_INET,SOCK_STREAM,0);
    this->sockfd=fd;

    bzero(&addr,sizeof(struct sockaddr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(my_port);
    inet_pton(AF_INET,my_ip,&addr.sin_addr.s_addr);

    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(struct sockaddr))<0)
    {
        perror("connect err");
        return -1;
    }
    return 1;
}

int NET_work::my_read(char *buf)//从套接字内读
{
    return read(sockfd,buf,128);
}



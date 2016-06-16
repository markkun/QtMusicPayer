#ifndef NET_WORK_H
#define NET_WORK_H

#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <QListWidget>
#include <iostream>
using namespace std;

class NET_work
{
public:
    int sockfd;  //套接字fd

    NET_work();
    int my_read(char *);
    int work_start();
    void my_get(long int,char*);

};

#endif // NET_WORK_H

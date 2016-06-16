#ifndef MUSIC_H
#define MUSIC_H


#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/soundcard.h>
#include<string.h>
#include<pthread.h>
#include<QThread>
#include<iostream>
#include <QStringList>
#include <QTime>
using namespace std;

typedef struct _WAVEFILE_HEADER//波形文件头结构体
{
        char ChunkID[4];    	//"RIFF"
        long ChunkSize;     	//整个文件大小-8bytes
        char Format[4];         //"wave"
        char Subchunk1ID[3];	//"fmt" 表示以下是format chunck
        long Subchunk1Size;		//格式数据的字节数 一般是16，特殊的也有18
        short AudioFormat;  	//编码方式
        short NumChannels;  	//声道数量
        long SampleRate;       //采样率
        long ByteRate;          //每秒所需字节数
        short BlockAlign;       //每次采样需要的字节数
        short BitsPerSample;    //每个采样需要的bit数
        char Subchunk2ID[3];    //'dat'字符串，标识以下是数据
        long Subchunk2Size;     //数据的大小
}WAVEFILE_HEADER;


class music
{

public:
    music(void*);
    FILE* music_fp;
    char *music_name;               //播放音乐文件的路径
    int dsp_fd;
    int mixer_fd;
    int vol;
    pthread_t music_tid;            //音乐线程ID
    WAVEFILE_HEADER head;
    void *arg;
    QStringList word_time;


    void set_music(char *);
    int setup_dsp(int ,int ,int );///安装dsp  设置 采样率   声道个数
    int sync_dsp(int );                  ///设置同步播放
    int play_music( );
    void close(int );
    int music_open();
    int set_vol();
};

#endif // MUSIC_H

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/soundcard.h>


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


///安装dsp  设置 			 采样率   声道个数
int setup_dsp(int dsp_fd,int rate,int channels)
{
	int format;
	
	if( ioctl(dsp_fd,SNDCTL_DSP_STEREO,&channels) == -1 )
	{
		perror("ioctl sterr failed");
		return -1;
	}
	
	switch(channels)
	{
		case 0:
			format = AFMT_U8;
			break;
		case 1:
			format = AFMT_S16_LE;
			break;
		default:
			perror("unsuport channels");
			return -1;
	}
	if(ioctl(dsp_fd,SNDCTL_DSP_SETFMT,&format) == -1)
	{
		perror("ioctl set formalt");
		return -1;
	}
	if(ioctl(dsp_fd,SNDCTL_DSP_SPEED,&rate) == -1)
	{
		perror("ioctl set_speed failed");
		return -1;
	}
	return 0;
}

///设置同步播放
int sync_dsp(int dsp_fd)
{
	if(ioctl(dsp_fd,SNDCTL_DSP_SYNC) != 0)
	{
		perror("ioctl sync failed");
		return -1;
	}
	return 0;
}


int play_music(char* music_name ,int dsp_fd)
{
	WAVEFILE_HEADER head;
	FILE* music_fp;
	int music_len;
	long rate;
	char* buf;
	long n;
	
	//打开 音乐文件
	music_fp = fopen(music_name,"r+");
	if(music_fp == NULL)
	{
		perror("fopen music file failed");
		return -1;
	}
	//读取音乐文件头
	if(fread(&head,sizeof(head),1,music_fp) != 1 )
	{
		perror("fread head err");
		return -1;
	}
	
	//验证文件头
	if(memcmp(&head.ChunkID,"RIFF",4) != 0 )
	{
		perror("music head chunkId err");
		return -1;
	}
	//验证比特率
	if(head.BitsPerSample == 32)
	{
		perror("unsuport samply size ");
		return -1;
	}
	music_len = head.ChunkSize+8;
	printf("music_len :%d",music_len);
	
	///安装dsp  设置 			 采样率   声道个数
	if(setup_dsp(dsp_fd,head.SampleRate,head.NumChannels) != 0)
	{
		perror("setup_dsp failed");
		return -1;
	}
	////生效setup_dsp设置
	if(sync_dsp(dsp_fd) != 0)
	{
		perror("sync dsp failed");
		return -1;
	}
	//-----设置采样率--------
    //采样位数＊采样率/8 =  字节数量
    //＋7 方式采样率不是8的整数倍的情况
    //  /8*2 = /4 都按照双声道处理
    //size =  (head.BitsPerSample*head.SampleRate+7)>>2;
    rate = head.ByteRate;	//每次读取的字节 = 字节速率
    buf = (char*)malloc(rate);	//音乐数据缓存
	printf("rate:%ld\n",rate);
	//打印需要播放的总时间
	printf("music time:%d",head.Subchunk2Size/head.ByteRate);
	
	if(buf != NULL)
	{
		do{
		
			n = fread(buf,1,rate,music_fp);
			printf("n:%ld\n",n);
			if(n > 0)
			{
				write(dsp_fd,buf,n);
			}
			
			if(feof(music_fp))
			{
				perror("play over.");
				break;
			}
		
		}while(1);
		
		
	}
	

}


int main(int argc,char** argv)
{
	int dsp_fd;
	if(access(argv[1],R_OK))
	{
		perror("access failed");
		exit(0);
	}
	
	dsp_fd = open("/dev/dsp",O_WRONLY);
	if(dsp_fd< 0)
	{
		perror("open dsp err");
		exit(0);
	}
	
	
	play_music(argv[1],dsp_fd);
	
	
}

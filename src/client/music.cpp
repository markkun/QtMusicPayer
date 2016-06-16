#include "music.h"
#include "mainwindow.h"

extern pthread_mutex_t my_mutex=PTHREAD_MUTEX_INITIALIZER; //用于暂停的互斥锁

music::music(void *main):arg(main)
{
    mixer_fd = open("/dev/mixer",O_RDWR);
    if(mixer_fd < 0)
    {
            perror("set vol err.");

    }
    vol=90;

}

int music::set_vol()
{

       /* if(ioctl(mixer_fd,SOUND_MIXER_READ_VOLUME,&vol) < 0)
        {
                perror("read vol err.");
                return -1;
        }*/




        ////设置 音量
        if(ioctl(mixer_fd,SOUND_MIXER_WRITE_VOLUME,&vol) < 0)
        {
                perror("set vol err.");
                return -1;
        }



        return 0;
}

void music::close(int fd) //关闭dsp_fd
{
    close(fd);
}

void music::set_music(char *name) //设置音乐绝对路径
{
    this->music_name=name;
}

int music::setup_dsp(int dsp_fd,int rate,int channels)
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
int music::sync_dsp(int dsp_fd)
{
        if(ioctl(dsp_fd,SNDCTL_DSP_SYNC) != 0)
        {
                perror("ioctl sync failed");
                return -1;
        }
        return 0;
}
int music::music_open()
{
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
}
int music::play_music()
{


    MainWindow *mine=(MainWindow*)arg;
    mine->my_words();
        int music_len;
        long rate;
        char* buf;
        long n;
        int min;
        min=ftell(music_fp);
        music_len = head.ChunkSize+8;

        mine->set_line(music_len,min);

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


        if(buf != NULL)
        {
                do{

                        pthread_mutex_lock(&my_mutex);
                        pthread_mutex_unlock(&my_mutex);

                        mine->word_move();
                        n = fread(buf,1,rate,music_fp);
                        //cout<<"n:"<<n<<endl;
                        if(n > 0)
                        {

                            write(dsp_fd,buf,n);

                        }

                        if(feof(music_fp))
                        {

                            perror("play over.");
                                break;
                        }

                        mine->add_second();
                        mine->move_line(ftell(music_fp));
                        this->set_vol();
                }while(1);


        }
    return 1;

}



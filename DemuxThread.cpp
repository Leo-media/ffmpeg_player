#include "DemuxThread.h"
#include <iostream>


DemuxThread::DemuxThread(AVPacketQueue *audio_queue, AVPacketQueue *video_queue):
    audio_queue_(audio_queue), video_queue_(video_queue)
{
    std::cout<<"DemuxThread"<<std::endl;
}

DemuxThread::~DemuxThread()
{
    std::cout<<"DemuxThread"<<std::endl;
}

//初始化，接收url，成功返回0，失败返回-1
int DemuxThread::Init(const char *url)
{
    //判断url为空
    if (!url) {
        printf("%s(%d) url is null\n", __FUNCTION__, __LINE__);
        return -1;
    }
    //判断视频流队列或音频流队列是否为空
    if (!audio_queue_ || !video_queue_) {
        printf("%s(%d) audio_queue_ or video_queue_ null\n", __FUNCTION__, __LINE__);
        return -1;
    }

    url_ = url;
    //创建一个 AVFormatContext 结构体
    ifmt_ctx_ = avformat_alloc_context();

    //打开文件
    int ret = avformat_open_input(&ifmt_ctx_, url_.c_str(), NULL, NULL);
    //检查打开是否成功
    if (ret < 0) {
        //将错误码转化成字符串
        av_strerror(ret, err2str, sizeof(err2str));
        printf("%s(%d) avformat_open_input failed:%d, %s\n", __FUNCTION__, __LINE__, ret, err2str);
        return -1;
    }

    //读取流信息
    ret = avformat_find_stream_info(ifmt_ctx_, NULL);
    //检查读取是否成功
    if (ret < 0) {
        av_strerror(ret, err2str, sizeof(err2str));
        printf("%s(%d) avformat_find_stream_info failed:%d, %s\n", __FUNCTION__, __LINE__, ret, err2str);
        return -1;
    }

    //打印文件信息
    av_dump_format(ifmt_ctx_, 0, url_.c_str(), 0);

    //找到音频流和视频流的索引，成功返回index，失败返回值小于0
    audio_stream_ = av_find_best_stream(ifmt_ctx_, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    video_stream_ = av_find_best_stream(ifmt_ctx_, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    printf("%s(%d) audio_stream_:%d, video_stream_:%d\n", __FUNCTION__, __LINE__, audio_stream_, video_stream_);
    if (audio_stream_ < 0 || video_stream_) {
        printf("no audio or no video\n");
        return -1;
    }
    return 0;
}

int DemuxThread::Start()
{
    thread_ = new std::thread(&DemuxThread::Run, this);
    if (!thread_->joinable()) {
        printf("new DemuxThread failed\n");
        return -1;
    }
    return 0;
}

int DemuxThread::Stop()
{
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
    Thread::Stop();
    return 0;
}

void DemuxThread::Run()
{
    printf("进入DemuxThread类run函数");
    AVPacket packet;
    int ret = 0;
    while (1) {
        if (abort_ == 1){
            break;
        }

        //从容器中读出一个音频或视频压缩包
        ret = av_read_frame(ifmt_ctx_, &packet);
        if (ret < 0) {
            av_strerror(ret, err2str, sizeof(err2str));
            printf("%s(%d) av_read_frame failed:%s\n", __FUNCTION__, __LINE__, err2str);
            break;
        }
        //判断流类型
        if (packet.stream_index == audio_stream_) {  //音频包队列
            audio_queue_->Push(&packet);
            printf("audio pkt size:%d\n", audio_queue_->Size());
        } else if (packet.stream_index == video_stream_) {  //视频包队列
            video_queue_->Push(&packet);
            printf("video pkt size:%d\n", video_queue_->Size());
        }
        //释放内存
        av_packet_unref(&packet);
    }

    //关闭文件，释放AVFormatContext
    avformat_close_input(&ifmt_ctx_);
    printf("退出DemuxThread类run函数");
}

#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H

#include "Thread.h"
#include "AVPacketQueue.h"

extern "C" {
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
}

class DemuxThread : public Thread
{
public:
    DemuxThread(AVPacketQueue* audio_queue, AVPacketQueue* video_queue);
    virtual ~DemuxThread();

    int Init(const char* url);
    virtual int Start();
    virtual int Stop();
    virtual void Run();

    //负责给DecodeThread传递数据
    AVCodecParameters* AudioCodecParameters();
    AVCodecParameters* VideoCodecParameters();

    AVRational AudioStreamTimebase(){
        return ifmt_ctx_->streams[audio_stream_]->time_base;
    }
    AVRational VideoStreamTimebase(){
        return ifmt_ctx_->streams[video_stream_]->time_base;
    }

private:
    std::string url_;
    //i=输入  fmt=format  ctx =context
    AVFormatContext* ifmt_ctx_ = nullptr;
    //err to str  错误语句转化为字符串
    char err2str[256] = {0};

    int audio_stream_ = -1;
    int video_stream_ = -1;

    AVPacketQueue* audio_queue_ = nullptr;
    AVPacketQueue* video_queue_ = nullptr;
};

#endif // DEMUXTHREAD_H

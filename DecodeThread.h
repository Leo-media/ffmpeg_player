#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include "Thread.h"
#include "AVPacketQueue.h"
#include "AVFrameQueue.h"

class DecodeThread : public Thread
{
public:
    DecodeThread(AVPacketQueue* packet_queue, AVFrameQueue* frame_queue);
    ~DecodeThread();

    int Init(AVCodecParameters* par);
    int Start();
    int Stop();
    void Run();

private:
    char err2str[256] = {0};
    AVCodecContext* codec_ctx_ = nullptr;
    AVPacketQueue* packet_queue_ = nullptr;
    AVFrameQueue* frame_queue_ = nullptr;
};

#endif // DECODETHREAD_H

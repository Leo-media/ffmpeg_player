#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include "AVFrameQueue.h"
#include "AVSync.h"

typedef struct _AudioParams
{
    int freq;                       //frequency  采样率
    AVChannelLayout ch_layout;      //Audio Video Channel Layout  通道布局
    enum AVSampleFormat fmt;        //Audio Video Sample Format  采样格式枚举
} AudioParams;

class AudioOutput
{
public:
    AudioOutput(AVSync* avsync, const AudioParams &aduio_params, AVFrameQueue* frame_queue, AVRational time_base);
    ~AudioOutput();
    int Init();
    int DeInit();

public:
    AVFrameQueue* frame_queue_ = nullptr;
    AudioParams src_tgt_;       //解码后的源pcm格式
    AudioParams dst_tgt_;       //SDL需要的格式

    struct SwrContext* swr_ctx_ = nullptr;

    uint8_t* audio_buf1_ = nullptr;
    uint32_t audio_buf1_size = 0;       //真正分配的空间大小 audio_buf_size <= audio_buf1_size;
    uint8_t* audio_buf_ = nullptr;
    uint32_t audio_buf_size = 0;        //真正重采样后总共占用字节
    uint32_t audio_buf_index = 0;

    AVRational time_base_;
    AVSync* avsync_ = nullptr;
    double pts = 0;
};

#endif // AUDIOOUTPUT_H

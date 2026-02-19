#define SDL_MAIN_HANDLED        //禁止SDL重定义main
#include "DemuxThread.h"
#include "AVFrameQueue.h"
#include "DecodeThread.h"
#include "AudioOutput.h"
#include "VideoOutput.h"
#include "AVSync.h"

extern "C" {
#include <libavutil/avutil.h>
}

using namespace std;

int main(int argc, char *argv[])
{
    //使控制台编码为UTF8，需要include <windows>
    // SetConsoleOutputCP(CP_UTF8);

    //打印命令行参数
    printf("url:%s\n", argv[1]);

    AVPacketQueue audio_packet_queue;
    AVPacketQueue video_packet_queue;
    AVFrameQueue audio_frame_queue;
    AVFrameQueue video_frame_queue;
    AVSync avsync;

    int ret = 0;
    DemuxThread* demux_thread = new DemuxThread(&audio_packet_queue, &video_packet_queue);

    //将命令行参数作为url传入DemuxThread的初始化函数init()
    ret = demux_thread->Init(argv[1]);
    //判断是否调用函数成功（0/-1）
    if (ret < 0) {
        printf("%s(%d) demux_thread init fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = demux_thread->Start();
    if (ret < 0) {
        printf("%s(%d) demux_thread init fail\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //音频解码线程
    DecodeThread* audio_decode_thread = new DecodeThread(&audio_packet_queue, &audio_frame_queue);
    ret = audio_decode_thread->Init(demux_thread->AudioCodecParameters());
    if (ret < 0) {
        printf("%s(%d) audio_decode_thread Init\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = audio_decode_thread->Start();
    if (ret < 0) {
        printf("%s(%d) audio_decode_thread Start\n", __FUNCTION__, __LINE__);
        return -1;
    }
    //视频解码线程
    DecodeThread* video_decode_thread = new DecodeThread(&video_packet_queue, &video_frame_queue);
    ret = video_decode_thread->Init(demux_thread->VideoCodecParameters());
    if (ret < 0) {
        printf("%s(%d) video_decode_thread Init\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = video_decode_thread->Start();
    if (ret < 0) {
        printf("%s(%d) video_decode_thread Start\n", __FUNCTION__, __LINE__);
        return -1;
    }

    avsync.InitClock();
    //音频输出
    AudioParams audio_params;
    memset(&audio_params, 0, sizeof(audio_params));
    audio_params.ch_layout = audio_decode_thread->GetAVCodecContext()->ch_layout;
    audio_params.fmt = audio_decode_thread->GetAVCodecContext()->sample_fmt;
    audio_params.freq = audio_decode_thread->GetAVCodecContext()->sample_rate;
    AudioOutput* audio_output = new AudioOutput(&avsync, audio_params, &audio_frame_queue, demux_thread->AudioStreamTimebase());
    ret = audio_output->Init();
    if (ret < 0) {
        printf("%s(%d) audio_output Init\n", __FUNCTION__, __LINE__);
        return -1;
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(100000));
    //视频输出
    VideoOutput* video_output = new VideoOutput(&avsync, &video_frame_queue, video_decode_thread->GetAVCodecContext()->width,
                                                video_decode_thread->GetAVCodecContext()->height, demux_thread->VideoStreamTimebase());
    ret = video_output->Init();
    if (ret < 0) {
        printf("%s(%d) video_output Init\n", __FUNCTION__, __LINE__);
        return -1;
    }
    video_output->MainLoop();

    printf("%s(%d) audio_output_thread delete\n", __FUNCTION__, __LINE__);
    audio_output->DeInit();
    delete audio_output;
    printf("%s(%d) demux_thread delete\n", __FUNCTION__, __LINE__);
    demux_thread->Stop();
    delete demux_thread;
    printf("%s(%d) audio_decode_thread delete\n", __FUNCTION__, __LINE__);
    audio_decode_thread->Stop();
    delete audio_decode_thread;
    printf("%s(%d) video_decode_thread delete\n", __FUNCTION__, __LINE__);
    video_decode_thread->Stop();
    delete video_decode_thread;
    printf("main finish");
    return 0;
}

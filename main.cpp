#include "DemuxThread.h"
#include "AVFrameQueue.h"
#include "DecodeThread.h"
#include <windows.h>

extern "C" {
#include <libavutil/avutil.h>
}

using namespace std;

int main(int argc, char *argv[])
{
    //使控制台编码为UTF8
    SetConsoleOutputCP(CP_UTF8);

    //打印命令行参数
    printf("url:%s\n", argv[1]);

    AVPacketQueue audio_packet_queue;
    AVPacketQueue video_packet_queue;
    AVFrameQueue audio_frame_queue;
    AVFrameQueue video_frame_queue;

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

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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

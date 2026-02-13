#include "DemuxThread.h"
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

    AVPacketQueue audio_queue;
    AVPacketQueue video_queue;

    int ret = 0;
    DemuxThread* demux_thread = new DemuxThread(&audio_queue, &video_queue);

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
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    demux_thread->Stop();
    delete demux_thread;
    printf("main finish");
    return 0;
}

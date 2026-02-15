#ifndef VIDEOOUTPUT_H
#define VIDEOOUTPUT_H

#include "AVFrameQueue.h"
//支持SSE3，防止包含SDL.h编译失败
#define __SSE3__ 1
extern "C" {
#include <SDL.h>
}

class VideoOutput
{
public:
    VideoOutput(AVFrameQueue* frame_queue, int video_width, int video_height);
    ~VideoOutput();
    int Init();
    int MainLoop();
    void RefreshLoopWaitEvent(SDL_Event* event);

private:
    void videoRefresh();
    AVFrameQueue* frame_queue_ = nullptr;
    SDL_Window* win_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* texture_ = nullptr;

    int video_width_ = 0;
    int video_height_ = 0;
};

#endif // VIDEOOUTPUT_H

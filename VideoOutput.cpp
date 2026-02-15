#include "VideoOutput.h"
#include <thread>

VideoOutput::VideoOutput(AVFrameQueue *frame_queue, int video_width, int video_height):
    frame_queue_(frame_queue), video_width_(video_width), video_height_(video_height)
{

}

VideoOutput::~VideoOutput()
{

}

int VideoOutput::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed\n");
        return -1;
    }
    win_ = SDL_CreateWindow("player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            video_width_, video_height_, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!win_) {
        printf("SDL_CreateWindow failed\n");
        return -1;
    }
    renderer_ = SDL_CreateRenderer(win_, -1, 0);
    if (!renderer_) {
        printf("SDL_CreateRender failed\n");
        return -1;
    }
    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, video_width_, video_height_);
    if (!texture_) {
        printf("SDL_CreateRenderer failed\n");
        return -1;
    }
    return 0;
}

int VideoOutput::MainLoop()
{
    SDL_Event event;
    while (true) {
        RefreshLoopWaitEvent(&event);
        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    printf("esc key down\n");
                    return 0;
                }
                break;
            case SDL_QUIT:
                printf("SDL_QUIT\n");
                return 0;
                break;
            default:
                break;
        }
    }
    return 0;
}

void VideoOutput::RefreshLoopWaitEvent(SDL_Event *event)
{
    SDL_PumpEvents();
    while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        videoRefresh();
        SDL_PumpEvents();
    }
}

void VideoOutput::videoRefresh()
{
    AVFrame* frame = nullptr;
    frame = frame_queue_->Front();
    if (frame) {
        printf("video pts:%ld\n", frame->pts);
        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = video_width_;
        rect.h = video_height_;
        SDL_UpdateYUVTexture(texture_, &rect, frame->data[0], frame->linesize[0],
                             frame->data[1], frame->linesize[1],
                             frame->data[2], frame->linesize[2]);
        SDL_RenderClear(renderer_);
        SDL_RenderCopy(renderer_, texture_, NULL, &rect);
        SDL_RenderPresent(renderer_);
        frame = frame_queue_->Pop(1);
        av_frame_free(&frame);
    }
}

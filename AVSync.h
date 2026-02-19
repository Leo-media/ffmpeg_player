#ifndef AVSYNC_H
#define AVSYNC_H

#include <chrono>
#include <ctime>
#include <math.h>

class AVSync
{
public:
    AVSync(){}
    ~AVSync(){}

    void InitClock(){
        SetClock(NAN);
    }

    //audio pts
    void SetClock(double pts){
        double time = GetMicroseconds() / 1000000.0;    //秒
        pts_drift_ = pts - time;
    }
    //由video刷新获取
    double GetClock(){
        double time = GetMicroseconds() / 1000000.0;
        return pts_drift_ + time;
    }

    //获取物理时间
    time_t GetMicroseconds(){
        std::chrono::system_clock::time_point time_point_new = std::chrono::system_clock::now();
        std::chrono::system_clock::duration duration = time_point_new.time_since_epoch();
        std::time_t us = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        return us;
    }
    double pts_drift_ = 0;
};

#endif // AVSYNC_H

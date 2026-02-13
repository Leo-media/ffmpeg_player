#ifndef THREAD_H
#define THREAD_H

#include <thread>

class Thread
{
public:
    Thread();
    virtual ~Thread();

    int Start();
    int Stop();
    virtual void Run() = 0;

protected:
    int abort_ = 0;
    std::thread* thread_ = nullptr;
};

#endif // THREAD_H

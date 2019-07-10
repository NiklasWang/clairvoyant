#ifndef _FRAME_GENERATOR_H_
#define _FRAME_GENERATOR_H_

#include "ThreadT.h"

namespace pandora {

class Clock {
public:
    class Ms {
    public:
        Ms(int32_t ms);
        bool valid();
        int32_t operator()();
        Ms operator=(const int32_t ms);
        bool operator==(const Ms &rhs);
        bool operator==(const int32_t &rhs);
    private:
        int32_t ms;
    };

    class Us {
    public:
        Us(int32_t us);
        bool valid();
        int32_t operator()();
        Us operator=(const int32_t us);
        bool operator==(const Us &rhs);
        bool operator==(const int32_t &rhs);
    private:
        int32_t us;
    };
};

class Timer :
    public noncopyable {
public:
    int32_t start(std::function<int32_t ()>);
    int32_t start(Clock::Ms ms, std::function<int32_t (Clock::Ms)>);
    int32_t start(Clock::Us us, std::function<int32_t (Clock::Us)>);
    int32_t repeat(std::function<int32_t ()>);
    int32_t repeat(Clock::Us us, std::function<int32_t (Clock::Ms)>);
    int32_t repeat(Clock::Us us, std::function<int32_t (Clock::Us)>);
    int32_t stop();
    int32_t reset();
    int32_t update(Clock::Ms ms);
    int32_t update(Clock::Us us);

public:
    Timer();
    Timer(Clock::Ms ms);
    Timer(Clock::Us us);
    ~Timer();

public:
    bool operator==(const Clock::Ms &ms);
    bool operator==(const Clock::Us &us);
    bool operator!=(const Clock::Ms &ms);
    bool operator!=(const Clock::Us &us);

private:
    int32_t runClock(Clock::Us &us);
    static void *runClock(void *thiz);
    int32_t replaceThread();
    static void *replaceThread(void *thiz);

private:
    enum TimerStatus {
        TIMER_STOPPED,
        TIMER_STARTED,
        TIMER_REMOVED
    };

private:
    Clock::Ms    mMs;
    Clock::Us    mUs;
    std::function<int32_t ()> mCb;
    ThreadT     *mThread;
    TimerStatus *mStat;
    ThreadT     *mReleaseThread;
};

class FrameGenerator :
    public noncopyable {
public:
    FrameGenerator();
    ~FrameGenerator();

public:
    int32_t start(Clock::Ms ms, std::function<int32_t (Clock::Ms)>);
    int32_t start(Clock::Us us, std::function<int32_t (Clock::Us)>);
    int32_t repeat(Clock::Us us, std::function<int32_t (Clock::Ms)>);
    int32_t repeat(Clock::Us us, std::function<int32_t (Clock::Us)>);
    int32_t stop();

private:
    Timer mTimer;
};

};

#endif


#include "FrameGenerator.h"

namespace pandora {

Clock::Ms::Ms(int32_t t)
{
    ASSERT_LOG(MODULE_UTILS, t > 0, "time %d ms is invalid", t);
    ms = t;
}

bool Clock::Ms::valid()
{
    return ms > 0;
}

int32_t Clock::Ms::operator()()
{
    return ms;
}

Clock::Ms Clock::Ms::operator=(const int32_t t)
{
    ASSERT_LOG(MODULE_UTILS, t > 0, "time %d ms is invalid", t);
    ms = t;
    return *this;
}

bool Clock::Ms::operator==(const Ms &rhs)
{
    bool result = false;

    if (rhs.ms == ms) {
        result = true;
    }

    return result;
}

bool Clock::Ms::operator==(const int32_t &rhs)
{
    bool result = false;

    if (rhs == ms) {
        result = true;
    }

    return result;
}

Clock::Us::Us(int32_t t)
{
    ASSERT_LOG(MODULE_UTILS, t > 0, "time %d us is invalid", t);
    us = t;
}

bool Clock::Us::valid()
{
    return us > 0;
}

int32_t Clock::Us::operator()()
{
    return us;
}

Clock::Us operator=(const int32_t t)
{
    ASSERT_LOG(MODULE_UTILS, t > 0, "time %d us is invalid", t);
    us = t;
    return *this;
}

bool Clock::Us::operator==(const Clock::Us &rhs)
{
    bool result = false;

    if (rhs.us == us) {
        result = true;
    }

    return result;
}

bool Clock::Us::operator==(const int32_t &rhs)
{
    bool result = false;

    if (rhs == us) {
        result = true;
    }

    return result;

}

#define CONSTRUCT_TIMER_THREAD() \
    ({ \
        int32_t _rc = NO_ERROR; \
        mThread = new ThreadT(); \
        _rc = mThread->construct(); \
        if (!SUCCEED(_rc)) { \
            LOGE(MODULE_UTILS, "Failed to construct thread, %d", _rc); \
            mThread = NULL; \
        } else { \
            mStat = (int32_t *)Malloc(sizeof(TimerStatus)); \
            if (ISNULL(mStat)) { \
                LOGE(MODULE_UTILS, "insufficient memory %d bytes.", \
                    sizeof(TimerStatus)); \
                _rc = NO_MEMORY; \
            } else { \
                *mStat = TIMER_STOPPED; \
            } \
        } \
        _rc;
    })

Timer::Timer() :
    mMs(0),
    mUs(0),
    mStat(NULL),
    mThread(NULL)
{
    CONSTRUCT_TIMER_THREAD();
}

Timer::Timer(Clock::Ms ms) :
    mMs(ms),
    mUs(0),
    mStat(NULL),
    mThread(NULL)
{
    CONSTRUCT_TIMER_THREAD();
}

Timer::Timer(Clock::Us us) :
    mMs(0),
    mUs(us),
    mStat(NULL),
    mThread(NULL)
{
    CONSTRUCT_TIMER_THREAD();
}

Timer::~Timer()
{
    int32_t rc = NO_ERROR;
    TimerStatus stat = *mStat;

    *mStat = TIMER_REMOVED;
    if (stat == TIMER_STOPPED) {
        if (NOTNULL(mThread)) {
            rc = mThread->destruct();
            if (!SUCCEED(rc)) {
                LOGE(MODULE_UTILS, "Failed to destruct thread, %d", rc);
            }
        }
    }

    SECURE_DELETE(mThread);
    SECURE_FREE(mStat);

    if (NOTNULL(mReleaseThread)) {
        rc = mReleaseThread->destruct();
        if (!SUCCEED(rc)) {
            LOGE(MODULE_UTILS, "Failed to destruct thread, %d", rc);
        }
        SECURE_DELETE(mThread);
    }
}

int32_t Timer::start(std::function<int32_t ()> notify);
int32_t Timer::start(Clock::Ms ms, std::function<int32_t (Clock::Ms)> notify);
int32_t Timer::start(Clock::Us us, std::function<int32_t (Clock::Us)> notify)
{
    int32_t rc = NO_ERROR;
    TimerStatus *stat = mStat;
    ThreadT *thread = mThread;

    if (!us.valid()) {
        LOGE(MODULE_UTILS, "Time %d us not valid, %d", us());
        rc = PARAM_INVALID;
    }

    if (SUCCEED(rc)) {
        if (*stat == TIMER_STARTED) {
            LOGE(MODULE_UTILS, "Timer started, will stop first.");
            rc = stop();
            if (!SUCCEED(rc)) {
                LOGE(MODULE_UTILS, "Failed to stop timer, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (*mStat != TIMER_REMOVED) {
            *mStat = TIMER_STARTED;
        } else {
            LOGE(MODULE_UTILS, "Can't start timer in removed status.");
            rc = USER_ABORTED;
        }
    }

    if (SUCCEED(rc)) {
        if (*this != us) {
            mUs = us;
            mMs = 0;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mThread)) {
            rc = CONSTRUCT_TIMER_THREAD();
            if (!SUCCEED(rc)) {
                LOGE(MODULE_UTILS, "Failed to construct thread, %d", rc);
            } else {
                thread = mThread;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mThread->run(
            [this]() -> int32_t {
                int32_t rc = runClock(us);
                if (!SUCCEED(rc)) {
                    LOGE(MODULE_UTILS, "Failed to run clock, %d", rc);
                }

                if (!*abort) {
                    notify(us);
                }

                return rc;
            }
        );
    }

// run this in new thread, create it when necessary
    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (*mStat == TIMER_REMOVED) {
            if (NOTNULL(mThread)) {
                int32_t rc = mThread->destruct();
                if (!SUCCEED(rc)) {
                    LOGE(MODULE_UTILS, "Failed to destruct thread, %d", rc);
                }
                SECURE_DELETE(mThread);
                SECURE_FREE(abort);
            }
        }
    }

    return rc;
}

int32_t Timer::runClock(Clock::Us &us)
{
    int32_t rc = NO_ERROR;

    struct timeval tv;
    tv.tv_sec = us() / 1e9;
    tv.tv_usec = us() % 1e9;

    LOGD(MODULE_UTILS, "+ Will sleep for %d.%9ds.", tv.tv_sec, tv.tv_usec);

    do {
        rc = select(0, NULL, NULL, NULL, &tv);
    } while(rc < 0 && errno == EINTR)

    if (!SUCCEED(rc)) {
        LOGE(MODULE_UTILS, "Failed to run select, %d", rc);
    } else {
        LOGD(MODULE_UTILS, "- Wake up after %d.%9ds.", tv.tv_sec, tv.tv_usec);
    }

    return rc;
}

int32_t Timer::repeat(std::function<int32_t ()> notify);
int32_t Timer::repeat(Clock::Ms us, std::function<int32_t (Clock::Ms)> notify);
int32_t Timer::repeat(Clock::Us us, std::function<int32_t (Clock::Us)> notify);

int32_t Timer::replaceThread()
{
    int32_t rc = NO_ERROR;
    pthread_t tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&tid, &attr, replaceThread, this)) {
        LOGE(MODULE_UTILS, "Failed to create thread");
        rc = UNKNOWN_ERROR;
    }

    return rc;
}

void *Timer::replaceThread(void *thiz)
{
    int32_t rc = NO_ERROR;
    Timer *pme = static_cast<Timer *>(thiz);
    ThreadT *thread = pme->mThread;
    int32_t *abort = pme->mAbort;

    if (ISNULL(pme)) {
        LOGE(MODULE_UTILS, "NULL arg received.");
        rc = PARAM_INVALID;
    }

    if (ISNULL(thread)) {
        LOGE(MODULE_UTILS, "thread not created.");
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        pme->mThread = new ThreadT();
        rc = pme->mThread->construct();
        if (!SUCCEED(rc)) {
            LOGE(MODULE_UTILS, "Failed to construct thread, %d", rc);
            pme->mThread = NULL;
        } else {
            *abort = TIMER_REMOVED;
            pme->mStat = (int32_t *)Malloc(sizeof(int32_t));
            if (ISNULL(pme->mStat)) {
                LOGE(MODULE_UTILS, "insufficient memory %d bytes.",
                    sizeof(int32_t));
                _rc = NO_MEMORY;
            } else {
                *pme->mStat = TIMER_STOPPED;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = thread->destruct();
        if (!SUCCEED(rc)) {
            LOGE(MODULE_UTILS, "Failed to destruct original thread, %d", rc);
        }
        SECURE_DELETE(thread);
        SECURE_FREE(abort);
    }

    pthread_exit(NULL);

    return NULL;
}

int32_t Timer::stop()
{
    int32_t rc = NO_ERROR;

    if (*mStat == TIMER_STARTED) {
        rc = replaceThread();
        if (!SUCCEED(rc)) {
            LOGE(MODULE_UTILS, "Failed to replace thread, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        *mStat = TIMER_STOPPED;
    }

    return rc;
}

int32_t Timer::reset()
{
    return stop();
}

int32_t Timer::update(Clock::Ms ms)
{
    mMs = ms;
    mUs = 0;
}

int32_t Timer::update(Clock::Us us)
{
    mUs = us;
    mMs = 0;
}

bool Timer::operator==(const Clock::Ms &ms)
{
    bool result = false;

    if (mUs == 0 && mMs == ms) {
        result = true;
    }

    return result;
}

bool Timer::operator==(const Clock::Us &us)
{
    bool result = false;

    if (mMs == 0 && mUs == us) {
        result = true;
    }

    return result;
}

bool Timer::operator!=(const Clock::Ms &ms)
{
    return !(*this == ms);
}

bool Timer::operator!=(const Clock::Us &us)
{
    return !(*this == us);
}

Timer mTimer;

FrameGenerator::FrameGenerator();
FrameGenerator::~FrameGenerator();
int32_t FrameGenerator::start(Clock::Ms ms, std::function<int32_t (Clock::Ms)>);
int32_t FrameGenerator::start(Clock::Us us, std::function<int32_t (Clock::Us)>);
int32_t FrameGenerator::repeat(Clock::Us us, std::function<int32_t (Clock::Ms)>);
int32_t FrameGenerator::repeat(Clock::Us us, std::function<int32_t (Clock::Us)>);
int32_t FrameGenerator::stop();

};

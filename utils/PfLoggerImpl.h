#ifndef PERFORMANCE_LOGGER_IMPL_H_
#define PERFORMANCE_LOGGER_IMPL_H_

#include <list>

#include "Common.h"
#include "RefBase.h"

namespace pandora {

#define INT64_INVALID_VALUE -1LL

class PfLoggerImpl {
public:
    int32_t start(const char *name, uint32_t times = 0);
    int32_t stop(const char *name);
    int32_t output();
    int32_t output(const char *name);
    void clear();
    int32_t clear(const char *name);

public:
    PfLoggerImpl();
    ~PfLoggerImpl() = default;

private:
    PfLoggerImpl(const PfLoggerImpl &rhs) = delete;

private:
    class Timer;
    sp<Timer> findTimer(const char *name);

private:
    class Timer :
        public RefBase {
    public:
        struct Time {
            int64_t usec;
            Time(int64_t u = INT64_INVALID_VALUE);
            int64_t update();
            int64_t operator()();
            Time operator-(const Time &rhs);
            Time operator-=(const Time &rhs);
            Time operator+(const Time &rhs);
            Time operator+=(const Time &rhs);
            Time operator/(const Time &rhs);
        };
        struct Set :
            public RefBase {
            Time start;
            Time stop;
            Time diff;
            Set();
            int64_t operator()();
            void output();
        };
        Timer(const char *n = "noname", uint32_t times = 0);
        int64_t operator()();
        void output();
        void clear();
        bool finished();
        std::list<sp<Set> > mSet;
        const char *name;
        uint32_t    times;
        bool        printed;
        int32_t     id;
        ModuleType  module;
        static int32_t cnt;
    };

private:
    ModuleType       mModule;
    std::list<sp<Timer> > mTimers;
};

};

#endif

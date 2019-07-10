#include "PfLogger.h"
#include "PfLoggerImpl.h"

namespace pandora {

int64_t getNowUs();

int32_t PfLoggerImpl::Timer::cnt = 0;

PfLoggerImpl::PfLoggerImpl() :
    mModule(MODULE_PERFORMANCE_CHECKER)
{
}

int32_t PfLoggerImpl::start(const char *name, uint32_t times)
{
    int32_t rc = NO_ERROR;
    sp<Timer> timer = findTimer(name);

    if (ISNULL(timer)) {
        timer = new Timer(name, times);
        if (NOTNULL(timer)) {
            mTimers.push_back(timer);
        } else {
            LOGE(mModule, "Failed to create new timer");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc) && NOTNULL(timer)) {
        if (!timer->finished()) {
            sp<Timer::Set> set = new Timer::Set();
            timer->mSet.push_back(set);
        }
    }

    return rc;
}

int32_t PfLoggerImpl::stop(const char *name)
{
    int32_t rc = NO_ERROR;
    sp<Timer> timer = findTimer(name);
    sp<Timer::Set> set = NULL;

    if (ISNULL(timer)) {
        LOGE(mModule, "Not found such test item %s", name);
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        for (auto &s : timer->mSet) {
            if (s->stop() == 0LL) {
                set = s;
                break;
            }
        }
        if (ISNULL(set)) {
            if (!timer->finished()) {
                LOGE(mModule, "Didn't found a started timer for %s", name);
                rc = NOT_INITED;
            }
        }
    }

    if (SUCCEED(rc) && NOTNULL(set)) {
        set->stop.update();
        (*set)();
        LOGI(mModule, "Stop testing %s, started at %lld, " \
            "stopped at %lld, consumed %.3f ms",
            name, set->start(), set->stop(), (1.0 * (*set)() / 1000));
    }

    if (SUCCEED(rc)) {
        if (timer->finished() && !timer->printed) {
            timer->output();
        }
    }

    return rc;
}

sp<PfLoggerImpl::Timer> PfLoggerImpl::findTimer(const char *name)
{
    sp<Timer> timer = NULL;

    for (auto &t : mTimers) {
        if (NOTNULL(t)) {
            if (!strcmp(t->name, name)) {
                timer = t;
            }
        }
    }

    return timer;
}

int32_t PfLoggerImpl::output()
{
    int32_t index = 0;

    LOGI(mModule, " > Dump all records in performance logger");
    for (auto &t : mTimers) {
        if (NOTNULL(t)) {
            index++;
            t->output();
        }
    }
    LOGI(mModule, " > Finished display all %d records", index);

    return NO_ERROR;
}

int32_t PfLoggerImpl::output(const char *name)
{
    int32_t rc = NO_ERROR;
    sp<Timer> timer = findTimer(name);

    if (NOTNULL(timer)) {
        timer->output();
    } else {
        LOGE(mModule, "Didn't found record %s", name);
        rc = NOT_INITED;
    }

    return rc;
}

void PfLoggerImpl::clear()
{
    while (mTimers.begin() != mTimers.end()) {
        mTimers.erase(mTimers.begin());
    }
}

int32_t PfLoggerImpl::clear(const char *name)
{
    int32_t rc = NOT_INITED;
    sp<Timer> timer = findTimer(name);

    if (NOTNULL(timer)) {
        timer->clear();
        rc = NO_ERROR;
    }

    return rc;
}

PfLoggerImpl::Timer::Time::Time(int64_t u) :
    usec(u)
{
    if (usec == INT64_INVALID_VALUE) {
        usec = getNowUs();
    }
}

int64_t PfLoggerImpl::Timer::Time::update()
{
    return usec = getNowUs();
}

int64_t PfLoggerImpl::Timer::Time::operator()()
{
    return usec;
}

PfLoggerImpl::Timer::Time
    PfLoggerImpl::Timer::Time::operator-(
    const PfLoggerImpl::Timer::Time &rhs)
{
    return Time(usec - rhs.usec);
}

PfLoggerImpl::Timer::Time
    PfLoggerImpl::Timer::Time::operator-=(
    const PfLoggerImpl::Timer::Time &rhs)
{
    return *this = *this - rhs;
}

PfLoggerImpl::Timer::Time
    PfLoggerImpl::Timer::Time::operator+(
    const PfLoggerImpl::Timer::Time &rhs)
{
    return Time(usec + rhs.usec);
}

PfLoggerImpl::Timer::Time
    PfLoggerImpl::Timer::Time::operator+=(
    const PfLoggerImpl::Timer::Time &rhs)
{
    return *this = *this + rhs;
}

PfLoggerImpl::Timer::Time
    PfLoggerImpl::Timer::Time::operator/(
    const PfLoggerImpl::Timer::Time &rhs)
{
    return Time(usec / rhs.usec);
}

PfLoggerImpl::Timer::Set::Set() :
    start(INT64_INVALID_VALUE),
    stop(0LL)
{
}

int64_t PfLoggerImpl::Timer::Set::operator()()
{
    diff = stop - start;
    return diff();
}

void PfLoggerImpl::Timer::Set::output()
{
    LOGI(MODULE_PERFORMANCE_CHECKER,
        " >>>> start time %lld, stop time %lld, diff time %.3f ms",
        start(), stop(), 1.0 * (*this)() / 1000);
}

PfLoggerImpl::Timer::Timer(const char *n, uint32_t t) :
    name(n), times(t), printed(false),
    id(cnt++), module(MODULE_PERFORMANCE_CHECKER)
{
}

int64_t PfLoggerImpl::Timer::operator()()
{
    int32_t index = 0;
    Time total = 0LL;

    for (auto &t : mSet) {
        if (NOTNULL(t)) {
            index++;
            total += (*t)();
        }
    }

    return index ? (total / index)() : 0LL;
}

void PfLoggerImpl::Timer::output()
{
    int32_t index = 0;
    Time total = 0LL;

    LOGI(module, " >> Dump performance record %s", name);
    for (auto &t : mSet) {
        if (NOTNULL(t)) {
            index++;
            total += (*t)();
            //t->output();
        }
    }

    clear();
    printed = true;

    LOGI(module, " >> Finished display record %s, " \
        "tested %d times, average %.3f ms", name, index,
        index ? (1.0 * (total / index)() / 1000) : 0.0f);
}

void PfLoggerImpl::Timer::clear()
{
    while (mSet.begin() != mSet.end()) {
        mSet.erase(mSet.begin());
    }
}

bool PfLoggerImpl::Timer::finished()
{
    return times ? (printed || (mSet.size() >= times)) : false;
}

};


#include <unistd.h>

#include "FDTrackerImpl.h"
#include "RefBase.h"
#include "Logs.h"

namespace pandora {

FDTrackerImpl::FDInfo::FDInfo(int f) :
    fd(f)
{
}

FDTrackerImpl::FDInfo::FDInfo(int f, const char *fun,
    uint32_t l, pthread_t t, const char *fil, char *c) :
    fd(f),
    func(fun),
    line(l),
    thread(t),
    file(fil)
{
    if (!ISNULL(c)) {
        comment = c;
    }
}

bool FDTrackerImpl::FDInfo::operator==(int f)
{
    bool rc = false;

    if (f == fd) {
        rc = true;
    }

    return rc;
}

bool FDTrackerImpl::FDInfo::operator==(const char *f)
{
    bool rc = false;

    if (ISNULL(f) && !func.length()) {
        rc = true;
    } else if (!strcmp(f, func.c_str())) {
        rc = true;
    }

    return rc;
}

bool FDTrackerImpl::FDInfo::operator==(pthread_t t)
{
    bool rc = false;

    if (t == thread) {
        rc = true;
    }

    return rc;
}

bool FDTrackerImpl::FDInfo::operator==(const FDTrackerImpl::FDInfo &rhs) const
{
    bool rc = false;

    if (rhs.fd == fd &&
        !strcmp(rhs.func.c_str(), func.c_str()) &&
        rhs.thread == thread &&
        rhs.line == line) {
        rc = true;
    }

    return rc;
}

void FDTrackerImpl::FDInfo::header()
{
    LOGI(MODULE_FD_TRACKER,
        "  FD   |  Line | Function | File | Thread ID | Comments");
}

void FDTrackerImpl::FDInfo::dump(const char *header)
{
    std::string fmt(header);
    fmt += "%6d : %6d | %s | %s | %lu | %s";
    LOGI(MODULE_FD_TRACKER, fmt.c_str(), fd, line,
        func.c_str(), file.c_str(), thread, comment.c_str());
}

FDTrackerImpl::FDTrackerImpl() :
    mDebug(false),
    mModule(MODULE_FD_TRACKER),
    mSize(0)
{
}

FDTrackerImpl::~FDTrackerImpl()
{
}

int32_t FDTrackerImpl::add(int fd, const char *func, uint32_t line,
    pthread_t thread, const char *file, char *comment)
{
    sp<FDInfo> info = new FDInfo(
        fd, func, line, thread, file, comment);

    if (fd > 0) {
        if (mDebug) {
            info->dump("New tracked fd ");
        }

        RWLock::AutoWLock l(mDbLock);
        mDb.push_back(info);
        updateSize();
    } else {
        info->dump("ERROR: Invalid fd to track ");
    }

    return NO_ERROR;
}

uint32_t FDTrackerImpl::updateSize()
{
    return mSize = mDb.size();
}

bool FDTrackerImpl::compare(FDInfo &orig, int32_t type, ...)
{
    bool rc = false;

    va_list va;
    va_start(va, type);

    switch (type) {
        case COMPARE_ALWAYS: {
            rc = true;
        } break;
        case COMPARE_FD: {
            int32_t fd = va_arg(va, int32_t);
            rc = (orig == fd);
        } break;
        case COMPARE_FUNC: {
            const char * func = va_arg(va, const char *);
            rc = (orig == func);
        } break;
        case COMPARE_FUNC_LINE: {
            const char * func = va_arg(va, const char *);
            uint32_t line = va_arg(va, uint32_t);
            rc = (orig == func) && (orig.line == line);
        } break;
        case COMPARE_THREAD: {
            pthread_t thread = va_arg(va, pthread_t);
            rc = (orig == thread);
        } break;
        default : {
            rc = false;
        } break;
    }

    va_end(va);

    return rc;
}

#define TRAVERSE_ACTION(type, arg1, arg2, remove, output, cnt) \
    do { \
        for (auto iter = mDb.begin(); iter != mDb.end();) { \
            bool __rc = compare(**iter, type, arg1, arg2); \
            if (__rc) { \
                cnt ++; \
                if ((mDebug && remove) || !remove) { \
                    (*iter)->dump(output); \
                } \
            } \
            if (__rc && remove) { \
                iter = mDb.erase(iter); \
            } else { \
                iter ++; \
            } \
        } \
    } while(0)

int32_t FDTrackerImpl::clear()
{
    uint32_t cnt = 0;

    RWLock::AutoWLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_ALWAYS, 0, 0,
        REMOVE_YES, "Clear tracked fd ", cnt);
    updateSize();

    return cnt;
}

int32_t FDTrackerImpl::remove(int fd, const char *func, uint32_t line)
{
    uint32_t cnt = 0;

    RWLock::AutoWLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_FD, fd, 0,
        REMOVE_YES, "Remove tracked fd ", cnt);
    if (!cnt) {
        LOGE(mModule, "Can't stop tracking a untracked fd %d | %s:%d",
            fd, func, line);
    }
    updateSize();

    return cnt;
}

int32_t FDTrackerImpl::remove(const char *func)
{
    uint32_t cnt = 0;

    RWLock::AutoWLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_FUNC, func, 0,
        REMOVE_YES, "Remove tracked fd ", cnt);
    if (!cnt) {
        LOGE(mModule, "Didn't found any tracked fd in function %s", func);
    }
    updateSize();

    return cnt;
}

int32_t FDTrackerImpl::remove(const char *func, uint32_t line)
{
    uint32_t cnt = 0;

    RWLock::AutoWLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_FUNC_LINE, func, line,
        REMOVE_YES, "Remove tracked fd ", cnt);
    if (!cnt) {
        LOGE(mModule, "Didn't found any tracked fd in function %s line %d",
            func, line);
    }
    updateSize();

    return cnt;
}

int32_t FDTrackerImpl::remove(pthread_t thread)
{
    uint32_t cnt = 0;

    RWLock::AutoWLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_THREAD, thread, 0,
        REMOVE_YES, "Remove tracked fd ", cnt);
    if (!cnt) {
        LOGE(mModule, "Didn't found any tracked fd in thread id %lu", thread);
    }
    updateSize();

    return cnt;
}

int32_t FDTrackerImpl::output()
{
    uint32_t cnt = 0;

    FDInfo::header();
    RWLock::AutoRLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_ALWAYS, 0, 0,
        REMOVE_NO, "Tracking fd ", cnt);
    updateSize();

    return cnt;
}

int32_t FDTrackerImpl::output(int fd)
{
    uint32_t cnt = 0;

    FDInfo::header();
    RWLock::AutoRLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_FD, fd, 0,
        REMOVE_NO, "Tracking fd ", cnt);
    updateSize();

    return cnt;
}

int32_t FDTrackerImpl::output(const char *func)
{
    uint32_t cnt = 0;

    FDInfo::header();
    RWLock::AutoRLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_FUNC, func, 0,
        REMOVE_NO, "Tracking fd ", cnt);
    updateSize();

    return cnt;
}

int32_t FDTrackerImpl::output(const char *func, uint32_t line)
{
    uint32_t cnt = 0;

    FDInfo::header();
    RWLock::AutoRLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_FUNC_LINE, func, line,
        REMOVE_NO, "Tracking fd ", cnt);
    updateSize();

    return cnt;
}

int32_t FDTrackerImpl::output(pthread_t thread)
{
    uint32_t cnt = 0;

    FDInfo::header();
    RWLock::AutoRLock l(mDbLock);
    TRAVERSE_ACTION(COMPARE_THREAD, thread, 0,
        REMOVE_NO, "Tracking fd ", cnt);
    updateSize();

    return cnt;
}

uint32_t FDTrackerImpl::size()
{
    return mSize;
}

int32_t FDTrackerImpl::forceRecycle()
{
    uint32_t cnt = 0;

    RWLock::AutoWLock l(mDbLock);

    auto iter = mDb.begin();
    while(iter != mDb.end()) {
        (*iter)->dump("Warning: Force recycle fd ");
        if (close((**iter).fd) == NO_ERROR) {
            cnt ++;
        } else {
            LOGE(mModule, "Failed to force recycle fd %d", (**iter).fd);
        }
        iter = mDb.erase(iter);
    }

    updateSize();

    return cnt;
}

};


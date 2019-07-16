#ifndef FD_TRACKER_IMPL_H_
#define FD_TRACKER_IMPL_H_

#include <list>
#include <string>

#include "Common.h"
#include "FDTracker.h"
#include "Modules.h"
#include "RefBase.h"
#include "RWLock.h"

namespace pandora {

class FDTrackerImpl :
    virtual public noncopyable {

public:
    int32_t add(int fd, const char *func, uint32_t line,
        pthread_t thread, const char *file, char *comment);

    int32_t remove(int fd, const char *func, uint32_t line);
    int32_t remove(const char *func);
    int32_t remove(const char *func, uint32_t line);
    int32_t remove(pthread_t thread);
    int32_t clear();

    int32_t output();
    int32_t output(int fd);
    int32_t output(const char *func);
    int32_t output(const char *func, uint32_t line);
    int32_t output(pthread_t thread);

    uint32_t size();
    int32_t forceRecycle();

public:
    FDTrackerImpl();
    ~FDTrackerImpl();

private:
    struct FDInfo :
        public RefBase {
        int32_t     fd;
        std::string func;
        uint32_t    line;
        pthread_t   thread;
        std::string file;
        std::string comment;
    public:
        FDInfo(int f);
        FDInfo(int f, const char *fun, uint32_t l,
        pthread_t t, const char *fil, char *c);
        bool operator==(int fd);
        bool operator==(const char *func);
        bool operator==(pthread_t thread);
        bool operator==(const FDInfo &rhs) const;
        static void header();
        void dump(const char *head = "");
    };

private:
    enum CompareType {
        COMPARE_ALWAYS,
        COMPARE_FD,
        COMPARE_FUNC,
        COMPARE_FUNC_LINE,
        COMPARE_THREAD,
    };

    enum RemoveType {
        REMOVE_NO,
        REMOVE_YES,
    };

    bool compare(FDInfo &fd, int32_t type, ...);
    uint32_t updateSize();

private:
    bool       mDebug;
    ModuleType mModule;
    uint32_t   mSize;
    RWLock     mDbLock;
    std::list<sp<FDInfo> > mDb;
};

};

#endif

#ifndef FD_TRACKER_H
#define FD_TRACKER_H

#include <stdint.h>
#include <pthread.h>

namespace pandora {

#ifndef COMPILE_C_ONLY

class FDTrackerImpl;

class FDTracker {
public:
    static FDTracker *getInstance();

public:
    int32_t add(int fd, const char *func = "", uint32_t line = 0,
        pthread_t thread = 0, const char *file = "", char *comment = NULL);

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
    int32_t showLeakage();
    int32_t forceRecycle();

private:
    FDTracker();
    FDTracker(const FDTracker &rhs);
    FDTracker &operator=(const FDTracker &rhs);

public:
    ~FDTracker();

private:
    FDTrackerImpl *impl;
    static FDTracker *gInstance;
};

#endif

#ifndef COMPILE_C_ONLY
extern "C" {
#endif

int32_t addFDTracker(int fd, const char *func, uint32_t line,
        pthread_t thread, const char *file, char *comment);

int32_t removeFDTrackerByFD(int fd, const char *func, uint32_t line);
int32_t removeFDTrackerByFunc(const char *func, uint32_t line);
int32_t removeFDTrackerByThread(pthread_t thread);

int32_t outputFDTracker();
int32_t outputFDTrackerByFD(int fd);
int32_t outputFDTrackerByFunc(const char *func, uint32_t line);
int32_t outputFDTrackerByThread(pthread_t thread);

int32_t clearFDTracker();

uint32_t sizeOfFDTracker();
int32_t showFDTrackerLeakage();
int32_t forceRecycleFDLeakage();


#define AddFDTracker(fd) \
    addFDTracker(fd, __FUNCTION__, __LINE__, pthread_self(), __FILE__, NULL);
#define RemoveFDTracker(fd) \
    removeFDTrackerByFD(fd, __FUNCTION__, __LINE__);

#define Open(path, flag, mode...) \
    ({ \
        int __fd = open(path, flag, ##mode); \
        AddFDTracker(__fd); \
        __fd; \
    })

#define Close(fd) \
    ({ \
        RemoveFDTracker(fd); \
        int __rc = close(fd); \
        __rc; \
    })

#ifndef COMPILE_C_ONLY
}
#endif

};

#endif

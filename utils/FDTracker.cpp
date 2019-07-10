#include "FDTracker.h"
#include "FDTrackerImpl.h"

namespace pandora {

#define ISNULL(ptr)  ((ptr) == NULL)
#define NOTNULL(ptr) (!ISNULL(ptr))
#define NOT_INITED      -ENODEV

FDTracker *FDTracker::gInstance = NULL;

FDTracker *FDTracker::getInstance()
{
    return ISNULL(gInstance) ? gInstance = new FDTracker() : gInstance;
}

FDTracker::FDTracker() :
    impl(new FDTrackerImpl())
{
}

FDTracker::~FDTracker()
{
    if (NOTNULL(impl)) {
        impl->clear();
        delete impl;
        impl = NULL;
    }
    gInstance = NULL;
}

int32_t FDTracker::add(int fd, const char *func, uint32_t line,
    pthread_t thread, const char *file, char *comment)
{
    return NOTNULL(impl) ? impl->add(
        fd, func, line, thread, file, comment) : NOT_INITED;
}

int32_t FDTracker::remove(int fd, const char *func, uint32_t line)
{
    return NOTNULL(impl) ? impl->remove(fd, func, line) : NOT_INITED;
}

int32_t FDTracker::remove(const char *func)
{
    return NOTNULL(impl) ? impl->remove(func) : NOT_INITED;
}

int32_t FDTracker::remove(const char *func, uint32_t line)
{
    return NOTNULL(impl) ? impl->remove(func, line) : NOT_INITED;
}

int32_t FDTracker::remove(pthread_t thread)
{
    return NOTNULL(impl) ? impl->remove(thread) : NOT_INITED;
}

int32_t FDTracker::clear()
{
    return NOTNULL(impl) ? impl->clear() : NOT_INITED;
}

int32_t FDTracker::output()
{
    return NOTNULL(impl) ? impl->output() : NOT_INITED;
}

int32_t FDTracker::output(int fd)
{
    return NOTNULL(impl) ? impl->output(fd) : NOT_INITED;
}

int32_t FDTracker::output(const char *func)
{
    return NOTNULL(impl) ? impl->output(func) : NOT_INITED;
}

int32_t FDTracker::output(const char *func, uint32_t line)
{
    return NOTNULL(impl) ? impl->output(func, line) : NOT_INITED;
}

int32_t FDTracker::output(pthread_t thread)
{
    return NOTNULL(impl) ? impl->output(thread) : NOT_INITED;
}

uint32_t FDTracker::size()
{
    return NOTNULL(impl) ? impl->size() : NOT_INITED;
}

int32_t FDTracker::showLeakage()
{
    return output();
}

int32_t FDTracker::forceRecycle()
{
    return NOTNULL(impl) ? impl->forceRecycle() : NOT_INITED;
}

extern "C" {

int32_t addFDTracker(int fd, const char *func, uint32_t line,
        pthread_t thread, const char *file, char *comment)
{
    return FDTracker::getInstance()->add(
        fd, func, line, thread, file, comment);
}

int32_t removeFDTrackerByFD(int fd, const char *func, uint32_t line)
{
    return FDTracker::getInstance()->remove(fd, func, line);
}

int32_t removeFDTrackerByFunc(const char *func, uint32_t line)
{
    return FDTracker::getInstance()->remove(func, line);
}

int32_t removeFDTrackerByThread(pthread_t thread)
{
    return FDTracker::getInstance()->remove(thread);
}

int32_t clearFDTracker()
{
    return FDTracker::getInstance()->clear();
}

int32_t outputFDTracker()
{
    return FDTracker::getInstance()->output();
}

int32_t outputFDTrackerByFD(int fd)
{
    return FDTracker::getInstance()->output(fd);
}

int32_t outputFDTrackerByFunc(const char *func, uint32_t line)
{
    return FDTracker::getInstance()->output(func, line);
}

int32_t outputFDTrackerByThread(pthread_t thread)
{
    return FDTracker::getInstance()->output(thread);
}

uint32_t sizeOfFDTracker()
{
    return FDTracker::getInstance()->size();
}

int32_t showFDTrackerLeakage()
{
    return outputFDTracker();
}

int32_t forceRecycleFDLeakage()
{
    return FDTracker::getInstance()->forceRecycle();
}

}; // extern "C"

};

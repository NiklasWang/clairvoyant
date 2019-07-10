#ifndef PERFORMANCE_LOGGER_H_
#define PERFORMANCE_LOGGER_H_

#include <stddef.h>
#include <stdint.h>

namespace pandora {

class PfLoggerImpl;

class PfLogger {
public:
    int32_t start(const char *name, uint32_t times = 0);
    int32_t stop(const char *name);
    int32_t output();
    int32_t output(const char *name);
    void clear();
    int32_t clear(const char *name);
    static PfLogger *getInstance();

private:
    PfLogger();
    PfLogger(const PfLogger &rhs);

public:
    ~PfLogger();

private:
    PfLoggerImpl *mP;
    static PfLogger *gInstance;
};

};

#endif

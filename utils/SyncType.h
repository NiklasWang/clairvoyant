#ifndef _SYNC_TYPE_H_
#define _SYNC_TYPE_H_

#include <stdint.h>

#include "Semaphore.h"

namespace pandora {

enum SyncTypeE {
    SYNC_TYPE,
    ASYNC_TYPE,
    SYNC_MAX_INVALID,
};

class SyncType {
public:
    int32_t wait();
    void signal();

public:
    typedef SyncTypeE value_type;

    SyncType(value_type type = SYNC_TYPE);
    ~SyncType();
    SyncType &operator=(const value_type &rhs);
    SyncType &operator=(const SyncType &rhs);
    bool operator== (const value_type &rhs);
    bool operator==(const SyncType &rhs);
    bool operator!=(const value_type &rhs);
    bool operator!=(const SyncType &rhs);
    static const char *whoami(value_type type);

private:
    value_type   mType;
    Semaphore   *mSem;
    static const char * const kTypeString[];
};

};

#endif

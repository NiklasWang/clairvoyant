#ifndef _COMMON_H__
#define _COMMON_H__

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "PandoraInterface.h"
#include "Version.h"
#include "Modules.h"
#include "Identifier.h"
#include "Logs.h"
#include "Logo.h"

namespace pandora {

enum err_raeson {
    NO_ERROR = 0,
    NOT_INITED,
    PARAM_INVALID,
    PERM_DENIED,
    NOT_READY,
    FORMAT_INVALID,
    NO_MEMORY,
    INSUFF_SIZE,
    TIMEDOUT,
    NOT_FOUND,
    NOT_EXIST,
    ALREADY_INITED,
    ALREADY_EXISTS,
    NOT_REQUIRED,
    NOT_SUPPORTED,
    USER_ABORTED,
    JUMP_DONE,
    INTERNAL_ERROR,
    NOT_NEEDED,
    SYS_ERROR,
    TEST_FAILED,
    UNKNOWN_ERROR,
};

#define SUCCEED(rc)      ((rc) == NO_ERROR)
#define FAILED(rc)       (!SUCCEED(rc))
#define ISNULL(ptr)      ((ptr) == NULL)
#define NOTNULL(ptr)     (!ISNULL(ptr))
#define POSITIVE_NUM(num) ((num) > 0)
#define NEGITIVE_NUM(num) ((num) < 0)
#define ISZERO(num)       ((num) == 0)
#define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#define INVALIDNUM       (-1)
#define EQUALPTR(a, b)   ((a) == (b))
#define MAX(a, b)        ((a) > (b) ? (a) : (b))
#define MIN(a, b)        ((a) < (b) ? (a) : (b))
#define RESETRESULT(rc)  ((rc) = NO_ERROR)
#define RETURNIGNORE(rc, ignore)  ((rc) & (~(ignore)))
#define SUCCEEDIGNORE(rc, ignore) (SUCCEED(rc) || ((rc) == (ignore)))
#define EPSINON          1e-7
#define EQUALFLOAT(a, b) (fabsf((a) - (b)) < EPSINON)

#define SECURE_FREE(ptr) \
    do { \
        if (!ISNULL(ptr)) { \
            Free(ptr); \
            (ptr) = NULL; \
        } \
    } while(0)

#define SECURE_DELETE(obj) \
    do { \
        if (!ISNULL(obj)) { \
            delete(obj); \
            (obj) = NULL; \
        } \
    } while(0)

#define COMPARE_SAME_STRING(LHS, RHS) \
    ({ \
        bool _result = true; \
        if (NOTNULL(LHS) && NOTNULL(RHS)) { \
            _result &= !strcmp(LHS, RHS); \
        } else if (ISNULL(LHS) && ISNULL(RHS)) { \
        } else { \
            _result = false; \
        } \
        _result; \
    })

#define COMPARE_SAME_LEN_STRING(LHS, RHS, len) \
    ({ \
        bool _result = true; \
        if (NOTNULL(LHS) && NOTNULL(RHS)) { \
            _result &= !strncmp(LHS, RHS, len); \
        } else if (ISNULL(LHS) && ISNULL(RHS)) { \
        } else { \
            _result = false; \
        } \
        _result; \
    })

#define COMPARE_SAME_DATA(LHS, RHS, SIZE) \
    ({ \
        bool _result = true; \
        if (NOTNULL(LHS) && NOTNULL(RHS)) { \
            _result &= !memcmp(LHS, RHS, SIZE); \
        } else if (ISNULL(LHS) && ISNULL(RHS)) { \
        } else { \
            _result = false; \
        } \
        _result; \
    })

#ifndef offsetof
#define offsetof(TYPE, MEMBER) \
    ((int32_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({                 \
    const typeof(((type *)0)->member) *__mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member)); })

#define align_len_to_size(len, size) ({                    \
        (((len) + (size) - 1) & ~((size) - 1)); });

class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}

private:
    noncopyable(const noncopyable&) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
};

template <typename T>
struct AnyType {
    // No real bussiness to do
    AnyType() = default;
    ~AnyType() = default;
    AnyType &operator=(const AnyType &rhs) = default;
    AnyType(const T &rhs) { *this = rhs; }
    AnyType &operator=(const T &) { return *this; }
};

};

#endif

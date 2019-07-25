#ifndef _GLOBAL_TRAITS_H_
#define _GLOBAL_TRAITS_H_

#include "Common.h"
#include "Pandora.h"
#include "AlgorithmType.h"

namespace pandora {

struct GlobalTaskType {
    void   *data;
    int32_t w;
    int32_t h;
    int32_t stride;
    int32_t scanline;
    size_t  size;
    FrameFormat format;
    FrameType   type;
    int64_t     ts;
    uint32_t    taskid;
    static uint32_t taskcnt;

public:
    GlobalTaskType() :
        data(NULL), taskid(taskcnt++) {}
    GlobalTaskType(void *_data, int32_t _w, int32_t _h,
        int32_t _stride, int32_t _scanline, size_t _size,
        FrameFormat _format = FRAME_FORMAT_YUV_420_NV21,
        FrameType _type = FRAME_TYPE_PREVIEW,
        int64_t _ts = 0LL) :
        data(_data), w(_w), h(_h), stride(_stride),
        scanline(_scanline), size(_size), format(_format),
        type(_type), ts(_ts), taskid(taskcnt++) {}
    GlobalTaskType &operator=(const GlobalTaskType &rhs) = default;
    virtual ~GlobalTaskType() = default;
};

enum FrameFormatMask {
    FRAME_FORMAT_MASK_YUV_420_NV21 = 0x01 << FRAME_FORMAT_YUV_420_NV21,
    FRAME_FORMAT_MASK_YUV_420_NV12 = 0x01 << FRAME_FORMAT_YUV_420_NV12,
    FRAME_FORMAT_MASK_YUV_MONO  = 0x01 << FRAME_FORMAT_YUV_MONO,
    FRAME_FORMAT_MASK_YUV_TELE  = 0x01 << FRAME_FORMAT_YUV_TELE,
    FRAME_FORMAT_MASK_JPEG      = 0x01 << FRAME_FORMAT_JPEG,
    FRAME_FORMAT_MASK_JPEG_MONO = 0x01 << FRAME_FORMAT_JPEG_MONO,
    FRAME_FORMAT_MASK_JPEG_TELE = 0x01 << FRAME_FORMAT_JPEG_TELE,
    FRAME_FORMAT_MASK_MAX_INVALID  = 0x0,
};

enum FrameTypeMask {
    FRAME_TYPE_MASK_PREVIEW  = 0x01 << FRAME_TYPE_PREVIEW,
    FRAME_TYPE_MASK_SNAPSHOT = 0x01 << FRAME_TYPE_SNAPSHOT,
    FRAME_TYPE_MASK_VIDEO    = 0x01 << FRAME_TYPE_VIDEO,
    FRAME_TYPE_MASK_MAX_INVALID = 0x0,
};

typedef int32_t FrameFormatMaskType;
typedef int32_t FrameTypeMaskType;

inline FrameTypeMaskType getFrameTypeMask(FrameType type) {
    return 0x01 << type;
};

inline FrameFormatMaskType getFrameFormatMask(FrameFormat format) {
    return 0x01 << format;
};

struct IAlgResult {
    bool     valid;
    uint32_t taskid;
    virtual void dump();
    virtual ~IAlgResult() = default;
};

enum AlgBroadcastDir {
    ALG_BROADCAST_DIR_NONE,
    ALG_BROADCAST_DIR_UPSTREAM,
    ALG_BROADCAST_DIR_DOWNSTREAM,
    ALG_BROADCAST_DIR_BOTH,
    ALG_BROADCAST_DIR_MAX_INVALID
};

struct GlobalResultType :
    public IAlgResult {
public:
    AlgType  type;

public:
    virtual ~GlobalResultType() = default;
};

struct GlobalUpdateType {
public:
    uint32_t  taskid;
    AlgType   type;
    AlgBroadcastDir dir;
    void     *result;

public:
    virtual ~GlobalUpdateType() = default;
};

struct GlobalParmType {
public:
    int32_t reserved;

public:
    virtual ~GlobalParmType() = default;
};

struct GlobalRequestType {
public:
    int32_t reserved;

public:
    virtual ~GlobalRequestType() = default;
};

struct GlobalStatusType {
public:
    int32_t rc;

public:
    virtual ~GlobalStatusType() = default;
};

struct Global {
}; // Name holder

template <typename T>
struct AlgTraits {
};

template <>
struct AlgTraits<Global> {
    typedef GlobalTaskType    TaskType;
    typedef GlobalResultType  ResultType;
    typedef GlobalUpdateType  UpdateType;
    typedef GlobalParmType    ParmType;
    typedef GlobalRequestType RequestType;
    typedef GlobalStatusType  StatusType;
};


#define DEFINE_ALGORITHM_TRAITS(Algorithm)  \
class Algorithm; \
template <>      \
struct AlgTraits<Algorithm> {                      \
    typedef Algorithm##TaskType     TaskType;      \
    typedef Algorithm##ResultType   ResultType;    \
    typedef Algorithm##UpdateType   UpdateType;    \
    typedef Algorithm##ParmType     ParmType;      \
    typedef Algorithm##RequestType  RequestType;   \
    typedef Algorithm##StatusType   StatusType;    \
    typedef AlgType    (Algorithm::*getTypeF)(); \
    typedef int32_t    (Algorithm::*initF)();    \
    typedef int32_t    (Algorithm::*deinitF)();  \
    typedef int32_t    (Algorithm::*processF)(TaskType &, ResultType &); \
    typedef int32_t    (Algorithm::*processExF)(TaskType &, TaskType &, ResultType &); \
    typedef int32_t    (Algorithm::*setParmF)(ParmType &);       \
    typedef int32_t    (Algorithm::*updateF)(UpdateType &);      \
    typedef int32_t    (Algorithm::*queryCapsF)(AlgCaps &);      \
    typedef int32_t    (Algorithm::*queryStatusF)(StatusType &); \
    static getTypeF     fnGetType;     \
    static initF        fnInit;        \
    static deinitF      fnDeinit;      \
    static processF     fnProcess;     \
    static processExF   fnProcessEx;   \
    static setParmF     fnSetParm;     \
    static updateF      fnUpdate;      \
    static queryCapsF   fnQueryCaps;   \
    static queryStatusF fnQueryStatus; \
};

#define INIT_ALGORITHM_TRAITS_FUNC(Algorithm) \
    AlgTraits<Algorithm>::getTypeF \
        AlgTraits<Algorithm>::fnGetType     = &Algorithm::getType; \
    AlgTraits<Algorithm>::initF \
        AlgTraits<Algorithm>::fnInit        = &Algorithm::init; \
    AlgTraits<Algorithm>::deinitF \
        AlgTraits<Algorithm>::fnDeinit      = &Algorithm::deinit; \
    AlgTraits<Algorithm>::processF \
        AlgTraits<Algorithm>::fnProcess     = &Algorithm::process; \
    AlgTraits<Algorithm>::processExF \
        AlgTraits<Algorithm>::fnProcessEx   = &Algorithm::process; \
    AlgTraits<Algorithm>::setParmF \
        AlgTraits<Algorithm>::fnSetParm     = &Algorithm::setParm; \
    AlgTraits<Algorithm>::updateF \
        AlgTraits<Algorithm>::fnUpdate      = &Algorithm::update; \
    AlgTraits<Algorithm>::queryStatusF \
        AlgTraits<Algorithm>::fnQueryStatus = &Algorithm::queryStatus; \
    AlgTraits<Algorithm>::queryCapsF \
        AlgTraits<Algorithm>::fnQueryCaps   = &Algorithm::queryCaps;

#define USE_DEFAULT_TASK_TYPE(Algorithm)        \
    struct Algorithm##TaskType :                \
        public AlgTraits<Global>::TaskType {    \
    };

#define USE_DEFAULT_RESULT_TYPE(Algorithm)      \
    struct Algorithm##ResultType :              \
        public AlgTraits<Global>::ResultType {  \
    };

#define USE_DEFAULT_PARM_TYPE(Algorithm)        \
    struct Algorithm##ParmType :                \
        public AlgTraits<Global>::ParmType {    \
    };

#define USE_DEFAULT_UPDATE_TYPE(Algorithm)      \
    struct Algorithm##UpdateType :              \
        public AlgTraits<Global>::UpdateType {  \
    };

#define USE_DEFAULT_REQUEST_TYPE(Algorithm)     \
    struct Algorithm##RequestType :             \
        public AlgTraits<Global>::RequestType { \
    };

#define USE_DEFAULT_STATUS_TYPE(Algorithm)      \
    struct Algorithm##StatusType :              \
        public AlgTraits<Global>::StatusType {  \
    };

};

#endif


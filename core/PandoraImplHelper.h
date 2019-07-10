#ifndef _PANDORA_IMPL_HELPER_
#define _PANDORA_IMPL_HELPER_

#include "Pandora.h"

namespace pandora {

enum ThreadTaskType {
    TT_PREPARE_PREVIEW,
    TT_UNPREPARE_PREVIEW,
    TT_PREPARE_SNAPSHOT,
    TT_UNPREPARE_SNAPSHOT,
    TT_PREPARE_RECORDING,
    TT_UNPREPARE_RECORDING,
    TT_METADATA_AVAILABLE,
    TT_COMMAND_AVAILABLE,
    TT_PARAMETER_AVAILABLE,
    TT_FRAME_READY,
    TT_MAX_INVALID,
};

template <ThreadTaskType type>
struct ReqArgs {
    void *ptr;
};

template <>
struct ReqArgs<TT_COMMAND_AVAILABLE> {
    int32_t command;
    int32_t arg1;
    int32_t arg2;
};

template <>
struct ReqArgs<TT_PARAMETER_AVAILABLE> {
    const void *parm;
};

template <>
struct ReqArgs<TT_FRAME_READY> {
    FrameInfo frame;
};

};

#endif

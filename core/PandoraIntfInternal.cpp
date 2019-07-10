#include "PandoraIntfInternal.h"

namespace pandora {

static const char * const gFrameFormatStr[] = {
    [FRAME_FORMAT_YUV_420_NV21] = "YUV_420_NV21",
    [FRAME_FORMAT_YUV_420_NV12] = "YUV_420_NV12",
    [FRAME_FORMAT_YUV_MONO]     = "YUV_MONO",
    [FRAME_FORMAT_YUV_TELE]     = "YUV_TELE",
    [FRAME_FORMAT_JPEG]         = "JPEG",
    [FRAME_FORMAT_JPEG_MONO]    = "JPEG_MONO",
    [FRAME_FORMAT_JPEG_TELE]    = "JPEG_TELE",
    [FRAME_FORMAT_MAX_INVALID]  = "MAX INVALID",
};

static const char * const gFrameTypeStr[] = {
    [FRAME_TYPE_PREVIEW]        = "PREVIEW",
    [FRAME_TYPE_SNAPSHOT]       = "SNAPSHOT",
    [FRAME_TYPE_VIDEO]          = "VIDEO",
    [FRAME_TYPE_MAX_INVALID]    = "MAX INVALID",
};


bool checkValid(FrameFormat format)
{
    bool rc = false;

    if (format >= 0 && format < FRAME_FORMAT_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

FrameFormat getValidType(FrameFormat format)
{
    return checkValid(format) ? format : FRAME_FORMAT_MAX_INVALID;
}

const char *frameFormatName(FrameFormat format)
{
    return gFrameFormatStr[getValidType(format)];
}

bool checkValid(FrameType type)
{
    bool rc = false;

    if (type >= 0 && type < FRAME_TYPE_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

FrameType getValidType(FrameType type)
{
    return checkValid(type) ? type : FRAME_TYPE_MAX_INVALID;
}

const char *frameTypeName(FrameType type)
{
    return gFrameTypeStr[getValidType(type)];
}

};

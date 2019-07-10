#ifndef _PANDORA_INTERFACE__
#define _PANDORA_INTERFACE__

#include <stdint.h>
#include <assert.h>

namespace pandora {

struct FrameInfo;

class PandoraInterface {
public:
    virtual int32_t startPreview() = 0;
    virtual int32_t stopPreview() = 0;
    virtual int32_t takePicture() = 0;
    virtual int32_t pictureTaken() = 0;
    virtual int32_t startRecording() = 0;
    virtual int32_t stopRecording() = 0;
    virtual int32_t onMetadataAvailable(void *metadata) = 0;
    virtual int32_t onCommandAvailable(int32_t command,
        int32_t arg1 = 0, int32_t arg2 = 0) = 0;
    virtual int32_t onParameterAvailable(const void *parameter) = 0;
    virtual int32_t onFrameReady(FrameInfo &frame) = 0;
    virtual ~PandoraInterface() {}
};

enum FrameFormat {
    FRAME_FORMAT_YUV_420_NV21,
    FRAME_FORMAT_YUV_420_NV12,
    FRAME_FORMAT_YUV_MONO,
    FRAME_FORMAT_YUV_TELE,
    FRAME_FORMAT_JPEG,
    FRAME_FORMAT_JPEG_MONO,
    FRAME_FORMAT_JPEG_TELE,
    FRAME_FORMAT_BAYER,
    FRAME_FORMAT_MAX_INVALID,
};

enum FrameType {
    FRAME_TYPE_PREVIEW,
    FRAME_TYPE_SNAPSHOT,
    FRAME_TYPE_VIDEO,
    FRAME_TYPE_MAX_INVALID,
};

struct FrameInfo {
    void   *frame;
    int32_t w;
    int32_t h;
    int32_t stride;
    int32_t scanline;
    size_t  size;
    FrameFormat format;
    FrameType   type;
    int64_t     ts;   // ms * 1e6 from 01/01/1970 00:00:00

public:
    FrameInfo(void *_frame,
        int32_t _w,
        int32_t _h,
        int32_t _stride,
        int32_t _scanline,
        size_t _size,
        FrameFormat _format = FRAME_FORMAT_YUV_420_NV21,
        FrameType _type = FRAME_TYPE_PREVIEW,
        int64_t _ts = 0LL) :
        frame(_frame),
        w(_w),
        h(_h),
        stride(_stride),
        scanline(_scanline),
        size(_size),
        format(_format),
        type(_type),
        ts(_ts)
    {
        assert(_frame != NULL);
        assert(_w > 0);
        assert(_h > 0);
        assert(_stride > 0);
        assert(_scanline > 0);
        assert(_size > 0);
        assert(_format >= 0);
        assert(_format < FRAME_FORMAT_MAX_INVALID);
        assert(_type >= 0);
        assert(_type < FRAME_TYPE_MAX_INVALID);
        assert(_ts >= 0LL);
    }
    FrameInfo() {}
};

};

#endif

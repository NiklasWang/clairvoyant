#ifndef _SIRIUS_CLIENT_INTF_H_
#define _SIRIUS_CLIENT_INTF_H_

#include "PandoraParameters.h"

namespace pandora {

struct SiriusTask {
    int32_t w;
    int32_t h;
    int32_t stride;
    int32_t scanline;
    void   *data;
    FrameType   type;
    FrameFormat format;
    int64_t ts;
};

class SiriusClientIntf {
public:
    virtual int32_t update(PreviewDim &preview, PictureDim &picture) = 0;
    virtual int32_t onFrameReady(SiriusTask &frame) = 0;
    virtual int32_t sendEvent(int32_t evt, int32_t arg1, int32_t arg2) = 0;
    virtual int32_t sendData(int32_t type, void *data, int32_t size) = 0;

public:
    virtual ~SiriusClientIntf() {}
};

};

#endif

#ifndef _SIRIUS_CLIENT_H_
#define _SIRIUS_CLIENT_H_

#include "SiriusClientIntf.h"

namespace pandora {

class SiriusClientImpl;

class SiriusClient :
    public SiriusClientIntf
{
public:
    int32_t update(PreviewDim &preview, PictureDim &picture) override;
    int32_t onFrameReady(SiriusTask &frame) override;
    int32_t sendEvent(int32_t evt, int32_t arg1, int32_t arg2) override;
    int32_t sendData(int32_t type, void *data, int32_t size) override;

public:
    SiriusClient();
    virtual ~SiriusClient();

private:
    SiriusClientImpl *mImpl;
};

};

#endif

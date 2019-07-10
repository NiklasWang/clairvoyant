#ifndef _SIRIUS_CLIENT_IMPL_H_
#define _SIRIUS_CLIENT_IMPL_H_

#include "SiriusClientIntf.h"

namespace sirius {
    class PreviewClient;
    class YuvPictureClient;
    class BayerPictureClient;
    class EventClient;
    class DataClient;
    class SiriusClientCore;
};

namespace pandora {

class SiriusClientImpl :
    public SiriusClientIntf
{
public:
    int32_t update(PreviewDim &preview, PictureDim &picture) override;
    int32_t onFrameReady(SiriusTask &frame) override;
    int32_t sendEvent(int32_t evt, int32_t arg1, int32_t arg2) override;
    int32_t sendData(int32_t type, void *data, int32_t size) override;

public:
    SiriusClientImpl();
    virtual ~SiriusClientImpl();

private:
    int32_t enablePreview();
    int32_t sendPreview(SiriusTask &frame);
    int32_t enableSnapshot();
    int32_t sendSnapshot(SiriusTask &frame);
    int32_t enableBayer();
    int32_t sendBayer(SiriusTask &frame);
    int32_t enableEvent();
    int32_t enableData();

private:
    sirius::PreviewClient      *mPreview;
    sirius::YuvPictureClient   *mSnapshot;
    sirius::BayerPictureClient *mBayer;
    sirius::EventClient        *mEvent;
    sirius::DataClient         *mData;
    sirius::SiriusClientCore   *mCore;
};

};

#endif

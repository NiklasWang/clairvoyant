#ifndef __PANDORA_H_
#define __PANDORA_H_

#include "PandoraInterface.h"

namespace pandora {

class PlatformOpsIntf;

class Pandora :
    public PandoraInterface {
public:
    int32_t startPreview();
    int32_t stopPreview();
    int32_t takePicture();
    int32_t pictureTaken();
    int32_t startRecording();
    int32_t stopRecording();
    int32_t onMetadataAvailable(void *metadata);
    int32_t onCommandAvailable(int32_t command,
        int32_t arg1 = 0, int32_t arg2 = 0);
    int32_t onParameterAvailable(const void *parameter);
    int32_t onFrameReady(FrameInfo &frame);

public:
    explicit Pandora(PlatformOpsIntf *platform);
    virtual ~Pandora();

private:
    Pandora(const Pandora &rhs);
    Pandora &operator =(const Pandora &rhs);

private:
    PandoraInterface *mImpl;
};

};

#endif

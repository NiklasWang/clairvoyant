#ifndef CAMERA_STATUS_MGR_H_
#define CAMERA_STATUS_MGR_H_

#include <math.h>

#include "CameraStatus.h"
#include "StatusContainer.h"

namespace pandora {

class Pal;

class CamStatusMgr {
public:
    bool inited();
    int32_t setPal(Pal *hal);
    void dump();
    uint32_t push_back(const AEInfo &ae);
    uint32_t push_back(const AFInfo &af);
    uint32_t push_back(const HdrInfo &hdr);
    bool checkFocusChanged();
    bool checkExposureChanged();
    bool checkAutoFlashOn();
    bool checkFlashChanged();
    bool checkAutoHdrOn();
    bool checkAutoHDRChanged();
    bool checkAutoImageStabOn();
    bool checkFocusScanning();
    bool checkLowLightChanged();

public:
    enum LumDir {
        LUMINANCE_TOWARDS_DARK,
        LUMINANCE_TOWARDS_BRIGHT,
        LUMINANCE_TOWARDS_UNKNOWN,
    };
    bool checkExposureChanged(LumDir &dir);
    bool checkFocusChanged(bool &isMacro);
    bool checkFlashChanged(bool &isOn);
    bool checkAutoHDRChanged(bool &isOn);
    bool checkLowLightChanged(bool &isOn);

private:
    bool checkFocusChanged(AFInfo &last, AFInfo &curr, bool *isMacro = NULL);
    bool checkExposureChanged(AEInfo &last, AEInfo &curr);
    bool checkAutoHDRChanged(HdrInfo &last, HdrInfo &curr, bool *isOn = NULL);
    bool checkAutoFlashOn(AEInfo &info);
    bool checkFlashChanged(AEInfo &last, AEInfo &curr, bool *isOn = NULL);
    bool checkAutoHdrOn(HdrInfo &info);
    bool checkAutoImageStabOn(AEInfo &info);
    bool checkLowLightChanged(AEInfo &last, AEInfo &curr, bool *isOn = NULL);
    bool checkLowLight(AEInfo &last);

public:
    CamStatusMgr();

private:
    template<typename T>
    uint32_t getLastCurrStatus(InfoContainer<T> &src,
        T &last, T &curr);
    template<typename T>
    uint32_t getCurrStatus(InfoContainer<T> &src, T &curr);
    template<typename T>
    uint32_t getLastStatus(InfoContainer<T> &src, T &last);

private:
    LumDir checkLuminanceTowards(AEInfo &last, AEInfo &curr);

private:
    bool mInited;
    Pal *mPal;
    InfoContainer<AEInfo> mAE;
    InfoContainer<AFInfo> mAF;
    InfoContainer<HdrInfo> mHdr;
    LumDir mLumDir;

public:
    static const uint32_t gAEContainerCapacity;
    static const uint32_t gAFContainerCapacity;
    static const uint32_t gHdrContainerCapacity;
};

};

#endif

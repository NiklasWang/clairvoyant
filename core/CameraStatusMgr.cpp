#include "CameraStatusMgr.h"
#include "ConfigInterface.h"
#include "Pal.h"

namespace pandora {

const uint32_t CamStatusMgr::gAEContainerCapacity = 6;
const uint32_t CamStatusMgr::gAFContainerCapacity = 2;
const uint32_t CamStatusMgr::gHdrContainerCapacity = 6;

bool CamStatusMgr::inited()
{
    return mInited;
}

void CamStatusMgr::dump()
{
    mAE.dump();
    mAF.dump();
    mHdr.dump();
}

CamStatusMgr::CamStatusMgr() :
    mInited(false),
    mPal(NULL),
    mAE(gAEContainerCapacity),
    mAF(gAFContainerCapacity),
    mHdr(gHdrContainerCapacity),
    mLumDir(LUMINANCE_TOWARDS_UNKNOWN)
{
    {
        AEInfo ae;
        ae.init();
        for (uint32_t i = 0; i < gAEContainerCapacity; i++) {
            mAE.push_back(ae);
        }
    }
    {
        AFInfo af;
        af.init();
        for (uint32_t i = 0; i < gAFContainerCapacity; i++) {
            mAF.push_back(af);
        }
    }
    {
        HdrInfo hdr;
        hdr.init();
        for (uint32_t i = 0; i < gHdrContainerCapacity; i++) {
            mHdr.push_back(hdr);
        }
    }
}

int32_t CamStatusMgr::setPal(Pal *hal)
{
    mPal = hal;
    mInited = NOTNULL(hal);
    return !mInited;
}

uint32_t CamStatusMgr::push_back(const AEInfo &ae)
{
    return mAE.push_back(ae);
}

uint32_t CamStatusMgr::push_back(const AFInfo &af)
{
    return mAF.push_back(af);
}

uint32_t CamStatusMgr::push_back(const HdrInfo &hdr)
{
    return mHdr.push_back(hdr);
}

template<typename T>
uint32_t CamStatusMgr::getLastCurrStatus(
    InfoContainer<T> &src, T &last, T &curr)
{
    uint32_t half =  src.get_taken() / 2;
    for (uint32_t i = 0; i < src.get_taken(); i++) {
        if (i < half) {
            last += src[i];
        } else {
            curr += src[i];
        }
    }
    last /= half;
    curr /= half;

    return NO_ERROR;
}

template<typename T>
uint32_t CamStatusMgr::getCurrStatus(
    InfoContainer<T> &src, T &curr)
{
    uint32_t half =  src.get_taken() / 2;
    for (uint32_t i = half; i < src.get_taken(); i++) {
            curr += src[i];
    }
    curr /= half;

    return NO_ERROR;
}

template<typename T>
uint32_t CamStatusMgr::getLastStatus(
    InfoContainer<T> &src, T &last)
{
    uint32_t half =  src.get_taken() / 2;
    for (uint32_t i = 0; i < half; i++) {
            last += src[i];
    }
    last /= half;

    return NO_ERROR;
}

bool CamStatusMgr::checkFocusChanged(
    AFInfo &last, AFInfo &curr, bool *isMacro)
{
    int32_t rc = NO_ERROR;
    int32_t macro1 = -1, macro2 = -1;

    if (NOTNULL(mPal)) {
        FocusEndConfig conf;
        rc = mPal->getParm(conf);

        if (SUCCEED(rc)) {
            macro1 = (last.curr_pos * 100 /
                last.total_pos < conf.macroEndRatio) ? 1 : macro1;
            macro2 = (curr.curr_pos * 100 /
                last.total_pos < conf.macroEndRatio) ? 1 : macro2;

            if (NOTNULL(isMacro)) {
                *isMacro = macro2 == 1;
            }
        }
    }


    return macro1 * macro2 == -1;
}

bool CamStatusMgr::checkFocusChanged(bool &isMacro)
{
    bool result = true;

    if (mAF.size() < gAFContainerCapacity) {
        result = false;
    }

    if (result) {
        AFInfo last, curr;
        last.init();
        curr.init();
        getLastCurrStatus(mAF, last, curr);
        result = checkFocusChanged(last, curr, &isMacro);
    }

    return result;
}

bool CamStatusMgr::checkFocusChanged()
{
    bool isMacro;
    return checkFocusChanged(isMacro);
}

bool CamStatusMgr::checkExposureChanged(
    AEInfo &last, AEInfo &curr)
{
    int32_t rc = NO_ERROR;
    bool result = false;

    if (NOTNULL(mPal)) {
        ExpChangeConfig conf;
        rc = mPal->getParm(conf);

        if (SUCCEED(rc)) {
            result = fabsf(last.exp_time - curr.exp_time) >
                    1.0f / conf.exposureChangeThres;
            result |= abs(last.iso_value - curr.iso_value) >
                    conf.isoChangeThres;
            result |= abs(last.lux_index - curr.lux_index) >
                    conf.luxChangeThres;
        }
    }

    return result;
}

bool CamStatusMgr::checkLowLight(AEInfo &info)
{
    int32_t rc = NO_ERROR;
    bool result = false;

    if (NOTNULL(mPal)) {
        LowLightConfig conf;
        rc = mPal->getConfig(conf);

        if (SUCCEED(rc)) {
            result = info.iso_value >= conf.isoThres ||
                info.exp_time >= conf.exposureThres ||
                info.lux_index > conf.luxThre;
        }
    }

    return result;
}


bool CamStatusMgr::checkLowLightChanged(
    AEInfo &last, AEInfo &curr, bool *isOn)
{
    bool lowlight = checkLowLight(curr);
    if (NOTNULL(isOn)) {
        *isOn = lowlight;
    }
    return checkLowLight(last) != lowlight;
}

bool CamStatusMgr::checkLowLightChanged()
{
    bool isOn;
    return checkLowLightChanged(isOn);
}

bool CamStatusMgr::checkLowLightChanged(bool &isOn)
{
    bool result = true;

    if (mAE.size() < gAEContainerCapacity) {
        result = false;
    }

    if (result) {
        AEInfo last, curr;
        last.init();
        curr.init();
        getLastCurrStatus(mAE, last, curr);
        result = checkLowLightChanged(last, curr, &isOn);
    }

    return result;
}

CamStatusMgr::LumDir CamStatusMgr::checkLuminanceTowards(
    AEInfo &last, AEInfo &curr)
{
    LumDir dir = LUMINANCE_TOWARDS_UNKNOWN;
    if (last.exp_time - curr.exp_time > 0.0f &&
        last.iso_value > curr.iso_value) {
        dir = LUMINANCE_TOWARDS_DARK;
    } else if (last.exp_time - curr.exp_time < 0.0f &&
        last.iso_value < curr.iso_value) {
        dir = LUMINANCE_TOWARDS_BRIGHT;
    }

    return dir;
}

bool CamStatusMgr::checkExposureChanged(LumDir &direction)
{
    bool result = true;

    if (mAE.size() < gAEContainerCapacity) {
        result = false;
    }

    if (result) {
        AEInfo last, curr;
        last.init();
        curr.init();
        getLastCurrStatus(mAE, last, curr);
        result = checkExposureChanged(last, curr);
        if (result) {
            LumDir dir = checkLuminanceTowards(last, curr);
            if (mLumDir != dir &&
                dir != LUMINANCE_TOWARDS_UNKNOWN) {
                mLumDir   = dir;
                direction = dir;
            } else {
                result    = false;
                direction = LUMINANCE_TOWARDS_UNKNOWN;
            }
        }
    }

    return result;
}


bool CamStatusMgr::checkExposureChanged()
{
    LumDir dir;
    return checkExposureChanged(dir);
}

bool CamStatusMgr::checkAutoHDRChanged(
    HdrInfo &last, HdrInfo &curr, bool *isOn)
{
    bool currOn = checkAutoHdrOn(curr);
    if (NOTNULL(isOn)) {
        *isOn = currOn;
    }
    return checkAutoHdrOn(last) != currOn;
}

bool CamStatusMgr::checkAutoHDRChanged()
{
    bool isOn;
    return checkAutoHDRChanged(isOn);
}

bool CamStatusMgr::checkAutoHDRChanged(bool &isOn)
{
    bool result = true;

    if (mHdr.size() < gHdrContainerCapacity) {
        result = false;
    }

    if (result) {
        HdrInfo last, curr;
        last.init();
        curr.init();
        getLastCurrStatus(mHdr, last, curr);
        result = checkAutoHDRChanged(last, curr, &isOn);
    }

    return result;
}

bool CamStatusMgr::checkAutoFlashOn(AEInfo &info)
{
    return info.flash_needed;
}

bool CamStatusMgr::checkAutoFlashOn()
{
    bool result = true;

    if (mAE.size() < gAEContainerCapacity) {
        result = false;
    }

    if (result) {
        AEInfo info;
        info.init();
        getCurrStatus(mAE, info);
        result = checkAutoFlashOn(info);
    }

    return result;
}

bool CamStatusMgr::checkFlashChanged(
    AEInfo &last, AEInfo &curr, bool *isOn)
{
    bool currOn = checkAutoFlashOn(curr);
    if (NOTNULL(isOn)) {
        *isOn = currOn;
    }
    return checkAutoFlashOn(last) != currOn;
}

bool CamStatusMgr::checkFlashChanged(bool &isOn)
{
    bool result = true;

    if (mAE.size() < gAEContainerCapacity) {
        result = false;
    }

    if (result) {
        AEInfo last, curr;
        last.init();
        curr.init();
        getLastCurrStatus(mAE, last, curr);
        result = checkFlashChanged(last, curr, &isOn);
    }

    return result;
}

bool CamStatusMgr::checkFlashChanged()
{
    bool isOn;
    return checkFlashChanged(isOn);
}

bool CamStatusMgr::checkAutoHdrOn(HdrInfo &info)
{
    int32_t rc = NO_ERROR;
    bool result = false;

    if (NOTNULL(mPal)) {
        ConfigInterface::ThresType conf;
        rc = mPal->getParm(conf);

        if (SUCCEED(rc)) {
            result = info.is_hdr_scene &&
                (info.hdr_confidence > conf);
        }
    }

    return result;
}

bool CamStatusMgr::checkAutoHdrOn()
{
    bool result = true;

    if (mHdr.size() < gHdrContainerCapacity) {
        result = false;
    }

    if (result) {
        HdrInfo info;
        info.init();
        getCurrStatus(mHdr, info);
        result = checkAutoHdrOn(info);
    }

    return result;
}

bool CamStatusMgr::checkAutoImageStabOn(AEInfo &info)
{
    int32_t rc = NO_ERROR;
    bool result = false;

    if (NOTNULL(mPal)) {
        NightStabConfig conf;
        rc = mPal->getParm(conf);

        if (SUCCEED(rc)) {
            result = (info.iso_value >= conf.isoThres ||
                info.exp_time >= conf.exposureThres ||
                info.lux_index > conf.luxThre);
        }
    }

    return result;
}

bool CamStatusMgr::checkAutoImageStabOn()
{
    bool result = true;

    if (mAE.size() < gAEContainerCapacity) {
        result = false;
    }

    if (result) {
        AEInfo info;
        info.init();
        getCurrStatus(mAE, info);
        result = checkAutoImageStabOn(info);
    }

    return result;
}

bool CamStatusMgr::checkFocusScanning()
{
    bool result = true;

    if (mAF.size() < gAFContainerCapacity) {
        result = false;
    }

    if (result) {
        AFInfo info;
        info.init();
        getCurrStatus(mAF, info);
        result = info.isFocusDone();
    }

    return result;
}

};


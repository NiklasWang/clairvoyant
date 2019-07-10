#include <math.h>

#include "Common.h"
#include "Logs.h"
#include "PandoraParameters.h"

namespace pandora {

void CamDimension::set(int32_t _w, int32_t _h)
{
    w = _w; h = _h;
}

int32_t CamDimension::size() const
{
    return w * h;
}

bool DualCamMode::enableRTBokeh()
{
    return funcMask & DUAL_CAMERA_MASK_RT_BOKEH;
}

bool DualCamMode::enableStillBokeh()
{
    return funcMask & DUAL_CAMERA_MASK_STILL_BOKEH;
}

bool DualCamMode::enableRefocus()
{
    return funcMask & DUAL_CAMERA_MASK_REFOCUS;
}

bool DualCamMode::enableNightshot()
{
    return funcMask & DUAL_CAMERA_MASK_NIGHT_SHOT;
}

bool DualCamMode::enableOpticalZoom()
{
    return funcMask & DUAL_CAMERA_MASK_OPTICAL_ZOOM;
}

bool DualCamMode::enableExternal()
{
    return funcMask & DUAL_CAMERA_MASK_EXTERNAL;
}

bool DualCamMode::enabledAny()
{
    return funcMask != DUAL_CAMERA_MASK_NONE_FUNC &&
        funcMask != DUAL_CAMERA_MASK_EXTERNAL;
}

bool DualCamMode::enabledNone()
{
    return !enabledAny();
}

size_t FaceInfo::add(FaceInfo::Face face)
{
    if (num < MAX_FACE_NUM) {
        faces[num++] = face;
    }
    return size();
}

void FaceInfo::update(Face face, uint32_t i)
{
    if (i < num) {
        faces[i] = face;
    } else {
        LOGE(MODULE_OTHERS, "Invalid index %d/%d", i, num);
    }
}

FaceInfo::Face FaceInfo::get(uint32_t i) const
{
    return faces[i];
}

void FaceInfo::clear()
{
    num = 0;
}

size_t FaceInfo::size() const
{
    return num;
}

size_t MultiExporeParm::add(int32_t value)
{
    if (num < MAX_MULTI_EXPOSURE_NUM) {
        exp[num++] = value;
    }
    return size();
}

int32_t MultiExporeParm::get(uint32_t i) const
{
    return exp[i];
}

void MultiExporeParm::clear()
{
    num = 0;
}

size_t MultiExporeParm::size() const
{
    return num;
}

void MultiExporeParm::set(size_t size)
{
    num = size;
}

bool CameraTypeInf::isBackCamera()
{
    return type == CAMERA_TYPE_BACK_0 || type == CAMERA_TYPE_BACK_1;
}

bool CameraTypeInf::isFrontCamera()
{
    return type == CAMERA_TYPE_FRONT_0 || type == CAMERA_TYPE_FRONT_1;
}

bool CameraTypeInf::isSubCamera()
{
    return !isBackCamera() && !isFrontCamera() && (!isNobody());
}

bool CameraTypeInf::isNobody()
{
    return type == CAMERA_TYPE_MAX_INVALID;
}

bool operator==(const LongExpMode& a1, const LongExpMode& a2)
{
    bool result = true;

    result &= a1.on == a2.on;
    result &= EQUALFLOAT(a1.time, a2.time);

    return result;
}

bool operator==(const FlipMode& a1, const FlipMode& a2)
{
    bool result = true;

    result &= a1.h == a2.h;
    result &= a1.v == a2.v;

    return result;
}

bool operator==(const WaterMarkInfo &a1, const WaterMarkInfo& a2)
{
    bool result = true;

    result &= a1.mode  == a2.mode;
    result &= a1.dp   == a2.dp;
    result &= a1.dpi   == a2.dpi;
    result &= a1.wmRatio.numerator   == a2.wmRatio.numerator;
    result &= a1.wmRatio.denominator == a2.wmRatio.denominator;
    result &= a1.debug == a2.debug;
    result &= a1.id    == a2.id;
    result &= a1.enableLogo  == a2.enableLogo;
    result &= a1.enableTexts == a2.enableTexts;
    result &= COMPARE_SAME_STRING(a1.product, a2.product);
    result &= COMPARE_SAME_STRING(a1.build,   a2.build);
    result &= COMPARE_SAME_STRING(a1.package, a2.package);

    return result;
}

bool operator==(const BeautySetting& a1, const BeautySetting& a2)
{
    bool result = true;

    result &= a1.mode     == a2.mode;
    result &= a1.strength == a2.strength;

    return result;
}

bool operator==(const Rectangle& a1, const Rectangle& a2)
{
    bool result = true;

    result &= a1.left   == a2.left;
    result &= a1.top    == a2.top;
    result &= a1.width  == a2.width;
    result &= a1.height == a2.height;

    return result;
}

bool operator==(const FaceInfo& a1, const FaceInfo& a2)
{
    bool result = true;

    result &= a1.size() == a2.size();
    if (result) {
        for (uint32_t i = 0; i < a1.size(); i++) {
            FaceInfo::Face face1 = a1.get(i);
            FaceInfo::Face face2 = a2.get(i);
            if (face1.rect      != face2.rect      ||
                face1.id        != face2.id        ||
                face1.score     != face2.score     ||
                face1.blink     != face2.blink     ||
                face1.roll_dir  != face2.roll_dir  ||
                face1.frameSize != face2.frameSize ||
                face1.frameType != face2.frameType) {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool operator==(const CameraTypeInf& a1, const CameraTypeInf& a2)
{
    return a1.type == a2.type;
}

bool operator==(const CommandInf& a1, const CommandInf& a2)
{
    bool result = true;

    result &= a1.type == a2.type;
    result &= a1.arg1 == a2.arg1;
    result &= a1.arg2 == a2.arg2;

    return result;
}

bool operator==(const SubCamData& a1, const SubCamData& a2)
{
    bool result = true;

    result &= a1.mainOTPSize   == a2.mainOTPSize;
    result &= a1.subOTPSize    == a2.subOTPSize;
    result &= a1.mainVCMDAC    == a2.mainVCMDAC;
    result &= a1.subVCMDAC     == a2.subVCMDAC;
    result &= a1.blurLevel     == a2.blurLevel;
    result &= a1.focusX        == a2.focusX;
    result &= a1.focusY        == a2.focusY;
    result &= a1.previewFocusX == a2.previewFocusX;
    result &= a1.previewFocusY == a2.previewFocusY;
    result &= a1.roiMapW       == a2.roiMapW;
    result &= a1.roiMapH       == a2.roiMapH;
    result &= COMPARE_SAME_STRING(a1.mainModuleName,  a2.mainModuleName);
    result &= COMPARE_SAME_STRING(a1.subModuleName,   a2.subModuleName);
    result &= COMPARE_SAME_STRING(a1.frontModuleName, a2.frontModuleName);
    result &= COMPARE_SAME_DATA(a1.mainOTP, a2.mainOTP, a1.mainOTPSize);
    result &= COMPARE_SAME_DATA(a1.subOTP,  a2.subOTP,  a1.subOTPSize);

    return result;
}

bool operator==(const CamDimension& a1, const CamDimension& a2)
{
    bool result = true;

    result &= a1.w == a2.w;
    result &= a1.h == a2.h;

    return result;
}

bool operator==(const DualCamMode& a1, const DualCamMode& a2)
{
    return a1.funcMask == a2.funcMask;
}

bool operator==(const ZoomInf& a1, const ZoomInf& a2)
{
    bool result = true;

    result &= a1.ratio   == a2.ratio;
    result &= a1.iso     == a2.iso;
    result &= a1.expTime == a2.expTime;

    return result;
}

bool operator==(const SingleBokehInf& a1, const SingleBokehInf& a2)
{
    bool result = true;

    result &= a1.mode      == a2.mode;
    result &= a1.blurLevel == a2.blurLevel;

    return result;
}

bool operator==(const PlatformPriv& a1, const PlatformPriv& a2)
{
    return a1.scaleDim == a2.scaleDim;
}

bool operator==(const MultiShotParm& a1, const MultiShotParm& a2)
{
    return a1.num == a2.num;
}

bool operator==(const MultiExporeParm& a1, const MultiExporeParm& a2)
{
    bool result = true;

    result &= a1.size() == a2.size();
    if (result) {
        for (uint32_t i = 0; i < a1.size(); i++) {
            if (a1.get(i) != a2.get(i)) {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool operator==(const PlatformPrivSet& a1, const PlatformPrivSet& a2)
{
    bool result = true;

    result &= a1.sharpen == a2.sharpen;
    result &= a1.denoise == a2.denoise;

    return result;
}


bool operator==(const EvtCallbackInf& a1, const EvtCallbackInf& a2)
{
    bool result = true;

    result &= a1.evt  == a2.evt;
    result &= a1.type == a2.type;
    result &= a1.arg  == a2.arg;

    return result;
}

bool operator==(const DataCallbackInf& a1, const DataCallbackInf& a2)
{
    bool result = true;

    result &= a1.type == a2.type;
    result &= a1.size == a2.size;
    result &= COMPARE_SAME_DATA(a1.data, a2.data, a1.size);

    return result;
}

#define OPERATOR_NOT_EQUAL(TYPE) \
    bool operator!=(const TYPE& a1, const TYPE& a2) \
    { \
        return !(a1 == a2); \
    }

OPERATOR_NOT_EQUAL(CamDimension);
OPERATOR_NOT_EQUAL(LongExpMode);
OPERATOR_NOT_EQUAL(FlipMode);
OPERATOR_NOT_EQUAL(WaterMarkInfo);
OPERATOR_NOT_EQUAL(BeautySetting);
OPERATOR_NOT_EQUAL(Rectangle);
OPERATOR_NOT_EQUAL(FaceInfo);
OPERATOR_NOT_EQUAL(CameraTypeInf);
OPERATOR_NOT_EQUAL(CommandInf);
OPERATOR_NOT_EQUAL(SubCamData);
OPERATOR_NOT_EQUAL(DualCamMode);
OPERATOR_NOT_EQUAL(ZoomInf);
OPERATOR_NOT_EQUAL(SingleBokehInf);
OPERATOR_NOT_EQUAL(PlatformPriv);
OPERATOR_NOT_EQUAL(MultiShotParm);
OPERATOR_NOT_EQUAL(MultiExporeParm);
OPERATOR_NOT_EQUAL(PlatformPrivSet);
OPERATOR_NOT_EQUAL(EvtCallbackInf);
OPERATOR_NOT_EQUAL(DataCallbackInf);

};

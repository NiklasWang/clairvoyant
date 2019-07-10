#ifndef _PAL_PARAMS_H_
#define _PAL_PARAMS_H_

#include "PandoraParameters.h"
#include "ConfigParameters.h"
#include "ConfigInterface.h"
#include "PalParmType.h"
#include "CameraStatus.h"

namespace pandora {

struct PalParms {
    enum PalParmType type;
    struct _g {
        enum GetParamType type;
        union _u {
            PreviewDim     previewSize;
            PictureDim     pictureSize;
            VideoDim       videoSize;
            FlashMode      flashMode;
            HdrMode        hdrMode;
            LongshotMode   longShot;
            LongExpMode    longExposure;
            SmartshotMode  smartShot;
            ImageStabMode  imageStab;
            VideoStabMode  videoStab;
            FlipMode       flipMode;
            NightShotMode  nightShot;
            BeautySetting  beautySetting;
            FaceInfo       faces;
            AEInfo         ae;
            AWBInfo        awb;
            AFInfo         af;
            HdrInfo        hdr;
            SceneInfo      scene;
            CameraTypeInf  camType;
            CommandInf     commandInf;
            DualCamMode    dualCamMode;
            SubCamData     subCamData;
            FairLightMode  fairLightMode;
            RotationAngle  rotation;
            WaterMarkInfo  watermark;
            ZoomInf        zoom;
            SingleBokehInf singleBokeh;
            PlatformPriv   platformPriv;
        } u;
    }g ;
    struct _s {
        enum SetParamType   type;
        union _u {
            MultiShotParm   multiShot;
            MultiExporeParm multiExposure;
            PlatformPrivSet platformPriv;
        } u;
    } s;
    struct _c {
        enum ConfigType     type;
        union _u {
            ConfigInterface::ThresType hdrConfiThres;
            ConfigInterface::IdType    extMsgID;
            FocusEndConfig  focusEnd;
            ExpChangeConfig expChange;
            NightStabConfig nightStabThres;
            LowLightConfig  lowlight;
            ParmsCategory   parmCategory;
        } u;
    } c;
    struct _o {
        enum OtherType      type;
        union _u {
            EvtCallbackInf  evt;
            DataCallbackInf data;
        } u;
    } o;

public:
    PalParms();
    PalParms(const PalParms &rhs) = default;
    PalParms &operator=(const PalParms &rhs) = default;

public:
    template<typename T>
    const char *getName(T type);
    const char *getName();
    const char *whoamI();

public:
    template<typename T>
    friend const T &operator<<(PalParms &parm, const T &rhs);
    template<typename T>
    friend const PalParms &operator>>(const PalParms &parm, T &rhs);
};

#define DECLARE_OPERATOR_IN(T) \
    template<> \
    const T &operator<<(PalParms &parm, const T &rhs)

#define DECLARE_OPERATOR_OUT(T) \
    template<> \
    const PalParms &operator>>(const PalParms &parm, T &rhs)

#define DECLARE_OPERATOR_IN_OUT(T) \
    DECLARE_OPERATOR_IN(T); \
    DECLARE_OPERATOR_OUT(T);


DECLARE_OPERATOR_IN_OUT(PalParmType);
DECLARE_OPERATOR_IN_OUT(GetParamType);
DECLARE_OPERATOR_IN_OUT(PreviewDim);
DECLARE_OPERATOR_IN_OUT(PictureDim);
DECLARE_OPERATOR_IN_OUT(VideoDim);
DECLARE_OPERATOR_IN_OUT(FlashMode);
DECLARE_OPERATOR_IN_OUT(HdrMode);
DECLARE_OPERATOR_IN_OUT(LongshotMode);
DECLARE_OPERATOR_IN_OUT(LongExpMode);
DECLARE_OPERATOR_IN_OUT(SmartshotMode);
DECLARE_OPERATOR_IN_OUT(ImageStabMode);
DECLARE_OPERATOR_IN_OUT(VideoStabMode);
DECLARE_OPERATOR_IN_OUT(FlipMode);
DECLARE_OPERATOR_IN_OUT(NightShotMode);
DECLARE_OPERATOR_IN_OUT(BeautySetting);
DECLARE_OPERATOR_IN_OUT(FaceInfo);
DECLARE_OPERATOR_IN_OUT(AEInfo);
DECLARE_OPERATOR_IN_OUT(AWBInfo);
DECLARE_OPERATOR_IN_OUT(AFInfo);
DECLARE_OPERATOR_IN_OUT(HdrInfo);
DECLARE_OPERATOR_IN_OUT(SceneInfo);
DECLARE_OPERATOR_IN_OUT(CameraTypeInf);
DECLARE_OPERATOR_IN_OUT(CommandInf);
DECLARE_OPERATOR_IN_OUT(DualCamMode);
DECLARE_OPERATOR_IN_OUT(SubCamData);
DECLARE_OPERATOR_IN_OUT(RotationAngle);
DECLARE_OPERATOR_IN_OUT(WaterMarkInfo);
DECLARE_OPERATOR_IN_OUT(ZoomInf);
DECLARE_OPERATOR_IN_OUT(SingleBokehInf);
DECLARE_OPERATOR_IN_OUT(PlatformPriv);

DECLARE_OPERATOR_IN_OUT(SetParamType);
DECLARE_OPERATOR_IN_OUT(MultiShotParm);
DECLARE_OPERATOR_IN_OUT(MultiExporeParm);
DECLARE_OPERATOR_IN_OUT(PlatformPrivSet);

DECLARE_OPERATOR_IN_OUT(ConfigType);
DECLARE_OPERATOR_IN_OUT(ConfigInterface::ThresType);
DECLARE_OPERATOR_IN_OUT(ConfigInterface::IdType);
DECLARE_OPERATOR_IN_OUT(FocusEndConfig);
DECLARE_OPERATOR_IN_OUT(ExpChangeConfig);
DECLARE_OPERATOR_IN_OUT(NightStabConfig);
DECLARE_OPERATOR_IN_OUT(LowLightConfig);
DECLARE_OPERATOR_IN_OUT(ParmsCategory);

DECLARE_OPERATOR_IN_OUT(OtherType);
DECLARE_OPERATOR_IN_OUT(EvtCallbackInf);
DECLARE_OPERATOR_IN_OUT(DataCallbackInf);

};

#endif

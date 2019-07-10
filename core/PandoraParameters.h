#ifndef _PANDORA_PARAMETERS_H_
#define _PANDORA_PARAMETERS_H_

#include <stdint.h>

#include "PandoraInterface.h"

namespace pandora {

#define MAX_FACE_NUM           5
#define MAX_MULTI_EXPOSURE_NUM 6

struct CameraTypeInf {
    enum CameraType {
        CAMERA_TYPE_BACK_0,
        CAMERA_TYPE_BACK_1,
        CAMERA_TYPE_FRONT_0,
        CAMERA_TYPE_FRONT_1,
        CAMERA_TYPE_MONO_0,
        CAMERA_TYPE_MONO_1,
        CAMERA_TYPE_TELE_0,
        CAMERA_TYPE_TELE_1,
        CAMERA_TYPE_MAX_INVALID
    };

    CameraType type;

public:
    bool isBackCamera();
    bool isFrontCamera();
    bool isSubCamera();
    bool isNobody();
};

enum ExtendedCBEvt {
    EEVT_FOCUS_CHANGED,
    EEVT_LUMINANCE_CHANGED,
    EEVT_SMART_SELECT,
    EEVT_ALG_PROCESSING,
    EEVT_ALG_FINISHED,
    EEVT_PORTRAIT_BLINK,
    EEVT_FLASH_STATUS,
    EEVT_HDR_STATUS,
    EEVT_IMAG_STAB_STATUS,
    EEVT_LIGHT_STATUS,
    EEVT_MAX_INVALID,
};

enum FlashEvt {
    FLASH_OFF_EVT,
    FLASH_ON_EVT,
};

enum HdrEvt {
    HDR_OFF_EVT,
    HDR_ON_EVT,
};

enum ImagStabEvt {
    IMAGE_STAB_OFF_EVT,
    IMAGE_STAB_ON_EVT,
};

enum LightEnvEvt {
    NORMAL_LIGHT_EVT,
    LOW_LIGHT_EVT,
};

enum PortraitBlinkEvt {
    PORTRAIT_EYE_OPEN_EVT,
    PORTRAIT_EYE_BLINK_EVT,
};

const char *getEvtName(ExtendedCBEvt evt);

bool isEvtValid(ExtendedCBEvt evt);

struct CamDimension {
    int32_t w;
    int32_t h;

public:
    void set(int32_t _w = 0, int32_t _h = 0);
    int32_t size() const;
};

struct PreviewDim :
    public CamDimension {
};

struct PictureDim :
    public CamDimension {
};

struct VideoDim :
    public CamDimension {
};

enum FlashMode {
    FLASH_MODE_AUTO,
    FLASH_MODE_TORCH,
    FLASH_MODE_FORCED_ON,
    FLASH_MODE_FORCED_OFF,
};

enum HdrMode {
    HDR_MODE_AUTO,
    HDR_MODE_FORCED_ON,
    HDR_MODE_FORCED_OFF,
};

struct LongExpMode {
    bool  on;
    float time;
};

struct FlipMode {
    bool h;
    bool v;
};

enum DualCamFunction {
    DUAL_CAMERA_FUNC_RT_BOKEH,
    DUAL_CAMERA_FUNC_STILL_BOKEH,
    DUAL_CAMERA_FUNC_REFOCUS,
    DUAL_CAMERA_FUNC_NIGHT_SHOT,
    DUAL_CAMERA_FUNC_OPTICAL_ZOOM,
    DUAL_CAMERA_FUNC_EXTERNAL,
    DUAL_CAMERA_FUNC_MAX_INVALID,
};

enum DualCamFunctionMask {
    DUAL_CAMERA_MASK_RT_BOKEH     = 0x01 << DUAL_CAMERA_FUNC_RT_BOKEH,
    DUAL_CAMERA_MASK_STILL_BOKEH  = 0x01 << DUAL_CAMERA_FUNC_STILL_BOKEH,
    DUAL_CAMERA_MASK_REFOCUS      = 0x01 << DUAL_CAMERA_FUNC_REFOCUS,
    DUAL_CAMERA_MASK_NIGHT_SHOT   = 0x01 << DUAL_CAMERA_FUNC_NIGHT_SHOT,
    DUAL_CAMERA_MASK_OPTICAL_ZOOM = 0x01 << DUAL_CAMERA_FUNC_OPTICAL_ZOOM,
    DUAL_CAMERA_MASK_EXTERNAL     = 0x01 << DUAL_CAMERA_FUNC_EXTERNAL,
    DUAL_CAMERA_MASK_NONE_FUNC    = 0x00,
};

struct DualCamMode {
    int32_t funcMask;

public:
    bool enableRTBokeh();
    bool enableStillBokeh();
    bool enableRefocus();
    bool enableNightshot();
    bool enableOpticalZoom();
    bool enableExternal();
    bool enabledAny();
    bool enabledNone();
};

enum FairLightMode {
    FAIRLIGHT_NONE,
    FAIRLIGHT_RAINBOW,
    FAIRLIGHT_MORNING,
    FAIRLIGHT_WAVE,
    FAIRLIGHT_CONTOUR,
    FAIRLIGHT_SHADOW,
    FAIRLIGHT_STAGE,
};

enum RotationAngle {
    ROTATION_ANGLE_0,
    ROTATION_ANGLE_90,
    ROTATION_ANGLE_180,
    ROTATION_ANGLE_270,
    ROTATION_ANGLE_MAX_INVALID,
};

enum WaterMarkWorkMode {
    WM_WORK_COVERAGE,
    WM_WORK_TRANSPARENT,
    WM_WORK_PUREWHITE,
};

enum WaterMarkMode {
    WATERMARK_MODE_OFF,
    WATERMARK_MODE_ON,
};

struct Ratio {
    int32_t numerator;
    int32_t denominator;
};

struct WaterMarkInfo {
    WaterMarkMode           mode;
    int32_t                 dp;
    int32_t                 dpi;
    Ratio                   wmRatio;
    char                    *product;
    char                    *build;
    char                    *package;
    bool                    debug;
    int32_t                 id;
    CameraTypeInf           camType;
    bool                    enableProduct;
    bool                    enableLogo;
    bool                    enableTexts;
};

enum LongshotMode {
    LONG_SHOT_MODE_OFF,
    LONG_SHOT_MODE_ON,
    LONG_SHOT_MODE_MAX_INVALID,
};

enum SmartshotMode {
    SMART_SHOT_MODE_OFF,
    SMART_SHOT_MODE_ON,
    SMART_SHOT_MODE_MAX_INVALID,
};

enum ImageStabMode {
    IMAGE_STAB_MODE_OFF,
    IMAGE_STAB_MODE_ON,
    IMAGE_STAB_MODE_AUTO,
    IMAGE_STAB_MODE_MAX_INVALID,
};

enum VideoStabMode {
    VIDEO_STAB_MODE_OFF,
    VIDEO_STAB_MODE_VIDEO_ONLY,
    VIDEO_STAB_MODE_PREVIEW_ONLY,
    VIDEO_STAB_MODE_PREVIEW_VIDEO,
    VIDEO_STAB_MODE_MAX_INVALID,
};

enum NightShotMode {
    NIGHT_SHOT_MODE_OFF,
    NIGHT_SHOT_MODE_ON,
    NIGHT_SHOT_MODE_AUTO,
    NIGHT_SHOT_MODE_MAX_INVALID,
};

enum BeautyMode {
    BEAUTY_MODE_OFF,
    BEAUTY_MODE_ON,
    BEAUTY_MODE_MANUAL,
    BEAUTY_MODE_UNDEFINED,
    BEAUTY_MODE_MAX_INVALID,
};

struct BeautySetting {
    BeautyMode mode;
    int32_t strength;
};

struct Rectangle {
    int32_t left;
    int32_t top;
    int32_t width;
    int32_t height;
};

struct FaceInfo {
    struct Face {
        Rectangle rect;
        int32_t id;    // For face recognition
        int32_t score; // Confidence, 0 - 100
        int32_t blink; // 0 - Open, 1 - Blink
        int32_t roll_dir;
        CamDimension frameSize;
        FrameType frameType;
    };

private:
    Face faces[MAX_FACE_NUM];
    size_t num;

public:
    size_t add(Face face);
    void update(Face face, uint32_t index);
    Face get(uint32_t i) const;
    void clear();
    size_t size() const;
};

struct MultiShotParm {
    uint32_t num;
};

struct MultiExporeParm {
private:
    uint32_t num;
    int32_t exp[MAX_MULTI_EXPOSURE_NUM];

public:
    size_t add(int32_t value);
    int32_t get(uint32_t i) const;
    void clear();
    size_t size() const;
    void set(size_t size);
};

struct PlatformPrivSet {
    int32_t sharpen;
    int32_t denoise;
};

struct EvtCallbackInf {
    int32_t evt;
    int32_t type;
    int32_t arg;
};

struct DataCallbackInf {
    int32_t type;
    size_t  size;
    void   *data;
    void  (*release)(void *arg);
};

struct CommandInf {
    enum CommandType {
        COMMAND_TYPE_LONGSHOT_OFF,
        COMMAND_TYPE_LONGSHOT_ON,
        COMMAND_TYPE_MAX_INVALID,
    };

    CommandType type;
    int32_t arg1;
    int32_t arg2;
};

struct SubCamData {
    char *mainModuleName;
    char *subModuleName;
    char *frontModuleName;
    uint8_t *mainOTP;
    int32_t mainOTPSize;
    uint8_t *subOTP;
    int32_t subOTPSize;
    int32_t mainVCMDAC;
    int32_t subVCMDAC;
    int32_t blurLevel;
    int32_t focusX;
    int32_t focusY;
    int32_t previewFocusX;
    int32_t previewFocusY;
    int32_t roiMapW;
    int32_t roiMapH;
};

struct ZoomInf {
    float   ratio;
    int32_t iso;
    float   expTime; // nsec

};

enum SingleBokehMode {
    SINGLE_CAM_BOKEH_OFF,
    SINGLE_CAM_BOKEH_ON,
    SINGLE_CAM_BOKEH_MAX_INVALID,
};

struct SingleBokehInf {
    SingleBokehMode mode;
    int32_t         blurLevel;
};


struct PlatformPriv {
    CamDimension scaleDim;
};


bool operator==(const CamDimension& a1,     const CamDimension& a2);
bool operator==(const LongExpMode& a1,     const LongExpMode& a2);
bool operator==(const FlipMode& a1,        const FlipMode& a2);
bool operator==(const WaterMarkInfo& a1,   const WaterMarkInfo& a2);
bool operator==(const BeautySetting& a1,   const BeautySetting& a2);
bool operator==(const Rectangle& a1,       const Rectangle& a2);
bool operator==(const FaceInfo& a1,        const FaceInfo& a2);
bool operator==(const CameraTypeInf& a1,   const CameraTypeInf& a2);
bool operator==(const CommandInf& a1,      const CommandInf& a2);
bool operator==(const SubCamData& a1,      const SubCamData& a2);
bool operator==(const DualCamMode& a1,     const DualCamMode& a2);
bool operator==(const ZoomInf& a1,         const ZoomInf& a2);
bool operator==(const SingleBokehInf& a1,  const SingleBokehInf& a2);
bool operator==(const PlatformPriv& a1,    const PlatformPriv& a2);
bool operator==(const MultiShotParm& a1,   const MultiShotParm& a2);
bool operator==(const MultiExporeParm& a1, const MultiExporeParm& a2);
bool operator==(const PlatformPrivSet& a1, const PlatformPrivSet& a2);
bool operator==(const EvtCallbackInf& a1,  const EvtCallbackInf& a2);
bool operator==(const DataCallbackInf& a1, const DataCallbackInf& a2);

bool operator!=(const CamDimension& a1,    const CamDimension& a2);
bool operator!=(const LongExpMode& a1,     const LongExpMode& a2);
bool operator!=(const FlipMode& a1,        const FlipMode& a2);
bool operator!=(const WaterMarkInfo& a1,   const WaterMarkInfo& a2);
bool operator!=(const BeautySetting& a1,   const BeautySetting& a2);
bool operator!=(const Rectangle& a1,       const Rectangle& a2);
bool operator!=(const FaceInfo& a1,        const FaceInfo& a2);
bool operator!=(const CameraTypeInf& a1,   const CameraTypeInf& a2);
bool operator!=(const CommandInf& a1,      const CommandInf& a2);
bool operator!=(const SubCamData& a1,      const SubCamData& a2);
bool operator!=(const DualCamMode& a1,     const DualCamMode& a2);
bool operator!=(const ZoomInf& a1,         const ZoomInf& a2);
bool operator!=(const SingleBokehInf& a1,  const SingleBokehInf& a2);
bool operator!=(const PlatformPriv& a1,    const PlatformPriv& a2);
bool operator!=(const MultiShotParm& a1,   const MultiShotParm& a2);
bool operator!=(const MultiExporeParm& a1, const MultiExporeParm& a2);
bool operator!=(const PlatformPrivSet& a1, const PlatformPrivSet& a2);
bool operator!=(const EvtCallbackInf& a1,  const EvtCallbackInf& a2);
bool operator!=(const DataCallbackInf& a1, const DataCallbackInf& a2);

};

#endif

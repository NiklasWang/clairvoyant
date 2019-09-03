#ifndef BEATUY_FACE_ARCSOFT_ENGINE_H_
#define BEATUY_FACE_ARCSOFT_ENGINE_H_

#include "Modules.h"
#include "PandoraInterface.h"

namespace vendor {
#include "merror.h"
#include "asvloffscreen.h"
#include "Beautyshot_Algorithm.h"
};

namespace pandora {

struct BeautyFaceParm {
    CameraTypeInf camType;
    int32_t       manualStrength;
    int32_t       luxIndex;
    bool          isBacklight;
    int32_t       age;
    GenderType    gender;
    RotationAngle rotation;
    BeautyFaceParm() :
        manualStrength(0),
        luxIndex(200),
        isBacklight(false),
        age(25),
        gender(GENDER_TYPE_FEMALE),
        rotation(ROTATION_ANGLE_0) {
            camType.type = CameraTypeInf::CAMERA_TYPE_MAX_INVALID;
        }
};

class BeautyFaceArcsoft
{
public:
    BeautyFaceArcsoft(FrameType type = FRAME_TYPE_PREVIEW);
    ~BeautyFaceArcsoft();
    int32_t init();
    int32_t deinit();
    int32_t setParm(BeautyFaceParm &parm);
    int32_t doProcess(unsigned char *frame,
        int32_t w, int32_t h, int32_t stride, int32_t scanline);

public:
    enum BeautyFaceParmItem {
        BEAUTY_FACE_SKIN_SOFTEN,
        BEAUTY_FACE_SKIN_WHITEN,
        BEAUTY_FACE_ANTI_SHINE,
        BEAUTY_FACE_TEETH_WHITEN,
        BEAUTY_FACE_EYE_ENLARGEMENT,
        BEAUTY_FACE_EYE_BRIGHTEN,
        BEAUTY_FACE_TZONE_HIGHTLIGHT,
        BEAUTY_FACE_SLENDER_FACE,
        BEAUTY_FACE_DE_BLEMISH,
        BEAUTY_FACE_DE_POUCH,
        BEAUTY_FACE_MAX_INVALID,
    };

    template<typename K, typename V>
    struct ParmMap {
        K key;
        V value;
    };

private:
    int32_t drawWaterMark(FrameType type,
        BeautyFaceParm &parm, GlobalTaskType &task, bool manual);
    int32_t drawManualWaterMark(FrameType type,
        BeautyFaceParm &parm, GlobalTaskType &task);
    int32_t drawAutoWaterMark(FrameType type,
        BeautyFaceParm &parm, GlobalTaskType &task);

private:
    bool       mInited;
    ModuleType mModule;
    FrameType  mType;
    BeautyFaceParm mParm;
    bool       mDebug;
    sp<IAlgorithm> mWM[FRAME_TYPE_MAX_INVALID];

private:
    vendor::ASVLOFFSCREEN         ImgDataInput;
    vendor::TFaces                mFaceInput;
    vendor::BeautyShot_Algorithm *mEngine;
};

};

#endif


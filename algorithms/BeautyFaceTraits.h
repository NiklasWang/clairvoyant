#ifndef _BEAUTY_FACE_TRAITS_H_
#define _BEAUTY_FACE_TRAITS_H_

#include "AlgorithmIntf.h"
#include "PandoraParameters.h"

namespace pandora {

USE_DEFAULT_TASK_TYPE(BeautyFace);

USE_DEFAULT_UPDATE_TYPE(BeautyFace);

USE_DEFAULT_REQUEST_TYPE(BeautyFace);

USE_DEFAULT_STATUS_TYPE(BeautyFace);

struct BeautyFaceResultType :
    public ResultType {
public:
    int32_t w;
    int32_t h;
    size_t  size;
    void   *data;

public:
    void dump() override {
        LOGI(MODULE_ALGORITHM,
            "BeautyFace result: w %d h %d size %ld data %p",
            w, h, size, data);
    }
};

enum GenderType {
    GENDER_TYPE_MALE,
    GENDER_TYPE_FEMALE,
    GENDER_TYPE_UNKNOWN,
    GENDER_TYPE_MAX_INVALID,
};

struct BeautyFaceParmType :
    public ParmType
{
    int32_t manual;
    int32_t luxIndex;
    int32_t backlight;
    int32_t age;
    GenderType    gender;
    CameraTypeInf camType;
    RotationAngle rotation;
};


DEFINE_ALGORITHM_TRAITS(BeautyFace);


};

#endif

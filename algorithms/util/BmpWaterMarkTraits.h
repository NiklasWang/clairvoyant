#ifndef _BMP_WATER_MARK_TRAITS_H_
#define _BMP_WATER_MARK_TRAITS_H_

#include "AlgorithmIntf.h"
#include "PandoraParameters.h"

namespace pandora {

USE_DEFAULT_TASK_TYPE(BmpWaterMark);

USE_DEFAULT_UPDATE_TYPE(BmpWaterMark);

USE_DEFAULT_REQUEST_TYPE(BmpWaterMark);

USE_DEFAULT_STATUS_TYPE(BmpWaterMark);

struct BmpWaterMarkResultType :
    public ResultType {
public:
    int32_t w;
    int32_t h;
    size_t  size;
    void   *data;

public:
    void dump() override {
        LOGI(MODULE_ALGORITHM, "BmpWaterMark result: "
            "w %d h %d size %ld data %p", w, h, size, data);
    }
};

enum BmpWaterMarkPosition {
    BMP_WATER_MARK_POSITION_TOP_LEFT,
    BMP_WATER_MARK_POSITION_BOTTOM_LEFT,
    BMP_WATER_MARK_POSITION_BOTTOM_RIGHT,
    BMP_WATER_MARK_POSITION_TOP_RIGHT,
    BMP_WATER_MARK_POSITION_MIDDLE,
    BMP_WATER_MARK_POSITION_MAX_INVALID,
};

struct  BmpWaterMarkParmType :
    public ParmType {
public:
    int32_t  dp;
    int32_t  dpi;
    Ratio    wmRatio;
    bool     needMirror;
    RotationAngle        rotation;
    BmpWaterMarkPosition position;
    WaterMarkWorkMode    workmode;
};


DEFINE_ALGORITHM_TRAITS(BmpWaterMark);


};

#endif


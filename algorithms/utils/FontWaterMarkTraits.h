#ifndef _FONT_WATER_MARK_TRAITS_H_
#define _FONT_WATER_MARK_TRAITS_H_

#include <list>
#include <string>

#include "AlgorithmIntf.h"
#include "PandoraParameters.h"

namespace pandora {

USE_DEFAULT_TASK_TYPE(FontWaterMark);

USE_DEFAULT_RESULT_TYPE(FontWaterMark);

USE_DEFAULT_UPDATE_TYPE(FontWaterMark);

USE_DEFAULT_REQUEST_TYPE(FontWaterMark);

USE_DEFAULT_STATUS_TYPE(FontWaterMark);

enum FontWaterMarkPosition {
    FONT_WATER_MARK_POSITION_TOP_LEFT,
    FONT_WATER_MARK_POSITION_BOTTOM_LEFT,
    FONT_WATER_MARK_POSITION_BOTTOM_RIGHT,
    FONT_WATER_MARK_POSITION_TOP_RIGHT,
    FONT_WATER_MARK_POSITION_MIDDLE,
    FONT_WATER_MARK_POSITION_MAX_INVALID,
};

struct FontWaterMarkTextList {
    std::string str;
    int32_t     size;
};

struct FontWaterMarkParmType :
    public ParmType {
public:
    typedef FontWaterMarkTextList TextType;

public:
    bool                  needMirror;
    RotationAngle         rotation;
    FontWaterMarkPosition position;
    std::list<TextType>  *texts;
};


DEFINE_ALGORITHM_TRAITS(FontWaterMark);


};

#endif


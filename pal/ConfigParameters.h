#ifndef _CONFIG_PARAMETERS_H_
#define _CONFIG_PARAMETERS_H_

#include "PalParmType.h"

namespace pandora {

struct FocusEndConfig {
    float macroEndRatio;
    float infiniteEndRatio;
};

struct ExpChangeConfig {
    int32_t  luxChangeThres;
    int32_t  isoChangeThres;
    int32_t  exposureChangeThres;
    float    apertureChangeThres;
};

struct NightStabConfig {
    int32_t  luxThre;
    int32_t  isoThres;
    float    exposureThres;
    float    apertureThres;
};

struct LowLightConfig {
    int32_t  luxThre;
    int32_t  isoThres;
    float    exposureThres;
    float    apertureThres;
};

enum ParmCategoryType {
    PARM_CATEGORY_TYPE_1,
    PARM_CATEGORY_TYPE_2,
    PARM_CATEGORY_TYPE_3,
    PARM_CATEGORY_TYPE_4,
    PARM_CATEGORY_TYPE_5,
    // More category can be added here,
    // Each category will run in indival thread.
    PARM_CATEGORY_TYPE_FAST,  // This type won't execute on thread
    PARM_CATEGORY_MAX_INVALID,
};

struct ParmCategory {
    PalParmType      type;
    union {
        GetParamType g;
        SetParamType s;
        ConfigType   c;
        OtherType    o;
    } u;
    ParmCategoryType category;
};

struct ParmsCategory {
    size_t size;
    ParmCategory *data;
};


bool operator==(const FocusEndConfig& a1,  const FocusEndConfig& a2);
bool operator==(const ExpChangeConfig& a1, const ExpChangeConfig& a2);
bool operator==(const NightStabConfig& a1, const NightStabConfig& a2);
bool operator==(const LowLightConfig& a1,  const LowLightConfig& a2);
bool operator==(const ParmCategory& a1,    const ParmCategory& a2);
bool operator==(const ParmsCategory& a1,   const ParmsCategory& a2);

bool operator!=(const FocusEndConfig& a1,  const FocusEndConfig& a2);
bool operator!=(const ExpChangeConfig& a1, const ExpChangeConfig& a2);
bool operator!=(const NightStabConfig& a1, const NightStabConfig& a2);
bool operator!=(const LowLightConfig& a1,  const LowLightConfig& a2);
bool operator!=(const ParmCategory& a1,    const ParmCategory& a2);
bool operator!=(const ParmsCategory& a1,   const ParmsCategory& a2);

};

#endif

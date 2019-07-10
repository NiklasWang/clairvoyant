#include <math.h>

#include "Common.h"
#include "ConfigParameters.h"

namespace pandora {

bool operator==(const FocusEndConfig& a1, const FocusEndConfig& a2)
{
    bool result = true;

    result &= EQUALFLOAT(a1.macroEndRatio,    a2.macroEndRatio);
    result &= EQUALFLOAT(a1.infiniteEndRatio, a2.infiniteEndRatio);

    return result;
}

bool operator==(const ExpChangeConfig& a1, const ExpChangeConfig& a2)
{
    bool result = true;

    result &= a1.luxChangeThres      == a2.luxChangeThres;
    result &= a1.isoChangeThres      == a2.isoChangeThres;
    result &= a1.exposureChangeThres == a2.exposureChangeThres;
    result &= EQUALFLOAT(a1.apertureChangeThres, a2.apertureChangeThres);

    return result;
}

bool operator==(const NightStabConfig& a1, const NightStabConfig& a2)
{
    bool result = true;

    result &= a1.luxThre  == a2.luxThre;
    result &= a1.isoThres == a2.isoThres;
    result &= EQUALFLOAT(a1.exposureThres, a2.exposureThres);
    result &= EQUALFLOAT(a1.apertureThres, a2.apertureThres);

    return result;
}

bool operator==(const LowLightConfig& a1, const LowLightConfig& a2)
{
    bool result = true;

    result &= a1.luxThre  == a2.luxThre;
    result &= a1.isoThres == a2.isoThres;
    result &= EQUALFLOAT(a1.exposureThres, a2.exposureThres);
    result &= EQUALFLOAT(a1.apertureThres, a2.apertureThres);

    return result;
}

bool operator==(const ParmCategory& a1, const ParmCategory& a2)
{
    bool result = true;

    result &= a1.type     == a2.type;
    result &= a1.category == a2.category;
    if (result) {
        switch (a1.type) {
            case PARM_TYPE_GET:
                result &= a1.u.g == a2.u.g;
                break;
            case PARM_TYPE_SET:
                result &= a1.u.s == a2.u.s;
                break;
            case PARM_TYPE_CONFIG:
                result &= a1.u.c == a2.u.c;
                break;
            case PARM_TYPE_OTHERS:
                result &= a1.u.o == a2.u.o;
                break;
            case PARM_TYPE_MAX_INVALID:
            default:
                result &= a1.u.g == a2.u.g;
                break;
        }
    }

    return result;
}

bool operator==(const ParmsCategory& a1, const ParmsCategory& a2)
{
    bool result = true;

    result &= a1.size == a2.size;
    if (result) {
        for (uint32_t i = 0; i < a1.size; i++) {
            if (a1.data[i] != a2.data[i]) {
                result = false;
                break;
            }
        }
    }

    return result;
}

#define OPERATOR_NOT_EQUAL(TYPE) \
    bool operator!=(const TYPE& a1, const TYPE& a2) \
    { \
        return !(a1 == a2); \
    }

OPERATOR_NOT_EQUAL(FocusEndConfig);
OPERATOR_NOT_EQUAL(ExpChangeConfig);
OPERATOR_NOT_EQUAL(NightStabConfig);
OPERATOR_NOT_EQUAL(LowLightConfig);
OPERATOR_NOT_EQUAL(ParmCategory);
OPERATOR_NOT_EQUAL(ParmsCategory);

};

#include "AlgorithmType.h"

namespace pandora {

static const char *const gAlgName[] = {
    [ALG_BEAUTY_FACE]          = "beauty face",
    [ALG_BEAUTY_FACE_CAP]      = "beauty face capture",
    [ALG_AGE_GENDER_DETECTION] = "age gender detection",
    [ALG_HDR_CHECKER]          = "hdr checker",
    [ALG_HDR]                  = "hdr",
    [ALG_IMAGE_STABILIZATION]  = "image stabilization",
    [ALG_SMART_SELECT]         = "smart select",
    [ALG_SMART_SHOT]           = "smart shot",
    [ALG_NIGHT_SHOT]           = "night shot",
    [ALG_ANTI_SHAKING]         = "anti shaking",
    [ALG_VIDEO_STAB_PREVIEW]   = "video stabilizaiton preview mode",
    [ALG_VIDEO_STAB_RECORDING] = "video stabilizaiton video mode",
    [ALG_PICTURE_ZOOM]         = "picture zoom",
    [ALG_FAIR_LIGHT]           = "fair light",
    [ALG_MICRO_PLASTIC]        = "micro plastic",
    [ALG_PICTURE_ZOOM2]        = "picture zoom2",
    [ALG_STILL_BOKEH]          = "still bokeh",
    [ALG_STILL_BOKEHA]         = "still bokeh",
    [ALG_RT_BOKEH]             = "real time bokeh",
    [ALG_DUAL_CAM_NIGHT_SHOT]  = "dual camera night shot",
    [ALG_SUPER_RESOLUTION]     = "super resolution",
    [ALG_SINGLE_BOKEH]         = "single camera bokeh",
    [ALG_SINGLE_BOKEH_CAP]     = "single camera bokeh cap",
    [ALG_UTILS]                = "util algorithm",
    [ALG_WATER_MARK]           = "water mark",
    [ALG_MAX_INVALID]          = "max invalid algorithm",
};

static bool checkValid(AlgType type)
{
    bool rc = false;

    if (type >= 0 && type < ALG_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

AlgType getValidType(AlgType type)
{
    return checkValid(type) ? type : ALG_MAX_INVALID;
}

const char *getAlgName(AlgType type)
{
    return gAlgName[getValidType(type)];
}

};


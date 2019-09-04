#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "CameraParameters.h"
#include "Logs.h"
#include "Common.h"

namespace pandora {

const char CameraParameters::ON[] = "on";
const char CameraParameters::OFF[] = "off";
const char CameraParameters::TRUE[] = "true";
const char CameraParameters::FALSE[] = "false";

const char CameraParameters::KEY_WATER_MARK[] = "water-mark";
const char CameraParameters::KEY_NIGHT_SHOT[] = "night-shot";
const char CameraParameters::KEY_BEAUTY_FACE[] = "beauty-face";
const char CameraParameters::KEY_BEAUTY_FACE_LEVEL[] = "beauty-face-level";
const char CameraParameters::KEY_LONG_SHOT[] = "long-shot";
const char CameraParameters::KEY_SMART_SHOT[] = "smart-shot";
const char CameraParameters::KEY_FAIRLIGHT_MODE[] = "fairlight-node";
const char CameraParameters::KEY_QC_SINGLE_BOKEH[] = "single-bokeh";
const char CameraParameters::KEY_QC_SINGLE_BOKEH_LEVEL[] = "single-bokeh-level";
const char CameraParameters::KEY_SCENE_MODE[] = "scene-mode";
const char CameraParameters::KEY_FLASH_MODE[] = "flash-mode";
const char CameraParameters::KEY_VIDEO_PLIP[] = "video-flip";
const char CameraParameters::KEY_MOVIE_SOLID[] = "movie-solid";
const char CameraParameters::KEY_LONG_EXPOSURE[] = "long-exposure";
const char CameraParameters::KEY_BLUR_LEVEL[] = "blur-level";
const char CameraParameters::KEY_ROTATION[] = "rotation";

const char CameraParameters::FLASH_MODE_OFF[] = "off";
const char CameraParameters::FLASH_MODE_AUTO[] = "auto";
const char CameraParameters::FLASH_MODE_ON[] = "on";
const char CameraParameters::FLASH_MODE_RED_EYE[] = "red-eye";
const char CameraParameters::FLASH_MODE_TORCH[] = "torch";

const char CameraParameters::SCENE_MODE_AUTO[] = "auto";
const char CameraParameters::SCENE_MODE_ACTION[] = "action";
const char CameraParameters::SCENE_MODE_PORTRAIT[] = "portrait";
const char CameraParameters::SCENE_MODE_LANDSCAPE[] = "landscape";
const char CameraParameters::SCENE_MODE_NIGHT[] = "night";
const char CameraParameters::SCENE_MODE_NIGHT_PORTRAIT[] = "night-portrait";
const char CameraParameters::SCENE_MODE_THEATRE[] = "theatre";
const char CameraParameters::SCENE_MODE_BEACH[] = "beach";
const char CameraParameters::SCENE_MODE_SNOW[] = "snow";
const char CameraParameters::SCENE_MODE_SUNSET[] = "sunset";
const char CameraParameters::SCENE_MODE_STEADYPHOTO[] = "steadyphoto";
const char CameraParameters::SCENE_MODE_FIREWORKS[] = "fireworks";
const char CameraParameters::SCENE_MODE_SPORTS[] = "sports";
const char CameraParameters::SCENE_MODE_PARTY[] = "party";
const char CameraParameters::SCENE_MODE_CANDLELIGHT[] = "candlelight";
const char CameraParameters::SCENE_MODE_BARCODE[] = "barcode";
const char CameraParameters::SCENE_MODE_HDR[] = "hdr";
const char CameraParameters::SCENE_MODE_ASD[] = "asd";

const char CameraParameters::FAIRLIGHT_VALUE_RAINBOW[] = "rainbow";
const char CameraParameters::FAIRLIGHT_VALUE_MORNING[] = "moring";
const char CameraParameters::FAIRLIGHT_VALUE_WAVE[] = "wave";
const char CameraParameters::FAIRLIGHT_VALUE_CONTOUR[] = "contour";
const char CameraParameters::FAIRLIGHT_VALUE_SHADOW[] = "shadow";
const char CameraParameters::FAIRLIGHT_VALUE_STAGE[] = "stage";

CameraParameters::CameraParameters():mMap()
{
}

CameraParameters::~CameraParameters()
{
}

void CameraParameters::set(const char * key, const char * value)
{
	mMap[key] = value;
}

void CameraParameters::set(const char * key, int value)
{
    char str[16];
    sprintf(str, "%d", value);
    set(key, str);
}

void CameraParameters::setFloat(const char * key, float value)
{
    char str[16];  // 14 should be enough. We overestimate to be safe.
    snprintf(str, sizeof(str), "%g", value);
    set(key, str);
}

const char* CameraParameters::get(const char * key) const
{
    if(mMap.find(key) != mMap.end())
    {
        return mMap.find(key)->second.c_str();
    }
    return NULL;
}

int CameraParameters::getInt(const char * key) const
{
    const char *v = get(key);
    if (v == 0)
        return -1;
    return strtol(v, 0, 0);
}

float CameraParameters::getFloat(const char *key) const
{
    const char *v = get(key);
    if (v == 0)
        return -1;
    return strtof(v, 0);
}


};

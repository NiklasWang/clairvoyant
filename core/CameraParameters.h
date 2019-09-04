#ifndef _CAMERA_PARAMETERS_H
#define _CAMERA_PARAMETERS_H

#include <map>

using namespace std;

namespace pandora {

struct Size {
    int width;
    int height;

    Size() {
        width = 0;
        height = 0;
    }

    Size(int w, int h) {
        width = w;
        height = h;
    }
};

enum {
    MODE_OFF,
    MODE_ON,
    MODE_MANUAL,
    MODE_INVALID,
};

class CameraParameters
{
public:
    CameraParameters();
    ~CameraParameters();

    void set(const char *  key, const char * value);
    void set(const char *  key, int value);
    void setFloat(const char * key, float value);
    const char* get(const char *  key) const;
    int getInt(const char *  key) const;
    float getFloat(const char *key) const;

    static const char ON[];
    static const char OFF[];
    static const char TRUE[];
    static const char FALSE[];

    static const char KEY_WATER_MARK[];
    static const char KEY_NIGHT_SHOT[];
    static const char KEY_BEAUTY_FACE[];
    static const char KEY_BEAUTY_FACE_LEVEL[];
    static const char KEY_LONG_SHOT[];
    static const char KEY_SMART_SHOT[];
    static const char KEY_FAIRLIGHT_MODE[];
    static const char KEY_QC_SINGLE_BOKEH[];
    static const char KEY_QC_SINGLE_BOKEH_LEVEL[];
    static const char KEY_SCENE_MODE[];
    static const char KEY_FLASH_MODE[];
    static const char KEY_VIDEO_PLIP[];
    static const char KEY_MOVIE_SOLID[];
    static const char KEY_LONG_EXPOSURE[];
    static const char KEY_BLUR_LEVEL[];
    static const char KEY_ROTATION[];

    static const char FLASH_MODE_OFF[];
    static const char FLASH_MODE_AUTO[];
    static const char FLASH_MODE_ON[];
    static const char FLASH_MODE_RED_EYE[];
    static const char FLASH_MODE_TORCH[];

    static const char SCENE_MODE_AUTO[];
    static const char SCENE_MODE_ACTION[];
    static const char SCENE_MODE_PORTRAIT[];
    static const char SCENE_MODE_LANDSCAPE[];
    static const char SCENE_MODE_NIGHT[];
    static const char SCENE_MODE_NIGHT_PORTRAIT[];
    static const char SCENE_MODE_THEATRE[];
    static const char SCENE_MODE_BEACH[];
    static const char SCENE_MODE_SNOW[];
    static const char SCENE_MODE_SUNSET[];
    static const char SCENE_MODE_STEADYPHOTO[];
    static const char SCENE_MODE_FIREWORKS[];
    static const char SCENE_MODE_SPORTS[];
    static const char SCENE_MODE_PARTY[];
    static const char SCENE_MODE_CANDLELIGHT[];
    static const char SCENE_MODE_BARCODE[];
    static const char SCENE_MODE_HDR[];
    static const char SCENE_MODE_ASD[];

    static const char FAIRLIGHT_VALUE_RAINBOW[];
    static const char FAIRLIGHT_VALUE_MORNING[];
    static const char FAIRLIGHT_VALUE_WAVE[];
    static const char FAIRLIGHT_VALUE_CONTOUR[];
    static const char FAIRLIGHT_VALUE_SHADOW[];
    static const char FAIRLIGHT_VALUE_STAGE[];

private:
    std::map<std::string, std::string> mMap;
};

};

#endif

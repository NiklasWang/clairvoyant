#ifndef _CAMERA_STATUS__
#define _CAMERA_STATUS__

#include <stdint.h>

#include "Modules.h"

namespace pandora {

class InfoBase {
protected:
    void init();

protected:
    ModuleType mModule;
};

#define DECLARE_DATA_CLASS_FUNC(T) \
    void init(); \
    bool operator!=(const T &rhs); \
    T operator+(const T &rhs); \
    T operator+=(const T &rhs); \
    T operator/(const int32_t div); \
    T operator/=(const int32_t div); \
    void dump();

class AEInfo :
    public InfoBase {
public:
    float exp_time;
    float real_gain;
    int32_t  iso_value;
    uint32_t flash_needed;
    int32_t  lux_index;
    uint32_t luma_target;

public:
    DECLARE_DATA_CLASS_FUNC(AEInfo);
};

class AWBInfo :
    public InfoBase {
public:
    int32_t color_temp;
    float rgain;
    float ggain;
    float bgain;

public:
    DECLARE_DATA_CLASS_FUNC(AWBInfo);
};

class AFInfo :
    public InfoBase {
public:
    enum AfStatus {
        AF_STATE_INACTIVE,
        AF_STATE_PASSIVE_SCAN,
        AF_STATE_PASSIVE_FOCUSED,
        AF_STATE_ACTIVE_SCAN,
        AF_STATE_FOCUSED_LOCKED,
        AF_STATE_NOT_FOCUSED_LOCKED,
        AF_STATE_PASSIVE_UNFOCUSED
    };
    AfStatus state;
    int32_t curr_pos;
    int32_t total_pos;
    int32_t start_pos;

public:
    DECLARE_DATA_CLASS_FUNC(AFInfo);
    bool isFocusDone();
};

class HdrInfo :
    public InfoBase {
public:
    uint32_t is_hdr_scene;
    float    hdr_confidence;

public:
    DECLARE_DATA_CLASS_FUNC(HdrInfo);
};

class SceneInfo :
    public InfoBase {
public:
    enum SceneType {
      SCENE_TYPE_NORMAL,
      SCENE_TYPE_SCENERY,
      SCENE_TYPE_PORTRAIT,
      SCENE_TYPE_PORTRAIT_BACKLIGHT,
      SCENE_TYPE_SCENERY_BACKLIGHT,
      SCENE_TYPE_BACKLIGHT,
      SCENE_TYPE_HDR,
      SCENE_TYPE_MAX_INVALID,
    };
    SceneType type;

public:
    DECLARE_DATA_CLASS_FUNC(SceneInfo);
};

};

#endif


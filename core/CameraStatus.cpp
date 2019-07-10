#include <math.h>

#include "Common.h"
#include "CameraStatus.h"

namespace pandora {

void InfoBase::init()
{
    mModule = MODULE_STATUS;
}

void AEInfo::init()
{
    exp_time  = 0.0f;
    real_gain = 0.0f;
    iso_value = 0;
    flash_needed = 0;
    lux_index    = 0;
    luma_target  = 0;
    InfoBase::init();
}

bool AEInfo::operator!=(const AEInfo &rhs)
{
    bool result = true;
    if ((fabsf(exp_time - rhs.exp_time) < EPSINON) &&
        (fabsf(real_gain - rhs.real_gain) < EPSINON) &&
        iso_value == rhs.iso_value &&
        flash_needed == rhs.flash_needed &&
        lux_index == rhs.lux_index &&
        luma_target == rhs.luma_target) {
        result = false;
    }
    return result;
}

AEInfo AEInfo::operator+(const AEInfo &rhs)
{
    AEInfo result;
    result.exp_time = exp_time + rhs.exp_time;
    result.real_gain = real_gain + rhs.real_gain;
    result.iso_value = iso_value + rhs.iso_value;
    result.flash_needed = flash_needed + rhs.flash_needed;
    result.lux_index = lux_index + rhs.lux_index;
    result.luma_target = luma_target + rhs.luma_target;

    return result;
}

AEInfo AEInfo::operator+=(const AEInfo &rhs)
{
    *this = *this + rhs;
    return *this;
}

AEInfo AEInfo::operator/(const int32_t div)
{
    assert(div != 0);

    AEInfo result;
    result.exp_time = exp_time / div;
    result.real_gain = real_gain / div;
    result.iso_value = iso_value / div;
    result.flash_needed = flash_needed > (uint32_t)(div / 2);
    result.lux_index = lux_index / div;
    result.luma_target = luma_target / div;

    return result;
}

AEInfo AEInfo::operator/=(const int32_t div)
{
    *this = *this / div;
    return *this;
}

void AEInfo::dump()
{
    LOGI(mModule, "ae info, exp %.2f gain %.2f iso %d " \
        "flash %d lux index %d luma target %d",
        exp_time, real_gain, iso_value,
        flash_needed, lux_index, luma_target);
}

void AWBInfo::init()
{
    color_temp = 0;
    rgain = 0;
    ggain = 0;
    bgain = 0;
    InfoBase::init();
}

bool AWBInfo::operator!=(const AWBInfo &rhs)
{
    bool result = true;
    if (color_temp == rhs.color_temp &&
        rgain == rhs.rgain &&
        ggain == rhs.ggain &&
        bgain == rhs.bgain) {
        result = false;
    }
    return result;
}

AWBInfo AWBInfo::operator+(const AWBInfo &rhs)
{
    AWBInfo result;
    result.color_temp = color_temp + rhs.color_temp;
    result.rgain = rgain + rhs.rgain;
    result.ggain = rgain + rhs.ggain;
    result.bgain = rgain + rhs.bgain;

    return result;
}

AWBInfo AWBInfo::operator+=(const AWBInfo &rhs)
{
    *this = *this + rhs;
    return *this;
}

AWBInfo AWBInfo::operator/(const int32_t div)
{
    assert(div != 0);

    AWBInfo result;
    result.color_temp = color_temp / div;
    result.rgain = rgain / div;
    result.ggain = ggain / div;
    result.bgain = bgain / div;

    return result;
}

AWBInfo AWBInfo::operator/=(const int32_t div)
{
    *this = *this / div;
    return *this;
}

void AWBInfo::dump()
{
    LOGI(mModule, "awb info, color temp %d rgain %.4f " \
        "ggain %.4f bgain %.4f", color_temp, rgain, ggain, bgain);
}

void AFInfo::init()
{
    state = AF_STATE_INACTIVE;
    curr_pos  = 0;
    total_pos = 1;
    start_pos = 1;
    InfoBase::init();
}

bool AFInfo::operator!=(const AFInfo &rhs)
{
    bool result = true;
    if (curr_pos == rhs.curr_pos &&
        total_pos == rhs.total_pos &&
        start_pos == rhs.start_pos &&
        state == rhs.state) {
        result = false;
    }
    return result;
}

AFInfo AFInfo::operator+(const AFInfo &rhs)
{
    AFInfo result;
    result.curr_pos = curr_pos + rhs.curr_pos;
    result.total_pos = total_pos + rhs.total_pos;
    result.start_pos = start_pos + rhs.start_pos;
    result.state = rhs.state;

    return result;
}

AFInfo AFInfo::operator+=(const AFInfo &rhs)
{
    *this = *this + rhs;
    return *this;
}

AFInfo AFInfo::operator/(const int32_t div)
{
    assert(div != 0);

    AFInfo result;
    result.curr_pos = curr_pos / div;
    result.total_pos = total_pos / div;
    result.start_pos = start_pos / div;
    result.state = state;

    return result;
}

AFInfo AFInfo::operator/=(const int32_t div)
{
    *this = *this / div;
    return *this;
}

bool AFInfo::isFocusDone()
{
    return state != AF_STATE_PASSIVE_SCAN &&
        state != AF_STATE_ACTIVE_SCAN;
}

void AFInfo::dump()
{
    LOGD(mModule, "af info, status %d focus position %d " \
        "total %d start %d", state, curr_pos, total_pos, start_pos);
}

void HdrInfo::init()
{
    is_hdr_scene   = 0;
    hdr_confidence = 0.0f;
    InfoBase::init();
}

bool HdrInfo::operator!=(const HdrInfo &rhs)
{
    bool result = true;
    if (is_hdr_scene == rhs.is_hdr_scene &&
        (fabsf(hdr_confidence - rhs.hdr_confidence) < EPSINON)) {
        result = false;
    }
    return result;
}

HdrInfo HdrInfo::operator+(const HdrInfo &rhs)
{
    HdrInfo result;
    result.is_hdr_scene = is_hdr_scene + rhs.is_hdr_scene;
    result.hdr_confidence = hdr_confidence + rhs.hdr_confidence;

    return result;
}

HdrInfo HdrInfo::operator+=(const HdrInfo &rhs)
{
    *this = *this + rhs;
    return *this;
}

HdrInfo HdrInfo::operator/(const int32_t div)
{
    assert(div != 0);

    HdrInfo result;
    result.is_hdr_scene = is_hdr_scene > (uint32_t)(div / 2);
    result.hdr_confidence = hdr_confidence / div;

    return result;
}

HdrInfo HdrInfo::operator/=(const int32_t div)
{
    *this = *this / div;
    return *this;
}

void HdrInfo::dump()
{
    LOGI(mModule, "hdr info, is hdr scene %d hdr confidence %.2f",
        is_hdr_scene, hdr_confidence);
}

void SceneInfo::init()
{
    type = SCENE_TYPE_NORMAL;
    InfoBase::init();
}

bool SceneInfo::operator!=(const SceneInfo &rhs)
{
    bool result = true;
    if (type == rhs.type) {
        result = false;
    }
    return result;
}

SceneInfo SceneInfo::operator+(const SceneInfo &rhs)
{
    SceneInfo result;
    result.type = rhs.type;

    return result;
}

SceneInfo SceneInfo::operator+=(const SceneInfo &rhs)
{
    *this = *this + rhs;
    return *this;
}

SceneInfo SceneInfo::operator/(const int32_t div)
{
    (void)div;
    assert(div != 0);

    SceneInfo result;
    result.type = type;

    return result;
}

SceneInfo SceneInfo::operator/=(const int32_t div)
{
    *this = *this / div;
    return *this;
}

void SceneInfo::dump()
{
    LOGI(mModule, "scene info, detected scene %d", type);
}

};


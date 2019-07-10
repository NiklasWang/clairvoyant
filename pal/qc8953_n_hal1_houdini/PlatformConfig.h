#ifndef _CONFIG_8996_N_HAL1_
#define _CONFIG_8996_N_HAL1_

#include "ConfigInterface.h"
#include "Platform.h"

namespace pandora {

class Config :
  public ConfigInterface {
public:
    bool isInited() override;
    int32_t getFocusEndThres(FocusEndConfig &conf) override;
    int32_t getExposureChangeThres(ExpChangeConfig &conf) override;
    int32_t getNightStabilizerThres(NightStabConfig &conf) override;
    int32_t getHdrConfidenceThres(ThresType &thres) override;
    int32_t getExtendedMsgID(IdType &id) override;
    int32_t getParmCategory(ParmsCategory &category) override;

private:
    static const ThresType gHdrConfidenceThres;
    static const IdType    gCameraExtMsgID;
    static const FocusEndConfig  gFocusEndThres;
    static const ExpChangeConfig gExpChangeThres;
    static const NightStabConfig gNightStabThres;
    static const ParmCategory    gParmCategory[];
};

};

#endif

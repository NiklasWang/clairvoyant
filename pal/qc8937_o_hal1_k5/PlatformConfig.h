#ifndef _CONFIG_8937_O_HAL1_
#define _CONFIG_8937_O_HAL1_

#include "ConfigInterface.h"
#include "Platform.h"

namespace pandora {

class Config :
  public ConfigInterface {
public:
    Config(uint32_t cameraid);
    bool isInited() override;
    int32_t getFocusEndThres(FocusEndConfig &conf) override;
    int32_t getExposureChangeThres(ExpChangeConfig &conf) override;
    int32_t getNightStabilizerThres(NightStabConfig &conf) override;
    int32_t getHdrConfidenceThres(ThresType &thres) override;
    int32_t getExtendedMsgID(IdType &id) override;
    int32_t getParmCategory(ParmsCategory &category) override;
    int32_t getLowLightThres(LowLightConfig &conf) override;

private:
    static const ConfigSettings gSettings[];
    static const ParmCategory   gParmCategory[];
};

};

#endif

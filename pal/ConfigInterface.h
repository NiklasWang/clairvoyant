#ifndef _CONFIG_INTERFACE_
#define _CONFIG_INTERFACE_

#include "Modules.h"
#include "PalParmType.h"
#include "RefBase.h"
#include "ConfigParameters.h"

namespace pandora {

class ConfigInterface :
    public RefBase {
public:
    typedef float   ThresType;
    typedef int32_t IdType;

public:
    virtual bool isInited() = 0;
    virtual int32_t getFocusEndThres(FocusEndConfig &conf) = 0;
    virtual int32_t getExposureChangeThres(ExpChangeConfig &conf) = 0;
    virtual int32_t getNightStabilizerThres(NightStabConfig &conf) = 0;
    virtual int32_t getHdrConfidenceThres(ThresType &thres) = 0;
    virtual int32_t getExtendedMsgID(IdType &id) = 0;
    virtual int32_t getLowLightThres(LowLightConfig &conf) = 0;
    virtual int32_t getParmCategory(ParmsCategory &category) = 0;

public:
    virtual ~ConfigInterface() = default;
    ConfigInterface(uint32_t camId) :
        mCamId(camId),
        mModule(MODULE_PLATFORM_OPS) {}

protected:
    struct ConfigSettings {
        FocusEndConfig  focusEnd;
        ExpChangeConfig expChange;
        NightStabConfig nightStab;
        ThresType       hdrConf;
        IdType          extendId;
        LowLightConfig  lowLight;
    };

protected:
    uint32_t   mCamId;
    ModuleType mModule;
};

};

#endif

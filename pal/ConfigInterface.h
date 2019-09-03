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
    virtual int32_t getParmCategory(ParmsCategory &category) = 0;

public:
    virtual ~ConfigInterface() = default;
    ConfigInterface() :
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
    ModuleType mModule;
};

};

#endif

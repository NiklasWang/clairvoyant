#ifndef _CONFIG_UBUNTU_
#define _CONFIG_UBUNTU_

#include "ConfigInterface.h"
#include "Platform.h"

namespace pandora {

class Config :
  public ConfigInterface {
public:

    Config();
    bool isInited() override;
    int32_t getParmCategory(ParmsCategory &category) override;

private:

    static const ParmCategory   gParmCategory[];
};

};

#endif

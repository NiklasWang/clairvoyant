#include <assert.h>

#include "Logo.h"

namespace pandora {

static uint8_t gLogo[] = {
    #include "resources/PandoraLog-1220x240.dat"
};

uint64_t getPandoraLogo(uint8_t **logo, uint64_t *size)
{
    assert(logo != NULL);
    assert(size != NULL);

    *logo = gLogo;
    *size = sizeof(gLogo);

    return *size;
}

};


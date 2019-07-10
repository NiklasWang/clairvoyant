#ifndef _PANDORA_WATER_MARK_BMP_H_
#define _PANDORA_WATER_MARK_BMP_H_

#include <stdint.h>

namespace pandora {

uint64_t getWaterMark(int32_t id, uint8_t **watermark, uint64_t *size);

uint64_t readWaterMark(int32_t id, uint8_t **watermark, uint64_t *size);

};

#endif

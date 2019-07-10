#ifndef FONT_TO_YUV_H_
#define FONT_TO_YUV_H_

#include <stdint.h>

#include "ft2build.h"
#include "freetype/freetype.h"

namespace pandora {

typedef struct {
    uint32_t w;
    uint32_t h;
    int32_t  pitch;
    int32_t  left;
    int32_t  top;
    uint8_t *buffer;
    uint32_t size;
    uint32_t unicode;
} font_bitmap_t;

int32_t initLib(FT_Library *lib);

int32_t deinitLib(FT_Library *lib);

int32_t getBitmap(FT_Library *lib, uint32_t unicode,
    int32_t size, const char *ttf, font_bitmap_t *bitmap);

int32_t printBitmaps(uint32_t num, font_bitmap_t *bitmap);

int32_t destroyBitmap(uint32_t num, font_bitmap_t *bitmap);

};

#endif

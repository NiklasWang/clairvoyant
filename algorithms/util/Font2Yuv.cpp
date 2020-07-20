#include <stdint.h>
#include <stdio.h>

#include "Common.h"
#include "MemMgmt.h"
#include "Font2Yuv.h"

#include <freetype/ftglyph.h>

#define MAX_LINE_LENGTH 150

#define module MODULE_UTILS

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
} fontfont_bitmap_t;

int32_t initLib(FT_Library *lib)
{
    return FT_Init_FreeType(lib);
}

int32_t deinitLib(FT_Library *lib)
{
    return FT_Done_FreeType(*lib);
}

int32_t copyBitmap(font_bitmap_t *info, uint8_t *buf, uint8_t *result)
{
    for (uint32_t i = 0; i < info->h; i++) {
        for (int32_t j = 0; j < info->pitch; j++) {
            *(result + i * info->pitch + j) = *(buf + i * info->pitch + j);
        }
    }

    return NO_ERROR;
}

int32_t convertBitmap(font_bitmap_t *info, uint8_t *buf, uint8_t *result)
{
    uint32_t w, h, wr, hr;
    int32_t pitch;

    pitch = info->pitch + info->left * 2;
    for (h = 0, hr = info->size - info->h;
        (h < info->h) && (hr < info->size);
        h++, hr++) {
        for (w = 0, wr = info->left;
            (w < (uint32_t)info->pitch) && (wr < info->size);
            w++, wr++) {
            if (*(buf + h * info->pitch + w)) {
                *(result + hr * pitch + wr) = 1;
            }
        }
    }

    info->h = info->size;
    info->pitch = pitch;

    return NO_ERROR;
}

int32_t getBitmap(FT_Library *lib, uint32_t unicode,
    int32_t size, const char *ttf, font_bitmap_t *bitmap)
{
    FT_Face face = NULL;
    FT_Glyph glyph = NULL;
    uint8_t *buffer = NULL;
    FT_BBox bbox;
    FT_Error rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = FT_New_Face(*lib, ttf, 0, &face);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to create face");
        }
    }

    if (SUCCEED(rc)) {
        rc = FT_Set_Pixel_Sizes(face, size, 0);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to set pixel size %d", size);
        }
    }

    if (SUCCEED(rc)) {
        rc = FT_Load_Glyph(face,
            FT_Get_Char_Index(face, unicode), FT_LOAD_DEFAULT);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to load glyph of unicode 0x%x", unicode);
        }
    }

    if (SUCCEED(rc)) {
        rc = FT_Get_Glyph(face->glyph, &glyph);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to get glyph, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_pixels, &bbox);
        rc = FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0 , 1);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to convert bitmap, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

        LOGD(module, "Font 0x%x size %d left %d top %d w %d h %d pitch %d "
            "face matrics ascender %ld descender %ld height %ld max advance %ld "
            "glyph metrics BearingX %ld BearingY %ld Advance %ld w %ld h %ld "
            "bbox xMin %ld xMax %ld yMin %ld yMax %ld",
            unicode, size,
            bitmap_glyph->left, bitmap_glyph->top,
            bitmap_glyph->bitmap.width, bitmap_glyph->bitmap.rows,
            bitmap_glyph->bitmap.pitch,
            face->size->metrics.ascender >> 6,
            face->size->metrics.descender >> 6,
            face->size->metrics.height >> 6,
            face->size->metrics.max_advance >> 6,
            face->glyph->metrics.horiBearingX >> 6,
            face->glyph->metrics.horiBearingY >> 6,
            face->glyph->metrics.horiAdvance >> 6,
            face->glyph->metrics.width >> 6,
            face->glyph->metrics.height >> 6,
            bbox.xMin, bbox.xMax, bbox.yMin, bbox.yMax);

        bitmap->w = bitmap_glyph->bitmap.width;
        bitmap->h = bitmap_glyph->bitmap.rows;
        bitmap->pitch = bitmap_glyph->bitmap.pitch;
        bitmap->left = bitmap_glyph->left;
        bitmap->top = bitmap_glyph->top;
        bitmap->size = size;
        bitmap->unicode = unicode;
        buffer = bitmap_glyph->bitmap.buffer;
        uint32_t bufSize = sizeof(uint8_t) * size * size;
        bitmap->buffer = (uint8_t *)Malloc(bufSize);
        memset(bitmap->buffer, 0, bufSize);
        if (ISNULL(bitmap->buffer)) {
            LOGE(module, "Failed to alloc %d bytes", bufSize);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = copyBitmap(bitmap, buffer, bitmap->buffer);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to copy bitmap, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (!ISNULL(glyph)) {
            FT_Done_Glyph(glyph);
            glyph = NULL;
        }

        if (!ISNULL(face)) {
            FT_Done_Face(face);
            face = NULL;
        }
    }

    return rc;
}

int32_t printBitmap(font_bitmap_t *bitmap)
{
    printf("    ");
    for (int32_t i = 0; i < bitmap->pitch; i++) {
        printf("%d", (i+ 1) % 10);
    }

    printf("");
    for (uint32_t i = 0; i < bitmap->h; i++) {
        printf("%3d ", i + 1);
        for (int32_t j = 0; j < bitmap->pitch; j++) {
            if (*(bitmap->buffer + i * bitmap->pitch + j)) {
                printf("*");
            } else {
                printf(" ");
            }
        }
        printf("");
    }

    return NO_ERROR;
}

int32_t printBitmapTable(uint32_t num, font_bitmap_t *bitmap)
{
    uint32_t max = 0;

    for (uint32_t i = 0; i < num; i++) {
        max = max < bitmap[i].h ? bitmap[i].h : max;
    }

    printf("");
    for (uint32_t n = 0; n < num; n++) {
        printf("    ");
        for (int32_t w = 0; w < bitmap[n].pitch; w++) {
            printf("%d", (w + 1) % 10);
        }
    }

    printf("");
    for (uint32_t h = 0; h < max; h++) {
        for (uint32_t n = 0; n < num; n++) {
            printf("%3d ", h + 1);
            for (int32_t w = 0; w < bitmap[n].pitch; w++) {
                if (h < bitmap[n].h &&
                    *(bitmap[n].buffer + h * bitmap[n].pitch + w)) {
                    printf("*");
                } else {
                    printf(" ");
                }
            }
        }
        printf("");
    }

    return NO_ERROR;
}

int32_t printBitmaps(uint32_t num, font_bitmap_t *bitmap)
{
    uint32_t i = 0;
    int32_t width = 0;
    uint32_t count = 0, printed = 0;

    for (i = 0; i < num; i++) {
        if ((width + bitmap[i].pitch) > MAX_LINE_LENGTH) {
            printBitmapTable(count, bitmap + printed);
            printed += count;
            count = 0;
            width = 0;
        }
        count ++;
        width += bitmap[i].pitch;
    }

    if (width > 0) {
        printBitmapTable(count, bitmap + printed);
    }

    return NO_ERROR;
}

int32_t destroyBitmap(uint32_t num, font_bitmap_t *bitmap)
{
    for (uint32_t i = 0; i < num; i++) {
        SECURE_FREE(bitmap[i].buffer);
    }

    return NO_ERROR;
}

};


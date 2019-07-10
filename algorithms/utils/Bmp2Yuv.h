#ifndef _BMP_TO_YUV_H_
#define _BMP_TO_YUV_H_

#include <stdint.h>

namespace pandora {

int32_t getBmpWidthHeight(uint8_t *bmp, uint32_t *w, uint32_t *h);

int32_t Bmp2Yuv(uint8_t *bmp, uint8_t *yuv);

int32_t Bmp2Yuv(char *bmp_path, char *yuv_path);

int32_t readFile(char *path, uint8_t **buf, uint32_t *size);

size_t writeFile(char *path, uint8_t *buf, uint32_t size);

};

#endif

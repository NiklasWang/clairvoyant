#ifndef ALGORITHM_HELPER_H_
#define ALGORITHM_HELPER_H_

namespace pandora {

#define SINGLE_FRAME_ALGORITHM_INPUT 1

size_t PadToNum(int32_t base, int32_t num);

size_t getAlignedStride(int32_t num, int32_t stride);

size_t getFrameLen(int32_t w, int32_t h, int32_t stride = 1);

size_t getFrameLen(int32_t w, int32_t h, int32_t stride, int32_t scanline);

size_t getFrameLen2(int32_t w, int32_t h, int32_t stride = 1);

size_t getFrameLen2(int32_t w, int32_t h, int32_t stride, int32_t scanline);

size_t getMaximumFrameLen(int32_t w, int32_t h, int32_t stride, int32_t scanline);

size_t getMinimumBufSize(int32_t w, int32_t h);

size_t getMinimumBufSize(int32_t w, int32_t h, int32_t scanline);

bool haveStride(int32_t w, int32_t h, size_t frame_len);

bool haveStride(int32_t w, int32_t h, size_t frame_len, int32_t scanline);

int32_t getStride(int32_t w, int32_t h, size_t frame_len);

int32_t getStride(int32_t w, int32_t h, size_t frame_len, int32_t scanline);

int32_t NV21RemoveStride(uint8_t* dst, uint8_t* src,
    int32_t w, int32_t h, int32_t stride);

int32_t NV21RemoveStride(uint8_t* dst, uint8_t* src,
    int32_t w, int32_t h, int32_t stride, int32_t scanline);

int32_t NV21AddStride(uint8_t* dst,
    uint8_t* src, int32_t w, int32_t h, int32_t stride);

int32_t NV21AddStride(uint8_t* dst, uint8_t* src,
    int32_t w, int32_t h, int32_t stride, int32_t scanline);

uint8_t *getNV21YData(uint8_t* frame,
    int32_t w, int32_t h, int32_t stride = 1);

uint8_t *getNV21YData(uint8_t* frame,
    int32_t w, int32_t h, int32_t stride, int32_t scanline);

uint8_t *getNV21UVData(uint8_t* frame,
    int32_t w, int32_t h, int32_t stride = 1);

uint8_t *getNV21UVData(uint8_t* frame,
    int32_t w, int32_t h, int32_t stride, int32_t scanline);

uint8_t *getI420YData(uint8_t *frame,
    int32_t w, int32_t h,
    int32_t ystride, int32_t ustride, int32_t vstride);

uint8_t *getI420UData(uint8_t *frame,
    int32_t w, int32_t h,
    int32_t ystride, int32_t ustride, int32_t vstride);

uint8_t *getI420VData(uint8_t *frame,
    int32_t w, int32_t h,
    int32_t ystride, int32_t ustride, int32_t vstride);

int32_t yuv420spToYuv420p(uint8_t *yuv420sp,
    uint8_t *yuv420p, int width, int height);

int32_t dumpNV21ToFile(uint8_t* src, const char *prefix,
    int32_t w, int32_t h, int32_t stride = 1);

int32_t dumpNV21ToFile(uint8_t* src, const char *prefix,
    int32_t w, int32_t h, int32_t stride, int32_t scanline);

int32_t dumpNV21ToFile(void* src, const char *prefix,
    int32_t w, int32_t h, int32_t stride, int32_t scanline);

int32_t dumpJpegToFile(const void *src, const char *prefix,
    int32_t w, int32_t h, size_t size);

int32_t dumpNV21ToJpeg (void *src, const char *prefix,
    int32_t w, int32_t h, int32_t stride, int32_t scanline);

int64_t getNowUs();

int32_t convertNV12ToNV21(uint8_t* dat,
    int32_t w, int32_t h, int32_t stride, int32_t scanline);

int32_t convertNV21ToNV12(uint8_t* dat,
    int32_t w, int32_t h, int32_t stride, int32_t scanline);

int32_t writeFileIfNonExist(const char *src,
    const char *dst);

};

#endif

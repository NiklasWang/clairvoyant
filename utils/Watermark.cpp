#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "Watermark.h"
#include "Logs.h"
#include "Modules.h"
#include "MemMgmt.h"
#include "c.h"

#define WATER_MARK_DAT_TYPE_1_PATH "resources/Z5-AI-1220x240.dat"

#define WATER_MARK_BMP_TYPE_1_PATH "/data/misc/camera/watermark_type_1_1100x216_24bit.bmp"

namespace pandora {

static uint8_t gWaterMark1[] = {
    #include WATER_MARK_DAT_TYPE_1_PATH
};

uint64_t getWaterMark(int32_t id, uint8_t **watermark, uint64_t *size)
{
    assert(watermark != NULL);
    assert(size != NULL);

    if (id == 1) {
        *watermark = gWaterMark1;
        *size = sizeof(gWaterMark1);
    }

    return *size;
}

uint64_t readWaterMark(int32_t id, uint8_t **watermark, uint64_t *size)
{
    int32_t rc = NO_ERROR;
    struct stat info;
    int fd = -1;
    uint64_t readSize = 0;
    uint8_t *buf = NULL;
    char fileName[PATH_MAX];

    if (id == 1) {
        snprintf(fileName, sizeof(fileName), "%s",
            WATER_MARK_BMP_TYPE_1_PATH);
    } else {
        rc = NOT_SUPPORTED;
    }

    if (SUCCEED(rc)) {
        if (stat(fileName, &info) != 0) {
            LOGE(MODULE_UTILS, "stat file %s error, %s",
                fileName, strerror(errno));
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        *size = info.st_size;
        fd = open(fileName, O_RDONLY);
        if (fd < 0) {
            LOGE(MODULE_UTILS, "Open watermark failed, %s",
                strerror(errno));
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        buf = (uint8_t *)Malloc(*size);
        if (ISNULL(buf)) {
            LOGE(MODULE_UTILS, "Failed to alloc %d bytes for "
                "watermark", *size);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        readSize = read(fd, buf, *size);
        if (readSize != *size) {
            LOGE(MODULE_UTILS, "Failed to read watermark, "
                "%lld/%lld bytes, %s", readSize, *size, strerror(errno));
            Free(buf);
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        *watermark = buf;
    }

    return *size;
}

};


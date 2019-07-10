#include "Common.h"
#include "AlgorithmHelper.h"
#include "FontWaterMark.h"
#include "BufHolder.h"
#include "Logs.h"

#include "Algorithm.h"

#define NEW_INTERFACE_TEST_TIMES 10
#define WRITE_DIRTY_DATA         "WRITE DIRTY DATA"
#define TEST_FONT_SIZE           100

namespace tester {

using namespace pandora;

int32_t testAlgorithmNewInterface(TaskType &task)
{
    int32_t rc = NO_ERROR;
    ModuleType module = MODULE_TESTER;
    BufHolder<Global> bufs;
    IAlgorithm *alg = NULL;
    FontWaterMark *wmAlg = NULL;
    std::list<AlgTraits<FontWaterMark>::ParmType::TextType> *texts = NULL;

    alg = Algorithm<FontWaterMark>::create(
        "FontWaterMark", "FontWaterMarkThread", false, 1, 2,
        FONT_WATER_MARK_POSITION_MIDDLE,
        ROTATION_ANGLE_90,
        "Powered by Pandora.",          TEST_FONT_SIZE,
        "Test Algorithm New Interface", TEST_FONT_SIZE,
        "STOP");
    if (ISNULL(alg)) {
        LOGE(module, "Failed to create font water mark algorithm");
        rc = INTERNAL_ERROR;
    }

    /* Bufs[0] - Origin frame buffer
     * Bufs[1] - Added watermark 1 in sync mode, process(TaskType &) intf
     * Bufs[2] - Added watermark 1 in async mode, process(TaskType &) intf
     * Bufs[3] - Added watermark 1 in sync mode, process(TaskType &, TaskType &) intf
     * Bufs[4] - Added watermark 1 in async mode, process(TaskType &, TaskType &) intf
     * Bufs[5] - Added watermark 2 in sync mode, process(TaskType &, TaskType &) intf
     * Bufs[6] - Added watermark 2 in async mode, process(TaskType &, TaskType &) intf
     */

    /* Run test 1, process(TaskType &) interface */
    if (SUCCEED(rc)) {
        dumpNV21ToFile(task.data, "alg_new_intf_origin",
            task.w, task.h, task.stride, task.scanline);

        rc = bufs.replace(0, task);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to add buffer, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = bufs.replace(1, task) || bufs.replace(2, task);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to add buffer, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        TaskType _task = task;
        _task.data = bufs[1];
        rc = alg->process(_task);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to process task, %d", rc);
        } else {
            dumpNV21ToFile(task.data,
                "alg_new_intf_process_sync_with_watermark1_1",
                task.w, task.h, task.stride, task.scanline);
        }
    }

    if (SUCCEED(rc)) {
        TaskType _task = task;
        _task.data = bufs[2];
        Semaphore sem;
        rc = alg->processAsync(_task, 0,
            [&sem](TaskType & /*result*/) -> int32_t {
                LOGD(MODULE_TESTER, "Test case 1 async task finished");
                sem.signal();
                return NO_ERROR;
            });
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to process task, %d", rc);
        } else {
            sem.wait();
            dumpNV21ToFile(task.data,
                "alg_new_intf_process_async_with_watermark1_2",
                task.w, task.h, task.stride, task.scanline);
        }
    }

    if (SUCCEED(rc)) {
        if (!memcmp(bufs[0], bufs[1], task.size)) {
            LOGE(module, "Bufs 0, bufs 1 frame should not be same");
            rc = INTERNAL_ERROR;
        }
        if (memcmp(bufs[1], bufs[2], task.size)) {
            LOGE(module, "Bufs 1, bufs 2 frame should be same");
            rc = INTERNAL_ERROR;
        }
        if (SUCCEED(rc)) {
            LOGD(module, "Test case 1, -> process(TaskType &) succeed.");
        }
    }

    /* Run test 2, process(TaskType &, TaskType &) interface */
    if (SUCCEED(rc)) {
        rc = bufs.replace(3, task) || bufs.replace(4, task);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to add buffer, %d", rc);
        } else {
            strcpy((char *)bufs[3], WRITE_DIRTY_DATA);
            strcpy((char *)bufs[4], WRITE_DIRTY_DATA);
        }
    }

    if (SUCCEED(rc)) {
        if (!memcmp(bufs[0], bufs[3], task.size) ||
            strncmp((char *)bufs[3], WRITE_DIRTY_DATA, strlen(WRITE_DIRTY_DATA))) {
            LOGE(module, "Bufs 0, Bufs 3 frame should not same");
            rc = INTERNAL_ERROR;
        }
        if (!memcmp(bufs[0], bufs[4], task.size) ||
            strncmp((char *)bufs[4], WRITE_DIRTY_DATA, strlen(WRITE_DIRTY_DATA))) {
            LOGE(module, "Bufs 0, Bufs 4 frame should not same");
            rc = INTERNAL_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        TaskType _task = task;
        _task.data = bufs[0];
        TaskType output = task;
        output.data = bufs[3];
        rc = alg->process(_task, output);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to process task, %d", rc);
        } else {
            dumpNV21ToFile(output.data,
                "alg_new_intf_processEx_sync_with_watermark1_3",
                task.w, task.h, task.stride, task.scanline);
        }
    }

    if (SUCCEED(rc)) {
        TaskType _task = task;
        _task.data = bufs[0];
        TaskType output = task;
        output.data = bufs[4];
        Semaphore sem;
        rc = alg->processAsync(_task, output, 0,
            [&sem](TaskType & /*result*/) -> int32_t {
                LOGD(MODULE_TESTER, "Test case 2 async task finished");
                sem.signal();
                return NO_ERROR;
            });
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to process task, %d", rc);
        } else {
            sem.wait();
            dumpNV21ToFile(output.data,
                "alg_new_intf_processEx_async_with_watermark1_4",
                task.w, task.h, task.stride, task.scanline);
        }
    }

    if (SUCCEED(rc)) {
        if (memcmp(task.data, bufs[0], task.size)) {
            LOGE(module, "Bufs 0, original frame should be same");
            rc = INTERNAL_ERROR;
        }
        if (!memcmp(bufs[0], bufs[3], task.size)) {
            LOGE(module, "Bufs 0, Bufs 3 frame should not be same");
            rc = INTERNAL_ERROR;
        }
        if (memcmp(bufs[1], bufs[3], task.size)) {
            LOGE(module, "Bufs 1, Bufs 3 frame should be same");
            rc = INTERNAL_ERROR;
        }
        if (!memcmp(bufs[0], bufs[4], task.size)) {
            LOGE(module, "Bufs 0, Bufs 4 frame should not be same");
            rc = INTERNAL_ERROR;
        }
        if (memcmp(bufs[1], bufs[4], task.size)) {
            LOGE(module, "Bufs 1, Bufs 4 frame should be same");
            rc = INTERNAL_ERROR;
        }
        if (memcmp(bufs[3], bufs[4], task.size)) {
            LOGE(module, "Bufs 3, Bufs 4 frame should be same");
            rc = INTERNAL_ERROR;
        }
        if (SUCCEED(rc)) {
            LOGD(module, "Test case 2, -> "
                "process(TaskType &, TaskType &) succeed.");
        }
    }

    /* Run test 3, config(ParmType &) interface */
    if (SUCCEED(rc)) {
        AlgTraits<FontWaterMark>::ParmType::TextType text;
        texts = new std::list<AlgTraits<FontWaterMark>::ParmType::TextType>();
        if (ISNULL(texts)) {
            LOGE(module, "Failed to new list");
            rc = NO_MEMORY;
        } else {
            text = { std::string("Powered by Pandora."), TEST_FONT_SIZE };
            texts->push_back(text);
            text = { std::string("Test Algorithm New Interface, configured"), TEST_FONT_SIZE };
            texts->push_back(text);
        }
    }

    if (SUCCEED(rc)) {
        AlgTraits<FontWaterMark>::ParmType parm;
        parm.rotation   = ROTATION_ANGLE_90;
        parm.position   = FONT_WATER_MARK_POSITION_TOP_LEFT;
        parm.needMirror = false;
        parm.texts      = texts;
        rc = alg->config(parm);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to config water mark, %d", rc);
        } else {
            LOGD(module, "Test case 3, -> config(ParmType &) succeed.");
        }
    }

    /* Run test 4, process(TaskType &, TaskType &) interface */
    if (SUCCEED(rc)) {
        rc = bufs.replace(5, task) || bufs.replace(6, task);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to add buffer, %d", rc);
        } else {
            strcpy((char *)bufs[5], WRITE_DIRTY_DATA);
            strcpy((char *)bufs[6], WRITE_DIRTY_DATA);
        }
    }

    if (SUCCEED(rc)) {
        if (!memcmp(bufs[0], bufs[5], task.size) ||
            strncmp((char *)bufs[5], WRITE_DIRTY_DATA, strlen(WRITE_DIRTY_DATA))) {
            LOGE(module, "Bufs 0, Bufs 5 frame should not same");
            rc = INTERNAL_ERROR;
        }
        if (!memcmp(bufs[0], bufs[6], task.size) ||
            strncmp((char *)bufs[6], WRITE_DIRTY_DATA, strlen(WRITE_DIRTY_DATA))) {
            LOGE(module, "Bufs 0, Bufs 6 frame should not same");
            rc = INTERNAL_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        TaskType _task = task;
        _task.data = bufs[0];
        TaskType output = task;
        output.data = bufs[5];
        rc = alg->process(_task, output);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to process task, %d", rc);
        } else {
            dumpNV21ToFile(output.data,
                "alg_new_intf_processEx_sync_with_watermark2_1",
                task.w, task.h, task.stride, task.scanline);
        }
    }

    if (SUCCEED(rc)) {
        TaskType _task = task;
        _task.data = bufs[0];
        TaskType output = task;
        output.data = bufs[6];
        Semaphore sem;
        rc = alg->processAsync(_task, output, 0,
            [&sem](TaskType & /*result*/)  -> int32_t {
                LOGD(MODULE_TESTER, "Test case 4 async task finished");
                sem.signal();
                return NO_ERROR;
            });
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to process task, %d", rc);
        } else {
            sem.wait();
            dumpNV21ToFile(output.data,
                "alg_new_intf_processEx_async_with_watermark2_2",
                task.w, task.h, task.stride, task.scanline);
        }
    }

    if (SUCCEED(rc)) {
        if (memcmp(task.data, bufs[0], task.size)) {
            LOGE(module, "Bufs 0, original frame should be same");
            rc = INTERNAL_ERROR;
        }
        if (!memcmp(bufs[0], bufs[5], task.size)) {
            LOGE(module, "Bufs 0, Bufs 5 frame should not be same");
            rc = INTERNAL_ERROR;
        }
        if (!memcmp(bufs[1], bufs[5], task.size)) {
            LOGE(module, "Bufs 1, Bufs 5 frame should not be same");
            rc = INTERNAL_ERROR;
        }
        if (!memcmp(bufs[0], bufs[6], task.size)) {
            LOGE(module, "Bufs 0, Bufs 6 frame should not be same");
            rc = INTERNAL_ERROR;
        }
        if (!memcmp(bufs[1], bufs[6], task.size)) {
            LOGE(module, "Bufs 1, Bufs 6 frame should not be same");
            rc = INTERNAL_ERROR;
        }
        if (memcmp(bufs[5], bufs[6], task.size)) {
            LOGE(module, "Bufs 5, Bufs 6 frame should be same");
            rc = INTERNAL_ERROR;
        }
        if (SUCCEED(rc)) {
            LOGD(module, "Test case 4, -> process("
                "TaskType &, TaskType &) succeed.");
        }
    }

    /* Run test 5, update(UpdateType &) interface */
    if (SUCCEED(rc)) {
        AlgTraits<FontWaterMark>::UpdateType info;
        info.taskid = 0;
        info.type = ALG_BEAUTY_FACE;
        info.dir  = ALG_BROADCAST_DIR_BOTH;

        rc = alg->update(info);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to update water mark, %d", rc);
        } else {
            LOGD(module, "Test case 5, -> "
                "update(UpdateType &) succeed.");
        }
    }

    /* Run test 6, queryStatus(StatusType &) interface */
    if (SUCCEED(rc)) {
        AlgTraits<FontWaterMark>::StatusType status;
        rc = alg->queryStatus(status);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to update water mark, %d", rc);
        } else {
            if (status.rc != NOT_SUPPORTED) {
                LOGE(module, "Query status error, expected %d, "
                    "actual %d", NOT_SUPPORTED, status.rc);
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        AlgTraits<FontWaterMark>::StatusType status;
        rc = alg->queryStatus(status);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to update water mark, %d", rc);
        } else {
            if (status.rc != NO_ERROR) {
                LOGE(module, "Query status error, expected %d, "
                    "actual %d", NO_ERROR, status.rc);
                rc = INTERNAL_ERROR;
            } else {
                LOGD(module, "Test case 6, -> "
                    "queryStatus(StatusType &) succeed.");
            }
        }
    }

    /* Run test 7, getAlgorithmInDanger() interface */
    if (SUCCEED(rc)) {
        IAlgorithmT<FontWaterMark> *tmp =
            static_cast<IAlgorithmT<FontWaterMark> *>(alg);
        wmAlg = tmp->getAlgorithmInDanger();
        if (ISNULL(wmAlg)) {
            LOGE(module, "Failed to get algorithm directly");
            rc = INTERNAL_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = wmAlg->testNewInterface(NO_ERROR);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to run private interface, "
                "expected %d, actual %d", NO_ERROR, rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = wmAlg->testNewInterface(NOT_SUPPORTED);
        if (rc != NOT_SUPPORTED) {
            LOGE(module, "Failed to run private interface, "
                "expected %d, actual %d", NOT_SUPPORTED, rc);
        } else {
            RESETRESULT(rc);
            LOGD(module, "Test case 7, -> "
                "getAlgorithmInDanger() succeed.");
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (NOTNULL(alg)) {
            SECURE_DELETE(alg);
        }
    }

    if (SUCCEED(rc)) {
        LOGI(module, "Test algorithm new interface, SUCCEED.");
    } else {
        LOGE(module, "Test algorithm new interface, FAILED.");
    }

    return rc;
}

}; // end of namespace tester

namespace pandora {

int32_t runAlgNewIntfTester(TaskType &task)
{
    int32_t rc = NO_ERROR;
    static int32_t i = 0;

    if (i++ < NEW_INTERFACE_TEST_TIMES) {
        rc = tester::testAlgorithmNewInterface(task);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Algorithm new interface test Failed.");
        }
    }

    return rc;
}

}; // end of namespace pandora

#define __GOTO_END_OF_THIS_FILE_


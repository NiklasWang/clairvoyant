LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES       :=
LOCAL_CFLAGS           :=
LOCAL_SRC_FILES        := MatrixRotateTest.c
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE           := matrix_rotate

include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)

LOCAL_C_INCLUDES        := external/freetype/include/freetype
LOCAL_CFLAGS            := -std=c99 -DBUILD_ANDROID_AP
LOCAL_SRC_FILES         := FontBitmapTest.c
LOCAL_SHARED_LIBRARIES  := libft2
LOCAL_MODULE            := font_bitmap

include $(BUILD_EXECUTABLE)

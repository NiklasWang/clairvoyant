LOCAL_PATH   := $(call my-dir)
PANDORA_PATH := $(LOCAL_PATH)/..
VENDOR_PATH  := $(LOCAL_PATH)

include $(VENDOR_PATH)/config.mk

ifneq "$(findstring libtsvideoprocess, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libtsvideoprocess
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/thundersoft/libtsvideoprocess.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE        := libmpbase
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MULTILIB      := 32
LOCAL_STRIP_MODULE  := false
LOCAL_MODULE_OWNER  := qcom
LOCAL_MODULE_TAGS   := optional
LOCAL_SRC_FILES     := lib/arcsoft/libmpbase.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
include $(BUILD_PREBUILT)

ifneq "$(findstring libarcsoft_beautyshot, $(LIBRARY-y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libarcsoft_beautyshot
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_SHARED_LIBRARIES := libmpbase
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/arcsoft/libarcsoft_beautyshot.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libarcsoft_agegender_estimation, $(LIBRARY-y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libarcsoft_agegender_estimation
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_SHARED_LIBRARIES := libmpbase
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/arcsoft/libarcsoft_agegender_estimation.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif


ifneq "$(findstring libarcsoft_night_shot, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libarcsoft_night_shot
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/arcsoft/libarcsoft_night_shot.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libarcsoft_antishaking, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libarcsoft_antishaking
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/arcsoft/libarcsoft_antishaking.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libarcsoft_piczoom, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libarcsoft_piczoom
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/arcsoft/libarcsoft_piczoom.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libmegface_portrait, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libmegface_portrait
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/face++/libmegface_portrait.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libMegviiHum, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libMegviiHum
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/face++/libMegviiHum.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libMegviiAIReshape, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libMegviiAIReshape
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/face++/libMegviiAIReshape.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libmorpho_image_stabilizer4, $(LIBRARY-y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libmorpho_image_stabilizer4
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/morpho/libmorpho_image_stabilizer4.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libmorpho_image_stab5, $(LIBRARY-y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libmorpho_image_stab5
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/morpho/libmorpho_image_stab5.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libmorpho_super_resolution, $(LIBRARY-y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libmorpho_super_resolution
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/morpho/libmorpho_super_resolution.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libalSDE2, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libalSDE2
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/altek/libalSDE2.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libalRnB, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libalRnB
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/altek/libalRnB.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libarcsoft_dualcam_refocus, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libarcsoft_dualcam_refocus
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/arcsoft/libarcsoft_dualcam_refocus.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libarcsoft_dualcam_low_light_shot, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libarcsoft_dualcam_low_light_shot
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/arcsoft/libarcsoft_dualcam_low_light_shot.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libmorpho_dr_checker2, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libmorpho_dr_checker2
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/morpho/libmorpho_dr_checker2.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libmorpho_easy_hdr, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libmorpho_easy_hdr
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/morpho/libmorpho_easy_hdr.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libmorpho_image_refiner, $(LIBRARY-Y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libmorpho_image_refiner
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/morpho/libmorpho_image_refiner.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libmorpho_defocusface, $(STATICLIB-y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libmorpho_defocusface
  LOCAL_MODULE_CLASS  := STATIC_LIBRARIES
  LOCAL_MODULE_SUFFIX := .a
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/morpho/libmorpho_defocusface.a
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring mpbase, $(STATICLIB-y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := mpbase
  LOCAL_MODULE_CLASS  := STATIC_LIBRARIES
  LOCAL_MODULE_SUFFIX := .a
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := lib/arcsoft/mpbase.a
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

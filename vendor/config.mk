# Compile configuration for vendor libraries

# Switches
PHOTO_SOLID_4 := n
PHOTO_SOLID_5 := y
BEAUTY_FACE   := y
GENDER_DETECT := y
SMART_SELECT  := n
SMART_SHOT    := n
ANTI_SHAKE    := n
NIGHT_SHOT    := n
MOVIE_SOLID   := n
PICTURE_ZOOM  := n
FAIR_LIGHT    := y
PICTURE_ZOOM2 := y
MICRO_PLASTIC := y
STILL_BOKEH   := n
STILL_BOKEHA  := n
RT_BOKEH      := n
DUAL_NIGHT    := n
HDRT          := n
HDRM          := n
HDRM4         := y
SUPER_RES     := n
SINGLE_BOKEH  := n

# Vars
SRC_FILE_PATH          := algorithms
VENDOR_INCLUDES_PATH   := vendor/include
VENDOR_SHARED_LIB_PATH := vendor/lib
VENDOR_STATIC_LIB_PATH := vendor/lib

# Files to compile for each algorithms
COMPILE-$(PHOTO_SOLID_4) := PhotoSolid4 PhotoSolid4Configuration
COMPILE-$(PHOTO_SOLID_5) += PhotoSolid5 PhotoSolid5Configuration
COMPILE-$(BEAUTY_FACE)   += BeautyFace BeautyFaceArcsoft BeautyFaceArcsoftConfiguration
COMPILE-$(GENDER_DETECT) += AgeGenderDetection
COMPILE-$(HDRT)          += HDRCheckerT HDRT
COMPILE-$(SMART_SELECT)  += SmartSelect
COMPILE-$(SMART_SHOT)    += SmartShot
COMPILE-$(ANTI_SHAKE)    += AntiShaking
COMPILE-$(NIGHT_SHOT)    += NightShot
COMPILE-$(MOVIE_SOLID)   += MovieSolid
COMPILE-$(PICTURE_ZOOM)  += PictureZoom
COMPILE-$(FAIR_LIGHT)    += FairLight
COMPILE-$(MICRO_PLASTIC) += MicroPlastic
COMPILE-$(PICTURE_ZOOM2) += PictureZoom2 PictureZoom2Configuration
COMPILE-$(STILL_BOKEH)   += StillBokeh
COMPILE-$(STILL_BOKEHA)  += StillBokehA
COMPILE-$(RT_BOKEH)      += RTBokeh
COMPILE-$(DUAL_NIGHT)    += DualCamNightShot
COMPILE-$(HDRM)          += HDRCheckerM HDRM HDRCheckerMConfiguration HDRMConfiguration
COMPILE-$(HDRM4)         += HDRCheckerM HDRM4 HDRCheckerMConfiguration
COMPILE-$(SUPER_RES)     += SuperResolution SuperResolutionConfiguration
COMPILE-$(SINGLE_BOKEH)  += SingleBokeh

# C flags for each vendor libraries
CFLAGS-$(PHOTO_SOLID_4)  := -DENABLE_IMAGE_STABILICATION -DUSE_PHOTO_SOLID_V4
CFLAGS-$(PHOTO_SOLID_5)  += -DENABLE_IMAGE_STABILICATION -DUSE_PHOTO_SOLID_V5
CFLAGS-$(BEAUTY_FACE)    += -DENABLE_BEAUTY_FACE
CFLAGS-$(GENDER_DETECT)  += -DENABLE_GENDER_DETECTION
CFLAGS-$(HDRT)           += -DENABLE_HDRT
CFLAGS-$(SMART_SELECT)   += -DENABLE_SMART_SELECT
CFLAGS-$(SMART_SHOT)     += -DENABLE_SMART_SHOT
CFLAGS-$(ANTI_SHAKE)     += -DENABLE_ANTI_SHAKE
CFLAGS-$(NIGHT_SHOT)     += -DENABLE_NIGHT_SHOT
CFLAGS-$(MOVIE_SOLID)    += -DENABLE_VIDEO_STABILIZATION
CFLAGS-$(PICTURE_ZOOM)   += -DENABLE_PICTURE_ZOOM
CFLAGS-$(FAIR_LIGHT)     += -DENABLE_FAIR_LIGHT
CFLAGS-$(MICRO_PLASTIC)  += -DENABLE_MICRO_PLASTIC
CFLAGS-$(PICTURE_ZOOM2)  += -DENABLE_PICTURE_ZOOM2
CFLAGS-$(STILL_BOKEH)    += -DENABLE_STILL_BOKEH
CFLAGS-$(STILL_BOKEHA)   += -DENABLE_STILL_BOKEHA
CFLAGS-$(RT_BOKEH)       += -DENABLE_RT_BOKEH
CFLAGS-$(DUAL_NIGHT)     += -DENABLE_DUAL_NIGHT
CFLAGS-$(HDRM)           += -DENABLE_HDRM
CFLAGS-$(HDRM4)          += -DENABLE_HDRM4
CFLAGS-$(SUPER_RES)      += -DENABLE_SUPER_RESOLUTION
CFLAGS-$(SINGLE_BOKEH)   += -DENABLE_SINGLE_BOKEH

# C includes for each vendor libraries
CINCLUDES-$(PHOTO_SOLID_4) := morpho
CINCLUDES-$(PHOTO_SOLID_5) += morpho
CINCLUDES-$(BEAUTY_FACE)   += arcsoft
CINCLUDES-$(GENDER_DETECT) += arcsoft
CINCLUDES-$(HDRT)          += thundersoft
CINCLUDES-$(SMART_SELECT)  += thundersoft
CINCLUDES-$(SMART_SHOT)    += thundersoft
CINCLUDES-$(ANTI_SHAKE)    += arcsoft
CINCLUDES-$(NIGHT_SHOT)    += arcsoft
CINCLUDES-$(MOVIE_SOLID)   += thundersoft
CINCLUDES-$(PICTURE_ZOOM)  += arcsoft
CINCLUDES-$(FAIR_LIGHT)    += face++
CINCLUDES-$(MICRO_PLASTIC) += face++
CINCLUDES-$(PICTURE_ZOOM2) += arcsoft
CINCLUDES-$(STILL_BOKEH)   += altek
CINCLUDES-$(STILL_BOKEHA)  += arcsoft
CINCLUDES-$(RT_BOKEH)      += arcsoft
CINCLUDES-$(DUAL_NIGHT)    += arcsoft
CINCLUDES-$(HDRM)          += morpho
CINCLUDES-$(HDRM4)         += morpho
CINCLUDES-$(SUPER_RES)     += morpho
CINCLUDES-$(SINGLE_BOKEH)  += morpho

# Vendor shared library to link with
LIBRARY-$(PHOTO_SOLID_4) := libmorpho_image_stabilizer4
LIBRARY-$(PHOTO_SOLID_5) += libmorpho_image_stab5
LIBRARY-$(BEAUTY_FACE)   += libarcsoft_beautyshot
LIBRARY-$(GENDER_DETECT) += libarcsoft_agegender_estimation
LIBRARY-$(HDRT)          += libtsvideoprocess
LIBRARY-$(SMART_SELECT)  += libtsvideoprocess
LIBRARY-$(SMART_SHOT)    += libtsvideoprocess
LIBRARY-$(ANTI_SHAKE)    += libarcsoft_antishaking
LIBRARY-$(NIGHT_SHOT)    += libarcsoft_night_shot
LIBRARY-$(MOVIE_SOLID)   += libmorpho_image_stab5 libtsvideoprocess
LIBRARY-$(PICTURE_ZOOM)  += libarcsoft_piczoom
LIBRARY-$(FAIR_LIGHT)    += libmegface_portrait libMegviiHum
LIBRARY-$(MICRO_PLASTIC) += libMegviiAIReshape
LIBRARY-$(PICTURE_ZOOM2) += libarcsoft_piczoom
LIBRARY-$(STILL_BOKEH)   += libalSDE2 libalRnB
LIBRARY-$(STILL_BOKEHA)  += libarcsoft_dualcam_refocus
LIBRARY-$(RT_BOKEH)      += libarcsoft_dualcam_refocus
LIBRARY-$(DUAL_NIGHT)    += libarcsoft_dualcam_low_light_shot
LIBRARY-$(HDRM)          += libmorpho_easy_hdr
LIBRARY-$(HDRM)          += libmorpho_dr_checker2
LIBRARY-$(HDRM4)         += libmorpho_image_refiner
LIBRARY-$(HDRM4)         += libmorpho_dr_checker2
LIBRARY-$(SUPER_RES)     += libmorpho_super_resolution

# Vendor static library to link
STATICLIB-$(PHOTO_SOLID_4) :=
STATICLIB-$(PHOTO_SOLID_5) +=
STATICLIB-$(BEAUTY_FACE)   +=
STATICLIB-$(GENDER_DETECT) += mpbase
STATICLIB-$(HDRT)          +=
STATICLIB-$(SMART_SELECT)  +=
STATICLIB-$(SMART_SHOT)    +=
STATICLIB-$(ANTI_SHAKE)    +=
STATICLIB-$(NIGHT_SHOT)    +=
STATICLIB-$(MOVIE_SOLID)   +=
STATICLIB-$(PICTURE_ZOOM)  +=
STATICLIB-$(FAIR_LIGHT)    +=
STATICLIB-$(MICRO_PLASTIC) +=
STATICLIB-$(PICTURE_ZOOM2) +=
STATICLIB-$(STILL_BOKEH)   += libpandora_yuv
STATICLIB-$(STILL_BOKEHA)  +=
STATICLIB-$(RT_BOKEH)      +=
STATICLIB-$(DUAL_NIGHT)    +=
STATICLIB-$(HDRM)          +=
STATICLIB-$(SUPER_RES)     +=
STATICLIB-$(SINGLE_BOKEH)  += libmorpho_defocusface

# Rules
ifeq ($(HDRM),y)
  ifeq ($(HDRT),y)
    $(error Can not enable HDR and HDRM at same time)
  endif
endif

ifeq ($(HDRM),n)
  ifeq ($(HDRT),n)
    CFLAGS-y += -DENABLE_DEFAULT_HDR
  endif
endif

ifeq ($(PHOTO_SOLID_4),y)
  ifeq ($(PHOTO_SOLID_5),y)
    $(error Can not enable Photo Solid v4 and Photo Solid v5 at same time)
  endif
endif

# Add prefix for all vars
COMPILE-y   := $(patsubst %, $(SRC_FILE_PATH)/%.cpp, $(sort $(COMPILE-y)))
CFLAGS-y    := $(sort $(CFLAGS-y))
CINCLUDES-y := $(patsubst %, $(VENDOR_INCLUDES_PATH)/%, $(sort $(CINCLUDES-y)))
LIBRARY-y   := $(sort $(LIBRARY-y))
STATICLIB-y := $(sort $(STATICLIB-y))
COMPILE-Y   := $(COMPILE-y)
CFLAGS-Y    := $(CFLAGS-y)
CINCLUDES-Y := $(CINCLUDES-y)
LIBRARY-Y   := $(LIBRARY-y)
STATICLIB-Y := $(STATICLIB-y)

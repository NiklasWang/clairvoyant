CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = libcore$(strip $(DYLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS =  liblog libmemory
STATIC_LIBS =  libsp libutil libutils libthreads libpal libalgorithms libyuv

CXXFLAGS += -I$(ROOT_DIR)/external/freetype/freetype-2.9.1/include

include $(ROOT_DIR)/vendor/config.mk
CXXFLAGS    += $(CFLAGS-Y)
SHARED_LIBS += $(LIBRARY-Y)
STATIC_LIBS += $(STATICLIB-Y)


include $(MAKE_RULE)/find.library.make.rule

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

include $(MAKE_RULE)/submodule.make.rule

all: compile link $(MAKE_SUB_MODULES)

compile: $(objects) $(COMPILE_SUB_MODULES)

link: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)
	$(MAKE) $(LINK_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean $(COMPILE_SUB_MODULES) $(LINK_SUB_MODULES) $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)

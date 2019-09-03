CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS) -pthread

include $(MAKE_RULE)/project.info.make.rule

TARGET      = $(PROJNAME)$(strip $(EXE_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

include $(MAKE_RULE)/find.all.modules.make.rule

SHARED_LIBS = $(DYLIB_MODULE)
STATIC_LIBS = $(STLIB_MODULE)

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

LDFLAGS += -L $(ROOT_DIR)/external/freetype/freetype-2.6/objs/libs -L $(ROOT_DIR)/external/libjpeg-turbo/libjpeg-turbo/libs -L /usr/lib/x86_64-linux-gnu -I/usr/include/harfbuzz  -I/usr/include/libpng  -Wl,-Bstatic  -lfreetype -lturbojpeg -lpng -lexif -lutils -lutil -Wl,-Bdynamic -llog -lharfbuzz -lz

all: compile link

compile: $(objects)

link: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean
CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)

TARGET      = libthreads$(strip $(STLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS = liblog libmemory libcore
STATIC_LIBS = libsp libutils libpal

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link

compile: $(objects)

link: $(objects)
	$(AR) $(ARFLAGS) $(TARGET) $(objects)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean

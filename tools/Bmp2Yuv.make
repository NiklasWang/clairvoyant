CC = gcc
CFLAGS = -std=c99
LDFLAGS = -lm
TARGET = bmp2yuv
sources = Bmp2Yuv.c Bmp2YuvTester.c
objects = $(sources:.c=.o)
dependence:=$(sources:.c=.d)

all: $(objects)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

include $(dependence)

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: clean

clean:
	rm -f $(TARGET) $(objects) $(dependence)
	rm -f *.nv21


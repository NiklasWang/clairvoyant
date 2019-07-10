CC = gcc
CFLAGS =
LDFLAGS =
TARGET = matrix_rotate
sources = MatrixRotateTest.c
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


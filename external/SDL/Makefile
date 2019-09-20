include $(MAKE_RULE)/color.print.make.rule

TARGET  = libSDL2$(strip $(STLIB_EXT))
CURRDIR = $(shell pwd)

FILENAME = libSDL2.a


#exist = $(shell if [ -f $(CURRDIR)/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all compile link:
	@echo -e $(NOTICE_COLOR)"This module requires following packages, make sure they are installed in your system."$(RESTORE_COLOR)
	@echo -e $(NOTICE_COLOR)" - automake, autopoint, libtool, gettext, etc."$(RESTORE_COLOR)
ifneq ($(TARGET), $(notdir $(wildcard $(CURRDIR)/$(TARGET))))
	$(call BUILD)
endif


define BUILD
	cd SDL2; \
	./configure; \
	make; \
	cp -ar $(CURRDIR)/SDL2/build/libs/$(FILENAME) $(CURRDIR)/
endef


clean:
	cd SDL2; \
	make clean; \
	rm -rf $(CURRDIR)/$(TARGET) \

.PHONY: clean

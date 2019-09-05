include $(MAKE_RULE)/color.print.make.rule

TARGET  = libfreetype$(strip $(STLIB_EXT))
CURRDIR = $(shell pwd)

FILENAME = libfreetype.a


#exist = $(shell if [ -f $(CURRDIR)/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all compile link:
	@echo -e $(NOTICE_COLOR)"This module requires following packages, make sure they are installed in your system."$(RESTORE_COLOR)
	@echo -e $(NOTICE_COLOR)" - automake, autopoint, libtool, gettext, etc."$(RESTORE_COLOR)
ifneq ($(TARGET), $(notdir $(wildcard $(CURRDIR)/$(TARGET))))
	$(call BUILD)
endif

 define BUILD
	cd freetype-2.6; \
	./configure; \
	make; \
	cp -ar $(CURRDIR)/freetype-2.6/objs/libs/$(FILENAME) $(CURRDIR)/
endef

clean:
	cd freetype-2.6; \
	make  clean; \
	rm -rf $(CURRDIR)/$(TARGET)
.PHONY: clean

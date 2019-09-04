include $(MAKE_RULE)/submodule.make.rule

all: $(MAKE_SUB_MODULES)

compile: $(COMPILE_SUB_MODULES)

link: $(LINK_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)
	rm -f *.so
	rm -f *.jpg
	rm -f $(PROJNAME)

.PHONY: all clean $(MAKE_SUB_MODULES) $(COMPILE_SUB_MODULES) $(LINK_SUB_MODULES) $(CLEAN_SUB_MODULES)

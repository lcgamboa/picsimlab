SUBDIRS= clock  editor getlinks  imageview motor  simple  sin LCDfont geditor
.PHONY: $(SUBDIRS)


all:$(SUBDIRS)

install:$(SUBDIRS)
	
clean:$(SUBDIRS)
	
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)


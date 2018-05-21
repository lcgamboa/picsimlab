
FILE=Makefile


SUBDIRS= src tools/espmsim tools/srtank

.PHONY: $(SUBDIRS)  

all:$(SUBDIRS) 

install:$(SUBDIRS) 
	
clean:$(SUBDIRS) 

uninstall:$(SUBDIRS) 

	
$(SUBDIRS):
	$(MAKE) -f $(FILE) -C $@ $(MAKECMDGOALS)

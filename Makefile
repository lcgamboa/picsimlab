
FILE=Makefile


SUBDIRS= src

.PHONY: $(SUBDIRS)  

all:$(SUBDIRS) 

install:$(SUBDIRS) 
	
clean:$(SUBDIRS) 

uninstall:$(SUBDIRS) 

	
$(SUBDIRS):
	$(MAKE) -f $(FILE) -C $@ $(MAKECMDGOALS)

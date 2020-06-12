
FILE=Makefile


SUBDIRS= src tools/espmsim tools/srtank

.PHONY: $(SUBDIRS)  

all:$(SUBDIRS) doc

install:$(SUBDIRS) 
	
clean:$(SUBDIRS) 
	rm -Rf *.exe *.zip

doc:
	doxygen
	gitchangelog > CHANGELOG.md
	! leasot -r markdown  src/*.{cc,h} src/*/*.{cc,h}  > TODO.md 

uninstall:$(SUBDIRS) 

	
$(SUBDIRS):
	$(MAKE) -f $(FILE) -C $@ $(MAKECMDGOALS)


FILE=Makefile


SUBDIRS= src tools/espmsim tools/srtank

.PHONY: $(SUBDIRS)  

all:$(SUBDIRS) 

exp:$(SUBDIRS) 

install:$(SUBDIRS) 

install_app:$(SUBDIRS) 

clean:$(SUBDIRS) 
	rm -Rf *.exe *.zip

doc:
	doxygen 1>/dev/null  2>/dev/null
	gitchangelog ^v0.8.0 HEAD > CHANGELOG_auto.md
	! leasot -r markdown  src/*.{cc,h} src/*/*.{cc,h}  > TODO.md 

uninstall:$(SUBDIRS) 

	
$(SUBDIRS):
	$(MAKE) -f $(FILE) -C $@ $(MAKECMDGOALS)

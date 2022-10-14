include ../Makefile.Conf

all:
static:
install:
	mkdir -p ${datadir}/lxrad_NOGUI/docs
	cp -v *.* ${datadir}/lxrad_NOGUI/docs
	cp -v README ${datadir}/lxrad_NOGUI/docs
	chmod 644 -R ${datadir}/lxrad_NOGUI/docs/*
	chmod 755 ${datadir}/lxrad_NOGUI/docs

clean:
uninstall:
	rm -f -R ${datadir}/lxrad_NOGUI/docs

#!/bin/bash -x 
rm -rf build_all/picsimlab*
sudo apt-get install debmake
git archive --format=tar master > build_all/picsimlab-0.8.0.tar 
cd build_all
tar  --delete -f picsimlab-0.8.0.tar "docs/kicad"
tar  --delete -f picsimlab-0.8.0.tar "docs/examples"
tar  --delete -f picsimlab-0.8.0.tar "docs/pdf"
tar  --delete -f picsimlab-0.8.0.tar "docs/srtank"
tar  --delete -f picsimlab-0.8.0.tar "docs/espmsim"
tar  --delete --wildcards -f picsimlab-0.8.0.tar "docs/*.png"
tar  --delete --wildcards -f picsimlab-0.8.0.tar "docs/*.html" 
tar  --delete --wildcards -f picsimlab-0.8.0.tar "docs/*.pdf" 
tar  --delete --wildcards -f picsimlab-0.8.0.tar "docs/*.css" 
gzip -q picsimlab-0.8.0.tar
mkdir picsimlab-0.8.0
cd picsimlab-0.8.0
tar xvfz ../picsimlab-0.8.0.tar.gz
mv src/Makefile.static src/Makefile
mv tools/srtank/Makefile.static tools/srtank/Makefile
mv tools0espmsim/Makefile.static tools/espmsim/Makefile
cd ..
tar cvfz  picsimlab-0.8.0.tar.gz picsimlab-0.8.0
mv picsimlab-0.8.0.tar.gz picsimlab_0.8.0.orig.tar.gz
cd picsimlab-0.8.0
sudo debuild


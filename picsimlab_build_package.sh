#!/bin/bash -x 
sudo apt-get install debmake
cd build_all
git clone https://github.com/lcgamboa/picsimlab.git
rm -Rf picsimlab/.git
rm -Rf picsimlab/docs/*.* 
rm -Rf picsimlab/docs/examples/ 
rm -Rf picsimlab/docs/pdf/ 
rm -Rf picsimlab/docs/srtank/ 
rm -Rf picsimlab/docs/espmsim/ 
rm -Rf picsimlab/docs/kicad/
mv picsimlab/src/Makefile.static picsimlab/src/Makefile
mv picsimlab/tools/srtank/Makefile.static picsimlab/tools/srtank/Makefile
mv picsimlab/tools0espmsim/Makefile.static picsimlab/tools/espmsim/Makefile
mv picsimlab picsimlab-0.8.0
tar cvfz  picsimlab-0.8.0.tar.gz picsimlab-0.8.0
cd picsimlab-0.8.0/
debmake
debuild


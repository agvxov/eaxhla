#!/bin/sh
# this script requires git.
cd "$(dirname "$(readlink -f "$0")")"
git clone https://github.com/amadvance/tommyds/ --depth=1
make -C tommyds
mkdir -v -p ../object
ln -v -s -f tommyds/tommy.o ../object/

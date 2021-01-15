#!/bin/bash

if [ ! -d "build" ]; then
	echo "Create build directory"
	mkdir build
fi
cd build

echo "Exporting PKG_CONFIG_PATH"

 #export PKG_CONFIG_PATH="$HOME/ffmpeg_build/lib/pkgconfig"
export PKG_CONFIG_PATH="/home/lguarezi/Desktop/libs/ffmpeg/lib/pkgconfig"


cmake ..
make


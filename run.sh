#!/bin/bash

if [ -d "build" ]; then
./build/screenCaptureTeste
else
	echo "build folder not exist"
fi

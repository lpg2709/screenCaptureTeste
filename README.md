# screenCaptureTeste

Screen record test for Xorg

> Not working, fail in put data into video stream

## Dependencies

- FFmpeg libs

- C compiler (GCC) > 9.3.0

- CMake > 3.0

- pkg-config > 0.29.1

## Scripts

- ```compile_debug.sh``` - Compile the code, for debug.

## Install FFmpeg

I recomand compile direct from source code.I have some problemes with the packages install.

```bash
git clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
cd ffmpeg
./configure
make
make install
```

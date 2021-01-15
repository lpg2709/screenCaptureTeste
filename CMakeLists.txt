cmake_minimum_required(VERSION 3.0)

project(screenCaptureTeste C)

add_subdirectory(lib/ffmpeg)

list(APPEND SOURCES main.c)
list(APPEND CLINKS "-lm")

add_executable(screenCaptureTeste ${SOURCES})

target_link_libraries(screenCaptureTeste FFmpeg ${CLINKS})

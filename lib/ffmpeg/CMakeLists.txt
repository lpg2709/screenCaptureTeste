cmake_minimum_required(VERSION 3.0)
project(FFmpeg)

find_package(PkgConfig REQUIRED)
pkg_check_modules(AVCODEC 	REQUIRED IMPORTED_TARGET libavcodec)


add_library(FFmpeg INTERFACE IMPORTED GLOBAL)

target_include_directories(FFmpeg INTERFACE
	${AVCODEC_INCLUDE_DIRS}
)

target_link_options(FFmpeg INTERFACE
	${AVCODEC_LDFAGS}
)

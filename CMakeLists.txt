cmake_minimum_required(VERSION 3.9.2 FATAL_ERROR)
project(OpenGLPAG VERSION 0.1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-module")
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# add thirdparties
include(thirdparty/thirdparty.cmake)

# subdirectories
add_subdirectory(src)
PROJECT(Engine)
CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_DEFINITIONS("/std:c++latest")

INCLUDE(conan_include.cmake)

SET(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/Bin/${BUILD_PLATFORM}/${CMAKE_BUILD_TYPE})
SET(EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)
SET(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

ADD_SUBDIRECTORY(${EXTERNAL_DIR})
ADD_SUBDIRECTORY(${SRC_DIR})
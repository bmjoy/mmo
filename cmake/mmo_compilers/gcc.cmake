# This file, when included, does some config options for GCC.

# GCC 7.0 or newer is required
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
if (NOT (GCC_VERSION VERSION_GREATER 7.0 OR GCC_VERSION VERSION_EQUAL 7.0))
	message(FATAL_ERROR "GCC 7.0 or newer is required in order to build this project, but you have ${GCC_VERSION}. Please consider an update or switch over to clang.")
endif()

# Enable C++17 standard
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_CXX_FLAGS "-std=c++17 -pthread")

# Zlib is required for GCC builds
find_package(ZLIB REQUIRED)

# - CODA-OSS CMake Build script.
#
# Author:  Scott A. Colcord

cmake_minimum_required(VERSION 3.14)

project(coda-oss)

set(CMAKE_CXX_STANDARD 11)

if (${CMAKE_PROJECT_NAME} STREQUAL coda-oss)
    # this is the top level project

    # set up warnings
    if (MSVC)
        # set warning level to /W3
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        add_compile_options(/W3 /wd4290 /wd4512)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
    elseif (UNIX)
        add_compile_options(
            -Wno-deprecated
            -Wno-unused-value
            -Wno-unused-but-set-variable
        )
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    include(CodaBuild)

    coda_initialize_build()

    # install our cmake build modules for downstream use
    install(DIRECTORY "cmake/"
            DESTINATION "${CODA_STD_PROJECT_LIB_DIR}/cmake/"
            FILES_MATCHING PATTERN "*.cmake")
endif()

add_subdirectory("modules")

# generate package config, with the following paths exported
set(JARS_DIR ${CODA_STD_PROJECT_LIB_DIR} CACHE INTERNAL
    "path to installed jars, needed by downstream projects")
set(SWIG_INCLUDE_DIR "${CODA_STD_PROJECT_INCLUDE_DIR}/swig" CACHE INTERNAL
    "path to installed SWIG includes, needed by downstream projects")
coda_generate_package_config(JARS_DIR SWIG_INCLUDE_DIR)

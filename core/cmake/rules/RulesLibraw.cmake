#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

message(STATUS "")
message(STATUS "--------------------------------------------------")
message(STATUS "digiKam RawEngine dependencies checks:")
message(STATUS "")

include(MacroOpenMP)
DETECT_OPENMP()

if(OPENMP_FOUND)

    message(STATUS "RawEngine will be compiled with OpenMP support")

else()

    # To force libraw to use OpenMP with Macports and MXE.

    if (APPLE OR MINGW)

        add_definitions(-DLIBRAW_FORCE_OPENMP)
        set(OPENMP_FOUND ON)
        message(STATUS "RawEngine will be compiled with OpenMP support")

    else()

        message(STATUS "RawEngine will not be compiled with OpenMP support")

    endif()

endif()

# Flag to use libjasper with LibRaw RedCine codec

if(Jasper_FOUND)

    add_definitions(-DUSE_JASPER)
    include_directories(${JASPER_INCLUDE_DIR})
    message(STATUS "RawEngine will be compiled with RedCine codec")

else()

    message(STATUS "RawEngine will not be compiled with RedCine codec")

endif()

# JPEG library check

message(STATUS "Looking for LibJpeg")

if(JPEG_FOUND)

    include_directories(${JPEG_INCLUDE_DIRS})

    # JPEG lib version is done on top level through MacroJPEG.cmake

    if (${JPEG_LIB_VERSION} LESS 80)

        set(JPEG8_FOUND FALSE)

    else()

        set(JPEG8_FOUND TRUE)

    endif()

endif()

# Flag to use libjpeg with LibRaw DNG lossy codec

if(JPEG8_FOUND)

    add_definitions(-DUSE_JPEG)
    add_definitions(-DUSE_JPEG8)
    message(STATUS "RawEngine will be compiled with DNG lossy codec")

else()

    message(STATUS "RawEngine will not be compiled with DNG lossy codec")

endif()

message(STATUS "Looking for PThreads")
set(PTHREADS_FOUND (CMAKE_USE_PTHREADS_INIT OR CMAKE_USE_WIN32_THREADS_INIT))

# Registration of Libraw configuration to a dedicated header

MACRO_BOOL_TO_01(JPEG8_FOUND  LIBRAW_USE_DNGLOSSYCODEC)
MACRO_BOOL_TO_01(OPENMP_FOUND LIBRAW_USE_OPENMP)
MACRO_BOOL_TO_01(Jasper_FOUND LIBRAW_USE_REDCINECODEC)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libs/rawengine/libraw/libraw_config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/libs/rawengine/libraw/libraw_config.h)

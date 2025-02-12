# A macro wrapper to find OpenMP library
#
# Syntax:  DETECT_OPENMP()
#
# Once done this will be adjusted:
#
# OPENMP_FOUND     - OpenMP detection flag
# OpenMP_CXX_FLAGS - flags to add to the CXX compiler for OpenMP support
#
# SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

macro(DETECT_OPENMP)

    message(STATUS "Looking for OpenMP")

    # NOTE: OpenMP under MacOSX do not support multithreading.

    if(NOT APPLE)

        find_package(OpenMP)

        if(OPENMP_FOUND)

            # Adjust some compiler flags depending of Operating system, that standard cmake find script do not set.

            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

            if("${OpenMP_CXX_FLAGS}" STREQUAL "-fopenmp")
                set(OPENMP_LDFLAGS "-lgomp")
            endif()

            if("${OpenMP_CXX_FLAGS}" STREQUAL "-xopenmp")
                set(OPENMP_LDFLAGS "-xopenmp")
            endif()

            message(STATUS "OpenMP found with CXX flags: ${OpenMP_CXX_FLAGS}")

        else()

            message(STATUS "OpenMP not found")

        endif()

    else()

        message(STATUS "OpenMP under OSX do not support multithreading.")
        message(STATUS "OpenMP detection is disabled.")
        set(OPENMP_FOUND FALSE)

    endif()

endmacro()

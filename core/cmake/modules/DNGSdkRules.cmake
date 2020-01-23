#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

#---------------------------------

# Common DNG SDK definitions for DNGWriter.
#
# Set platforms flags.
# NOTE: see bug #195735: do not enable Mac flags provided by Adobe.
#       Sounds like all compile fine like under Linux.

add_definitions(-DEnablePluginManager=0)
add_definitions(-DXMP_StaticBuild=1)

if(MSVC)
    add_definitions(
                    # XMP SDK
                    -DWIN_ENV=1
                    -DUNIX_ENV=0

                    # DNG SDK
                    -DqWinOS=1
                    -DqMacOS=0
                    -DqDNGUseStdInt=0
                    )
else()
    add_definitions(
                     # XMP SDK
                    -DWIN_ENV=0
                    -DUNIX_ENV=1

                    # DNG SDK
                    -DqWinOS=0
                    -DqMacOS=0
                    -DqDNGUseStdInt=1
                   )
endif()

# Check processor endianness
include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)

if(NOT IS_BIG_ENDIAN)
    add_definitions(-DqDNGLittleEndian)
endif()

# Thread safe support under Mac and Linux using pthread library

if(NOT MSVC)
    add_definitions(-DqDNGThreadSafe)
endif()

# Mode definition for console output.

add_definitions(-DqDNGValidateTarget)

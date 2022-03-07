# Script to build jpegXL library for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(PREFIX_ext_jpegxl "${EXTPREFIX}")

set(JpegXL_Conf -DBUILD_SHARED_LIBS=ON
)

JoinListAsString("${JpegXL_Conf}" " " BASH_OPTIONS)

if(MINGW)

    ExternalProject_Add(ext_jpegxl
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/libjxl/libjxl.git
        GIT_TAG v0.6.1

        CONFIGURE_COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo ${BASH_OPTIONS}

        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE) -j

        INSTALL_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE) install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

elseif(APPLE)

    ExternalProject_Add(ext_jpegxl
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        # To use official release tarball

        GIT_REPOSITORY https://github.com/libjxl/libjxl.git
        GIT_TAG v0.6.1

        CONFIGURE_COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.macports <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.macports ${PREFIX_ext_jpegxl} RelWithDebInfo x86_64 ${BASH_OPTIONS}

        BUILD_COMMAND cd <SOURCE_DIR>/build && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR>/build && $(MAKE) install/fast

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

else() # Linux

    ExternalProject_Add(ext_jpegxl
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/libjxl/libjxl.git
        GIT_TAG v0.6.1

        CONFIGURE_COMMAND $(CMAKE_COMMAND) . -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_jpegxl}
                                             -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                                             -DICONV_INCLUDE_DIR=${PREFIX_ext_jpegxl}/include
                                             ${JpegXL_Conf}
                                             ${GLOBAL_PROFILE}

        BUILD_COMMAND cd <SOURCE_DIR> && $(MAKE)

        INSTALL_COMMAND cd <SOURCE_DIR> && $(MAKE) install/fast

        UPDATE_COMMAND ""
        ALWAYS 0
    )

endif()

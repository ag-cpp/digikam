# Script to build static version of OpenSSL for AppImage bundle.
#
# SPDX-FileCopyrightText: 2015-2024 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
#

if (NOT APPLE)

    SET(EXTPREFIX_openssl "${EXTPREFIX}/local/")

    ExternalProject_Add(ext_openssl

        GIT_REPOSITORY https://github.com/janbar/openssl-cmake
        GIT_TAG 1.1.1w-20231130

        INSTALL_DIR ${EXTPREFIX_openssl}

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_openssl}
                   -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                   -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE
                   -DWITH_APPS=OFF
                   -DWITH_DOCS=OFF
                   -DWITH_TESTS=OFF
                   ${GLOBAL_PROFILE}

        BUILD_COMMAND ${BUILD_CLI}

        UPDATE_COMMAND ""
        ALWAYS 0
    )

endif()

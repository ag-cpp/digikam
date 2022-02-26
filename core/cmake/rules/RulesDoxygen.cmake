#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(Doxygen_FOUND)

    message(STATUS "Doxygen is found.......................... ${Doxygen_FOUND}")
    message(STATUS "Graphviz Dot is found..................... ${DOXYGEN_DOT_FOUND}")

    if(Doxygen_FOUND AND DOXYGEN_DOT_FOUND)

        message(STATUS "To build API documentation use 'make doc'")

        add_custom_target(doc ${DOXYGEN_EXECUTABLE}
                          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    endif()

endif()

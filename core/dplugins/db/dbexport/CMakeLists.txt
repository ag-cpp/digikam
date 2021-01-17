#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (C) 2021, Nghia Duong <minhnghiaduong997 at gmail dot com>

# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(dbexportplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dbexportplugin.cpp
)

DIGIKAM_ADD_DB_PLUGIN(NAME    DatabaseExport
                      SOURCES ${dbexportplugin_SRCS}
)


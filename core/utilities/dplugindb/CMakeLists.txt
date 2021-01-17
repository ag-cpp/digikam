#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2021      by Nghia Duong, <minhnghiaduong997 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libdplugindb_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dplugins/dplugindb.cpp
)

include_directories($<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::WidgetsAddons,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(gui_dplugindb_obj OBJECT ${libdplugindb_SRCS} ${libdlib_SRCS})

# Used by digikamgui
target_compile_definitions(gui_dplugindb_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

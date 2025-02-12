# Macro to rule install paths for Qt5 and Qt6 target depending of ECM provided variables
#
# SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

if (NOT ${KDE_INSTALL_FULL_KNOTIFY5RCDIR} STREQUAL "")

    set(KDE_INSTALL_FULL_KNOTIFYRCDIR ${KDE_INSTALL_FULL_KNOTIFY5RCDIR})

endif()

if (NOT ${KDE_INSTALL_FULL_KXMLGUI5DIR} STREQUAL "")

    set(KDE_INSTALL_FULL_KXMLGUIDIR ${KDE_INSTALL_FULL_KXMLGUI5DIR})

endif()

if (NOT ${KDE_INSTALL_FULL_KSERVICES5DIR} STREQUAL "")

    set(KDE_INSTALL_FULL_KSERVICESDIR ${KDE_INSTALL_FULL_KSERVICES5DIR})

endif()

message(STATUS "")
message(STATUS "--------------------------------------------------")
message(STATUS "digiKam Install Paths:")
message(STATUS "")

message(STATUS "INSTALL DATA DIR      : ${KDE_INSTALL_FULL_DATADIR}")
message(STATUS "INSTALL ICON DIR      : ${KDE_INSTALL_FULL_ICONDIR}")
message(STATUS "INSTALL APP DIR       : ${KDE_INSTALL_FULL_APPDIR}")
message(STATUS "INSTALL KNOTIFYRC DIR : ${KDE_INSTALL_FULL_KNOTIFYRCDIR}")
message(STATUS "INSTALL BIN DIR       : ${KDE_INSTALL_FULL_BINDIR}")
message(STATUS "INSTALL MAN DIR       : ${KDE_INSTALL_FULL_MANDIR}")
message(STATUS "INSTALL PLUGIN DIR    : ${KDE_INSTALL_FULL_PLUGINDIR}")
message(STATUS "INSTALL KXMLGUI DIR   : ${KDE_INSTALL_FULL_KXMLGUIDIR}")
message(STATUS "INSTALL SERVICES DIR  : ${KDE_INSTALL_FULL_KSERVICESDIR}")
message(STATUS "INSTALL LIBEXEC DIR   : ${KDE_INSTALL_FULL_LIBEXECDIR}")
message(STATUS "INSTALL METAINFO DIR  : ${KDE_INSTALL_FULL_METAINFODIR}")
message(STATUS "---")

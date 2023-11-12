#!/bin/bash

# SPDX-FileCopyrightText: 2013-2023 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

########################################################################

# About short pathes to use with VCPKG, see https://github.com/microsoft/vcpkg/issues/24751
# /c/ is C:/ drive
# /e/ is E:/ drive

# Absolute path where are installed VCPKG scripts, portfiles, and configurations.
VCPKG_DIR="/c/vcpkg"

# Absolute path where are installed all compiled targets.
INSTALL_DIR="/e/dk"

# Absolute path where are downloaded all tarballs to compile.
DOWNLOAD_DIR="/e/d"

# Absolute path where are compiled all tarballs
BUILDING_DIR="/e/b"

VCPKG_TRIPLET="x64-windows"

VCPKG_COMMON_OPTIONS=(
"--disable-metrics"
"--no-print-usage"
"--triplet ${VCPKG_TRIPLET}"
"--x-buildtrees-root=${BUILDING_DIR}"
"--x-install-root=${INSTALL_DIR}"
"--downloads-root=${DOWNLOAD_DIR}"
"--vcpkg-root=${VCPKG_DIR}"
)

VCPKG_INSTALL_PREFIX=$INSTALL_DIR/$VCPKG_TRIPLET

#-------------------------------------------------------------------------------------------

# URL to git repository to checkout digiKam source code
# git protocol version which require a developer account with ssh keys.
DK_GITURL="git@invent.kde.org:graphics/digikam.git"
# https protocol version which give annonyous access.
#DK_GITURL="https://invent.kde.org/graphics/digikam.git"

# digiKam tarball information.
DK_URL="http://download.kde.org/stable/digikam"

# Location to build source code.
DK_BUILDTEMP=$BUILDING_DIR/dktemp

# KDE Plasma version.
# See official release here: https://download.kde.org/stable/plasma/
DK_KP_VERSION="5.27.8"

# KDE Application version.
# See official release here: https://download.kde.org/stable/release-service/
DK_KA_VERSION="23.08.1"

# KDE KF5 frameworks version.
# See official release here: https://download.kde.org/stable/frameworks/
DK_KDE_VERSION="master"

# Qt version to use in bundle and provided by MXE.
DK_QTVERSION="6"

# digiKam tag version from git. Official tarball do not include extra shared libraries.
# The list of tags can be listed with this url: https://invent.kde.org/graphics/digikam/-/tags
# If you want to package current implementation from git, use "master" as tag.
#DK_VERSION=v7.6.0
DK_VERSION=master
#DK_VERSION=qt5-maintenance

# Installer sub version to differentiates newer updates of the installer itself, even if the underlying application hasn’t changed.
#DK_SUBVER="-01"

# Installer will include or not digiKam debug symbols
DK_DEBUG=0

# Sign bundles with GPG. Passphrase must be hosted in ~/.gnupg/dkorg-gpg-pwd.txt
DK_SIGN=0

# Upload automatically bundle to files.kde.org (pre-release only).
DK_UPLOAD=1
DK_UPLOADURL="digikam@tinami.kde.org"
DK_UPLOADDIR="/srv/archives/files/digikam/unstable"     # Qt6 version is considerated unstable for the moment.

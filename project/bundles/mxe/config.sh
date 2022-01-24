#!/bin/bash

# Copyright (c) 2013-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

########################################################################

# Absolute path where are downloaded all tarballs to compile.
DOWNLOAD_DIR="`pwd`/temp.dwnld"

# Absolute path where are compiled all tarballs
BUILDING_DIR="`pwd`/temp.build"

#-------------------------------------------------------------------------------------------

# MXE configuration

#------------
# IMPORTANT: Target Windows architecture to build installer. Possible values: 32 or 64 bits.
#            We do not support 32 bits anymore due to lack of memory allocation on this kind
#            of architecture.
MXE_ARCHBITS=64
#------------

if [[ $MXE_ARCHBITS == 32 ]]; then
    # Windows 32 bits shared
    MXE_BUILD_TARGETS="i686-w64-mingw32.shared"
    MXE_BUILDROOT="`pwd`/build.win32"
elif [[ $MXE_ARCHBITS == 64 ]]; then
    # Windows 64 bits shared
    MXE_BUILD_TARGETS="x86_64-w64-mingw32.shared"
    MXE_BUILDROOT="`pwd`/build.win64"
else
    echo "Unsupported or wrong target Windows architecture: $MXE_ARCHBITS bits."
    exit -1
fi

echo "Target Windows architecture: $MXE_ARCHBITS bits."

MXE_GIT_URL="https://github.com/mxe/mxe.git"
MXE_GIT_REVISION=master
MXE_INSTALL_PREFIX=${MXE_BUILDROOT}/usr/${MXE_BUILD_TARGETS}/
MXE_TOOLCHAIN=${MXE_INSTALL_PREFIX}/share/cmake/mxe-conf.cmake

#-------------------------------------------------------------------------------------------

# URL to git repository to checkout digiKam source code
# git protocol version which require a developer account with ssh keys.
DK_GITURL="git@invent.kde.org:graphics/digikam.git"
# https protocol version which give annonyous access.
#DK_GITURL="https://invent.kde.org/graphics/digikam.git"

# digiKam tarball information.
DK_URL="http://download.kde.org/stable/digikam"

# Location to build source code.
DK_BUILDTEMP=~/dktemp

# KDE Application version.
DK_KA_VERSION="21.12.1"

# KDE KF5 frameworks version.
DK_KF5_VERSION="5.90"

# ExifTool Windows version to embed in the bundle.
DK_EXIFTOOL_VERSION="12.39"

# digiKam tag version from git. Official tarball do not include extra shared libraries.
# The list of tags can be listed with this url: https://invent.kde.org/graphics/digikam/-/tags
# If you want to package current implementation from git, use "master" as tag.
#DK_VERSION=v7.0.0
DK_VERSION=master
#DK_VERSION=development/dplugins

# Installer sub version to differentiates newer updates of the installer itself, even if the underlying application hasn’t changed.
#DK_SUBVER="-01"

# Installer will include or not digiKam debug symbols
DK_DEBUG=0

# Sign bundles with GPG. Passphrase must be hosted in ~/.gnupg/dkorg-gpg-pwd.txt
DK_SIGN=0

# Upload automatically bundle to files.kde.org (pre-release only).
DK_UPLOAD=1
DK_UPLOADURL="digikam@deino.kde.org"
DK_UPLOADDIR="/srv/archives/files/digikam/"

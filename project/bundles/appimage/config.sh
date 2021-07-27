#!/bin/bash

# Configuration for script to build AppImage bundle.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

########################################################################

# Absolute path where are downloaded all tarballs to compile.
DOWNLOAD_DIR="/d"

# Absolute path where are compiled all tarballs
BUILDING_DIR="/b"

# Absolute path where are installed all software components
INSTALL_DIR="/usr"

# Absolute path where are bundled all software components
APP_IMG_DIR="/digikam.appdir"

########################################################################

# URL to git repository to checkout digiKam source code
DK_GITURL="git@invent.kde.org:graphics/digikam.git"

# Location to build source code.
DK_BUILDTEMP=$BUILDING_DIR/dktemp

# digiKam tag version from git. Official tarball do not include extra shared libraries.
# The list of tags can be listed with this url: https://invent.kde.org/graphics/digikam/-/tags
# If you want to package current implemntation from git, use "master" as tag.
#DK_VERSION=v7.0.0
DK_VERSION=master
#DK_VERSION=gsoc18-exporttools-o2

# Installer sub version to differentiates newer updates of the installer itself, even if the underlying application hasn’t changed.
#DK_SUBVER="-01"

# Installer will include or not digiKam debug symbols
DK_DEBUG=0

# Option to use QtWebEngine instead QtWebkit
DK_QTWEBENGINE=1

# Sign bundles with GPG. Passphrase must be hosted in ~/.gnupg/dkorg-gpg-pwd.txt
DK_SIGN=0

# Upload automatically bundle to files.kde.org (pre-release only).
DK_UPLOAD=1
DK_UPLOADURL="digikam@deino.kde.org"
DK_UPLOADDIR="/srv/archives/files/digikam/"

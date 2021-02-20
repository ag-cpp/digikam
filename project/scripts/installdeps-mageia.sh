#!/bin/bash

# Script to install dependencies under Mageia Linux to compile digiKam.
# This script must be run as sudo
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

# Check root as root

if [[ $EUID -ne 0 ]]; then
    echo "This script should be run as root using sudo command."
    exit 1
else
    echo "Check run as root passed..."
fi

urpmi --auto \
      git \
      subversion \
      cmake \
      extra-cmake-modules \
      gcc-c++ \
      gperf \
      ruby \
      bison \
      flex \
      libgomp-devel \
      lib64expat-devel \
      eigen3-devel \
      lib64lcms2-devel \
      lib64qtav-devel \
      lib64ffmpeg-devel \
      lib64boost-devel \
      lib64gphoto-devel \
      lib64sane1-devel \
      lib64jasper-devel \
      lib64xslt-devel \
      lib64kf5calendarcore-devel \
      lib64xml2-devel \
      lib64jpeg-devel \
      lib64png-devel \
      lib64tiff-devel \
      lib64lqr-devel \
      lib64fftw-devel \
      lib64curl-devel \
      lib64magick-devel \
      lib64qt5core-devel \
      lib64qt5widgets-devel \
      lib64qt5x11extras-devel \
      lib64qt5test-devel \
      lib64qt5xml-devel \
      lib64qt5xmlpatterns-devel \
      lib64qt5concurrent-devel \
      lib64qt5opengl-devel \
      lib64qt5printsupport-devel \
      lib64qt5sql-devel \
      lib64qt5svg-devel \
      lib64exiv2-devel \
      lensfun-devel \
      marble-devel \
      opencv-devel \
      lib64kf5sane-devel \
      lib64kf5xmlgui-devel \
      lib64kf5threadweaver-devel \
      lib64qt5webengine-devel \
      lib64kf5kio-devel \
      lib64kf5notifications-devel \
      lib64kf5notifyconfig-devel \
      lib64kf5filemetadata-devel \




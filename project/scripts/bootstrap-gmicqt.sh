#!/bin/bash

# Copyright (c) 2008-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
#
#
# Copy this script on root folder where are source code

# We will work on command line using MinGW compiler
export MAKEFILES_TYPE='Unix Makefiles'

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

cmake -G "$MAKEFILES_TYPE" . \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_PREFIX=/usr \
      -DENABLE_ASAN=OFF \
      -DENABLE_SYSTEM_GMIC=OFF \
      -DGMIC_QT_HOST=digikam \
      -Wno-dev \
      ..


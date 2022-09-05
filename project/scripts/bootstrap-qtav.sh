#!/bin/bash

# Copyright (c) 2008-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
#
#
# Copy this script on root folder where are source code

#export VERBOSE=1

# We will work on command line using MinGW compiler
export MAKEFILES_TYPE='Unix Makefiles'

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

export QtAvOptions='-DBUILD_EXAMPLES=OFF \
                    -DBUILD_PLAYERS=OFF \
                    -DBUILD_TESTS=OFF \
                    -DBUILD_QT5OPENGL=ON \
                    -DBUILD_QML=OFF'

cmake -G "$MAKEFILES_TYPE" . \
      -DCMAKE_INSTALL_PREFIX=/usr \
      -Wno-dev \
      $QtAvOptions \
      ..


#!/bin/bash

# Script to build a Linux Host installation to compile an AppImage bundle of digiKam.
# This script must be run as sudo
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

#################################################################################################
# Manage script traces to log file

mkdir -p ./logs
exec > >(tee ./logs/build-host.full.log) 2>&1

#################################################################################################

echo "01-build-host.sh : build a Linux host installation to compile an AppImage bundle."
echo "---------------------------------------------------------------------------------"

#################################################################################################
# Pre-processing checks

. ./config.sh
. ./common.sh
ChecksRunAsRoot
StartScript
ChecksCPUCores
HostAdjustments
RegisterRemoteServers
CheckSystemReleaseID

ORIG_WD="`pwd`"

if   [[ "$OS_NAME" == "mageia" ]] ; then

    . ./host_mageia.inc

elif [[ "$OS_NAME" == "ubuntu" ]] ; then

    . ./host_ubuntu.inc

fi

# Clean up previous openssl install

rm -fr /usr/local/lib/libssl.a    || true
rm -fr /usr/local/lib/libcrypto.a || true
rm -fr /usr/local/include/openssl || true

#################################################################################################

echo -e "---------- Prepare Linux host to Compile Extra Dependencies\n"

# Make sure we build from the /, parts of this script depends on that. We also need to run as root...
cd /

# Create the build dir for the 3rdparty deps
if [ ! -d $BUILDING_DIR ] ; then
    mkdir -p $BUILDING_DIR
fi

if [ ! -d $DOWNLOAD_DIR ] ; then
    mkdir -p $DOWNLOAD_DIR
fi

if [ ! -d /opt/cmake ] ; then
    mkdir -p /opt/cmake
fi

#################################################################################################

cd $BUILDING_DIR

rm -rf $BUILDING_DIR/* || true

cmake $ORIG_WD/../3rdparty \
      -DCMAKE_INSTALL_PREFIX:PATH=/opt/cmake \
      -DINSTALL_ROOT=/opt/cmake \
      -DENABLE_QTVERSION=$DK_QTVERSION \
      -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
      -DKA_VERSION=$DK_KA_VERSION \
      -DKF5_VERSION=$DK_KF5_VERSION \
      -DENABLE_QTVERSION=$DK_QTVERSION \
      -DENABLE_QTWEBENGINE=$DK_QTWEBENGINE

# Install new cmake recent version to /opt

cmake --build . --config RelWithDebInfo --target ext_cmake        -- -j$CPU_CORES

#################################################################################################

cd $BUILDING_DIR

rm -rf $BUILDING_DIR/* || true

/opt/cmake/bin/cmake $ORIG_WD/../3rdparty \
      -DCMAKE_INSTALL_PREFIX:PATH=/usr \
      -DINSTALL_ROOT=/usr \
      -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
      -DKA_VERSION=$DK_KA_VERSION \
      -DKF5_VERSION=$DK_KF5_VERSION \
      -DENABLE_QTVERSION=$DK_QTVERSION \
      -DENABLE_QTWEBENGINE=$DK_QTWEBENGINE \
      -DQTWEBENGINE_VERSION=$DK_QTWEBENGINEVERSION

# Low level libraries and Qt5 dependencies
# NOTE: The order to compile each component here is very important.

#/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_libicu        -- -j$CPU_CORES
#/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_openssl       -- -j$CPU_CORES

/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_qt            -- -j$CPU_CORES    # depend of tiff, png, jpeg

#cp $DOWNLOAD_DIR/qt_manifest.txt $ORIG_WD/data/

if [[ "$DK_QTVERSION" = "5.15-LTS" && $DK_QTWEBENGINE = 1 ]] ; then

    # Patch QtWebEngine cmake config files to be compatible with Qt5.15 LTS which is versionned as 5.15.3

    sed -e "s/$DK_QTWEBENGINEVERSION ${_Qt5WebEngine_FIND_VERSION_EXACT}/5.15.3 ${_Qt5WebEngine_FIND_VERSION_EXACT}/g" /usr/lib/cmake/Qt5WebEngine/Qt5WebEngineConfig.cmake > ./tmp.cmake ; mv -f ./tmp.cmake /usr/lib/cmake/Qt5WebEngine/Qt5WebEngineConfig.cmake
    sed -e "s/$DK_QTWEBENGINEVERSION ${_Qt5WebEngineCore_FIND_VERSION_EXACT}/5.15.3 ${_Qt5WebEngineCore_FIND_VERSION_EXACT}/g" /usr/lib/cmake/Qt5WebEngineCore/Qt5WebEngineCoreConfig.cmake > ./tmp.cmake ; mv -f ./tmp.cmake /usr/lib/cmake/Qt5WebEngineCore/Qt5WebEngineCoreConfig.cmake
    sed -e "s/$DK_QTWEBENGINEVERSION ${_Qt5WebEngineWidgets_FIND_VERSION_EXACT}/5.15.3 ${_Qt5WebEngineWidgets_FIND_VERSION_EXACT}/g" /usr/lib/cmake/Qt5WebEngineWidgets/Qt5WebEngineWidgetsConfig.cmake > ./tmp.cmake ; mv -f ./tmp.cmake Qt5WebEngineWidgetsConfig.cmake

fi

if [[ $DK_QTWEBENGINE = 0 ]] ; then
    /opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_qtwebkit  -- -j$CPU_CORES    # depend of Qt and libicu
fi

# Clean up previous openssl install

rm -fr /usr/local/lib/libssl.a    || true
rm -fr /usr/local/lib/libcrypto.a || true
rm -fr /usr/local/include/openssl || true

/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_jasper        -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_libde265      -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_libjxl        -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_libaom        -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_libavif       -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_ffmpeg        -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_imagemagick   -- -j$CPU_CORES
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_opencv        -- -j$CPU_CORES

#################################################################################################

TerminateScript

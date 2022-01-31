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
ORIG_WD="`pwd`"

#################################################################################################

echo -e "---------- Update Linux Host\n"

urpmi --auto --auto-update

#################################################################################################

if [[ "$(arch)" = "x86_64" ]] ; then
    LIBSUFFIX=lib64
else
    LIBSUFFIX=lib
fi

echo -e "---------- Install New Development Packages\n"

# Packages for base dependencies and Qt5.
urpmi --auto \
      wget \
      tar \
      bzip2 \
      gettext \
      git \
      subversion \
      libtool \
      which \
      fuse \
      automake \
      cmake \
      ccache \
      gcc-c++ \
      patch \
      libdrm-devel \
      libxcb \
      libxcb-devel \
      xcb-util-keysyms-devel \
      xcb-util-devel \
      libxkbcommon-utils \
      xkeyboard-config \
      xscreensaver \
      gperf \
      ruby \
      bison \
      flex \
      zlib-devel \
      expat-devel \
      fuse-devel \
      glibc-devel \
      mysql-devel \
      eigen3-devel \
      cppunit-devel \
      libstdc++-devel \
      libstdc++-static-devel \
      libxml2-devel \
      lcms2-devel \
      glibc-devel \
      libudev-devel \
      sqlite-devel \
      libexif-devel \
      libxslt-devel \
      xz-devel \
      lz4-devel \
      inotify-tools-devel \
      cups-devel \
      imagemagick \
      openal-soft-devel \
      libical-devel \
      libcap-devel \
      fontconfig-devel \
      freetype-devel \
      patchelf \
      dpkg \
      ninja \
      clang \
      python \
      ruby \
      ruby-devel \
      sqlite3-devel \
      ffmpeg-devel \
      boost-devel \
      gphoto2-devel \
      sane-backends \
      jasper-devel \
      nodejs-devel \
      python3-lxml \
      appstream \
      openssh-askpass \
      ${LIBSUFFIX}openssl-devel \
      ${LIBSUFFIX}nss-devel \
      ${LIBSUFFIX}xkbcommon-devel \
      ${LIBSUFFIX}xkbfile-devel \
      ${LIBSUFFIX}xinerama-devel \
      ${LIBSUFFIX}sane1-devel \
      ${LIBSUFFIX}xcb-util1 \
      ${LIBSUFFIX}xi-devel \
      ${LIBSUFFIX}xtst-devel \
      ${LIBSUFFIX}xrandr-devel \
      ${LIBSUFFIX}xcursor-devel \
      ${LIBSUFFIX}xcomposite-devel \
      ${LIBSUFFIX}xrender-devel \
      ${LIBSUFFIX}xscrnsaver-devel \
      ${LIBSUFFIX}ltdl-devel \
      ${LIBSUFFIX}glib2.0-devel \
      ${LIBSUFFIX}usb1.0-devel \
      ${LIBSUFFIX}jpeg-devel \
      ${LIBSUFFIX}png-devel \
      ${LIBSUFFIX}tiff-devel \
      ${LIBSUFFIX}lqr-devel \
      ${LIBSUFFIX}fftw-devel \
      ${LIBSUFFIX}curl-devel \
      ${LIBSUFFIX}magick-devel \
      ${LIBSUFFIX}wayland-devel \
      ${LIBSUFFIX}clang-devel \
      ${LIBSUFFIX}sm-devel \
      ${LIBSUFFIX}freeglut-devel \
      ${LIBSUFFIX}mesaegl-devel \
      ${LIBSUFFIX}input-devel

if [[ "$DK_QTVERSION" = "5.14" ]] ; then

    urpmi --auto \
          ${LIBSUFFIX}mesagl1-devel \
          ${LIBSUFFIX}mesaglu1-devel \
          ${LIBSUFFIX}mesaegl1-devel \
          ${LIBSUFFIX}mesaegl1

fi

if [[ "$DK_QTVERSION" = "5.15" || "$DK_QTVERSION" = "5.15-LTS" ]] ; then

    urpmi --auto \
          ${LIBSUFFIX}xcb-util-cursor-devel \
          ${LIBSUFFIX}xcb-util-image-devel \
          ${LIBSUFFIX}xcb-util-renderutil-devel \
          ${LIBSUFFIX}xcb-util-wm-devel \
          ${LIBSUFFIX}xcb-xrm-devel

fi

echo -e "---------- Clean-up Old Packages\n"

# Remove system based devel package to prevent conflict with new one.
urpme --auto --force ${LIBSUFFIX}qt5core5 || true

# Clean up previous openssl install

rm -fr /usr/local/lib/libssl.a    || true
rm -fr /usr/local/lib/libcrypto.a || true
rm -fr /usr/local/include/openssl || true

#################################################################################################

echo -e "---------- Prepare Linux host to Compile Extra Dependencies\n"

# Workaround for: On Mageia 6, .pc files in /usr/lib/pkgconfig are not recognized
# However, this is where .pc files get installed when building libraries... (FIXME)
# I found this by comparing the output of librevenge's "make install" command
# between Ubuntu and CentOS 6
ln -sf /usr/share/pkgconfig /usr/lib/pkgconfig

# Make sure we build from the /, parts of this script depends on that. We also need to run as root...
cd /

# Create the build dir for the 3rdparty deps
if [ ! -d $BUILDING_DIR ] ; then
    mkdir $BUILDING_DIR
fi

if [ ! -d $DOWNLOAD_DIR ] ; then
    mkdir $DOWNLOAD_DIR
fi

if [ ! -d /opt/cmake ] ; then
    mkdir /opt/cmake
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
/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_openssl       -- -j$CPU_CORES

/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_qt            -- -j$CPU_CORES    # depend of tiff, png, jpeg

cp $DOWNLOAD_DIR/qt_manifest.txt $ORIG_WD/data/

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

/opt/cmake/bin/cmake --build . --config RelWithDebInfo --target ext_opencv        -- -j$CPU_CORES

#################################################################################################

TerminateScript

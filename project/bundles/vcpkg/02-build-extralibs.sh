#!/bin/bash

# Script to build extra libraries using VCPKG.
#
# SPDX-FileCopyrightText: 2015-2023 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

#################################################################################################
# Manage script traces to log file

mkdir -p ./logs
exec > >(tee ./logs/build-extralibs.full.log) 2>&1

#################################################################################################

echo "02-build-extralibs.sh : build extra libraries."
echo "----------------------------------------------"

#################################################################################################
# Pre-processing checks

. ./common.sh
. ./config.sh
StartScript
ChecksCPUCores
RegisterRemoteServers

ORIG_WD="`pwd`"

export PATH=$PATH:/c/bison:/c/icoutils/bin:$INSTALL_DIR/$VCPKG_TRIPLET/tools/gperf:$INSTALL_DIR/$VCPKG_TRIPLET/tools/curl:$INSTALL_DIR/$VCPKG_TRIPLET/tools/python3:$INSTALL_DIR/$VCPKG_TRIPLET/tools/pkgconf:$INSTALL_DIR/$VCPKG_TRIPLET/bin:$INSTALL_DIR/$VCPKG_TRIPLET/tools/Qt6/bin
echo "PATH=$PATH"

#################################################################################################

if [ ! -d $BUILDING_DIR/dk_cmake ] ; then
    mkdir -p $BUILDING_DIR/dk_cmake
fi

cd $BUILDING_DIR/dk_cmake

rm -rf $BUILDING_DIR/dk_cmake/* || true

cmake $ORIG_WD/../3rdparty \
      -DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake \
      -DVCPKG_TARGET_TRIPLET=$VCPKG_TRIPLET \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_COLOR_MAKEFILE=ON \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/$VCPKG_TRIPLET \
      -DINSTALL_ROOT=$INSTALL_DIR/$VCPKG_TRIPLET \
      -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
      -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOAD_DIR \
      -DKA_VERSION=$DK_KA_VERSION \
      -DKP_VERSION=$DK_KP_VERSION \
      -DKDE_VERSION=$DK_KDE_VERSION \
      -DENABLE_QTVERSION=$DK_QTVERSION \
      -DENABLE_QTWEBENGINE=$DK_QTWEBENGINE \
      -Wno-dev

# NOTE: The order to compile each component here is very important.

# core KDE frameworks dependencies
cmake --build . --parallel --config RelWithDebInfo --target ext_extra-cmake-modules
cmake --build . --parallel --config RelWithDebInfo --target ext_kconfig
cmake --build . --parallel --config RelWithDebInfo --target ext_breeze-icons
cmake --build . --parallel --config RelWithDebInfo --target ext_kcoreaddons
cmake --build . --parallel --config RelWithDebInfo --target ext_kwindowsystem
cmake --build . --parallel --config RelWithDebInfo --target ext_solid
cmake --build . --parallel --config RelWithDebInfo --target ext_threadweaver
cmake --build . --parallel --config RelWithDebInfo --target ext_karchive
cmake --build . --parallel --config RelWithDebInfo --target ext_kdbusaddons
cmake --build . --parallel --config RelWithDebInfo --target ext_ki18n
cmake --build . --parallel --config RelWithDebInfo --target ext_kcrash
cmake --build . --parallel --config RelWithDebInfo --target ext_kcodecs
cmake --build . --parallel --config RelWithDebInfo --target ext_kguiaddons
cmake --build . --parallel --config RelWithDebInfo --target ext_kwidgetsaddons
cmake --build . --parallel --config RelWithDebInfo --target ext_kitemviews
cmake --build . --parallel --config RelWithDebInfo --target ext_kcompletion

if [[ $DK_QTVERSION == 6 ]] ; then

    cmake --build . --parallel --config RelWithDebInfo --target ext_kcolorscheme

fi

cmake --build . --parallel --config RelWithDebInfo --target ext_kconfigwidgets
cmake --build . --parallel --config RelWithDebInfo --target ext_kiconthemes
cmake --build . --parallel --config RelWithDebInfo --target ext_kservice
cmake --build . --parallel --config RelWithDebInfo --target ext_kxmlgui
cmake --build . --parallel --config RelWithDebInfo --target ext_kbookmarks
cmake --build . --parallel --config RelWithDebInfo --target ext_kimageformats

# Extra support for digiKam

# Desktop integration support
cmake --build . --parallel --config RelWithDebInfo --target ext_knotifications
cmake --build . --parallel --config RelWithDebInfo --target ext_kjobwidgets
cmake --build . --parallel --config RelWithDebInfo --target ext_kio
cmake --build . --parallel --config RelWithDebInfo --target ext_knotifyconfig

# libksane support
cmake --build . --parallel --config RelWithDebInfo --target ext_sonnet
cmake --build . --parallel --config RelWithDebInfo --target ext_ktextwidgets

# Geolocation support
cmake --build . --parallel --config RelWithDebInfo --target ext_marble

# Calendar support
cmake --build . --parallel --config RelWithDebInfo --target ext_kcalendarcore

# Breeze style support
cmake --build . --parallel --config RelWithDebInfo --target ext_breeze

# Marble install shared lib at wrong place.
mv $INSTALL_DIR/$VCPKG_TRIPLET/libmarble* $INSTALL_DIR/$VCPKG_TRIPLET/bin || true
mv $INSTALL_DIR/$VCPKG_TRIPLET/libastro*  $INSTALL_DIR/$VCPKG_TRIPLET/bin || true

#################################################################################################

if [[ $DK_QTVERSION == 6 ]] ; then

    KF6_GITREV_LST=$ORIG_WD/data/kf6_manifest.txt

    echo "Populate git sub-module revisions in $KF6_GITREV_LST"

    if [ -f $KF6_GITREV_LST ] ; then
        rm -f $KF6_GITREV_LST
    fi

    currentDate=`date +"%Y-%m-%d"`
    echo "+KF6 Snapshot $currentDate" > $KF6_GITREV_LST

    # --- List git revisions for all sub-modules

    DIRS=$(find $BUILDING_DIR/dk_cmake/ext_kf6/ -name "ext_*-prefix")

    for ITEM in $DIRS ; do

        BASE=$(basename $ITEM | awk -F'_' '{print $2}')
		COMPONENT=${BASE%-prefix}
        SUBDIR=$ITEM/src/ext_$COMPONENT

		if [[ -f "$SUBDIR/.git" ]] ; then 
			echo "Parsed dir: $SUBDIR"
			cd $SUBDIR
			echo "$BASE:$(git rev-parse HEAD)" >> $KF6_GITREV_LST
			cd $ORIG_WD
		fi

    done

    cat $KF6_GITREV_LST

fi

TerminateScript

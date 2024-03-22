#!/bin/bash

# SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run Clazy analyzer on whole digiKam source code.
# https://github.com/KDE/clazy
# Dependencies : Python BeautifulSoup and SoupSieve at run-time.
#
# If '--webupdate' is not passed as argument, static analyzer results are just created locally.
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

. ./common.sh

# Skip directories from the analysis.
. ../../.scan-build

# Check run-time dependencies

if [ ! -f /usr/bin/clazy ] ; then

    echo "Clazy static analyzer is not installed in /opt/clazy."
    echo "Please install Clazy from https://github.com/KDE/clazy"
    echo "Aborted..."
    exit -1


else

    echo "Check Clazy static analyzer passed..."

fi

checksCPUCores

ORIG_WD="`pwd`"
REPORT_DIR="${ORIG_WD}/report.clazy"
WEBSITE_DIR="${ORIG_WD}/site"

# Get active git branches to create report description string
TITLE="digiKam-$(parseGitBranch)$(parseGitHash)"
echo "Clazy Static Analyzer task name: $TITLE"

# Print the skipped directories taken from the config file.

for DROP_ITEM in $IGNORE_DIRS ; do

    if [[ $DROP_ITEM != *exclude ]] ; then

        echo "Skipped dir: $DROP_ITEM"

    fi

done

export CLAZY_IGNORE_DIRS=$IGNORE_DIRS

export CLAZY_CHECKS="\
level2,\
no-fully-qualified-moc-types,\
no-qproperty-without-notify,\
no-old-style-connect,\
no-rule-of-three,\
no-inefficient-qlist-soft,\
no-qstring-allocations,\
no-qstring-arg,\
no-qstring-insensitive-allocation,\
no-qstring-ref,\
no-function-args-by-value,\
qt6-qhash-signature,\
qt6-deprecated-api-fixes,\
qt6-fwd-fixes,\
qt6-header-fixes,\
unexpected-flag-enumerator-value,\
signal-with-return-value,\
raw-environment-function,\
qt-keywords,\
missing-qobject-macro\
"

#qt6-qlatin1stringchar-to-u         What is that exactly ???

echo "CHECKERS CONFIGURATION: $CLAZY_CHECKS"

# Clean up and prepare to scan.

rm -fr $REPORT_DIR
rm -fr $WEBSITE_DIR
mkdir -p $REPORT_DIR

cd ../..

rm -fr build.clazy
mkdir -p build.clazy
cd build.clazy

if [[ -d /opt/qt6 ]] ; then

    export BUILD_WITH_QT6=1
    export Qt6_DIR=/opt/qt6
    QTPATHS="/opt/qt6/bin/qtpaths6"
    export CMAKE_BINARY=/opt/qt6/bin/cmake

else

    export BUILD_WITH_QT6=0
    QTPATHS="qtpaths"
    export CMAKE_BINARY=cmake

fi

$CMAKE_BINARY -G "Unix Makefiles" . \
      -DCMAKE_BUILD_TYPE=debug \
      -DCMAKE_CXX_COMPILER=clazy \
      -DBUILD_WITH_QT6=$BUILD_WITH_QT6 \
      -DBUILD_TESTING=ON \
      -DDIGIKAMSC_CHECKOUT_PO=OFF \
      -DDIGIKAMSC_CHECKOUT_DOC=OFF \
      -DDIGIKAMSC_COMPILE_PO=OFF \
      -DDIGIKAMSC_COMPILE_DOC=OFF \
      -DENABLE_KFILEMETADATASUPPORT=ON \
      -DENABLE_AKONADICONTACTSUPPORT=ON \
      -DENABLE_MYSQLSUPPORT=ON \
      -DENABLE_INTERNALMYSQL=ON \
      -DENABLE_MEDIAPLAYER=ON \
      -DENABLE_QTMULTIMEDIA=ON \
      -DENABLE_DBUS=ON \
      -DENABLE_APPSTYLES=ON \
      -DENABLE_GEOLOCATION=ON \
      -DENABLE_QWEBENGINE=ON \
      -Wno-dev \
      ..

make -j$CPU_CORES 2> ${REPORT_DIR}/trace.log

cd $ORIG_WD

python3 ./clazy_visualizer.py $REPORT_DIR/trace.log

rm -f $REPORT_DIR/trace.log
mv clazy.html $REPORT_DIR/index.html

if [[ $1 == "--webupdate" ]] ; then

    # update www.digikam.org report section.
    updateReportToWebsite "clazy" $REPORT_DIR $TITLE $(parseGitBranch)

fi

cd $ORIG_DIR

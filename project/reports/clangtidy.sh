#!/bin/bash

# Copyright (c) 2013-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run Clang tidy lint static analyzer on whole digiKam source code.
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

. ./common.sh

checksCPUCores

ORIG_WD="`pwd`"
REPORT_DIR="${ORIG_WD}/report.tidy"
WEBSITE_DIR="${ORIG_WD}/site"

# Get active git branches to create report description string
TITLE="digiKam-$(parseGitBranch)$(parseGitHash)"
echo "Clang Tidy Static Analyzer task name: $TITLE"

# Clean up and prepare to scan.

rm -fr $REPORT_DIR
rm -fr $WEBSITE_DIR

mkdir -p $REPORT_DIR

/usr/share/clang/run-clang-tidy.py -quiet -j$CPU_CORES -p  ../../build/ | tee $REPORT_DIR/clang-tidy.log

./clang-visualizer.py $REPORT_DIR/clang-tidy.log

rm -f $REPORT_DIR/clang-tidy.log

updateReportToWebsite "tidy" $REPORT_DIR $TITLE $(parseGitBranch)

cd $ORIG_DIR


#!/bin/bash

# Copyright (c) 2013-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run Krazy static analyzer on whole digiKam source code.
# https://github.com/Krazy-collection/krazy
# Dependencies:
#  - Perl:Tie::IxHash and Perl:XML::LibXML modules at run-time.
#  - Saxon 9HE java xml parser (saxon.jar) to export report as HTML [https://www.saxonica.com/download/java.xml].
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

. ./common.sh

checksCPUCores

# Check run-time dependencies

if [ ! -f /opt/saxon/saxon9he.jar ] ; then

    echo "Java Saxon 9HE XML parser is not installed in /opt/saxon."
    echo "Please install Saxon from https://www.saxonica.com/download/java.xml"
    echo "Aborted..."
    exit -1


else

    echo "Check Java Saxon 9HE XML parser  passed..."

fi

if [ ! -f /opt/krazy/bin/krazy2all ] ; then

    echo "Krazy Static analyzer is not installed in /opt/krazy."
    echo "Please install Krazy from https://github.com/Krazy-collection/krazy"
    echo "Aborted..."
    exit -1

else

    echo "Check Krazy static analyzer passed..."

fi

export PATH=$PATH:/opt/krazy/bin

ORIG_WD="`pwd`"
REPORT_DIR="${ORIG_WD}/report.krazy"
WEBSITE_DIR="${ORIG_WD}/site"

# Get active git branches to create report description string
TITLE="digiKam-$(parseGitBranch)$(parseGitHash)"
echo "Krazy Static Analyzer task name: $TITLE"

rm -fr $REPORT_DIR
rm -fr $WEBSITE_DIR

# Compute static analyzer output as XML
TITLE_EXT=$TITLE+"
This is the static analyzis processed with Extra checkers.
See <a href=\"http://ebn.kde.org/krazy/reports/extragear/graphics/digikam/\">EBN</a> for standard analyzis."

krazy2all --export xml \
          --title $TITLE \
          --no-brief \
          --strict all \
          --priority all \
          --verbose \
          --topdir ../../ \
          --outfile ./report.krazy.xml

# Clean up XML file

sed -i "s/repo-rev value=\"unknown\"/repo-rev value=\"$(parseGitBranch)$(parseGitHash)\"/g" ./report.krazy.xml

DROP_PATH=$(echo $ORIG_WD | rev | cut -d'/' -f3- | rev | sed 's_/_\\/_g')
sed -i "s/$DROP_PATH//g" ./report.krazy.xml

mkdir -p $REPORT_DIR

# Process XML file to generate HTML

java -jar /opt/saxon/saxon9he.jar \
     -o:$REPORT_DIR/index.html \
     -im:krazy2ebn \
     ./report.krazy.xml \
     ./krazy/krazy-main.xsl \
     module=graphics \
     submodule=digikam \
     component=extragear

cp ./krazy/style.css $REPORT_DIR/

# update www.digikam.org report section.
updateReportToWebsite "krazy" $REPORT_DIR $TITLE $(parseGitBranch)

cd $ORIG_DIR

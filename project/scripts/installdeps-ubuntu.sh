#!/bin/bash

# Script to install dependencies under Ubuntu Linux to compile digiKam.
# This script must be run as sudo
# *Ubuntu* compatible version >= 18.04
#
# Copyright (c) 2021      by TRAN Quoc Hung <quochungtran1999 at gmail dot com>
# Copyright (c) 2021      by Surya K M      <suryakm_is20 dot rvitm@rvei dot edu dot in>
# Copyright (c) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Check OS name and version.

OS_NAME=$(awk '/DISTRIB_ID=/' /etc/*-release | sed 's/DISTRIB_ID=//' | sed 's/\"//' | sed 's/\"//' | tr '[:upper:]' '[:lower:]')
OS_ARCH=$(uname -m | sed 's/x86_//;s/i[3-6]86/32/')
OS_VERSION=$(awk '/DISTRIB_RELEASE=/' /etc/*-release | sed 's/DISTRIB_RELEASE=//' | sed 's/[.]0/./')

echo $OS_NAME
echo $OS_ARCH
echo $OS_VERSION

if [[ "$OS_NAME" != "ubuntu" ]] ; then
    echo "Not running Linux ubuntu..."
    exit -1
fi

echo "Check the list of dependencies in the online doc API : https://www.digikam.org/api/index.html#externaldeps"
echo "-------------------------------------------------------------------"

# for downloading package information from all configured sources.'

sudo apt-get update
sudo apt-get upgrade

# benefit from a higher version of certain software , update the key

sudo apt-key adv --refresh-keys --keyserver keyserver.ubuntu.com
sudo add-apt-repository "deb http://security.ubuntu.com/ubuntu xenial-security main"

# To fix GPP key error with some reporsitories
# See: https://www.skyminds.net/linux-resoudre-les-erreurs-communes-de-cle-gpg-dans-apt/

sudo apt-get update 2>&1 | \
    sed -ne 's?^.*NO_PUBKEY ??p' | \
    xargs -r -- sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys

# Install dependencies to Checkout Source Code

sudo apt-get install -y git

echo "-------------------------------------------------------------------"

sudo apt-get install -y perl

echo "-------------------------------------------------------------------"

# Install required dependencies to Compile And Link Source Code

required_packages=("cmake"                   # To Compile Source Code
                   "ninja-build"             # To Compile Source Code
                   "extra-cmake-modules"     # To Compile Source Code
                   "build-essential"         # To Compile Source Code
                   "qttools5-dev-tools"      # To handle Qt5 configuration.

                   "libqt5core5a"            # Qt 5 core module
                   "libqt5gui5"              # Qt 5 Gui module
                   "libqt5widgets5"          # Qt 5 widget module
                   "libqt5network5"          # Qt 5 network module
                   "libqt5sql5"              # Qt 5 SQL module
                   "libqt5xml5"              # Qt 5 XML module
                   "libqt5concurrent5"       # Qt 5 Concurrent module
                   "libqt5printsupport5"     # Qt 5 PrintSupport module
                   "libqt5svg5"              # Qt 5 Svg module
                   "libqt5webengine5"        # Qt 5 webengine module
                   "libqt5networkauth5-dev"  # Qt 5 network authentification.

                   "libkf5config-dev"        # Configuration settings framework for Qt
                   "libkf5xmlgui-dev"        # User configurable main windows
                   "libkf5i18n-dev"          # Advanced internationalization framework
                   "libkf5windowsystem-dev"  # Development files for kwindowsystem
                   "libkf5service-dev"       # Development files for kservice
                   "libkf5solid-dev"         # Qt library to query and control hardware
                   "libkf5coreaddons-dev"    # Development files - KDE Frameworks 5 addons to QtCore

                   "libopencv-dev"           # Development files for opencv
                   "libpthread-stubs0-dev"   # Development files for pthread
                   "libtiff-dev"             # Tag image file format library
                   "libpng-dev"              # PNG library
                   "libjpeg-dev"             # JPEG library
                   "libboost-all-dev"        # Boots C++ Libraries
                   "liblcms2-dev"            # Little CMS 2 color management library
                   "libexpat1-dev"           # XML parsing C library
                   "libexiv2-dev"            # Metadata manipulation library
                    )


for pkg in ${required_packages[@]}; do

    sudo apt-get install -y ${pkg}

    current_version=$(dpkg-query --showformat='${Version}' --show ${pkg})

    case "${pkg}" in
    "cmake")
        required_version=3.3.2
        ;;
    "extra-cmake-modules")
        required_version=5.5.0
        ;;
    "build-essential")
        required_version=7.2.0
        ;;
    "libqt5core5a")
        required_version=5.9.0
        ;;
    "libkf5config-dev")
        required_version=5.5.0
        ;;
    "libopencv-dev")
        required_version=3.3.0
        ;;
    "libpthread-stubs0-dev")
        required_version=2.0.0
        ;;
    "libtiff-dev")
        required_version=4.0.0
        ;;
    "libpng-dev")
        required_version=1.6.0
        ;;
    "libjpeg-dev")
        required_version=6b
        ;;
    "libboost-all-dev")
        required_version=1.55.0
        ;;
    "liblcms2-dev")
        required_version=2.0.0
        ;;
    "libexpat1-dev")
        required_version=2.1.0
        ;;
    "libexiv2-dev")
        required_version=0.27.0
        ;;
    esac

    echo $current_version

    if $(dpkg --compare-versions "$current_version" "lt" "$required_version"); then
            echo "less than $required_version";
            echo "please upgrade newer version or another packages";
    else
            echo "greater than $required_version ............. accepted";
    fi

    echo "-------------------------------------------------------------------"

done

# Install optional dependencies to Compile And Link Source Code

optional_packages=("ruby"                               # For i18n extraction
                   "subversion"                         # For i18n extraction
                   "hunspell"                           # For check spelling
                   "valgrind"                           # For debuging
                   "keychain"                           # For git-ssh
                   "ssh-askpass"                        # For git-ssh
                   "lzip"                               # For CI/CD
                   "gzip"                               # For CI/CD
                   "unzip"                              # For CI/CD
                   "unrar"                              # For CI/CD
                   "gperf"                              # For MXE build
                   "intltool"                           # For MXE build
                   "scons"                              # For MXE build
                   "icoutils"                           # For MXE build
                   "gcc-mingw-w64"                      # For MXE build
                   "python3-mako"                       # For MXE build
                   "python3-pygments"                   # For Cppcheck static analysis
                   "python3-bs4"                        # For Clazy static analysis
                   "python3-soupsieve"                  # For Clazy static analisys
                   "cppcheck"                           # For static analysis
                   "clang"                              # For static analysis
                   "clang-tidy"                         # For static analysis
                   "clang-tools"                        # For static analysis
                   "libasan4"                           # For static analysis
                   "openjdk-18-jre"                     # For static analysis
                   "libsaxonhe-java"                    # For static analysis
                   "libxml-perl"                        # For static analysis
                   "libxml-libxml-perl"                 # For static analysis
                   "libyaml-libyaml-perl"               # For static analysis
                   "libjson-perl"                       # For static analysis
                   "llvm"                               # For static analysis
                   "clazy"                              # For static analysis
                   "libclang-dev"                       # For static analysis
                   "doxygen"                            # For API doc
                   "graphviz"                           # For API doc
                   "kate"                               # For debuging
                   "ktexteditor-katepart"               # For debuging
                   "ccache"                             # For compiling
                   "hugin"                              # For run-time processing
                   "exiftool"                           # For metadata processing
                   "bison"                              # For Qt build (>= 2.5.0)
                   "flex"                               # For compiling (>= 2.5.0)
                   "curl"                               # For CI/CD
                   "wget"                               # For CI/CD
                   "coreutils"                          # For CI/CD
                   "dmg2img"                            # For CI/CD (MacOS)
                   "tesseract-ocr"                      # For CI/CD (MacOS)
                   "libeigen3-dev"                      # >= 3.2
                   "liblensfun-dev"                     # >= 0.2.8
                   "libmarble-dev"                      # >= 0.22
                   "marble-data"                        # >= 5.0
                   "libgomp1"                           # For Libraw compilation
                   "libavdevice-dev"                    # >= 3.3.x
                   "libavfilter-dev"                    # >= 3.3.x
                   "libavformat-dev"                    # >= 3.3.x
                   "libavcodec-dev"                     # >= 3.3.x
                   "libavutils-dev"                     # >= 3.3.x
                   "libpostproc-dev"                    # >= 3.3.x
                   "libgphoto2-dev"                     # >= 2.5
                   "libsane-dev"                        # >= 5.0.0
                   "libjasper-dev"                      # >= 1.900.1
                   "libheif-dev"
                   "libxslt-dev"                        # >= 1.1.0
                   "libxml2-dev"                        # >= 2.7.0
                   "libtiff-dev"                        # >= 4.0
                   "liblqr-dev"                         # >= 0.4.2
                   "libfftw3-dev"                       # For GMic-Qt compilation
                   "libx265-dev"                        # >= 2.2
                   "libmagick++-dev"                    # >= 6.7.0
                   "libqt5x11extras5-dev"               # >= 5.9
                   "libqt5test5"                        # >= 5.9
                   "libqt5xml5"                         # >= 5.9
                   "qttools5-dev"                       # >= 5.9
                   "libqt5xmlpatterns5-dev"             # >= 5.9
                   "libqt5opengl5-dev"                  # >= 5.9
                   "libkf5sane-dev"                     # >= 5.5.0
                   "libkf5threadweaver-dev"             # >= 5.5.0
                   "libkf5kio-dev"                      # >= 5.5.0
                   "libkf5sonnet-dev"                   # >= 5.5.0
                   "libkf5notifications-dev"            # >= 5.5.0
                   "libkf5notifyconfig-dev"             # >= 5.5.0
                   "libkf5filemetadata-dev"             # >= 5.5.0
                   "libkf5calendarcore-dev"             # >= 5.5.0
                   "libkf5akonadicontact-dev"           # >= 4.89.0
                   "libkf5doctools-dev"
                   "libkf5filemetadata-dev"             # Files indexer engine for Plasma desktop
                    #TODO add new optional packages
                    )


for pkg in ${optional_packages[@]}; do
    sudo apt-get install -y ${pkg}
    echo "-------------------------------------------------------------------"
done

# Add symbolic links for Krazy static analyzer

sudo ln -sf /usr/share/java              /opt/saxon
sudo ln -sf /usr/share/java/Saxon-HE.jar /usr/share/java/saxon9he.jar

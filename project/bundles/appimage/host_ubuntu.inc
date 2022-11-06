#!/bin/bash

# Rules to prepare Ubuntu Linux host.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

#################################################################################################

echo -e "---------- Update Linux Ubuntu Host\n"

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

echo -e "---------- Install New Development Packages\n"

# Install dependencies to Checkout Source Code

sudo apt-get install -y git
sudo apt-get install -y perl

required_packages=("cmake"                   # To Compile Source Code
                   "ninja-build"             # To Compile Source Code
                   "build-essential"         # To Compile Source Code
                   "libpthread-stubs0-dev"   # Development files for pthread
                   "libtiff-dev"             # Tag image file format library
                   "libpng-dev"              # PNG library
                   "libjpeg-dev"             # JPEG library
                   "liblcms2-dev"            # Little CMS 2 color management library
                   "libexpat1-dev"           # XML parsing C library
                   "libssl-dev"              # For MXE build
                    )


for pkg in ${required_packages[@]}; do

    sudo apt-get install -y ${pkg}

    current_version=$(dpkg-query --showformat='${Version}' --show ${pkg})

    case "${pkg}" in
    "cmake")
        required_version=3.3.2
        ;;
    "build-essential")
        required_version=11.0.0
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
    "liblcms2-dev")
        required_version=2.0.0
        ;;
    "libexpat1-dev")
        required_version=2.1.0
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
                   "keychain"                           # For git-ssh
                   "ssh-askpass"                        # For git-ssh
                   "ccache"                             # For compiling
                   "bison"                              # For Qt build (>= 2.5.0)
                   "flex"                               # For compiling (>= 2.5.0)
                   "mariadb-server"                     # Run-time: mysql internal server init
                   "libeigen3-dev"                      # >= 3.2
                   "libgomp1"                           # For Libraw compilation
                   "libjasper-dev"                      # >= 1.900.1
                   "libxslt-dev"                        # >= 1.1.0
                   "libxml2-dev"                        # >= 2.7.0
                   "liblqr-dev"                         # >= 0.4.2
                   "libfftw3-dev"                       # For GMic-Qt compilation
                   "libx265-dev"                        # >= 2.2
                   "wget"
                   "tar"
                   "bzip2"
                   "gettext"
                   "libtool"
                   "fuse"
                   "automake"
                   "ccache"
                   "yasm"
                   "patch"
                   "libdrm-dev"
                   "libxcb1-dev"
                   "libxcb-util0-dev"
                   "libxcb-keysyms1-dev"
                   "libx11-keyboard-perl"
                   "xscreensaver"
                   "gperf"
                   "zlib1g-dev"
                   "libfuse-dev"
                   "libc6-dev"
                   "libmariadb-dev"
                   "libcppunit-dev"
                   "libc++-dev"
                   "libudev-dev"
                   "libsqlite3-dev"
                   "libexif-dev"
                   "liblzma-dev"
                   "liblz-dev"
                   "libinotifytools0-dev"
                   "libcups2-dev"
                   "libopenal-dev"
                   "libpulse-dev"
                   "libical-dev"
                   "libcap-dev"
                   "libfontconfig1-dev"
                   "patchelf"
                   "dpkg"
                   "clang"
                   "python"
                   "libboost-all-dev"
                   "libgphoto2-dev"
                   "libsane-dev"
                   "nodejs-dev"
                   "python3-lxml"
                   "appstream"
                   "libnss3-dev"
                   "libxkbcommon-dev"
                   "libxkbfile-dev"
                   "libxinerama-dev"
                   "libxcb-util-dev"
                   "libxcb-xrm-dev"
                   "libxi-dev"
                   "libxtst-dev"
                   "libxrandr-dev"
                   "libxcursor-dev"
                   "libxcomposite-dev"
                   "libxrender-dev"
                   "libltdl-dev"
                   "libglib2.0-dev"
                   "libusb-1.0-0-dev"
                   "libcurl4-openssl-dev"
                   "libwayland-dev"
                   "libclang-dev"
                   "libsm-dev"
                   "freeglut3-dev"
                   "libinput-dev"
                   "libfdk-aac-dev"
                   "libx264-dev"
                   "libx265-dev"
                   "libxvidcore-dev"
                   "libvpx-dev"
                   "libtheora-dev"
                   "libvorbis-dev"
                   "libopencore-amrnb-dev"
                   "librtmp-dev"
                   "libopus-dev"
                   "libspeex-dev"
                   "libmp3lame-dev"
                   "libfreetype6-dev"
                   "libass-dev"
                   "libegl1-mesa-dev"
                   "libgl1-mesa-dev"
                   "libgles2-mesa-dev"
                   "libglu1-mesa-dev"
                   "libxcb-image0-dev"
                   "libxcb-cursor-dev"
                   "libxcb-render-util0-dev"
                   "libxcb-xrm-dev"
                   ""
                   ""
                   ""
                  )


for pkg in ${optional_packages[@]}; do
    sudo apt-get install -y ${pkg}
    echo "-------------------------------------------------------------------"
done

echo -e "---------- Clean-up Old Packages\n"

# Remove system based devel package to prevent conflict with new one.
sudo apt remove -y libqt5core5a || true

return

# Packages for base dependencies and Qt5.
urpmi --auto \
#      wget \
#      tar \
#      bzip2 \
#      gettext \
#      git \
#      subversion \
#      libtool \
#      which \
#      fuse \
#      automake \
#      cmake \
#      ccache \
#      gcc-c++ \
#      yasm \
#      patch \
#      libdrm-devel \
#      libxcb \
#      libxcb-devel \
#      xcb-util-keysyms-devel \
#      xcb-util-devel \
#      xkeyboard-config \
#      xscreensaver \
#      gperf \
#      ruby \
#      bison \
#      flex \
#      zlib-devel \
#      expat-devel \
#      fuse-devel \
#      glibc-devel \
#      mysql-devel \
#      eigen3-devel \
#      cppunit-devel \
#      libstdc++-devel \
#      libstdc++-static-devel \
#      libxml2-devel \
#      lcms2-devel \
#      libudev-devel \
#      sqlite-devel \
#      libexif-devel \
#      libxslt-devel \
#      xz-devel \
#      lz4-devel \
#      inotify-tools-devel \
#      cups-devel \
#      openal-soft-devel \
#      lib64pulseaudio-devel \
#      libical-devel \
#      libcap-devel \
#      fontconfig-devel \
#      patchelf \
#      dpkg \
#      ninja \
#      clang \
      cdialog \
#      python \
#      ruby \
#      ruby-devel \
#      boost-devel \
#      gphoto2-devel \
#      sane-backends \
#      nodejs-devel \
#      python3-lxml \
#      perl-YAML-PP \
#      perl-YAML-Syck \
#      appstream \
#      openssh-askpass \
#      ${LIBSUFFIX}openssl-devel \
#      ${LIBSUFFIX}nss-devel \
#      ${LIBSUFFIX}xkbcommon-devel \
#      ${LIBSUFFIX}xkbfile-devel \
#      ${LIBSUFFIX}xinerama-devel \
#      ${LIBSUFFIX}xcb-util1 \
      ${LIBSUFFIX}xcb-util-cursor-devel \
      ${LIBSUFFIX}xcb-util-wm-devel \
#      ${LIBSUFFIX}xcb-xrm-devel \
#      ${LIBSUFFIX}xi-devel \
#      ${LIBSUFFIX}xtst-devel \
#      ${LIBSUFFIX}xrandr-devel \
#      ${LIBSUFFIX}xcursor-devel \
#      ${LIBSUFFIX}xcomposite-devel \
#      ${LIBSUFFIX}xrender-devel \
      ${LIBSUFFIX}xscrnsaver-devel \
#      ${LIBSUFFIX}ltdl-devel \
#      ${LIBSUFFIX}glib2.0-devel \
#      ${LIBSUFFIX}usb1.0-devel \
#      ${LIBSUFFIX}jpeg-devel \
#      ${LIBSUFFIX}jasper-devel \
#      ${LIBSUFFIX}png-devel \
#      ${LIBSUFFIX}tiff-devel \
#      ${LIBSUFFIX}lqr-devel \
#      ${LIBSUFFIX}fftw-devel \
#      ${LIBSUFFIX}curl-devel \
#      ${LIBSUFFIX}wayland-devel \
#      ${LIBSUFFIX}clang-devel \
#      ${LIBSUFFIX}sm-devel \
#      ${LIBSUFFIX}freeglut-devel \
#      ${LIBSUFFIX}input-devel \
#      ${LIBSUFFIX}fdk-aac-devel \
#      ${LIBSUFFIX}x264-devel \
#      ${LIBSUFFIX}x265-devel \
#      ${LIBSUFFIX}xvidcore-devel \
#      ${LIBSUFFIX}vpx-devel \
#      ${LIBSUFFIX}theora-devel \
#      ${LIBSUFFIX}vorbis-devel \
#      ${LIBSUFFIX}opencore-amr-devel \
#      ${LIBSUFFIX}rtmp-devel \
#      ${LIBSUFFIX}opus-devel \
#      ${LIBSUFFIX}speex-devel \
#      ${LIBSUFFIX}mp3lame-devel \
#      ${LIBSUFFIX}freetype2-devel \
#      ${LIBSUFFIX}ass-devel


if [[ "$DK_QTVERSION" = "5.14" ]] ; then

    urpmi --auto \
#          ${LIBSUFFIX}mesagl1-devel \
#          ${LIBSUFFIX}mesaglu1-devel \
#          ${LIBSUFFIX}mesaegl1-devel \
#          ${LIBSUFFIX}mesaegl1

fi

if [[ "$DK_QTVERSION" = "5.15" || "$DK_QTVERSION" = "5.15-LTS" ]] ; then

    urpmi --auto \
#          ${LIBSUFFIX}xcb-util-cursor-devel \
#          ${LIBSUFFIX}xcb-util-image-devel \
#          ${LIBSUFFIX}xcb-util-renderutil-devel \
          ${LIBSUFFIX}xcb-util-wm-devel \
#          ${LIBSUFFIX}xcb-xrm-devel

fi

if [[ $DK_QTVERSION == 6.* ]] ; then

    urpmi --auto \
          libxkbcommon-utils \
          ${LIBSUFFIX}mesaegl-devel

fi


# Workaround for: On Mageia 6, .pc files in /usr/lib/pkgconfig are not recognized
# However, this is where .pc files get installed when building libraries... (FIXME)
# I found this by comparing the output of librevenge's "make install" command
# between Ubuntu and CentOS 6
ln -sf /usr/share/pkgconfig /usr/lib/pkgconfig

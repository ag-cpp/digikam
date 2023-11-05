#! /bin/bash

# Script to bundle data using previously-built KDE and digiKam installation
# and create a Windows installer file with NSIS application
# Dependency : NSIS makensis program for Linux.
#
# SPDX-FileCopyrightText: 2015-2023 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

export LANG=C

#################################################################################################
# Manage script traces to log file

mkdir -p ./logs
exec > >(tee ./logs/build-installer.full.log) 2>&1

#################################################################################################

echo "04-build-installer.sh : build digiKam Windows installer."
echo "--------------------------------------------------------"

#################################################################################################
# Pre-processing checks

. ./config.sh
. ./common.sh
StartScript
ChecksCPUCores
RegisterRemoteServers

#################################################################################################
# Check if NSIS CLI tools is installed

if ! which "/c/Program Files (x86)/NSIS/Bin/makensis" ; then
    echo "NSIS CLI tool is not installed"
    echo "See https://nsis.sourceforge.io for details."
    exit 1
else
    echo "Check NSIS CLI tools passed..."
fi

#################################################################################################
# Configurations

export PATH=$PATH:/c/bison:/c/icoutils/bin:$INSTALL_DIR/$VCPKG_TRIPLET/tools/gperf:$INSTALL_DIR/$VCPKG_TRIPLET/tools/python3:$INSTALL_DIR/$VCPKG_TRIPLET/bin
echo "PATH=$PATH"

# Directory where this script is located (default - current directory)
BUILDDIR="$PWD"

# Directory where bundle files are located
BUNDLEDIR="$BUILDDIR/temp"

ORIG_WD="`pwd`"

DK_RELEASEID=`cat $ORIG_WD/data/RELEASEID.txt`

#################################################################################################
# Copy files

echo -e "\n---------- Copy files in bundle directory\n"

# Directories creation -----------------------------------------------------------------------

cd $ORIG_WD

if [ -d "$BUNDLEDIR" ]; then
    rm -fr $BUNDLEDIR
    mkdir $BUNDLEDIR
fi

mkdir -p $BUNDLEDIR/data
mkdir -p $BUNDLEDIR/etc
mkdir -p $BUNDLEDIR/share
mkdir -p $BUNDLEDIR/translations

# Data files ---------------------------------------------------------------------------------

echo -e "\n---------- Marble data"
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/data/*                                        $BUNDLEDIR/data                 2>/dev/null

echo -e "\n---------- Generics data"
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/share/lensfun                                 $BUNDLEDIR/data                 2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/share/digikam                                 $BUNDLEDIR/data                 2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/share/showfoto                                $BUNDLEDIR/data                 2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/share/solid                                   $BUNDLEDIR/data                 2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/data/k*                                   $BUNDLEDIR/data                 2>/dev/null

echo -e "\n---------- Qt config"
cp    $BUILDDIR/data/qt.conf                                                    $BUNDLEDIR/                     2>/dev/null

echo -e "\n---------- icons-set"
cp    $INSTALL_DIR/$VCPKG_TRIPLET/bin/data/icons/breeze/breeze-icons.rcc               $BUNDLEDIR/breeze.rcc           2>/dev/null
cp    $INSTALL_DIR/$VCPKG_TRIPLET/bin/data/icons/breeze-dark/breeze-icons-dark.rcc     $BUNDLEDIR/breeze-dark.rcc      2>/dev/null

echo -e "\n---------- i18n"
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/translations/                                 $BUNDLEDIR/translations         2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/data/locale                               $BUNDLEDIR/data                 2>/dev/null

echo -e "\n---------- Xdg"
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/etc/xdg                                       $BUNDLEDIR/etc                  2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/data/xdg                                  $BUNDLEDIR/share                2>/dev/null

# See bug 471058
echo -e "\n---------- Freedesktop"
mkdir -p $BUNDLEDIR/share/mime/packages/                                                                        2>/dev/null
cp -r  $ORIG_WD/data/freedesktop.org.xml                                        $BUNDLEDIR/share/mime/packages  2>/dev/null

echo -e "\n---------- Copy Git Revisions Manifest"

touch $BUNDLEDIR/MANIFEST.txt

FILES=$(ls $ORIG_WD/data/*_manifest.txt)

for FILE in $FILES ; do
    echo $FILE
    cat $FILE >> $BUNDLEDIR/MANIFEST.txt
done

# Plugins Shared libraries -------------------------------------------------------------------

echo -e "\n---------- Qt plugins"
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/Qt6/plugins                                   $BUNDLEDIR/                     2>/dev/null

echo -e "\n---------- Marble plugins"
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/plugins/*.dll                                 $BUNDLEDIR/plugins              2>/dev/null

echo -e "\n---------- digiKam and KF5 plugins"
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/lib/plugins                                   $BUNDLEDIR/                     2>/dev/null

echo -e "\n---------- OpenAL for QtAV"
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/OpenAL32.dll                              $BUNDLEDIR/                     2>/dev/null

echo -e "\n---------- DrMinGw run-time"
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/exchndl.dll                               $BUNDLEDIR/                     2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/mgwhelp.dll                               $BUNDLEDIR/                     2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/dbgcore.dll                               $BUNDLEDIR/                     2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/symsrv.dll                                $BUNDLEDIR/                     2>/dev/null
cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/symsrv.yes                                $BUNDLEDIR/                     2>/dev/null

# Do not include this file from DrMinGW as it require extra devel dll from Microsoft at run-time.
#cp -r $INSTALL_DIR/$VCPKG_TRIPLET/bin/dbghelp.dll                               $BUNDLEDIR/                    2>/dev/null

echo -e "\n---------- libgphoto2 drivers"
mkdir $BUNDLEDIR/libgphoto2                                                                             2>/dev/null
mkdir $BUNDLEDIR/libgphoto2_port                                                                        2>/dev/null
cp $INSTALL_DIR/$VCPKG_TRIPLET/lib/libgphoto2/*/*.dll                           $BUNDLEDIR/libgphoto2           2>/dev/null
cp $INSTALL_DIR/$VCPKG_TRIPLET/lib/libgphoto2_port/*/*.dll                      $BUNDLEDIR/libgphoto2_port      2>/dev/null

echo -e "\n---------- Copy executables with recursive dependencies in bundle directory\n"

# Executables and plugins shared libraries dependencies scan ---------------------------------

EXE_FILES="\
$INSTALL_DIR/$VCPKG_TRIPLET/bin/digikam.exe \
$INSTALL_DIR/$VCPKG_TRIPLET/bin/showfoto.exe \
$INSTALL_DIR/$VCPKG_TRIPLET/bin/kbuildsycoca5.exe \
$INSTALL_DIR/$VCPKG_TRIPLET/qt5/bin/QtWebNetworkProcess.exe \
$INSTALL_DIR/$VCPKG_TRIPLET/qt5/bin/QtWebProcess.exe \
$INSTALL_DIR/$VCPKG_TRIPLET/qt5/bin/QtWebStorageProcess.exe \
"
for app in $EXE_FILES ; do

    cp $app $BUNDLEDIR/
    $ORIG_WD/rll.py --copy --installprefix $INSTALL_DIR/$VCPKG_TRIPLET --odir $BUNDLEDIR --efile $app

done

DLL_FILES="\
`find  $INSTALL_DIR/$VCPKG_TRIPLET/lib/plugins         -name "*.dll" -type f | sed 's|$INSTALL_DIR/$VCPKG_TRIPLET/libs/plugins||'`        \
`find  $INSTALL_DIR/$VCPKG_TRIPLET/qt5/plugins         -name "*.dll" -type f | sed 's|$INSTALL_DIR/$VCPKG_TRIPLET/qt5/plugins||'`         \
`find  $INSTALL_DIR/$VCPKG_TRIPLET/plugins             -name "*.dll" -type f | sed 's|$INSTALL_DIR/$VCPKG_TRIPLET/plugins/||'`            \
`find  $INSTALL_DIR/$VCPKG_TRIPLET/lib/libgphoto2      -name "*.dll" -type f | sed 's|$INSTALL_DIR/$VCPKG_TRIPLET/lib/libgphoto2||'`      \
`find  $INSTALL_DIR/$VCPKG_TRIPLET/lib/libgphoto2_port -name "*.dll" -type f | sed 's|$INSTALL_DIR/$VCPKG_TRIPLET/lib/libgphoto2_port||'` \
$INSTALL_DIR/$VCPKG_TRIPLET/bin/OpenAL32.dll \
$INSTALL_DIR/$VCPKG_TRIPLET/bin/exchndl.dll  \
$INSTALL_DIR/$VCPKG_TRIPLET/bin/mgwhelp.dll  \
"

for app in $DLL_FILES ; do

    $ORIG_WD/rll.py --copy --installprefix $INSTALL_DIR/$VCPKG_TRIPLET --odir $BUNDLEDIR --efile $app

done

#################################################################################################
# Cleanup symbols in binary files to free space.
# NOTE: NSIS only support < 2Gb of file to package in the same installer. If size is bigger, a bus error exception is genenrated.
# The following code to do lets all debug symbols in each digiKam componets, else size will be largest than 2Gb.
# only the digiKam/Showfoto executable and shared libraries debug symbols are preserved. All digiKam plugins are stripped.

echo -e "\n---------- Strip symbols in binary files\n"

if [[ $DK_DEBUG = 1 ]] ; then

    DEBUG_EXE_STRIP_ALL="`find $BUNDLEDIR -name \*exe | grep -Ev '(digikam|showfoto)'`"
    echo "DEBUG_EXE_STRIP_ALL=$DEBUG_EXE_STRIP_ALL"
    ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip $DEBUG_EXE_STRIP_ALL

    DEBUG_DLL_STRIP_ALL="`find $BUNDLEDIR -name \*dll | grep -Ev '(digikam|showfoto)'`"
    echo "DEBUG_DLL_STRIP_ALL=$DEBUG_DLL_STRIP_ALL"
    ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip $DEBUG_DLL_STRIP_ALL

#    DEBUG_EXE_STRIP="`find $BUNDLEDIR -name \*exe | grep -E '(digikam|showfoto)'`"
#    echo "DEBUG_EXE_STRIP=$DEBUG_EXE_STRIP"
#    ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip --only-keep-debug $DEBUG_EXE_STRIP

    DEBUG_DLL_STRIP="`find $BUNDLEDIR -name \*dll | grep -E '(digikam|showfoto)' | grep -Ev '(libdigikam|digikam.dll|showfoto.dll)'`"
    echo "DEBUG_DLL_STRIP=$DEBUG_DLL_STRIP"
    ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip $DEBUG_DLL_STRIP

else

    find $BUNDLEDIR -name \*exe | xargs ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip --strip-all
    find $BUNDLEDIR -name \*dll | xargs ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip --strip-all

fi

#################################################################################################
# Install ExifTool binary.

cd $DOWNLOAD_DIR

#if [ ! -f $DOWNLOAD_DIR/exiftool.zip ] ; then
    wget https://files.kde.org/digikam/exiftool/exiftool.zip -O exiftool.zip
#fi

unzip -o $DOWNLOAD_DIR/exiftool.zip -d $BUNDLEDIR
mv "$BUNDLEDIR/exiftool(-k).exe" "$BUNDLEDIR/exiftool.exe"

#################################################################################################

if [[ $DK_DEBUG = 1 ]] ; then

    DEBUG_SUF="-debug"

fi

if [[ $DK_VERSION != v* ]] ; then

    # with non-official release version, use build time-stamp as sub-version string.
    DK_SUBVER="-`cat $ORIG_WD/data/BUILDDATE.txt`"

else

    # with official release version, disable upload to KDE server, as this break check for new version function.
    echo -e "Official release version detected, upload is disabled.\n"
    DK_UPLOAD=0

fi

#################################################################################################
# Build NSIS installer and Portable archive.

echo -e "\n---------- Build NSIS installer and Portable archive\n"

mkdir -p $ORIG_WD/bundle

TARGET_INSTALLER=digiKam-$DK_RELEASEID$DK_SUBVER-Win64$DEBUG_SUF.exe
PORTABLE_FILE=digiKam-$DK_RELEASEID$DK_SUBVER-Win64$DEBUG_SUF.tar.xz
CHECKSUM_FILE=digiKam-$DK_RELEASEID$DK_SUBVER-Win64$DEBUG_SUF.sum
rm -f $ORIG_WD/bundle/*Win64$DEBUG_SUF* || true

cd $ORIG_WD/installer

"/c/Program Files (x86)/NSIS/Bin/makensis" -DVERSION=$DK_RELEASEID -DBUNDLEPATH=$BUNDLEDIR -DTARGETARCH=$MXE_ARCHBITS -DOUTPUT=$ORIG_WD/bundle/$TARGET_INSTALLER ./digikam.nsi

cd $ORIG_WD
tar cf - `basename $BUNDLEDIR` --transform s/temp/digiKam/ | xz -4e > $ORIG_WD/bundle/$PORTABLE_FILE

#################################################################################################
# Show resume information and future instructions to host target files on remote server

echo -e "\n---------- Compute installer checksums for digiKam $DK_RELEASEID\n"          >  $ORIG_WD/bundle/$TARGET_INSTALLER.sum
echo    "File       : $TARGET_INSTALLER"                                                >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
echo -n "Size       : "                                                                 >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
du -h "$ORIG_WD/bundle/$TARGET_INSTALLER"        | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
echo -n "SHA256 sum : "                                                                 >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
shasum -a256 "$ORIG_WD/bundle/$TARGET_INSTALLER" | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum

# Checksums to post on Phabricator at release time.
shasum -a256 "$ORIG_WD/bundle/$TARGET_INSTALLER" > $ORIG_WD/bundle/$CHECKSUM_FILE

echo -e "\n---------- Compute Portable archive checksums for digiKam $DK_RELEASEID\n"   >  $ORIG_WD/bundle/$PORTABLE_FILE.sum
echo    "File       : $PORTABLE_FILE"                                                   >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
echo -n "Size       : "                                                                 >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
du -h "$ORIG_WD/bundle/$PORTABLE_FILE"        | { read first rest ; echo $first ; }     >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
echo -n "SHA256 sum : "                                                                 >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
shasum -a256 "$ORIG_WD/bundle/$PORTABLE_FILE" | { read first rest ; echo $first ; }     >> $ORIG_WD/bundle/$PORTABLE_FILE.sum

# Checksums to post on Phabricator at release time.
shasum -a256 "$ORIG_WD/bundle/$PORTABLE_FILE" >> $ORIG_WD/bundle/$CHECKSUM_FILE

if [[ $DK_SIGN = 1 ]] ; then

    echo -e "\n---------- Compute Signature checksums for digiKam installer $DK_RELEASEID\n"    >  $ORIG_WD/bundle/$TARGET_INSTALLER.sum

    cat ~/.gnupg/dkorg-gpg-pwd.txt | gpg --batch --yes --passphrase-fd 0 -sabv "$ORIG_WD/bundle/$TARGET_INSTALLER"
    mv -f $ORIG_WD/bundle/$TARGET_INSTALLER.asc $ORIG_WD/bundle/$TARGET_INSTALLER.sig

    echo    "File       : $TARGET_INSTALLER.sig"                                                >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
    echo -n "Size       : "                                                                     >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
    du -h "$ORIG_WD/bundle/$TARGET_INSTALLER.sig"        | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
    echo -n "SHA256 sum : "                                                                     >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
    shasum -a256 "$ORIG_WD/bundle/$TARGET_INSTALLER.sig" | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum

    # Checksums to post on Phabricator at release time.
    shasum -a256 "$ORIG_WD/bundle/$TARGET_INSTALLER.sig" >> $ORIG_WD/bundle/$CHECKSUM_FILE

    echo -e "\n---------- Compute Signature checksums for digiKam Portable $DK_RELEASEID\n"     >  $ORIG_WD/bundle/$PORTABLE_FILE.sum

    cat ~/.gnupg/dkorg-gpg-pwd.txt | gpg --batch --yes --passphrase-fd 0 -sabv "$ORIG_WD/bundle/$PORTABLE_FILE"
    mv -f $ORIG_WD/bundle/$PORTABLE_FILE.asc $ORIG_WD/bundle/$PORTABLE_FILE.sig

    echo    "File       : $PORTABLE_FILE.sig"                                                >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
    echo -n "Size       : "                                                                  >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
    du -h "$ORIG_WD/bundle/$PORTABLE_FILE.sig"        | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
    echo -n "SHA256 sum : "                                                                  >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
    shasum -a256 "$ORIG_WD/bundle/$PORTABLE_FILE.sig" | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$PORTABLE_FILE.sum

    # Checksums to post on Phabricator at release time.
    shasum -a256 "$ORIG_WD/bundle/$PORTABLE_FILE.sig" >> $ORIG_WD/bundle/$CHECKSUM_FILE

fi

cat $ORIG_WD/bundle/$TARGET_INSTALLER.sum
cat $ORIG_WD/bundle/$PORTABLE_FILE.sum

if [[ $DK_UPLOAD = 1 ]] ; then

    echo -e "---------- Cleanup older Windows bundle files from files.kde.org repository \n"

    if [ $MXE_BUILD_TARGETS == "i686-w64-mingw32.shared" ]; then
        sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win32$DEBUG_SUF.exe*"
        sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win32$DEBUG_SUF.tar.xz*"
    else
        sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win64$DEBUG_SUF.exe*"
        sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win64$DEBUG_SUF.tar.xz*"
    fi

    echo -e "---------- Upload new Windows bundle files to files.kde.org repository \n"

    rsync -r -v --progress -e ssh $ORIG_WD/bundle/$TARGET_INSTALLER $DK_UPLOADURL:$DK_UPLOADDIR
    rsync -r -v --progress -e ssh $ORIG_WD/bundle/$PORTABLE_FILE $DK_UPLOADURL:$DK_UPLOADDIR

    if [[ $DK_SIGN = 1 ]] ; then
        scp $ORIG_WD/bundle/$TARGET_INSTALLER.sig $DK_UPLOADURL:$DK_UPLOADDIR
        scp $ORIG_WD/bundle/$PORTABLE_FILE.sig $DK_UPLOADURL:$DK_UPLOADDIR
    fi

    # update remote files list

    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "ls digi*" > $ORIG_WD/bundle/ls.txt
    tail -n +2 $ORIG_WD/bundle/ls.txt > $ORIG_WD/bundle/ls.tmp
    cat $ORIG_WD/bundle/ls.tmp | grep -E '(.pkg |.appimage |.exe )' | grep -Ev '(debug)' > $ORIG_WD/bundle/FILES
    rm $ORIG_WD/bundle/ls.tmp
    rm $ORIG_WD/bundle/ls.txt
    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm FILES"
    rsync -r -v --progress -e ssh $BUILDDIR/bundle/FILES $DK_UPLOADURL:$DK_UPLOADDIR

else

    echo -e "\n------------------------------------------------------------------"
    curl https://download.kde.org/README_UPLOAD
    echo -e "------------------------------------------------------------------\n"

fi

#################################################################################################

TerminateScript

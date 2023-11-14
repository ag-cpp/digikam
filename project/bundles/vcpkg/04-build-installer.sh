#! /bin/bash

# Script to bundle data using previously-built KDE and digiKam installation
# and create a Windows installer file with NSIS application
# Dependencies:
#   - NSIS makensis program for Windows.
#   - DumpBin from VSCommunity C++ profiling tools.
#   - ImageMagick for the rune-time dll.
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
AppendVCPKGPaths

#################################################################################################

# Check if NSIS CLI tool is installed.

if ! which "/c/Program Files (x86)/NSIS/Bin/makensis" ; then
    echo "NSIS CLI tool is not installed"
    echo "See https://nsis.sourceforge.io for details."
    exit 1
else
    echo "Check NSIS CLI tools passed..."
fi

# Check if DumpBin CLI tool is installed.

DUMP_BIN="`find "/c/Program Files/Microsoft Visual Studio/" -name "dumpbin.exe" -type f -executable | grep 'Hostx64/x64/dumpbin.exe'`"
echo "$DUMP_BIN"

if [ ! -f "$DUMP_BIN" ] ; then
    echo "DumpBin CLI tool is not installed"
    echo "Install VSCommunity C++ Profiling tools component."
    exit 1
else
    echo "Check DumpBin CLI tools passed..."
fi

# Check for redistributable VSCommunity compatibility dll.

# Check for the ImageMagick install directory.

MAGICK_PREFIX="`find "/c/Program Files/" -maxdepth 1 -name "ImageMagick*" -type d`"
echo "$MAGICK_PREFIX"

if [ ! -d "$MAGICK_PREFIX" ] ; then
    echo "ImageMagick is not installed"
    exit 1
else
    echo "Check ImageMagick install path passed..."
fi

#################################################################################################
# Configurations

# Directory where this script is located (default - current directory)
BUILDDIR="$PWD"

# Directory where bundle files are located
BUNDLEDIR="$BUILDDIR/temp"

ORIG_WD="`pwd`"

DK_RELEASEID=`cat $ORIG_WD/data/RELEASEID.txt`

#################################################################################################
# Copy files

echo -e "\n---------- Preparing files to host in the bundle"

# Directories creation -----------------------------------------------------------------------

cd $ORIG_WD

if [ -d "$BUNDLEDIR" ] ; then
    echo "Cleanup temporary directory $BUNDLEDIR..."
    rm -fr "$BUNDLEDIR"
    mkdir "$BUNDLEDIR"
fi

mkdir -p $BUNDLEDIR/data
mkdir -p $BUNDLEDIR/etc
mkdir -p $BUNDLEDIR/share
mkdir -p $BUNDLEDIR/translations

# Data files ---------------------------------------------------------------------------------

echo -e "\n---------- Marble data"
cp -r $VCPKG_INSTALL_PREFIX/data/*                                        $BUNDLEDIR/data                       2>/dev/null

echo -e "\n---------- Generics data"
cp -r $VCPKG_INSTALL_PREFIX/share/lensfun                                 $BUNDLEDIR/data                       2>/dev/null
cp -r $VCPKG_INSTALL_PREFIX/share/digikam                                 $BUNDLEDIR/data                       2>/dev/null
cp -r $VCPKG_INSTALL_PREFIX/share/showfoto                                $BUNDLEDIR/data                       2>/dev/null
cp -r $VCPKG_INSTALL_PREFIX/share/solid                                   $BUNDLEDIR/data                       2>/dev/null
cp -r $VCPKG_INSTALL_PREFIX/share/kxmlgui5                                $BUNDLEDIR/data                       2>/dev/null
cp -r $VCPKG_INSTALL_PREFIX/share/knotifications6                         $BUNDLEDIR/data                       2>/dev/null
cp -r $VCPKG_INSTALL_PREFIX/bin/data/k*                                   $BUNDLEDIR/data                       2>/dev/null
cp -r $VCPKG_INSTALL_PREFIX/resources                                     $BUNDLEDIR/                           2>/dev/null

echo -e "\n---------- Qt config"
cp    $BUILDDIR/data/qt.conf                                              $BUNDLEDIR/                           2>/dev/null

echo -e "\n---------- icons-set"
cp    $VCPKG_INSTALL_PREFIX/bin/data/icons/breeze/breeze-icons.rcc               $BUNDLEDIR/breeze.rcc          2>/dev/null
cp    $VCPKG_INSTALL_PREFIX/bin/data/icons/breeze-dark/breeze-icons-dark.rcc     $BUNDLEDIR/breeze-dark.rcc     2>/dev/null

echo -e "\n---------- i18n"
# Qt framekork translations
cp -r $VCPKG_INSTALL_PREFIX/translations/Qt6/*                            $BUNDLEDIR/translations               2>/dev/null
rm -fr $BUNDLEDIR/translations/assistant*.qm
rm -fr $BUNDLEDIR/translations/designer*.qm
rm -fr $BUNDLEDIR/translations/linguist*.qm
rm -fr $BUNDLEDIR/translations/qt_help*.qm
# KDE framework translations
cp -r $VCPKG_INSTALL_PREFIX/bin/data/locale                               $BUNDLEDIR/data                       2>/dev/null
# digiKam translations
cp -r $VCPKG_INSTALL_PREFIX/share/locale                                  $BUNDLEDIR/data                       2>/dev/null

echo -e "\n---------- Xdg"
cp -r $VCPKG_INSTALL_PREFIX/etc/xdg                                       $BUNDLEDIR/etc                        2>/dev/null
cp -r $VCPKG_INSTALL_PREFIX/bin/data/xdg                                  $BUNDLEDIR/share                      2>/dev/null

# See bug 471058
echo -e "\n---------- Freedesktop"
mkdir -p $BUNDLEDIR/share/mime/packages/                                                                        2>/dev/null
cp -r  $ORIG_WD/data/freedesktop.org.xml                                  $BUNDLEDIR/share/mime/packages        2>/dev/null

echo -e "\n---------- Copy Git Revisions Manifest"

touch $BUNDLEDIR/MANIFEST.txt

FILES=$(ls $ORIG_WD/data/*_manifest.txt)

for FILE in $FILES ; do
    echo $FILE
    cat $FILE >> $BUNDLEDIR/MANIFEST.txt
done

# Plugins Shared libraries -------------------------------------------------------------------

echo -e "\n---------- Qt6 plugins"
cp -r $VCPKG_INSTALL_PREFIX/Qt6/plugins                                   $BUNDLEDIR/                           2>/dev/null

echo -e "\n---------- Marble plugins"
cp -r $VCPKG_INSTALL_PREFIX/plugins/*.dll                                 $BUNDLEDIR/plugins                    2>/dev/null

echo -e "\n---------- digiKam and KF6 plugins"
cp -r $VCPKG_INSTALL_PREFIX/lib/plugins                                   $BUNDLEDIR/                           2>/dev/null

echo -e "\n---------- OpenAL for QtAV"
cp -r $VCPKG_INSTALL_PREFIX/bin/OpenAL32.dll                              $BUNDLEDIR/                           2>/dev/null

echo -e "\n---------- ImageMagick"

cp -r "$MAGICK_PREFIX"/CORE_RL_*.dll                                      $BUNDLEDIR/                           2>/dev/null
cp -r "$MAGICK_PREFIX"/modules/coders/*.dll                               $BUNDLEDIR/                           2>/dev/null
cp -r "$MAGICK_PREFIX"/modules/filters/*.dll                              $BUNDLEDIR/                           2>/dev/null

#################################################################################################
# Add debug symbols for few binary files to optimize space.
# NOTE: NSIS only support < 2Gb of file to package in the same installer. If size is bigger, a bus error exception is genenrated.
# Only the digiKam shared libraries debug symbols are preserved. All digiKam plugins are not present.

# First drop all exisisting pdb files from the bundles if any.

echo -e "\n---------- Clean up debug symbols in the bundle"

PDB_FILES="`find  $VCPKG_INSTALL_PREFIX/lib/plugins -name "*.pdb" -type f`"

for pdb in $PDB_FILES ; do

    echo -e "\n   => $pdb"
    rm -rf "$pdb"

done

# Second add only wanted pdb files in the bundles

if [[ $DK_DEBUG = 1 ]] ; then

    echo -e "\n---------- Add necessary debug symbols in the bundle"

    cp -r $VCPKG_INSTALL_PREFIX/bin/digikam*.pdb                       $BUNDLEDIR/                              2>/dev/null

fi

echo -e "\n---------- Copy executables with recursive dependencies in bundle directory\n"

# Executables and plugins shared libraries dependencies scan ---------------------------------

EXE_FILES="\
$VCPKG_INSTALL_PREFIX/bin/digikam.exe \
$VCPKG_INSTALL_PREFIX/bin/showfoto.exe \
$VCPKG_INSTALL_PREFIX/bin/kbuildsycoca6.exe \
$VCPKG_INSTALL_PREFIX/tools/Qt6/bin/QtWebEngineProcess.exe \
"
for app in $EXE_FILES ; do

    cp $app $BUNDLEDIR/
    CopyReccursiveDependencies "$DUMP_BIN" "$app" "$BUNDLEDIR/" "$VCPKG_INSTALL_PREFIX/bin"

done

DLL_FILES="\
`find  $VCPKG_INSTALL_PREFIX/lib/plugins -name "*.dll" -type f | sed 's|$VCPKG_INSTALL_PREFIX/libs/plugins||'` \
`find  $VCPKG_INSTALL_PREFIX/Qt6/plugins -name "*.dll" -type f | sed 's|$VCPKG_INSTALL_PREFIX/Qt6/plugins||'`  \
`find  $VCPKG_INSTALL_PREFIX/plugins     -name "*.dll" -type f | sed 's|$VCPKG_INSTALL_PREFIX/plugins||'`      \
$VCPKG_INSTALL_PREFIX/bin/OpenAL32.dll                                                                         \
"

for app in $DLL_FILES ; do

    CopyReccursiveDependencies "$DUMP_BIN" "$app" "$BUNDLEDIR/" "$VCPKG_INSTALL_PREFIX/bin"

done

echo -e "\n---------- Copy redistributable VSCommunity compatibility dlls"

VS_DLL_FILES="\
$VCPKG_INSTALL_PREFIX/bin/msvcp140.dll              \
$VCPKG_INSTALL_PREFIX/bin/msvcp140_1.dll            \
$VCPKG_INSTALL_PREFIX/bin/msvcp140_2.dll            \
$VCPKG_INSTALL_PREFIX/bin/msvcp140_atomic_wait.dll  \
$VCPKG_INSTALL_PREFIX/bin/msvcp140_codecvt_ids.dll  \
$VCPKG_INSTALL_PREFIX/bin/vcruntime140_1.dll        \
$VCPKG_INSTALL_PREFIX/bin/vcruntime140.dll          \
$VCPKG_INSTALL_PREFIX/bin/concrt140.dll             \
"

for vsdll in $VS_DLL_FILES ; do

    cp -r "$vsdll" $BUNDLEDIR/ 2>/dev/null

done

#################################################################################################
# Install ExifTool binary.

echo -e "\n---------- Copy ExifTool executable"

cd $DOWNLOAD_DIR
curl -LO https://files.kde.org/digikam/exiftool/exiftool.zip
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

TARGET_INSTALLER=digiKam-$DK_RELEASEID$DK_SUBVER-Win64-Qt6$DEBUG_SUF.exe
PORTABLE_FILE=digiKam-$DK_RELEASEID$DK_SUBVER-Win64-Qt6$DEBUG_SUF.tar.xz
CHECKSUM_FILE=digiKam-$DK_RELEASEID$DK_SUBVER-Win64-Qt6$DEBUG_SUF.sum
rm -f $ORIG_WD/bundle/*Win64-Qt6$DEBUG_SUF* || true

cd $ORIG_WD/installer

"/c/Program Files (x86)/NSIS/Bin/makensis" -DVERSION=$DK_RELEASEID -DBUNDLEPATH=$BUNDLEDIR -DOUTPUT=$ORIG_WD/bundle/$TARGET_INSTALLER ./digikam.nsi

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

    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win64-Qt6$DEBUG_SUF.exe*"
    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win64-Qt6$DEBUG_SUF.tar.xz*"

    echo -e "---------- Upload new Windows bundle files to files.kde.org repository \n"

    scp $ORIG_WD/bundle/$TARGET_INSTALLER $DK_UPLOADURL:$DK_UPLOADDIR
    scp $ORIG_WD/bundle/$PORTABLE_FILE $DK_UPLOADURL:$DK_UPLOADDIR

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
    scp $BUILDDIR/bundle/FILES $DK_UPLOADURL:$DK_UPLOADDIR

else

    echo -e "\n------------------------------------------------------------------"
    curl https://download.kde.org/README_UPLOAD
    echo -e "------------------------------------------------------------------\n"

fi

#################################################################################################

TerminateScript

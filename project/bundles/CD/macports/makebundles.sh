#!/bin/bash

# SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Ask to run as root
(( EUID != 0 )) && exec sudo -- "$0" "$@"

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

ORIG_WD="`pwd`"

cd $ORIG_WD/8.x/project/bundles/macports
git pull
./update.sh

cd $ORIG_WD


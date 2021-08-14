#!/bin/bash

# Script to build Qt 6.2.0-beta2 from git repository
#
# Copyright (c) 2021 by Anjani Kumar  <anjanik012 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set -e
set -C

git checkout v6.2.0-beta2

# initialize qt6 submodules

perl init-repository --module-subset=qtbase,qt5compat,qtquickcontrols2,qtwebchannel,qtwebengine




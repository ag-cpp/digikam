#!/bin/bash

# Copyright (c) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Script to run all Continuous Integration scripts at the same time.

./cppcheck.sh && ./krazy.sh && ./clangscan.sh && ./clazy.sh && ./coverity.sh

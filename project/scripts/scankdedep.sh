#!/bin/bash

# Copyright (c) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
#
#
# Script to show all KF5 dependencies by macro includes scan

grep -r "#include <k" ../../core/*          | \
    grep -v "#include <klocalizedstring.h>" | \
    grep -v "config"                        | \
    grep -v "kernel"                        | \
    grep -v "kvm"                           | \
    grep -v "kstat"                         | \
    grep -v "keychain"                      | \
    grep -v "scankdedep.sh"

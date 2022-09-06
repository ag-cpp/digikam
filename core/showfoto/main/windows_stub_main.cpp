/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-07-28
 * Description : main program from digiKam (stub for Windows binary)
 *
 * Copyright (C) 2002-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

extern "C"
{
    __declspec(dllimport) int showfoto_main(int argc, char** argv);
}

int main(int argc, char** argv)
{
    return showfoto_main(argc, argv);
}

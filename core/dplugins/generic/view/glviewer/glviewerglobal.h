/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-11
 * Description : a tool to show image using an OpenGL interface.
 *
 * SPDX-FileCopyrightText: 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Uncomment this line to enable performance checks.
//#define PERFORMANCE_ANALYSIS

/// Images cache settings. Keep in mind that one cache entry takes 20MB for a 5mpix pic
#define CACHESIZE 4
#define EMPTY     99999

namespace DigikamGenericGLViewerPlugin
{

enum OGLstate
{
    oglOK,
    oglNoRectangularTexture,
    oglNoContext
};

} // namespace DigikamGenericGLViewerPlugin

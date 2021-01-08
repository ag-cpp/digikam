/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-11
 * Description : a tool to show image using an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "glviewertimer.h"

// Qt includes

#include <QDateTime>
#include <QElapsedTimer>

// Local includes

#include "digikam_debug.h"

namespace DigikamGenericGLViewerPlugin
{

class Q_DECL_HIDDEN GLViewerTimer::Private
{
public:

    explicit Private()
    {
        meantime = 0;
    }

    QElapsedTimer timer;
    int           meantime;
};

GLViewerTimer::GLViewerTimer()
    : d(new Private)
{
}

GLViewerTimer::~GLViewerTimer()
{
    delete d;
}

void GLViewerTimer::start()
{
    d->timer.start();
    d->meantime = 0;
}

void GLViewerTimer::at(const QString& s)
{
    d->meantime = d->timer.elapsed() - d->meantime;

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "stopwatch:" << s
                                         << ": " << d->meantime
                                         << " ms    overall: " 
                                         << d->timer.elapsed() << " ms";
}

} // namespace DigikamGenericGLViewerPlugin

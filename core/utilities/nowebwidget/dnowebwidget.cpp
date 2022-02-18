/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-08
 * Description : Dummy Widget if no Webkit is found.
 *
 * Copyright (C) 2021 by Anjani Kumar <anjanik012 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QUrl>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dnowebwidget.h"

namespace Digikam
{
DNoWebWidget::DNoWebWidget(QWidget* parent)
    : QLabel(parent)
{
    setText(i18n("Warning: No WebEngine/Webkit was found. Web content cannot be loaded"));
}

void DNoWebWidget::setUrl(const QUrl &url)
{
    Q_UNUSED(url);
}

void DNoWebWidget::setSharedGeoIfaceObject(GeoIfaceSharedData* const sharedData)
{
    Q_UNUSED(sharedData);
}

void DNoWebWidget::load(const QUrl &url)
{
    Q_UNUSED(url);
}

QVariant DNoWebWidget::runScript(const QString& scriptCode, bool async)
{
    Q_UNUSED(scriptCode);
    Q_UNUSED(async);

    return {};
}

bool DNoWebWidget::runScript2Coordinates(const QString &scriptCode, GeoCoordinates* const coordinates)
{
    Q_UNUSED(scriptCode);
    Q_UNUSED(coordinates);

    return false;
}

void DNoWebWidget::setSelectionRectangle(const GeoCoordinates::Pair& searchCoordinates)
{
    Q_UNUSED(searchCoordinates);
}

void DNoWebWidget::removeSelectionRectangle()
{
}

void DNoWebWidget::mouseModeChanged(const GeoMouseModes mouseMode)
{
    Q_UNUSED(mouseMode);
}

void DNoWebWidget::centerOn(const qreal west, const qreal north, const qreal east, const qreal south,
                            const bool useSaneZoomLevel)
{
    Q_UNUSED(west);
    Q_UNUSED(north);
    Q_UNUSED(east);
    Q_UNUSED(south);
    Q_UNUSED(useSaneZoomLevel);
}

}

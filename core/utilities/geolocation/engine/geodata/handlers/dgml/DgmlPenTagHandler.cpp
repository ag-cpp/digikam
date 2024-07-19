/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team <marble dot kde dot org>
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "DgmlPenTagHandler.h"

// Qt includes

#include <QPen>
#include <QColor>
#include <QString>

// Local includes

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneGeodata.h"

namespace Marble
{

namespace dgml
{

DGML_DEFINE_TAG_HANDLER(Pen)

GeoNode* DgmlPenTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Pen)));

    QString color = parser.attribute(dgmlAttr_color).trimmed();
    QString style = parser.attribute(dgmlAttr_style).toLower().trimmed();
    QString widthString = parser.attribute(dgmlAttr_width);
    qreal  width = widthString.isEmpty() ? 1.0 : widthString.toDouble();

    QPen pen;

    if (!color.isEmpty() && QColor(color).isValid())
    {
        pen.setColor(QColor(color));
    }

    if (!style.isEmpty())
    {
        if (style == QLatin1String("nopen"))
        {
            pen.setStyle(Qt::NoPen);
        }

        else if (style == QLatin1String("solidline"))
        {
            pen.setStyle(Qt::SolidLine);
        }

        else if (style == QLatin1String("dashline"))
        {
            pen.setStyle(Qt::DashLine);
        }

        else if (style == QLatin1String("dotline"))
        {
            pen.setStyle(Qt::DotLine);
        }

        else if (style == QLatin1String("dashdotline"))
        {
            pen.setStyle(Qt::DashDotLine);
        }

        else if (style == QLatin1String("dashdotdotline"))
        {
            pen.setStyle(Qt::DashDotDotLine);
        }
    }

    if (width != 0.0)
    {
        pen.setWidthF(width);
    }

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(dgmlTag_Vector)
        || parentItem.represents(dgmlTag_Geodata))
    {
        GeoSceneGeodata* geodata = parentItem.nodeAs<GeoSceneGeodata>();
        geodata->setPen(pen);
    }

    return nullptr;
}

} // namespace dgml

} // namespace Marble

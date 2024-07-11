/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "KmlWhenTagHandler.h"

// Qt includes

#include <QDateTime>

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTrack.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(when)

GeoNode* KmlwhenTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_when)));

    GeoStackItem parentItem = parser.parentElement();

    QString whenString = parser.readElementText().trimmed();
    GeoDataTimeStamp::TimeResolution resolution = modify(whenString);
    QDateTime when = QDateTime::fromString(whenString, Qt::ISODate);

    if (parentItem.represents(kmlTag_TimeStamp))
    {
        parentItem.nodeAs<GeoDataTimeStamp>()->setWhen(when);
        parentItem.nodeAs<GeoDataTimeStamp>()->setResolution(resolution);
    }

    else if (parentItem.represents(kmlTag_Track))
    {
        parentItem.nodeAs<GeoDataTrack>()->appendWhen(when);
    }

    return nullptr;
}

QDateTime KmlwhenTagHandler::parse(const QString& dateTime)
{
    QString iso = dateTime;
    modify(iso);
    return QDateTime::fromString(iso, Qt::ISODate);
}

GeoDataTimeStamp KmlwhenTagHandler::parseTimestamp(const QString& dateTime)
{
    GeoDataTimeStamp result;
    QString input = dateTime;
    result.setResolution(modify(input));
    result.setWhen(parse(input));
    return result;
}

GeoDataTimeStamp::TimeResolution KmlwhenTagHandler::modify(QString& whenString)
{
    switch (whenString.length())
    {
        case 4:
            whenString += QLatin1String("-01-01");
            return GeoDataTimeStamp::YearResolution;

        case 7:
            whenString += QLatin1String("-01");
            return GeoDataTimeStamp::MonthResolution;

        case 10:
            return GeoDataTimeStamp::DayResolution;

        default:
            return GeoDataTimeStamp::SecondResolution;
    }

    return GeoDataTimeStamp::SecondResolution;
}

} // namespace kml

} // namespace Marble

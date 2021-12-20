/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Extraction of focus points by exiftool data
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#include "focuspoints_extractor.h"

// Local includes

#include "exiftoolparser.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN FocusPointsExtractor::Private
{

public:

    explicit Private()
        : exifToolAvailable(false),
          afPointsReadOnly (true)
    {
    }

    ListAFPoints                    af_points;
    bool                            exifToolAvailable;
    ExifToolParser::ExifToolData    metadata;
    bool                            afPointsReadOnly;
    QString                         make;
    QString                         model;
};

FocusPointsExtractor::FocusPointsExtractor(QObject* const parent,const QString& image_path)
    : QObject(parent),
      d      (new Private)
{
    QScopedPointer<ExifToolParser> const exiftool(new ExifToolParser(this));

    exiftool->load(image_path);

    d->exifToolAvailable = exiftool->exifToolAvailable();
    d->metadata          = exiftool->currentData();
    d->make              = findValue(QLatin1String("EXIF.IFD0.Camera.Make")).toString();
    d->make              = d->make.split(QLatin1String(" "))[0].toUpper();
    d->model             = findValue(QLatin1String("EXIF.IFD0.Camera.Model")).toString();
    d->model             = d->model.split(QLatin1String(" "))[0].toUpper();
    d->af_points         = findAFPoints();
}

FocusPointsExtractor::~FocusPointsExtractor()
{
    delete d;
}

QVariant FocusPointsExtractor::findValue(const QString& tagName, bool isList) const
{
    QVariantList result = d->metadata.value(tagName);

    if (result.empty())
    {
        return QVariant();
    }

    if (isList)
    {
        return result[0].toString().split(QLatin1String(" "));
    }
    else
    {
        return result[0];
    }
}

QVariant FocusPointsExtractor::findValue(const QString& tagNameRoot, const QString& key, bool isList) const
{
    return findValue(tagNameRoot + QLatin1String(".") + key,isList);
}

QVariant FocusPointsExtractor::findValueFirstMatch(const QStringList& listTagNames, bool isList) const
{
    for (const QString& tagName : listTagNames)
    {
        QVariant tmp = findValue(tagName,isList);

        if (!tmp.isNull())
        {
            return tmp;
        }
    }

    return QVariant();
}

QVariant FocusPointsExtractor::findValueFirstMatch(const QString& tagNameRoot, const QStringList& keys, bool isList) const
{
    for (const QString& key : keys)
    {
        QVariant tmp = findValue(tagNameRoot,key,isList);

        if (!tmp.isNull())
        {
            return tmp;
        }
    }

    return QVariant();
}

FocusPointsExtractor::ListAFPoints FocusPointsExtractor::findAFPoints() const
{
    if (!d->exifToolAvailable)
    {
        return ListAFPoints();
    }

    if (!d->make.isNull())
    {
        if (d->make == QLatin1String("APPLE"))
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: use Apple Exif metadata";

            return getAFPoints_exif();
        }

        if (d->make == QLatin1String("CANON"))
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: use Canon makernotes";

            return getAFPoints_canon();
        }

        if (d->make == QLatin1String("NIKON"))
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: use Nikon makernotes";

            return getAFPoints_nikon();
        }

        if (d->make == QLatin1String("PANASONIC"))
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: use Panasonic makernotes";

            return getAFPoints_panasonic();
        }

        if (d->make == QLatin1String("SONY"))
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: use Sony makernotes";

            return getAFPoints_sony();
        }
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: use Exif or XMP metadata";

    return getAFPoints_exif();
}

FocusPointsExtractor::ListAFPoints FocusPointsExtractor::get_af_points(FocusPoint::TypePoint type)
{
    ListAFPoints points;

    for (const auto point : d->af_points)
    {
        if (type == FocusPoint::TypePoint::Inactive)
        {
            if (point.getType() == type)
            {
                points.push_back(point);
            }
        }
        else
        {
            if ((point.getType() & type) == type)
            {
                points.push_back(point);
            }
        }
    }

    return points;
}

FocusPointsExtractor::ListAFPoints FocusPointsExtractor::get_af_points()
{
    return d->af_points;
}

bool FocusPointsExtractor::isAFPointsReadOnly() const
{
    findAFPoints();

    return d->afPointsReadOnly;
}

void FocusPointsExtractor::setAFPointsReadOnly(bool readOnly) const
{
    d->afPointsReadOnly = readOnly;
}

QString FocusPointsExtractor::make() const
{
    return d->make;
}

QString FocusPointsExtractor::model() const
{
    return d->model;
}

} // namespace Digikam

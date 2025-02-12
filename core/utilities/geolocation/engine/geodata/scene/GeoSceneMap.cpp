/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "GeoSceneMap.h"

// Qt includes

#include <QColor>

// Local includes

#include "GeoSceneTypes.h"
#include "GeoSceneLayer.h"
#include "GeoSceneFilter.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

// FIXME: Filters are a Dataset.

class Q_DECL_HIDDEN GeoSceneMapPrivate
{
public:

    GeoSceneMapPrivate()
    {
    }

    ~GeoSceneMapPrivate()
    {
        qDeleteAll(m_layers);
        qDeleteAll(m_filters);
    }

    QVariantList                m_center;

    /// The vector holding all the sections in the legend.
    /// (We want to preserve the order and don't care
    /// much about speed here), so we don't use a hash
    QVector<GeoSceneLayer*>     m_layers;

    /// The vector holding all the filters in the map.
    QVector<GeoSceneFilter*>    m_filters;

    QColor                      m_backgroundColor;
    QColor                      m_labelColor;

    /// This color will be used to highlight
    /// a region when it's clicked on.
    QColor                      m_highlightBrushColor;
    QColor                      m_highlightPenColor;
};

GeoSceneMap::GeoSceneMap()
    : d(new GeoSceneMapPrivate)
{
}

GeoSceneMap::~GeoSceneMap()
{
    delete d;
}

const char* GeoSceneMap::nodeType() const
{
    return GeoSceneTypes::GeoSceneMapType;
}

void GeoSceneMap::addLayer(GeoSceneLayer* layer)
{
    // Remove any layer that has the same name
    QVector<GeoSceneLayer*>::iterator it = d->m_layers.begin();

    while (it != d->m_layers.end())
    {
        GeoSceneLayer* currentLayer = *it;

        if (currentLayer->name() == layer->name())
        {
            delete currentLayer;
            d->m_layers.erase(it);
            break;
        }

        else
        {
            ++it;
        }
    }

    if (layer)
    {
        d->m_layers.append(layer);
    }
}

GeoSceneLayer* GeoSceneMap::layer(const QString& name)
{
    GeoSceneLayer* layer = nullptr;

    QVector<GeoSceneLayer*>::const_iterator it = d->m_layers.constBegin();
    QVector<GeoSceneLayer*>::const_iterator end = d->m_layers.constEnd();

    for (; it != end; ++it)
    {
        if ((*it)->name() == name)
        {
            layer = *it;
            break;
        }
    }

    if (!layer)
    {
        layer = new GeoSceneLayer(name);
        addLayer(layer);
    }

    return layer;
}

const GeoSceneLayer* GeoSceneMap::layer(const QString& name) const
{
    const GeoSceneLayer* layer = nullptr;

    QVector<GeoSceneLayer*>::const_iterator it = d->m_layers.constBegin();
    QVector<GeoSceneLayer*>::const_iterator end = d->m_layers.constEnd();

    for (; it != end; ++it)
    {
        if ((*it)->name() == name)
        {
            layer = *it;
            break;
        }
    }

    return layer;
}

QVector<GeoSceneLayer*> GeoSceneMap::layers() const
{
    return d->m_layers;
}

void GeoSceneMap::addFilter(GeoSceneFilter* filter)
{
    // Remove any filter that has the same name
    QVector<GeoSceneFilter*>::iterator it = d->m_filters.begin();

    while (it != d->m_filters.end())
    {
        GeoSceneFilter* currentFilter = *it;

        if (currentFilter->name() == filter->name())
        {
            delete currentFilter;
            d->m_filters.erase(it);
            break;
        }

        else
        {
            ++it;
        }
    }

    if (filter)
    {
        d->m_filters.append(filter);
    }
}

QVariantList GeoSceneMap::center() const
{
    return d->m_center;
}

void GeoSceneMap::setCenter(const QString& coordinatesString)
{
    QStringList coordinatesList = coordinatesString.split(QLatin1String(","));

    if (coordinatesList.count() == 2)
    {
        bool success = false;
        const GeoDataCoordinates coordinates = GeoDataCoordinates::fromString(coordinatesString, success);

        if (success)
        {
            QVariantList lonLat;
            lonLat << QVariant(coordinates.longitude(GeoDataCoordinates::Degree))
                   << QVariant(coordinates.latitude(GeoDataCoordinates::Degree));
            d->m_center = lonLat;
        }
    }

    // LatLonBox
    else if (coordinatesList.count() == 4)
    {
        QVariantList northSouthEastWest;
        d->m_center << QVariant(coordinatesList.at(0)) << QVariant(coordinatesList.at(1))
                    << QVariant(coordinatesList.at(2)) << QVariant(coordinatesList.at(3));
    }
}

GeoSceneFilter* GeoSceneMap::filter(const QString& name)
{
    GeoSceneFilter* filter = nullptr;

    QVector<GeoSceneFilter*>::const_iterator it = d->m_filters.constBegin();
    QVector<GeoSceneFilter*>::const_iterator end = d->m_filters.constEnd();

    for (; it != end; ++it)
    {
        if ((*it)->name() == name)
        {
            filter = *it;
            break;
        }
    }

    if (!filter)
    {
        filter = new GeoSceneFilter(name);
        addFilter(filter);
    }

    return filter;
}

QVector<GeoSceneFilter*> GeoSceneMap::filters() const
{
    return d->m_filters;
}

bool GeoSceneMap::hasTextureLayers() const
{
    QVector<GeoSceneLayer*>::const_iterator it = d->m_layers.constBegin();
    QVector<GeoSceneLayer*>::const_iterator end = d->m_layers.constEnd();

    for (; it != end; ++it)
    {
        if (((*it)->backend() == QLatin1String(dgml::dgmlValue_texture) ||
             (*it)->backend() == QLatin1String(dgml::dgmlValue_vectortile)) && (*it)->datasets().count() > 0)
        {
            return true;
        }
    }

    return false;
}

bool GeoSceneMap::hasVectorLayers() const
{
    QVector<GeoSceneLayer*>::const_iterator it = d->m_layers.constBegin();
    QVector<GeoSceneLayer*>::const_iterator end = d->m_layers.constEnd();

    for (; it != end; ++it)
    {
        if (((*it)->backend() == QLatin1String(dgml::dgmlValue_vectortile) ||
             (*it)->backend() == QLatin1String(dgml::dgmlValue_vector)) && (*it)->datasets().count() > 0)
        {
            return true;
        }
    }

    return false;
}

QColor GeoSceneMap::backgroundColor() const
{
    return d->m_backgroundColor;
}

void GeoSceneMap::setBackgroundColor(const QColor& backgroundColor)
{
    d->m_backgroundColor = backgroundColor;
}


QColor GeoSceneMap::labelColor() const
{
    return d->m_labelColor;
}

void GeoSceneMap::setLabelColor(const QColor& backgroundColor)
{
    d->m_labelColor = backgroundColor;
}

QColor GeoSceneMap::highlightBrushColor() const
{
    return d->m_highlightBrushColor;
}

void GeoSceneMap::setHighlightBrushColor(const QColor& highlightBrushColor)
{
    d->m_highlightBrushColor = highlightBrushColor;
}

QColor GeoSceneMap::highlightPenColor() const
{
    return d->m_highlightPenColor;
}

void GeoSceneMap::setHighlightPenColor(const QColor& highlightPenColor)
{
    d->m_highlightPenColor = highlightPenColor;
}

} // namespace Marble

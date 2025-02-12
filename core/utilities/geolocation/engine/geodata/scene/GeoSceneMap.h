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

#pragma once

// Qt includes

#include <QVector>
#include <QVariant>

// Local includes

#include "GeoDocument.h"
#include "digikam_export.h"

class QColor;
class QString;

namespace Marble
{

class GeoSceneLayer;
class GeoSceneFilter;
class GeoSceneMapPrivate;

/**
 * @short Map layer structure of a GeoScene document.
 */
class DIGIKAM_EXPORT GeoSceneMap : public GeoNode
{
public:

    GeoSceneMap();
    ~GeoSceneMap() override;
    const char* nodeType() const override;

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor&);

    QColor labelColor() const;
    void setLabelColor(const QColor&);

    QColor highlightBrushColor() const;
    void setHighlightBrushColor(const QColor&);

    QColor highlightPenColor() const;
    void setHighlightPenColor(const QColor&);
    /**
     * @brief  Add a new layer to the map
     * @param  layer  The new layer
     */
    void addLayer(GeoSceneLayer*);
    /**
     * @ brief Set starting center with lon lat cooredinates
     * used if a scene downloadUrl do not handle elements in other locations
     */
    void setCenter(const QString& coordinateString);
    /**
     * @breif Get starting center with cooredinates
     * used if a scene downloadUrl do not handle elements in other locations
     * return A QVariantList of lon lat as specified in the dgml
     */
    QVariantList center() const;
    /**
     * @brief  Return a layer by its name
     * @param  name  The name of the layer
     * @return A pointer to the layer request by its name
     */
    GeoSceneLayer* layer(const QString& name);
    const GeoSceneLayer* layer(const QString& name) const;

    /**
     * @brief  Return all layers
     * @return A vector that contains pointers to all available layers
     */
    QVector<GeoSceneLayer*> layers() const;

    /**
     * @brief  Add a new filter to the map
     * @param  filter  The new filter
     */
    void addFilter(GeoSceneFilter*);

    /**
     * @brief  Return a filter by its name
     * @param  name  The name of the filter
     * @return A pointer to the filter request by its name
     */
    GeoSceneFilter* filter(const QString& name);

    /**
     * @brief  Return all filters
     * @return A vector that contains pointers to all available filters
     */
    QVector<GeoSceneFilter*> filters() const;

    /**
     * @brief  Checks for valid layers that contain texture data
     * @return Whether a texture layer got created internally
     *
     * NOTE: The existence of the file(s) that contain the actual data
     *       still needs to get checked at runtime!
     */
    bool hasTextureLayers() const;

    /**
     * @brief  Checks for valid layers that contain vector data
     * @return Whether a vector layer got created internally
     *
     * NOTE: The existence of the file(s) that contain the actual data
     *       still needs to get checked at runtime!
     */
    bool hasVectorLayers() const;

private:

    Q_DISABLE_COPY(GeoSceneMap)

private:

    GeoSceneMapPrivate* const d = nullptr;
};

} // namespace Marble

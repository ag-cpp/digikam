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

#include <QObject>
#include <QList>

// Local includes

#include "MarbleGlobal.h"
#include "digikam_export.h"

namespace Marble
{

class GeoGraphicsItem;
class GeoDataFeature;
class GeoDataLatLonBox;
class GeoGraphicsScenePrivate;
class GeoDataPlacemark;

/**
 * @short This is the home of all GeoGraphicsItems to be shown on the map.
 */
class DIGIKAM_EXPORT GeoGraphicsScene : public QObject
{
    Q_OBJECT

public:

    /**
     * Creates a new instance of GeoGraphicsScene
     * @param parent the QObject parent of the Scene
     */
    explicit GeoGraphicsScene(QObject* parent = nullptr);
    ~GeoGraphicsScene() override;

    /**
     * @brief Add an item to the GeoGraphicsScene
     * Adds the item @p item to the GeoGraphicsScene
     */
    void addItem(GeoGraphicsItem* item);

    /**
     * @brief Remove all concerned items from the GeoGraphicsScene
     * Removes all items which are associated with @p object from the GeoGraphicsScene
     */
    void removeItem(const GeoDataFeature* feature);

    /**
     * @brief Remove all items from the GeoGraphicsScene
     */
    void clear();

    /**
     * @brief Get the list of items in the specified Box
     *
     * @param box The box around the items.
     * @param maxZoomLevel The max zoom level of tiling
     * @return The list of items in the specified box in no specific order.
     */
    QList<GeoGraphicsItem*> items(const GeoDataLatLonBox& box, int maxZoomLevel) const;

    /**
     * @brief Get the list of items which belong to a placemark
     * that has been clicked.
     * @return Returns a list of selected Items
     */
    QList<GeoGraphicsItem*> selectedItems() const;

    void resetStyle();

public Q_SLOTS:

    void applyHighlight(const QVector<GeoDataPlacemark*>&);

Q_SIGNALS:

    void repaintNeeded();

private:

    GeoGraphicsScenePrivate* const d = nullptr;
};

} // Namespace Marble

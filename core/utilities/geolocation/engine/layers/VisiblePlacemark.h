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

//
// VisiblePlacemarks are those Placemarks which become visible on the map
//

#pragma once

// Qt includes

#include <QObject>
#include <QPixmap>
#include <QPoint>
#include <QRectF>

// Local includes

#include "GeoDataStyle.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

class GeoDataPlacemark;

static const qreal s_labelOutlineWidth = 2.5;

/**
 * @short A class which represents the visible place marks on a map.
 *
 * This class is used by PlacemarkLayout to pass the visible place marks
 * to the PlacemarkPainter.
 */
class VisiblePlacemark : public QObject
{
    Q_OBJECT

public:

    explicit VisiblePlacemark(const GeoDataPlacemark* placemark, const GeoDataCoordinates& coordinates, const GeoDataStyle::ConstPtr& style);

    /**
     * Returns the index of the place mark model which
     * is associated with this visible place mark.
     */
    const GeoDataPlacemark* placemark() const;

    /**
     * Returns the pixmap of the place mark symbol.
     */
    const QPixmap& symbolPixmap() const;

    /**
     * Returns the id for the place mark symbol.
     */
    const QString& symbolId() const;

    /**
     * Returns the state of the place mark.
     */
    bool selected() const;

    /**
     * Sets the state of the place mark.
     */
    void setSelected(bool selected);

    /**
     * Returns the position of the place mark symbol on the map.
     */
    const QPointF& symbolPosition() const;

    /**
     * Returns the top left corner of the place mark symbol's hot spot
     */
    const QPointF hotSpot() const;

    /**
     * Sets the @p position of the place mark symbol on the map.
     */
    void setSymbolPosition(const QPointF& position);

    /**
     * Returns the pixmap of the place mark name label.
     */
    const QPixmap& labelPixmap();

    /**
     * Returns the area covered by the place mark name label on the map.
     */
    const QRectF& labelRect() const;

    /**
     * Sets the @p area covered by the place mark name label on the map.
     */
    void setLabelRect(const QRectF& area);

    enum LabelStyle
    {
        Normal = 0,
        Glow,
        Selected
    };

    void setStyle(const GeoDataStyle::ConstPtr& style);

    GeoDataStyle::ConstPtr style() const;

    QRectF symbolRect() const;

    QRectF boundingBox() const;

    const GeoDataCoordinates& coordinates() const;

Q_SIGNALS:

    void updateNeeded();

private Q_SLOTS:

    void setSymbolPixmap();

private:

    static void drawLabelText(QPainter& labelPainter, const QString& text, const QFont& labelFont, LabelStyle labelStyle, const QColor& color);
    void drawLabelPixmap();

private:

    const GeoDataPlacemark* m_placemark = nullptr;

    // View stuff
    QPointF                 m_symbolPosition; // position of the placemark's symbol
    bool                    m_selected;       // state of the placemark
    QPixmap                 m_labelPixmap;    // the text label (most often name)
    bool                    m_labelDirty;
    QRectF                  m_labelRect;      // bounding box of label

    GeoDataStyle::ConstPtr  m_style;
    GeoDataCoordinates      m_coordinates;

    mutable QPixmap         m_symbolPixmap;   // cached value
    QString                 m_symbolId;
};

} // namespace Marble

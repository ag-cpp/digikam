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

#pragma once

// Qt includes

#include <QSharedPointer>
#include <QMetaType>

// Local includes

#include "GeoDataStyleSelector.h"
#include "digikam_export.h"

class QFont;
class QColor;

namespace Marble
{

class GeoDataBalloonStyle;
class GeoDataIconStyle;
class GeoDataLabelStyle;
class GeoDataLineStyle;
class GeoDataListStyle;
class GeoDataPolyStyle;
class GeoDataStylePrivate;

/**
 * @short an addressable style group
 *
 * A GeoDataStyle defines an addressable style group that can be
 * referenced by GeoDataStyleMaps and GeoDataFeatures. GeoDataStyles
 * affect how Geometry is presented in the 3D viewer (not yet
 * implemented) and how Features appear. Shared styles are collected
 * in a GeoDataDocument and must have an id defined for them so that
 * they can be referenced by the individual Features that use them.
 *
 * @see GeoDataIconStyle
 * @see GeoDataLabelStyle
 * @see GeoDataLineStyle
 * @see GeoDataPolyStyle
 * @see GeoDataBalloonStyle
 * @see GeoDataListStyle
 */
class DIGIKAM_EXPORT GeoDataStyle : public GeoDataStyleSelector
{
public:

    using Ptr      = QSharedPointer<GeoDataStyle>;
    using ConstPtr = QSharedPointer<const GeoDataStyle>;

    /// Construct a default style
    GeoDataStyle();
    GeoDataStyle(const GeoDataStyle& other);

    /**
     * @brief Construct a new style
     * @param  iconPath used to construct the icon style
     * @param  font   used to construct the label styles
     * @param  color  used to construct the label styles
     */
    GeoDataStyle(const QString& iconPath,
                 const QFont& font, const QColor& color);
    ~GeoDataStyle() override;

    /// Provides type information for downcasting a GeoNode
    const char* nodeType() const override;

    /// set the icon style
    void setIconStyle(const GeoDataIconStyle& style);
    /// Return the icon style of this style
    GeoDataIconStyle& iconStyle();
    const GeoDataIconStyle& iconStyle() const;
    /// set the label style
    void setLabelStyle(const GeoDataLabelStyle& style);
    /// Return the label style of this style
    GeoDataLabelStyle& labelStyle();
    const GeoDataLabelStyle& labelStyle() const;
    /// set the line style
    void setLineStyle(const GeoDataLineStyle& style);
    /// Return the label style of this style
    GeoDataLineStyle& lineStyle();
    const GeoDataLineStyle& lineStyle() const;
    /// set the poly style
    void setPolyStyle(const GeoDataPolyStyle& style);
    /// Return the label style of this style
    GeoDataPolyStyle& polyStyle();
    const GeoDataPolyStyle& polyStyle() const;
    /// set the balloon style
    void setBalloonStyle(const GeoDataBalloonStyle& style);
    /// Return the balloon style of this style
    GeoDataBalloonStyle& balloonStyle();
    const GeoDataBalloonStyle& balloonStyle() const;
    /// set the list style
    void setListStyle(const GeoDataListStyle& style);
    /// Return the list style of this style
    GeoDataListStyle& listStyle();
    const GeoDataListStyle& listStyle() const;

    /**
    * @brief assignment operator
    * @param other the GeoDataStyle that gets duplicated
    */
    GeoDataStyle& operator=(const GeoDataStyle& other);

    bool operator==(const GeoDataStyle& other) const;
    bool operator!=(const GeoDataStyle& other) const;

    /**
     * @brief Serialize the style to a stream
     * @param  stream  the stream
     */
    void pack(QDataStream& stream) const override;
    /**
     * @brief  Unserialize the style from a stream
     * @param  stream  the stream
     */
    void unpack(QDataStream& stream) override;

private:

    GeoDataStylePrivate* const d = nullptr;
};

} // namespace Marble

Q_DECLARE_METATYPE(Marble::GeoDataStyle*)
Q_DECLARE_METATYPE(const Marble::GeoDataStyle*)

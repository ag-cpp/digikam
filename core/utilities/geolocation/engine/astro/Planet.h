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

#include <QtGlobal>
#include <QDateTime>
#include <QStringList>
#include <QColor>

// Local includes

#include "digikam_export.h"

namespace Marble
{

class PlanetPrivate;

class DIGIKAM_EXPORT Planet
{

    // Not a QObject because we don't need sigs/slots

public:

    /** Constructor to use for a custom planet
     * All of the orbital elements are left empty, so you can fill them in yourself.
     */
    Planet();

    /**
     * @deprecated Please use PlanetFactory::construct(id) instead.
     */
    explicit Planet(const QString& id);

    ///Copy Constructor
    Planet(const Planet& other);
    ///Destructor
    ~Planet();

    /* Getter functions */
    /// for calculating mean anomaly
    qreal M_0() const;
    qreal M_1() const;

    /// for calculating equation of center
    qreal C_1() const;
    qreal C_2() const;
    qreal C_3() const;
    qreal C_4() const;
    qreal C_5() const;
    qreal C_6() const;

    /// ecliptic longitude of the perihelion
    qreal Pi() const;

    /// obliquity of the ecliptic plane
    qreal epsilon() const;

    /// for calculating sidereal time
    qreal theta_0() const;
    qreal theta_1() const;

    /// the radius of the planet, in metres
    qreal radius() const;

    /// the twilight zone of the planet, in radians
    qreal twilightZone() const;

    /** The user visible name of the planet */
    QString name() const;
    /** The internal, nonlocalized name of the planet */
    QString id() const;

    /**
     * Fills the longitude and latitude with the planet's sun position.
     *
     * @param lon the longitude of the sun, in radian
     * @param lat the latitude of the sun, in radian
     * @param dateTime the time for which the sun position is to be calculated
     */
    void sunPosition(qreal& lon, qreal& lat, const QDateTime& dateTime) const;

    /* Setter functions */
    void setM_0(qreal M_0);
    void setM_1(qreal M_1);

    void setC_1(qreal C_1);
    void setC_2(qreal C_2);
    void setC_3(qreal C_3);
    void setC_4(qreal C_4);
    void setC_5(qreal C_5);
    void setC_6(qreal C_6);

    void setPi(qreal Pi);

    void setEpsilon(qreal epsilon);

    void setTheta_0(qreal theta_0);
    void setTheta_1(qreal theta_1);

    void setRadius(qreal radius);

    void setTwilightZone(qreal twilightZone);

    void setName(const QString& name);
    void setId(const QString& id);

    Planet& operator=(const Planet& rhs);

    /**
     * @deprecated Please use PlanetFactory::localizedName(id) instead.
     */
    static QString name(const QString& id);
    /**
     * @deprecated Please use PlanetFactory::planetList() instead.
     */
    static QStringList planetList();

    bool hasAtmosphere() const;
    void setHasAtmosphere(bool enabled);

    QColor atmosphereColor() const;
    void setAtmosphereColor(const QColor& color);

private:

    PlanetPrivate* const d = nullptr;
};

} // namespace Marble

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

#include "Planet.h"

// Qt includes

#include <QString>

// Local includes

#include "PlanetFactory.h"
#include "MarbleGlobal.h"
#include "MarbleColors.h"
#include "solarsystem.h"
#include "digikam_debug.h"

namespace Marble
{

class Q_DECL_HIDDEN PlanetPrivate
{
public:

    qreal   M_0, M_1;                       // for calculating mean anomaly
    qreal   C_1, C_2, C_3, C_4, C_5, C_6;   // for calculating equation of center
    qreal   Pi;                             // ecliptic longitude of the perihelion
    qreal   epsilon;                        // obliquity of the ecliptic plane
    qreal   theta_0, theta_1;               // for calculating sidereal time
    qreal   radius;                         // in metres
    qreal   twilightZone;
    QString name, id;                       // localized and nonlocalized names
    bool    atmosphere;
    QColor  atmosphereColor;

    PlanetPrivate()
      : M_0(0.0),
        M_1(0.0),
        C_1(0.0),
        C_2(0.0),
        C_3(0.0),
        C_4(0.0),
        C_5(0.0),
        C_6(0.0),
        Pi(0.0),
        epsilon(0.0),
        theta_0(0.0),
        theta_1(0.0),
        radius(10000000.0),
        twilightZone(0),
        name(),
        id(),
        atmosphere(false)
    {
        // nothing to do
    }
};

//Constructor
Planet::Planet()
    : d(new PlanetPrivate)
{
    // nothing to do
}

Planet::Planet(const QString& id)
    : d(new PlanetPrivate)
{
    *this = PlanetFactory::construct(id);
}

//Copy Constructor
Planet::Planet(const Planet& other)
    : d(new PlanetPrivate)
{
    // PlanetPrivate does not have pointer members, so we can just
    // use its (compiler generated) assignment operator.
    *d = *other.d;
}

//Destructor
Planet::~Planet()
{
    delete d;
}

/* Getter functions */

// for calculating mean anomaly
qreal Planet::M_0() const
{
    return d->M_0;
}

qreal Planet::M_1() const
{
    return d->M_1;
}

// for calculating equation of center
qreal Planet::C_1() const
{
    return d->C_1;
}

qreal Planet::C_2() const
{
    return d->C_2;
}

qreal Planet::C_3() const
{
    return d->C_3;
}

qreal Planet::C_4() const
{
    return d->C_4;
}

qreal Planet::C_5() const
{
    return d->C_5;
}

qreal Planet::C_6() const
{
    return d->C_6;
}

// ecliptic longitude of the perihelion
qreal Planet::Pi() const
{
    return d->Pi;
}

// obliquity of the ecliptic plane
qreal Planet::epsilon() const
{
    return d->epsilon;
}

// for calculating sidereal time
qreal Planet::theta_0() const
{
    return d->theta_0;
}

qreal Planet::theta_1() const
{
    return d->theta_1;
}

// the radius of the planet, in metres
qreal Planet::radius() const
{
    return d->radius;
}

qreal Planet::twilightZone() const
{
    return d->twilightZone;
}

QString Planet::name() const
{
    return d->name;
}

QString Planet::id() const
{
    return d->id;
}

void Planet::sunPosition(qreal& lon, qreal& lat, const QDateTime& dateTime) const
{
    SolarSystem sys;
    sys.setCurrentMJD(
        dateTime.date().year(), dateTime.date().month(), dateTime.date().day(),
        dateTime.time().hour(), dateTime.time().minute(),
        (double)dateTime.time().second());
    const QString pname = d->id.at(0).toUpper() + d->id.right(d->id.size() - 1);
    QByteArray name = pname.toLatin1();
    sys.setCentralBody(name.data());

    double ra = 0.0;
    double decl = 0.0;
    sys.getSun(ra, decl);

    double _lon = 0.0;
    double _lat = 0.0;
    sys.getPlanetographic(ra, decl, _lon, _lat);

    lon = _lon * DEG2RAD;
    lat = _lat * DEG2RAD;
}

/* Setter functions */

void Planet::setM_0(qreal M_0)
{
    d->M_0 = M_0;
}

void Planet::setM_1(qreal M_1)
{
    d->M_1 = M_1;
}

void Planet::setC_1(qreal C_1)
{
    d->C_1 = C_1;
}

void Planet::setC_2(qreal C_2)
{
    d->C_2 = C_2;
}

void Planet::setC_3(qreal C_3)
{
    d->C_3 = C_3;
}

void Planet::setC_4(qreal C_4)
{
    d->C_4 = C_4;
}

void Planet::setC_5(qreal C_5)
{
    d->C_5 = C_5;
}

void Planet::setC_6(qreal C_6)
{
    d->C_6 = C_6;
}

void Planet::setPi(qreal Pi)
{
    d->Pi = Pi;
}

void Planet::setEpsilon(qreal epsilon)
{
    d->epsilon = epsilon;
}

void Planet::setTheta_0(qreal theta_0)
{
    d->theta_0 = theta_0;
}

void Planet::setTheta_1(qreal theta_1)
{
    d->theta_1 = theta_1;
}

void Planet::setRadius(qreal radius)
{
    d->radius = radius;
}

void Planet::setTwilightZone(qreal twilightZone)
{
    d->twilightZone = twilightZone;
}

void Planet::setName(const QString& name)
{
    d->name = name;
}

void Planet::setId(const QString& id)
{
    d->id = id;
}

QString Planet::name(const QString& id)
{
    return PlanetFactory::localizedName(id);
}

QStringList Planet::planetList()
{
    return PlanetFactory::planetList();
}

Planet& Planet::operator=(const Planet& rhs)
{
    // PlanetPrivate does not have pointer members, so we can just
    // use its (compiler generated) assignment operator.
    *d = *rhs.d;
    return *this;
}

bool Planet::hasAtmosphere() const
{
    return d->atmosphere;
}

void Planet::setHasAtmosphere(bool enabled)
{
    d->atmosphere = enabled;
}

QColor Planet::atmosphereColor() const
{
    return d->atmosphereColor;
}

void Planet::setAtmosphereColor(const QColor& color)
{
    d->atmosphereColor = color;
}

} // namespace Marble


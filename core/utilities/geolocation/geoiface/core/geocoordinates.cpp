/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-16
 * Description : GeoCoordinates class
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "geocoordinates.h"

// Marble includes

#ifdef HAVE_GEOLOCATION

#   include "GeoDataCoordinates.h"

#endif

namespace Digikam
{

GeoCoordinates::GeoCoordinates(const double inLat,
                               const double inLon)
    : m_lat     (inLat),
      m_lon     (inLon),
      m_hasFlags(HasCoordinates)
{
}

GeoCoordinates::GeoCoordinates(const double inLat,
                               const double inLon,
                               const double inAlt)
    : m_lat     (inLat),
      m_lon     (inLon),
      m_alt     (inAlt),
      m_hasFlags(HasCoordinates | HasAltitude)
{
}

bool GeoCoordinates::operator==(const GeoCoordinates& other) const
{
    return (
            (hasCoordinates() == other.hasCoordinates())  &&
            (hasCoordinates() ? ((lat() == other.lat()) && (lon() == other.lon()))
                              : true)                     &&
            (hasAltitude()    == other.hasAltitude())     &&
            (hasAltitude()    ? (alt() == other.alt())
                              : true)
           );
}

double GeoCoordinates::lat() const
{
    return m_lat;
}

double GeoCoordinates::lon() const
{
    return m_lon;
}

double GeoCoordinates::alt() const
{
    return m_alt;
}

bool GeoCoordinates::hasCoordinates() const
{
    return m_hasFlags.testFlag(HasCoordinates);
}

bool GeoCoordinates::hasLatitude() const
{
    return m_hasFlags.testFlag(HasLatitude);
}

bool GeoCoordinates::hasLongitude() const
{
    return m_hasFlags.testFlag(HasLongitude);
}

void GeoCoordinates::setLatLon(const double inLat,
                               const double inLon)
{
    m_lat       = inLat;
    m_lon       = inLon;
    m_hasFlags |= HasCoordinates;
}

bool GeoCoordinates::hasAltitude()  const
{
    return m_hasFlags.testFlag(HasAltitude);
}

GeoCoordinates::HasFlags GeoCoordinates::hasFlags() const
{
    return m_hasFlags;
}

void GeoCoordinates::setAlt(const double inAlt)
{
    m_hasFlags |= HasAltitude;
    m_alt       = inAlt;
}

void GeoCoordinates::clearAlt()
{
    m_hasFlags &= ~HasAltitude;
}

void GeoCoordinates::clear()
{
    m_hasFlags = HasNothing;
}

QString GeoCoordinates::altString() const
{
    return (m_hasFlags.testFlag(HasAltitude)  ? QString::number(m_alt, 'g', 12)
                                              : QString());
}

QString GeoCoordinates::latString() const
{
    return (m_hasFlags.testFlag(HasLatitude)  ? QString::number(m_lat, 'g', 12)
                                              : QString());
}

QString GeoCoordinates::lonString() const
{
    return (m_hasFlags.testFlag(HasLongitude) ? QString::number(m_lon, 'g', 12)
                                              : QString());
}

QString GeoCoordinates::geoUrl() const
{
    if (!hasCoordinates())
    {
        return QString();
    }

    if (m_hasFlags.testFlag(HasAltitude))
    {
        return QString::fromLatin1("geo:%1,%2,%3").arg(latString()).arg(lonString()).arg(altString());
    }
    else
    {
        return QString::fromLatin1("geo:%1,%2").arg(latString()).arg(lonString());
    }
}

bool GeoCoordinates::sameLonLatAs(const GeoCoordinates& other) const
{
    return (
            m_hasFlags.testFlag(HasCoordinates)       &&
            other.m_hasFlags.testFlag(HasCoordinates) &&
            (m_lat == other.m_lat)                    &&
            (m_lon == other.m_lon)
           );
}

GeoCoordinates GeoCoordinates::fromGeoUrl(const QString& url,
                                          bool* const parsedOkay)
{
    // parse geo:-uri according to (only partially implemented):
    // tools.ietf.org/html/draft-ietf-geopriv-geo-uri-04
    // TODO: verify that we follow the spec fully!

    if (!url.startsWith(QLatin1String("geo:")))
    {
        // TODO: error

        if (parsedOkay)
        {
            *parsedOkay = false;
        }

        return GeoCoordinates();
    }

    const QStringList parts = url.mid(4).split(QLatin1Char(','));

    GeoCoordinates position;

    if ((parts.size() == 3) || (parts.size() == 2))
    {
        bool okay               = true;
        double ptLongitude      = 0.0;
        double ptAltitude       = 0.0;
        const bool hasAltitude  = (parts.size() == 3);
        const double ptLatitude = parts[0].toDouble(&okay);

        if (okay)
        {
            ptLongitude = parts[1].toDouble(&okay);
        }

        if (okay && (hasAltitude))
        {
            ptAltitude = parts[2].toDouble(&okay);
        }

        if (!okay)
        {
            *parsedOkay = false;

            return GeoCoordinates();
        }

        position = GeoCoordinates(ptLatitude, ptLongitude);

        if (hasAltitude)
        {
            position.setAlt(ptAltitude);
        }
    }
    else
    {
        if (parsedOkay)
        {
            *parsedOkay = false;
        }

        return GeoCoordinates();
    }

    if (parsedOkay)
    {
        *parsedOkay = true;
    }

    return position;
}

GeoCoordinates::Pair GeoCoordinates::makePair(const qreal lat1,
                                              const qreal lon1,
                                              const qreal lat2,
                                              const qreal lon2)
{
    return Pair(GeoCoordinates(lat1, lon1), GeoCoordinates(lat2, lon2));
}

#ifdef HAVE_GEOLOCATION

Marble::GeoDataCoordinates GeoCoordinates::toMarbleCoordinates() const
{
    Marble::GeoDataCoordinates marbleCoordinates;
    marbleCoordinates.setLongitude(lon(), Marble::GeoDataCoordinates::Degree);
    marbleCoordinates.setLatitude(lat(),  Marble::GeoDataCoordinates::Degree);

    if (hasAltitude())
    {
        marbleCoordinates.setAltitude(alt());
    }

    return marbleCoordinates;
}

GeoCoordinates GeoCoordinates::fromMarbleCoordinates(const Marble::GeoDataCoordinates& marbleCoordinates)
{
    /// @TODO looks like Marble does not differentiate between having and not having altitude..

    return GeoCoordinates(marbleCoordinates.latitude(Marble::GeoDataCoordinates::Degree),
                          marbleCoordinates.longitude(Marble::GeoDataCoordinates::Degree),
                          marbleCoordinates.altitude());
}

#endif // HAVE_GEOLOCATION

} // namespace Digikam

QDebug operator<<(QDebug debug, const Digikam::GeoCoordinates& coordinate)
{
    debug << coordinate.geoUrl();

    return debug;
}

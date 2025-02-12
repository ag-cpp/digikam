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

#include "GeoDataLink.h"

// Local includes

#include "GeoDataTypes.h"

namespace Marble
{

class Q_DECL_HIDDEN GeoDataLinkPrivate
{
public:

    QString                         m_href;
    GeoDataLink::RefreshMode        m_refreshMode;
    qreal                           m_refreshInterval;
    GeoDataLink::ViewRefreshMode    m_viewRefreshMode;
    qreal                           m_viewRefreshTime;
    qreal                           m_viewBoundScale;
    QString                         m_viewFormat;
    QString                         m_httpQuery;

    GeoDataLinkPrivate();
};

GeoDataLinkPrivate::GeoDataLinkPrivate() :
    m_href(),
    m_refreshMode(GeoDataLink::OnChange),
    m_refreshInterval(4.0),
    m_viewRefreshMode(GeoDataLink::Never),
    m_viewRefreshTime(4.0),
    m_viewBoundScale(1.0),
    m_viewFormat(QLatin1String("BBOX=[bboxWest],[bboxSouth],[bboxEast],[bboxNorth]")),
    m_httpQuery()
{
    // nothing to do
}

GeoDataLink::GeoDataLink() : d(new GeoDataLinkPrivate)
{
    // nothing to do
}

GeoDataLink::GeoDataLink(const Marble::GeoDataLink& other) :
    GeoDataObject(other), d(new GeoDataLinkPrivate(*other.d))
{
    // nothing to do
}

GeoDataLink& GeoDataLink::operator=(const GeoDataLink& other)
{
    GeoDataObject::operator=(other);
    *d = *other.d;
    return *this;
}

bool GeoDataLink::operator==(const GeoDataLink& other) const
{
    return equals(other) &&
           d->m_href == other.d->m_href &&
           d->m_refreshMode == other.d->m_refreshMode &&
           d->m_refreshInterval == other.d->m_refreshInterval &&
           d->m_viewRefreshMode == other.d->m_viewRefreshMode &&
           d->m_viewRefreshTime == other.d->m_viewRefreshTime &&
           d->m_viewBoundScale == other.d->m_viewBoundScale &&
           d->m_viewFormat == other.d->m_viewFormat &&
           d->m_httpQuery == other.d->m_httpQuery;
}

bool GeoDataLink::operator!=(const GeoDataLink& other) const
{
    return !this->operator==(other);
}

GeoDataLink::~GeoDataLink()
{
    delete d;
}

const char* GeoDataLink::nodeType() const
{
    return GeoDataTypes::GeoDataLinkType;
}

QString GeoDataLink::href() const
{
    return d->m_href;
}

void GeoDataLink::setHref(const QString& href)
{
    d->m_href = href;
}

GeoDataLink::RefreshMode GeoDataLink::refreshMode() const
{
    return d->m_refreshMode;
}

void GeoDataLink::setRefreshMode(RefreshMode refreshMode)
{
    d->m_refreshMode = refreshMode;
}

qreal GeoDataLink::refreshInterval() const
{
    return d->m_refreshInterval;
}

void GeoDataLink::setRefreshInterval(qreal refreshInterval)
{
    d->m_refreshInterval = refreshInterval;
}

GeoDataLink::ViewRefreshMode GeoDataLink::viewRefreshMode() const
{
    return d->m_viewRefreshMode;
}

void GeoDataLink::setViewRefreshMode(ViewRefreshMode viewRefreshMode)
{
    d->m_viewRefreshMode = viewRefreshMode;
}

qreal GeoDataLink::viewRefreshTime() const
{
    return d->m_viewRefreshTime;
}

void GeoDataLink::setViewRefreshTime(qreal viewRefreshTime)
{
    d->m_viewRefreshTime = viewRefreshTime;
}

qreal GeoDataLink::viewBoundScale() const
{
    return d->m_viewBoundScale;
}

void GeoDataLink::setViewBoundScale(qreal viewBoundScale)
{
    d->m_viewBoundScale = viewBoundScale;
}

QString GeoDataLink::viewFormat() const
{
    return d->m_viewFormat;
}

void GeoDataLink::setViewFormat(const QString& viewFormat)
{
    d->m_viewFormat = viewFormat;
}

QString GeoDataLink::httpQuery() const
{
    return d->m_httpQuery;
}
void GeoDataLink::setHttpQuery(const QString& httpQuery)
{
    d->m_httpQuery = httpQuery;
}

} // namespace Marble

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-04-30
 * Description : Class for geonames.org based altitude lookup
 *
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_LOOKUP_ALTITUDE_GEONAMES_H
#define DIGIKAM_LOOKUP_ALTITUDE_GEONAMES_H

// Qt includes

#include <QNetworkReply>

// Local includes

#include "lookupaltitude.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT LookupAltitudeGeonames : public LookupAltitude
{
    Q_OBJECT

public:

    explicit LookupAltitudeGeonames(QObject* const parent);
    virtual ~LookupAltitudeGeonames();

    virtual QString backendName()                           const override;
    virtual QString backendHumanName()                      const override;

    virtual void addRequests(const Request::List& requests)       override;
    virtual Request::List getRequests()                     const override;
    virtual Request getRequest(const int index)             const override;

    virtual void startLookup()                                    override;
    virtual StatusAltitude getStatus()                      const override;
    virtual QString errorMessage()                          const override;
    virtual void cancel()                                         override;

private Q_SLOTS:

    void slotFinished(QNetworkReply* reply);

private:

    void startNextRequest();

private:

    class Private;
    const QScopedPointer<Private> d;
};

} // namespace Digikam

#endif // DIGIKAM_LOOKUP_ALTITUDE_GEONAMES_H

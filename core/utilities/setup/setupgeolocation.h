/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-24
 * Description : geo-location setup page.
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SETUP_GEOLOCATION_H
#define DIGIKAM_SETUP_GEOLOCATION_H

// Qt includes

#include <QScrollArea>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT SetupGeolocation : public QScrollArea
{
    Q_OBJECT

public:

    enum GeolocationTab
    {
        View = 0,
        Plugins,
        GoogleMaps
    };

public:

    explicit SetupGeolocation(QWidget* const parent = nullptr);
    ~SetupGeolocation() override;

    void applySettings();

    void setActiveTab(GeolocationTab tab);
    GeolocationTab activeTab() const;

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_GEOLOCATION_H

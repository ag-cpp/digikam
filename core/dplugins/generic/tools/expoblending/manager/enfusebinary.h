/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect enfuse binary program and version
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_ENFUSE_BINARY_H
#define DIGIKAM_ENFUSE_BINARY_H

// Local includes

#include "dbinaryiface.h"

using namespace Digikam;

namespace DigikamGenericExpoBlendingPlugin
{

class EnfuseBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit EnfuseBinary()
        : DBinaryIface(QLatin1String("enfuse"),
                       QLatin1String("3.2"),
                       QLatin1String("enfuse "),
                       0,
                       QLatin1String("Enblend"),
                       QLatin1String("https://sourceforge.net/projects/enblend/files/latest/download"),
                       QLatin1String("ExpoBlending"),
                       QStringList(QLatin1String("-V"))),
                       versionDouble(0)
    {
        setup();
    }

    ~EnfuseBinary() override
    {
    }

    double getVersion() const;

Q_SIGNALS:

    void signalEnfuseVersion(double version);

protected:

    bool parseHeader(const QString& output) override;

private:

    double versionDouble;

private:

    // Disable
    explicit EnfuseBinary(QObject*) = delete;
};

} // namespace DigikamGenericExpoBlendingPlugin

#endif // DIGIKAM_ENFUSE_BINARY_H

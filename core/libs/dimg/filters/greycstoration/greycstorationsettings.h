/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-09-13
 * Description : Greycstoration settings widgets
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_GREY_CSTORATION_SETTINGS_H
#define DIGIKAM_GREY_CSTORATION_SETTINGS_H

// Qt includes

#include <QObject>
#include <QFile>
#include <QString>

// Local includes

#include "digikam_export.h"
#include "greycstorationfilter.h"

class QTabWidget;

namespace Digikam
{

class DIGIKAM_EXPORT GreycstorationSettings : public QObject
{
    Q_OBJECT

public:

    explicit GreycstorationSettings(QTabWidget* const parent);
    ~GreycstorationSettings() override;

    void setSettings(const GreycstorationContainer& settings);
    GreycstorationContainer settings() const;

    void setDefaultSettings(const GreycstorationContainer& settings);

    bool loadSettings(QFile& file, const QString& header);
    void saveSettings(QFile& file, const QString& header);

    void setEnabled(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_GREY_CSTORATION_SETTINGS_H

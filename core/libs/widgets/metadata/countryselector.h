/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-07-07
 * Description : country selector combo-box.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_COUNTRY_SELECTOR_H
#define DIGIKAM_COUNTRY_SELECTOR_H

// Qt includes

#include <QString>
#include <QWidget>
#include <QComboBox>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT CountrySelector : public QComboBox
{
    Q_OBJECT

public:

    explicit CountrySelector(QWidget* const parent);
    ~CountrySelector() override;

    void setCountry(const QString& countryCode);
    bool country(QString& countryCode, QString& countryName) const;

public:

    static QString countryForCode(const QString& countryCode);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_COUNTRY_SELECTOR_H

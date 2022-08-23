/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality settings widget
 *
 * Copyright (C) 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
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

#ifndef DIGIKAM_IMAGE_QUALITY_SETTINGS_H
#define DIGIKAM_IMAGE_QUALITY_SETTINGS_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "imagequalitycontainer.h"

namespace Digikam
{

class DIGIKAM_EXPORT ImageQualitySettings : public QWidget
{
    Q_OBJECT

public:

    explicit ImageQualitySettings(QWidget* const parent = nullptr);
    ~ImageQualitySettings() override;

    void applySettings();
    void readSettings();
    ImageQualityContainer getImageQualityContainer() const;

private Q_SLOTS:
    void setDisableOptionViews(bool b);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_IMAGE_QUALITY_SETTINGS_H

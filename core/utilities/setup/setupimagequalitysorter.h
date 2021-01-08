/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality setup page
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SETUP_IMAGE_QUALITY_SORTER_H
#define DIGIKAM_SETUP_IMAGE_QUALITY_SORTER_H

// Qt includes

#include <QScrollArea>

namespace Digikam
{

class SetupImageQualitySorter : public QScrollArea
{
    Q_OBJECT

public:

    explicit SetupImageQualitySorter(QWidget* const parent = nullptr);
    ~SetupImageQualitySorter() override;

    void applySettings();

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_IMAGE_QUALITY_SORTER_H

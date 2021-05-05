/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-06
 * Description : setup tab for image versioning
 *
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SETUP_VERSIONING_H
#define DIGIKAM_SETUP_VERSIONING_H

// Qt includes

#include <QScrollArea>

namespace Digikam
{

class SetupVersioning : public QScrollArea
{
    Q_OBJECT

public:

    explicit SetupVersioning(QWidget* const parent = nullptr);
    ~SetupVersioning() override;

    void applySettings();

private:

    void readSettings();

private Q_SLOTS:

    void showNonDestructiveInformation();
    void showFormatInformation();
    void showSnapshotInformation();
    void showViewInformation();
    void enableToggled(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_VERSIONING_H

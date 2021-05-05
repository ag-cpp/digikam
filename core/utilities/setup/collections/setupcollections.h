/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-02-01
 * Description : collections setup tab
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SETUP_COLLECTIONS_H
#define DIGIKAM_SETUP_COLLECTIONS_H

// Qt includes

#include <QScrollArea>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT SetupCollections : public QScrollArea
{
    Q_OBJECT

public:

    enum CollectionsTab
    {
        Collections = 0,
        IgnoreDirs
    };

public:

    explicit SetupCollections(QWidget* const parent = nullptr);
    ~SetupCollections() override;

    void applySettings();

private:

    void readSettings();

private Q_SLOTS:

    void slotShowCurrentIgnoredDirectoriesSettings() const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_COLLECTIONS_H

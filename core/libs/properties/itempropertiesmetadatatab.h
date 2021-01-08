/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-17
 * Description : a tab to display item metadata information
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_PROPERTIES_METADATA_TAB_H
#define DIGIKAM_ITEM_PROPERTIES_METADATA_TAB_H

// Qt includes

#include <QWidget>
#include <QTabWidget>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "dmetadata.h"

namespace Digikam
{

class DIGIKAM_EXPORT ItemPropertiesMetadataTab : public QTabWidget
{
    Q_OBJECT

public:

    explicit ItemPropertiesMetadataTab(QWidget* const parent);
    ~ItemPropertiesMetadataTab() override;

    void setCurrentURL(const QUrl& url = QUrl());
    void setCurrentData(DMetadata* const metadata = nullptr,
                        const QString& filename = QString());

    void loadFilters();

    void readSettings(const KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalSetupMetadataFilters(int);

private Q_SLOTS:

    void slotSetupMetadataFilters();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_PROPERTIES_METADATA_TAB_H

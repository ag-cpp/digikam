/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-21
 * Description : metadatahub manager
 *
 * Copyright (C) 2015 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
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

#ifndef DIGIKAM_METADATA_HUB_MNGR_H
#define DIGIKAM_METADATA_HUB_MNGR_H

// Qt includes

#include <QPointer>

namespace Digikam
{

class ItemInfo;

class MetadataHubMngr : public QObject
{
    Q_OBJECT

public:

    static MetadataHubMngr* instance();
    ~MetadataHubMngr() override;

    static QPointer<MetadataHubMngr> internalPtr;
    static bool                      isCreated();

    void addPending(const ItemInfo& info);
    void requestShutDown();

Q_SIGNALS:

    void signalPendingMetadata(int numbers);

public Q_SLOTS:

    void slotApplyPending();

private:

    // Disable
    MetadataHubMngr();
    explicit MetadataHubMngr(QObject*);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_METADATA_HUB_MNGR_H

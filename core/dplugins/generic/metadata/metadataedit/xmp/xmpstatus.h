/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-10-24
 * Description : XMP workflow status settings page.
 *
 * SPDX-FileCopyrightText: 2007-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidget>

// Local includes

#include "dmetadata.h"
#include "metadataeditpage.h"

using namespace Digikam;

namespace DigikamGenericMetadataEditPlugin
{

class XMPStatus : public MetadataEditPage
{
    Q_OBJECT

public:

    explicit XMPStatus(QWidget* const parent);
    ~XMPStatus() override;

    void applyMetadata(const DMetadata& meta);
    void readMetadata(const DMetadata& meta);

Q_SIGNALS:

    void signalModified();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamGenericMetadataEditPlugin

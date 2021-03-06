/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-08
 * Description : A tab to display import item information
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMPORT_ITEM_PROPERTIES_TAB_H
#define DIGIKAM_IMPORT_ITEM_PROPERTIES_TAB_H

// Qt includes

#include <QString>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "dmetadata.h"
#include "camiteminfo.h"
#include "dexpanderbox.h"

namespace Digikam
{

class ImportItemPropertiesTab : public DExpanderBox
{
    Q_OBJECT

public:

    explicit ImportItemPropertiesTab(QWidget* const parent);
    ~ImportItemPropertiesTab() override;

    void setCurrentItem(const CamItemInfo& itemInfo = CamItemInfo(),
                        DMetadata* const meta = nullptr);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_IMPORT_ITEM_PROPERTIES_TAB_H

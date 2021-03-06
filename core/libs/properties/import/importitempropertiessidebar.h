/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-08
 * Description : item properties side bar used by import tool.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_IMPORT_ITEM_PROPERTIES_SIDEBAR_H
#define DIGIKAM_IMPORT_ITEM_PROPERTIES_SIDEBAR_H

// Qt includes

#include <QUrl>
#include <QWidget>

// Local includes

#include "sidebar.h"
#include "digikam_export.h"

namespace Digikam
{

class SidebarSplitter;
class CamItemInfo;
class DMetadata;

class DIGIKAM_GUI_EXPORT ImportItemPropertiesSideBarImport : public Sidebar
{
    Q_OBJECT

public:

    explicit ImportItemPropertiesSideBarImport(QWidget* const parent,
                                          SidebarSplitter* const splitter,
                                          Qt::Edge side = Qt::LeftEdge,
                                          bool mimimizedDefault = false);
    ~ImportItemPropertiesSideBarImport() override;

    void applySettings();

    void itemChanged(const CamItemInfo& itemInfo, const DMetadata& meta);

    QUrl url() const;

public Q_SLOTS:

    virtual void slotNoCurrentItem();

Q_SIGNALS:

    void signalFirstItem();
    void signalPrevItem();
    void signalNextItem();
    void signalLastItem();

protected:

    /**
     * load the last view state from disk - called by StateSavingObject#loadState()
     */
    void doLoadState() override;

    /**
     * save the view state to disk - called by StateSavingObject#saveState()
     */
    void doSaveState() override;

private Q_SLOTS:

    virtual void slotChangedTab(QWidget* tab);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_IMPORT_ITEM_PROPERTIES_SIDEBAR_H

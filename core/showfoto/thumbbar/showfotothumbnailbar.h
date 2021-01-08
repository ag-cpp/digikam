/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 02-08-2013
 * Description : Thumbnail bar for Showfoto
 *
 * Copyright (C) 2013      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef SHOW_FOTO_THUMB_NAIL_BAR_H
#define SHOW_FOTO_THUMB_NAIL_BAR_H

// Local Includes

#include "showfotocategorizedview.h"

namespace ShowFoto
{

class ShowfotoItemViewToolTip;

class ShowfotoThumbnailBar : public ShowfotoCategorizedView
{
    Q_OBJECT

public:

    explicit ShowfotoThumbnailBar(QWidget* const parent = nullptr);
    ~ShowfotoThumbnailBar() override;

    /**
     * This installs a duplicate filter model, if the ShwofotoItemModel may contain duplicates.
     * Otherwise, just use setModels().
     */
    void setModelsFiltered(ShowfotoItemModel* model, ShowfotoSortFilterModel* filterModel);

    QModelIndex nextIndex(const QModelIndex& index)     const;
    QModelIndex previousIndex(const QModelIndex& index) const;
    QModelIndex firstIndex()                            const;
    QModelIndex lastIndex()                             const;

    /// Sets the policy always for the one scroll bar which is relevant, depending on orientation
    void setScrollBarPolicy(Qt::ScrollBarPolicy policy);
    void setFlow(QListView::Flow newFlow);

    ShowfotoItemInfo findItemByUrl(const QUrl& url);

    void installOverlays();

public Q_SLOTS:

    void slotDockLocationChanged(Qt::DockWidgetArea area);

protected:

    void slotSetupChanged() override;
    bool event(QEvent*)     override;

private:

    // Hidden copy constructor and assignment operator.
    ShowfotoThumbnailBar(const ShowfotoThumbnailBar&);
    ShowfotoThumbnailBar& operator=(const ShowfotoThumbnailBar&);

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOW_FOTO_THUMB_NAIL_BAR_H

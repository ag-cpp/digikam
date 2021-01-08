/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-22-07
 * Description : Icon view for import tool items
 *
 * Copyright (C) 2012      by Islam Wazery <wazery at ubuntu dot com>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMPORT_ICON_VIEW_H
#define DIGIKAM_IMPORT_ICON_VIEW_H

// Local includes

#include "importcategorizedview.h"

namespace Digikam
{

class ItemViewUtilities;

class ImportIconView : public ImportCategorizedView
{
    Q_OBJECT

public:

    explicit ImportIconView(QWidget* const parent = nullptr);
    ~ImportIconView() override;

    ItemViewUtilities* utilities() const;

    int fitToWidthIcons();

    CamItemInfo  camItemInfo(const QString& folder, const QString& file);
    CamItemInfo& camItemInfoRef(const QString& folder, const QString& file);

    void setThumbnailSize(const ThumbnailSize& size) override;

public Q_SLOTS:

    void deleteSelected(bool permanently = false);
    void deleteSelectedDirectly(bool permanently = false);

    void createGroupFromSelection();
    void createGroupByTimeFromSelection();
    void ungroupSelected();
    void removeSelectedFromGroup();
    void rename();

    void assignTagToSelected(int tagID);
    void removeTagFromSelected(int tagID);

    void assignPickLabel(const QModelIndex& index, int pickId);
    void assignPickLabelToSelected(int pickId);

    void assignColorLabel(const QModelIndex& index, int colorId);
    void assignColorLabelToSelected(int colorId);

    void assignRating(const QList<QModelIndex>& index, int rating);
    void assignRatingToSelected(int rating);

Q_SIGNALS:

    void previewRequested(const CamItemInfo& info, bool downloadPreview);
/*
    void signalPopupTagsView();
*/
private Q_SLOTS:

    void slotRotateLeft(const QList<QModelIndex>&);
    void slotRotateRight(const QList<QModelIndex>&);
/*
    void slotInitProgressIndicator();
*/

protected:

    void activated(const CamItemInfo& info, Qt::KeyboardModifiers modifiers)      override;
    void showContextMenuOnInfo(QContextMenuEvent* event, const CamItemInfo& info) override;
    void showContextMenu(QContextMenuEvent* event)                                override;
    void slotSetupChanged()                                                       override;

private:

    // Hidden copy constructor and assignment operator.
    ImportIconView(const ImportIconView&);
    ImportIconView& operator=(const ImportIconView&);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_IMPORT_ICON_VIEW_H

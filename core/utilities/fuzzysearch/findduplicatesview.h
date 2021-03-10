/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-19
 * Description : Find Duplicates View.
 *
 * Copyright (C) 2016-2017 by Mario Frank <mario dot frank at uni minus potsdam dot de>
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_FIND_DUPLICATES_VIEW_H
#define DIGIKAM_FIND_DUPLICATES_VIEW_H

// Qt includes

#include <QWidget>

class QTreeWidgetItem;

namespace Digikam
{
class Album;
class SAlbum;
class PAlbum;
class TAlbum;

class FindDuplicatesView : public QWidget
{
    Q_OBJECT

public:

    explicit FindDuplicatesView(QWidget* const parent = nullptr);
    ~FindDuplicatesView() override;

    QList<SAlbum*> currentFindDuplicatesAlbums() const;
    void setActive(bool val);
    void populateTreeView();

public Q_SLOTS:

    void slotSelectFirstItem();
    void slotSetSelectedAlbums(const QList<PAlbum*>& albums);
    void slotSetSelectedAlbums(const QList<TAlbum*>& albums);
    void slotRemoveDuplicates();

private Q_SLOTS:

    void initAlbumUpdateConnections();
    void slotAlbumAdded(Album* a);
    void slotAlbumDeleted(Album* a);
    void slotSearchUpdated(SAlbum* a);
    void slotClear();
    void slotFindDuplicates();
    void slotUpdateDuplicates(const QList<SAlbum*>& sAlbumsToRebuild,
                              const QList<qlonglong>& deletedImages);
    void slotDuplicatesAlbumActived();
    void slotComplete();
    void slotUpdateFingerPrints();
    void slotCheckForValidSettings();
    void slotApplicationSettingsChanged();

private:

    void enableControlWidgets(bool);

    void updateSimilarityRangeInterval();

    void resetAlbumsAndTags();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FIND_DUPLICATES_VIEW_H

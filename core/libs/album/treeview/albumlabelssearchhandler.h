/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-05-17
 * Description : Album Labels Search Tree View.
 *
 * Copyright (C) 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ALBUM_LABELS_SEARCH_HANDLER_H
#define DIGIKAM_ALBUM_LABELS_SEARCH_HANDLER_H

#include "labelstreeview.h"

namespace Digikam
{

class AlbumLabelsSearchHandler : public QObject
{
    Q_OBJECT

public:

    explicit AlbumLabelsSearchHandler(LabelsTreeView* const treeWidget);
    ~AlbumLabelsSearchHandler() override;

    /**
     * @return Album pointer of the currently selected labels
     */
    Album* albumForSelectedItems()         const;

    /**
     * @brief Gets the list of images generated, for exporting
     *
     * @return QUrl List of images Urls
     */
    QList<QUrl> imagesUrls()               const;

    /**
     * @return A string for a name generated by @see
     *         generateAlbumNameForExporting()
     */
    QString generatedName()                const;

    /**
     * @brief Restores the selection of the tree-view from history
     *
     * @param neededLabels a hash to restore selection from it
     */
    void restoreSelectionFromHistory(const QHash<LabelsTreeView::Labels,
                                     QList<int> >& neededLabels);

    /**
     * @return true if the tree-view is restoring the selection state
     *         from history to block searching until the restoring
     *         is done
     */
    bool isRestoringSelectionFromHistory() const;

private:

    /**
     * @brief Create XML for currently selected labels
     *
     * @param selectedLabels to generate XML for it
     * @return XML generated
     */
    QString createXMLForCurrentSelection(const QHash<LabelsTreeView::Labels, QList<int> >& selectedLabels);

    /**
     * @brief Passes the generated XML to CoreDB class
     *        search methods
     *
     * @param xml generated from @see createXMLForCurrentSelection
     * @return A pointer of the search album created by the CoreDB
     */
    SAlbum* search(const QString& xml)     const;

    /**
     * @brief Generates album name based on the selected labels for
     *        example if rating 2 and 5 are selected then the name is
     *        "Rating: 2, 5", If there are any selected picks for example
     *        Pending, then the generated name is "Rating: 2, 5 | Picks: Pending"
     */
    void generateAlbumNameForExporting(const QList<int>& ratings,
                                       const QList<int>& colorsList,
                                       const QList<int>& picksList);

    /**
     * @brief Passes the current album DB url to a DBJobsManager
     *        to get images urls in this album
     */
    void imagesUrlsForCurrentAlbum();

    /**
     * @return "Labels Album" if the tree-widget is not checkable
     *         and returns "Exporting Album" if it was checkable
     */
    QString getDefaultTitle()              const;

private Q_SLOTS:

    void slotSelectionChanged();
    void slotCheckStateChanged();
    void slotSetCurrentAlbum();
    void slotResult();
    void slotData(const QList<ItemListerRecord>& data);

Q_SIGNALS:

    void checkStateChanged(Album* album, Qt::CheckState checkState);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ALBUM_LABELS_SEARCH_HANDLER_H

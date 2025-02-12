/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2000-12-05
 * Description : helper class used to modify search albums in views
 *
 * SPDX-FileCopyrightText: 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2014-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>

// Local includes

#include "album.h"

namespace Digikam
{

class SketchWidget;
class ItemInfo;

/**
 * Range of a contiguous dates selection <start date, end date>.
 */
typedef QPair<QDateTime, QDateTime> DateRange;

/**
 * List of dates range selected.
 */
typedef QList<DateRange> DateRangeList;

/**
 * Utility class providing methods to modify search albums (SAlbum) in a way
 * useful to implement views.
 *
 * @author jwienke
 */
class SearchModificationHelper: public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent the parent for qt parent child mechanism
     * @param dialogParent parent widget for dialogs displayed by this object
     */
    SearchModificationHelper(QObject* const parent, QWidget* const dialogParent);

    /**
     * Destructor.
     */
    ~SearchModificationHelper() override;

    /**
     * @see slotCreateFuzzySearchFromSketch()
     * @return the newly created album
     */
    SAlbum* createFuzzySearchFromSketch(const QString& name,
                                        SketchWidget* sketchWidget,
                                        unsigned int numberOfResults,
                                        const QList<int>& targetAlbums,
                                        bool overwriteIfExisting = false);

    /**
     * @see slotCreateFuzzySearchFromDropped()
     * @return the newly created album
     */
    SAlbum* createFuzzySearchFromDropped(const QString& name,
                                        const QString& filePath,
                                        float threshold,
                                        float maxThreshold,
                                        const QList<int>& targetAlbums,
                                        bool overwriteIfExisting = false);

    /**
     * @see slotCreateFuzzySearchFromImage()
     * @return the newly created album
     */
    SAlbum* createFuzzySearchFromImage(const QString& name,
                                       const ItemInfo& image,
                                       float threshold,
                                       float maxThreshold,
                                       const QList<int>& targetAlbums,
                                       bool overwriteIfExisting = false);

public Q_SLOTS:

    /**
     * Deletes the given search after prompting the user.
     *
     * @param searchAlbum search to delete
     */
    void slotSearchDelete(SAlbum* searchAlbum);

    /**
     * Renames the given search via a dialog.
     *
     * @param searchAlbum search to rename
     */
    void slotSearchRename(SAlbum* searchAlbum);

    /**
     * Creates a new timeline search.
     *
     * @param desiredName desired name for the search. If this name already
     *                    exists and overwriteIfExisting is false, then the user
     *                    will be prompted for a new name
     * @param dateRanges date ranges to contain in this timeline search. If this
     *                        is empty, no search will be created.
     * @param overwriteIfExisting if true, an existing search with the desired
     *                            name will be overwritten without prompting the
     *                            user for a new name
     */
    SAlbum* slotCreateTimeLineSearch(const QString& desiredName,
                                     const DateRangeList& dateRanges,
                                     bool overwriteIfExisting = false);

    /**
     * Creates a new fuzzy search based on a sketch created by the user and
     * selects it in the AlbumManager after creation.
     *
     * @param name the name of the new sketch search
     * @param sketchWidget the widget containing the sketch of the user
     * @param numberOfResults max number of results to display
     * @param targetAlbums The image must be in one of these albums
     * @param overwriteIfExisting if true, an existing search with the desired
     *                            name will be overwritten without prompting the
     *                            user for a new name
     */
    void slotCreateFuzzySearchFromSketch(const QString& name,
                                         SketchWidget* sketchWidget,
                                         unsigned int numberOfResults,
                                         const QList<int>& targetAlbums,
                                         bool overwriteIfExisting = false);

    /**
     * Creates a new fuzzy search for finding similar photos based on one photo
     * and selects it in the album manager after creation.
     *
     * @param name of the new search
     * @param image the image to base this search on
     * @param threshold the threshold for image search, 0 <= threshold <= 1
     * @param maxThreshold the maximum threshold of similarity.
     * @param targetAlbums The image must be in one of these albums
     * @param overwriteIfExisting if true, an existing search with the desired
     *                            name will be overwritten without prompting the
     *                            user for a new name
     */
    void slotCreateFuzzySearchFromImage(const QString& name,
                                        const ItemInfo& image,
                                        float threshold,
                                        float maxThreshold,
                                        const QList<int>& targetAlbums,
                                        bool overwriteIfExisting = false);

    /**
     * Creates a new fuzzy search for finding similar photos based on the file
     * path of a photo
     * and selects it in the album manager after creation.
     *
     * @param name of the new search
     * @param filePath path of the image to base this search on
     * @param threshold minimum threshold for image search
     * @param maxThreshold maximum threshold for image search
     * @param targetAlbums The image must be in one of these albums
     * @param overwriteIfExisting if true, an existing search with the desired
     *                            name will be overwritten without prompting the
     *                            user for a new name
     */
    void slotCreateFuzzySearchFromDropped(const QString& name,
                                          const QString& filePath,
                                          float threshold,
                                          float maxThreshold,
                                          const QList<int>& targetAlbums,
                                          bool overwriteIfExisting);

private:

    bool checkAlbum(const QString& name) const;
    bool checkName(QString& name);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam

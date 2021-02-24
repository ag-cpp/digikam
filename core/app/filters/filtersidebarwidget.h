/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2000-12-05
 * Description : filters view for the right sidebar
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2011 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C)      2014 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef DIGIKAM_FILTER_SIDE_BAR_WIDGET_H
#define DIGIKAM_FILTER_SIDE_BAR_WIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include "itemfiltersettings.h"
#include "statesavingobject.h"
#include "textfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

enum FilterType
{
    TEXT = 0,
    MIME,
    GEOLOCATION,
    TAGS,
    LABELS
};

class TagModel;
class TAlbum;

/**
 * Sidebar widget containing the all filter widgets.
 *
 * @author jwienke
 */
class FilterSideBarWidget : public DVBox,
                            public StateSavingObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent the parent for qt parent child mechanism
     * @param tagFilterModel tag model to work on
     */
    explicit FilterSideBarWidget(QWidget* const parent, TagModel* const tagFilterModel);

    /**
     * Destructor.
     */
    ~FilterSideBarWidget()                         override;

    void setConfigGroup(const KConfigGroup& group) override;
    void doLoadState()                             override;
    void doSaveState()                             override;

    void setFocusToTextFilter();

Q_SIGNALS:

    /**
     * Emitted if the selected filter has changed.
     *
     * @param includedTags a list of included tag ids
     * @param excludedTags a list of excluded tag ids
     * @param matchingCond condition to join the selected tags
     * @param showUnTagged if this is true, only photos without a tag shall be shown
     * @param clTagIds     a list of color label tag ids
     * @param plTagIds     a list of pick label tag ids
     */
    void signalTagFilterChanged(const QList<int>& includedTags,
                                const QList<int>& excludedTags,
                                ItemFilterSettings::MatchingCondition matchingCond,
                                bool showUnTagged,
                                const QList<int>& clTagIds,
                                const QList<int>& plTagIds);

    void signalRatingFilterChanged(int, ItemFilterSettings::RatingCondition, bool);
    void signalMimeTypeFilterChanged(int);
    void signalGeolocationFilterChanged(ItemFilterSettings::GeolocationCondition);
    void signalSearchTextFilterChanged(const SearchTextFilterSettings&);

public Q_SLOTS:

    /**
     * Resets all selected filters.
     */
    void slotResetFilters();

    void slotFilterMatchesForText(bool);

private Q_SLOTS:

    void slotCheckedTagsChanged(const QList<TAlbum*>& includedTags,
                                const QList<TAlbum*>& excludedTags);
    void slotColorLabelFilterChanged(const QList<ColorLabel>&);
    void slotPickLabelFilterChanged(const QList<PickLabel>&);
    void slotWithoutTagChanged(int newState);
    void slotItemExpanded(int id, bool b);
    void slotTagOptionsTriggered(QAction*);
    void slotTagOptionsMenu();

private:

    void checkFilterChanges();

private:

    class Private;
    Private* const d;
};

} // nameSpace Digikam

#endif // DIGIKAM_FILTER_SIDE_BAR_WIDGET_H

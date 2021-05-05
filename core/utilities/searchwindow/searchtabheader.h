/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-02-26
 * Description : Upper widget in the search sidebar
 *
 * Copyright (C) 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_SEARCH_TAB_HEADER_H
#define DIGIKAM_SEARCH_TAB_HEADER_H

// Qt includes

#include <QWidget>

// Local includes

#include "coredbalbuminfo.h"

namespace Digikam
{

class Album;
class SAlbum;
class SearchWindow;

class SearchTabHeader : public QWidget
{
    Q_OBJECT

public:

    explicit SearchTabHeader(QWidget* const parent);
    ~SearchTabHeader() override;

public Q_SLOTS:

    void selectedSearchChanged(Album* album);
    void editSearch(SAlbum* album);
    void newKeywordSearch();
    void newAdvancedSearch();

Q_SIGNALS:

    void searchShallBeSelected(const QList<Album*>& albums);

private Q_SLOTS:

    void keywordChanged();
    void keywordChangedTimer();
    void editCurrentAdvancedSearch();
    void saveSearch();
    void storedKeywordChanged();
    void editStoredAdvancedSearch();
    void advancedSearchEdited(int id, const QString& query);

private:

    void          setCurrentSearch(DatabaseSearch::Type type, const QString& query, bool selectCurrentAlbum = true);
    QString       queryFromKeywords(const QString& keywords) const;
    QString       keywordsFromQuery(const QString& query) const;
    SearchWindow* searchWindow() const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SEARCH_TAB_HEADER_H

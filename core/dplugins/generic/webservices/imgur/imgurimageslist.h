/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to Imgur web service
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
 * Copyright (C) 2013-2020 by Caulier Gilles <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_IMGUR_IMAGES_LIST_H
#define DIGIKAM_IMGUR_IMAGES_LIST_H

// Qt includes

#include <QWidget>

// Local includes

#include "ditemslist.h"
#include "imgurtalker.h"

using namespace Digikam;

namespace DigikamGenericImgUrPlugin
{

class ImgurImageListViewItem;

class ImgurImagesList : public DItemsList
{
    Q_OBJECT

public:

    /// The different columns in a list.
    enum FieldType
    {
        Title           = DItemsListView::User1,
        Description     = DItemsListView::User2,
        URL             = DItemsListView::User3,
        DeleteURL       = DItemsListView::User4
    };

public:

    explicit ImgurImagesList(QWidget* const parent = nullptr);
    ~ImgurImagesList() override = default;

    QList<const ImgurImageListViewItem*> getPendingItems();

public Q_SLOTS:

    void slotAddImages(const QList<QUrl>& list) override;
    void slotSuccess(const ImgurTalkerResult& result);
    void slotDoubleClick(QTreeWidgetItem* element, int i);
};

// -------------------------------------------------------------------------

class ImgurImageListViewItem : public DItemsListViewItem
{
public:

    explicit ImgurImageListViewItem(DItemsListView* const view, const QUrl& url);
    ~ImgurImageListViewItem() override = default;

    void setTitle(const QString& str);
    QString Title()          const;

    void setDescription(const QString& str);
    QString Description()    const;

    void setImgurUrl(const QString& str);
    QString ImgurUrl()       const;

    void setImgurDeleteUrl(const QString& str);
    QString ImgurDeleteUrl() const;
};

} // namespace DigikamGenericImgUrPlugin

#endif // DIGIKAM_IMGUR_IMAGES_LIST_H

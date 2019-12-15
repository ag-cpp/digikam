/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-05-17
 * Description : Album Labels Search Tree View.
 *
 * Copyright (C) 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2014-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "albumlabelssearchhandler.h"

// QT includes

#include <QApplication>
#include <QTreeWidget>
#include <QPainter>
#include <QUrl>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "coredbsearchxml.h"
#include "searchtabheader.h"
#include "albummanager.h"
#include "albumtreeview.h"
#include "coredbconstants.h"
#include "itemlister.h"
#include "statesavingobject.h"
#include "coredbaccess.h"
#include "coredb.h"
#include "colorlabelfilter.h"
#include "picklabelfilter.h"
#include "tagscache.h"
#include "applicationsettings.h"
#include "dnotificationwrapper.h"
#include "digikamapp.h"
#include "ratingwidget.h"
#include "dbjobsmanager.h"

namespace Digikam
{

class Q_DECL_HIDDEN AlbumLabelsSearchHandler::Private
{
public:

    explicit Private()
      : treeWidget(nullptr),
        dbJobThread(nullptr),
        restoringSelectionFromHistory(false),
        currentXmlIsEmpty(false),
        albumForSelectedItems(nullptr)
    {
    }

    LabelsTreeView*       treeWidget;
    SearchesDBJobsThread* dbJobThread;
    bool                  restoringSelectionFromHistory;
    bool                  currentXmlIsEmpty;
    QString               oldXml;
    Album*                albumForSelectedItems;
    QString               generatedAlbumName;
    QList<QUrl>           urlListForSelectedAlbum;
};

AlbumLabelsSearchHandler::AlbumLabelsSearchHandler(LabelsTreeView* const treeWidget)
    : d(new Private)
{
    d->treeWidget = treeWidget;

    if (!d->treeWidget->isCheckable())
    {
        connect(d->treeWidget, SIGNAL(itemSelectionChanged()),
                this, SLOT(slotSelectionChanged()));

        connect(d->treeWidget, SIGNAL(signalSetCurrentAlbum()),
                this, SLOT(slotSetCurrentAlbum()));
    }
    else
    {
        connect(d->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                this, SLOT(slotCheckStateChanged()));
    }
}

AlbumLabelsSearchHandler::~AlbumLabelsSearchHandler()
{
    delete d;
}

Album *AlbumLabelsSearchHandler::albumForSelectedItems() const
{
    return d->albumForSelectedItems;
}

QList<QUrl> AlbumLabelsSearchHandler::imagesUrls() const
{
    return d->urlListForSelectedAlbum;
}

QString AlbumLabelsSearchHandler::generatedName() const
{
    return d->generatedAlbumName;
}

void AlbumLabelsSearchHandler::restoreSelectionFromHistory(const QHash<LabelsTreeView::Labels, QList<int> >& neededLabels)
{
    d->restoringSelectionFromHistory = true;
    d->treeWidget->restoreSelectionFromHistory(neededLabels);
    d->restoringSelectionFromHistory = false;
    slotSelectionChanged();
}

bool AlbumLabelsSearchHandler::isRestoringSelectionFromHistory() const
{
    return d->restoringSelectionFromHistory;
}

QString AlbumLabelsSearchHandler::createXMLForCurrentSelection(const QHash<LabelsTreeView::Labels, QList<int> >& selectedLabels)
{
    SearchXmlWriter writer;
    writer.setFieldOperator(SearchXml::standardFieldOperator());
    QList<int>      ratings;
    QList<int>      colorsAndPicks;

    foreach (int rate, selectedLabels[LabelsTreeView::Ratings])
    {
        if (rate == 0)
        {
            ratings << -1;
        }

        ratings << rate;
    }

    foreach (int color, selectedLabels[LabelsTreeView::Colors])
    {
        colorsAndPicks << TagsCache::instance()->tagForColorLabel(color);
    }

    foreach (int pick, selectedLabels[LabelsTreeView::Picks])
    {
        colorsAndPicks << TagsCache::instance()->tagForPickLabel(pick);
    }

    d->currentXmlIsEmpty = (ratings.isEmpty() && colorsAndPicks.isEmpty()) ? true : false;

    if (!ratings.isEmpty() && !colorsAndPicks.isEmpty())
    {
        foreach (int val, ratings)
        {
            writer.writeGroup();
            writer.writeField(QLatin1String("rating"), SearchXml::Equal);
            writer.writeValue(val);
            writer.finishField();

            writer.writeField(QLatin1String("tagid"), SearchXml::InTree);
            writer.writeValue(colorsAndPicks);
            writer.finishField();

            writer.finishGroup();
        }
    }
    else if (!ratings.isEmpty())
    {
        foreach (int rate, ratings)
        {
            writer.writeGroup();
            writer.writeField(QLatin1String("rating"), SearchXml::Equal);
            writer.writeValue(rate);
            writer.finishField();
            writer.finishGroup();
        }
    }
    else if (!colorsAndPicks.isEmpty())
    {
        writer.writeGroup();
        writer.writeField(QLatin1String("tagid"), SearchXml::InTree);
        writer.writeValue(colorsAndPicks);
        writer.finishField();
        writer.finishGroup();
    }
    else
    {
        writer.writeGroup();
        writer.finishGroup();
    }

    writer.finish();

    generateAlbumNameForExporting(selectedLabels[LabelsTreeView::Ratings],
                                  selectedLabels[LabelsTreeView::Colors],
                                  selectedLabels[LabelsTreeView::Picks]);
    return writer.xml();
}

SAlbum* AlbumLabelsSearchHandler::search(const QString& xml) const
{
    SAlbum* album = nullptr;
    int     id    = 0;

    if (!d->treeWidget->isCheckable())
    {
        album = AlbumManager::instance()->findSAlbum(SAlbum::getTemporaryTitle(DatabaseSearch::AdvancedSearch));

        if (album)
        {
            id = album->id();
            CoreDbAccess().db()->updateSearch(id, DatabaseSearch::AdvancedSearch,
                                              SAlbum::getTemporaryTitle(DatabaseSearch::AdvancedSearch), xml);
        }
        else
        {
            id = CoreDbAccess().db()->addSearch(DatabaseSearch::AdvancedSearch,
                                                SAlbum::getTemporaryTitle(DatabaseSearch::AdvancedSearch), xml);
        }

        album = new SAlbum(getDefaultTitle(), id);
    }
    else
    {
        album = AlbumManager::instance()->findSAlbum(getDefaultTitle());

        if (album)
        {
            id = album->id();
            CoreDbAccess().db()->updateSearch(id, DatabaseSearch::AdvancedSearch,
                                              getDefaultTitle(), xml);
        }
        else
        {
            id = CoreDbAccess().db()->addSearch(DatabaseSearch::AdvancedSearch,
                                                getDefaultTitle(), xml);
        }

        album = new SAlbum(d->generatedAlbumName, id);
    }

    if (!album->isUsedByLabelsTree())
    {
        album->setUsedByLabelsTree(true);
    }

    return album;
}

void AlbumLabelsSearchHandler::generateAlbumNameForExporting(const QList<int>& ratings,
                                                             const QList<int>& colorsList,
                                                             const QList<int>& picksList)
{
    QString name;
    QString ratingsString;
    QString picksString;
    QString colorsString;

    if (!ratings.isEmpty())
    {
        ratingsString += i18n("Rating: ");

        QListIterator<int> it(ratings);

        while (it.hasNext())
        {
            int rating = it.next();

            if (rating == -1)
            {
                ratingsString += i18n("No Rating");
            }
            else
            {
                ratingsString += QString::number(rating);
            }

            if (it.hasNext())
            {
                ratingsString += QLatin1String(", ");
            }
        }
    }

    if (!colorsList.isEmpty())
    {
        colorsString += i18n("Colors: ");

        QListIterator<int> it(colorsList);

        while (it.hasNext())
        {
            switch (it.next())
            {
                case NoColorLabel:
                    colorsString += i18n("No Color");
                    break;
                case RedLabel:
                    colorsString += i18n("Red");
                    break;
                case OrangeLabel:
                    colorsString += i18n("Orange");
                    break;
                case YellowLabel:
                    colorsString += i18n("Yellow");
                    break;
                case GreenLabel:
                    colorsString += i18n("Green");
                    break;
                case BlueLabel:
                    colorsString += i18n("Blue");
                    break;
                case MagentaLabel:
                    colorsString += i18n("Magenta");
                    break;
                case GrayLabel:
                    colorsString += i18n("Gray");
                    break;
                case BlackLabel:
                    colorsString += i18n("Black");
                    break;
                case WhiteLabel:
                    colorsString += i18n("White");
                    break;
                default:
                    break;
            }

            if (it.hasNext())
            {
                colorsString += QLatin1String(", ");
            }
        }
    }

    if (!picksList.isEmpty())
    {
        picksString += i18n("Picks: ");

        QListIterator<int> it(picksList);

        while (it.hasNext())
        {
            switch (it.next())
            {
                case NoPickLabel:
                    picksString += i18n("No Pick");
                    break;
                case RejectedLabel:
                    picksString += i18n("Rejected");
                    break;
                case PendingLabel:
                    picksString += i18n("Pending");
                    break;
                case AcceptedLabel:
                    picksString += i18n("Accepted");
                    break;
                default:
                    break;
            }

            if (it.hasNext())
            {
                picksString += QLatin1String(", ");
            }
        }
    }

    if (ratingsString.isEmpty() && picksString.isEmpty())
    {
        name = colorsString;
    }
    else if (ratingsString.isEmpty() && colorsString.isEmpty())
    {
        name = picksString;
    }
    else if (colorsString.isEmpty() && picksString.isEmpty())
    {
        name = ratingsString;
    }
    else if (ratingsString.isEmpty())
    {
        name = picksString + QLatin1String(" | ") + colorsString;
    }
    else if (picksString.isEmpty())
    {
        name = ratingsString + QLatin1String(" | ") + colorsString;
    }
    else if (colorsString.isEmpty())
    {
        name = ratingsString + QLatin1String(" | ") + picksString;
    }
    else
    {
        name = ratingsString + QLatin1String(" | ") + picksString + QLatin1String(" | ") + colorsString;
    }

    d->generatedAlbumName = name;
}

void AlbumLabelsSearchHandler::imagesUrlsForCurrentAlbum()
{
    SearchesDBJobInfo jobInfo;
    jobInfo.setSearchId(d->albumForSelectedItems->id());
    jobInfo.setRecursive();

    d->dbJobThread = DBJobsManager::instance()->startSearchesJobThread(jobInfo);

    connect(d->dbJobThread, SIGNAL(finished()),
            this, SLOT(slotResult()));

    connect(d->dbJobThread, SIGNAL(data(QList<ItemListerRecord>)),
            this, SLOT(slotData(QList<ItemListerRecord>)));
}

QString AlbumLabelsSearchHandler::getDefaultTitle() const
{
    if (d->treeWidget->isCheckable())
    {
        return i18n("Exported Labels");
    }
    else
    {
        return i18n("Labels Album");
    }
}

void AlbumLabelsSearchHandler::slotSelectionChanged()
{
    if (d->treeWidget->isLoadingState() || d->restoringSelectionFromHistory)
    {
        return;
    }

    QString xml         = createXMLForCurrentSelection(d->treeWidget->selectedLabels());
    SAlbum* const album = search(xml);

    if (album)
    {
        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << album);
        d->albumForSelectedItems = album;
        d->oldXml                = xml;
    }
}

void AlbumLabelsSearchHandler::slotCheckStateChanged()
{
    QString currentXml = createXMLForCurrentSelection(d->treeWidget->selectedLabels());

    if (currentXml == d->oldXml)
    {
        return;
    }

    if (d->albumForSelectedItems)
    {
        emit checkStateChanged(d->albumForSelectedItems, Qt::Unchecked);
    }

    SAlbum* const album = search(currentXml);

    if (album)
    {
        if (!d->currentXmlIsEmpty)
        {
            d->albumForSelectedItems = album;
            imagesUrlsForCurrentAlbum();
        }
        else
        {
            d->albumForSelectedItems = nullptr;
        }

        emit checkStateChanged(album, Qt::Checked);
    }

    d->oldXml = currentXml;
}

void AlbumLabelsSearchHandler::slotSetCurrentAlbum()
{
    slotSelectionChanged();
}

void AlbumLabelsSearchHandler::slotResult()
{
    if (d->dbJobThread != sender())
    {
        return;
    }

    if (d->dbJobThread->hasErrors())
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to list urls: " << d->dbJobThread->errorsList().first();

        // Pop-up a message about the error.
        DNotificationWrapper(QString(),
                             d->dbJobThread->errorsList().first(),
                             DigikamApp::instance(),
                             DigikamApp::instance()->windowTitle());
    }
}

void AlbumLabelsSearchHandler::slotData(const QList<ItemListerRecord>& data)
{
    if ((d->dbJobThread != sender()) || data.isEmpty())
    {
        return;
    }

    QList<QUrl> urlList;

    foreach (const ItemListerRecord& record, data)
    {
        ItemInfo info(record);
        urlList << info.fileUrl();
    }

    d->urlListForSelectedAlbum = urlList;
}

} // namespace Digikam

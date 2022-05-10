/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#include "PlayList.h"

// Qt includes

#include <QFileDialog>
#include <QListView>
#include <QLayout>
#include <QToolButton>
#include <QFile>
#include <QDataStream>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "PlayListModel.h"
#include "PlayListDelegate.h"
#include "common.h"
#include "digikam_debug.h"

namespace QtAVPlayer
{

PlayList::PlayList(QWidget* parent)
    : QWidget(parent)
{
    mFirstShow = true;
    mMaxRows   = -1;
    mpModel    = new PlayListModel(this);
    mpDelegate = new PlayListDelegate(this);
    mpListView = new QListView;

    //mpListView->setResizeMode(QListView::Adjust);

    mpListView->setModel(mpModel);
    mpListView->setItemDelegate(mpDelegate);
    mpListView->setSelectionMode(QAbstractItemView::ExtendedSelection); //ctrl,shift
    mpListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mpListView->setToolTip(QString::fromLatin1("Ctrl/Shift + ") + i18n("Click to select multiple"));
    QVBoxLayout* vbl = new QVBoxLayout;
    setLayout(vbl);
    vbl->addWidget(mpListView);
    QHBoxLayout* hbl = new QHBoxLayout;

    mpClear          = new QToolButton(0);
    mpClear->setText(i18n("Clear"));
    mpRemove         = new QToolButton(0);
    mpRemove->setText(QString::fromLatin1("-"));
    mpRemove->setToolTip(i18n("Remove selected items"));
    mpAdd            = new QToolButton(0);
    mpAdd->setText(QString::fromLatin1("+"));

    hbl->addWidget(mpClear);
    hbl->addSpacing(width());
    hbl->addWidget(mpRemove);
    hbl->addWidget(mpAdd);
    vbl->addLayout(hbl);

    connect(mpClear, SIGNAL(clicked()),
            this, SLOT(clearItems()));

    connect(mpRemove, SIGNAL(clicked()),
            this, SLOT(removeSelectedItems()));

    connect(mpAdd, SIGNAL(clicked()),
            this, SLOT(addItems()));

    connect(mpListView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(onAboutToPlay(QModelIndex)));

    // enter to highight
/*
    connect(mpListView, SIGNAL(entered(QModelIndex)),
            this, SLOT(highlight(QModelIndex)));
*/
}

PlayList::~PlayList()
{
    qCDebug(DIGIKAM_QTAVPLAYER_LOG).noquote() << QString::asprintf("+++++++++++++~PlayList()");
    save();
}

void PlayList::setSaveFile(const QString &file)
{
    mFile = file;
}

QString PlayList::saveFile() const
{
    return mFile;
}

void PlayList::load()
{
    QFile f(mFile);

    if (!f.exists())
        return;

    if (!f.open(QIODevice::ReadOnly))
        return;

    QDataStream ds(&f);
    QList<PlayListItem> list;
    ds >> list;

    for (int i = 0 ; i < list.size() ; ++i)
    {
        insertItemAt(list.at(i), i);
    }
}

void PlayList::save()
{
    QFile f(mFile);

    if (!f.open(QIODevice::WriteOnly))
    {
        qCWarning(DIGIKAM_QTAVPLAYER_LOG).noquote()
            << QString::asprintf("File open error: %s", qPrintable(f.errorString()));

        return;
    }

    QDataStream ds(&f);
    ds << mpModel->items();
}

PlayListItem PlayList::itemAt(int row)
{
    if (mpModel->rowCount() < 0)
    {
        qCWarning(DIGIKAM_QTAVPLAYER_LOG).noquote() << QString::asprintf("Invalid rowCount");

        return PlayListItem();
    }

    return mpModel->data(mpModel->index(row), Qt::DisplayRole).value<PlayListItem>();
}

void PlayList::insertItemAt(const PlayListItem &item, int row)
{
    if (mMaxRows > 0 && mpModel->rowCount() >= mMaxRows)
    {
        // +1 because new row is to be inserted

        mpModel->removeRows(mMaxRows, mpModel->rowCount() - mMaxRows + 1);
    }

    int i = mpModel->items().indexOf(item, row+1);

    if (i > 0)
    {
        mpModel->removeRow(i);
    }

    if (!mpModel->insertRow(row))
        return;

    if (row > 0)
    {
        i = mpModel->items().lastIndexOf(item, row - 1);

        if (i >= 0)
            mpModel->removeRow(i);
    }

    setItemAt(item, row);
}

void PlayList::setItemAt(const PlayListItem &item, int row)
{
    mpModel->setData(mpModel->index(row), QVariant::fromValue(item), Qt::DisplayRole);
}

void PlayList::insert(const QString &url, int row)
{
    PlayListItem item;
    item.setUrl(url);
    item.setDuration(0);
    item.setLastTime(0);
    QString title = url;

    if (!url.contains(QLatin1String("://")) || url.startsWith(QLatin1String("file://")))
    {
        title = QFileInfo(url).fileName();
    }

    item.setTitle(title);
    insertItemAt(item, row);
}

void PlayList::remove(const QString &url)
{
    for (int i = mpModel->rowCount() - 1 ; i >= 0 ; --i)
    {
        PlayListItem item = mpModel->data(mpModel->index(i), Qt::DisplayRole).value<PlayListItem>();

        if (item.url() == url)
        {
            mpModel->removeRow(i);
        }
    }
}

void PlayList::setMaxRows(int r)
{
    mMaxRows = r;
}

int PlayList::maxRows() const
{
    return mMaxRows;
}

void PlayList::removeSelectedItems()
{
    QItemSelectionModel *selection = mpListView->selectionModel();

    if (!selection->hasSelection())
        return;

    QModelIndexList s = selection->selectedIndexes();

    for (int i = s.size()-1 ; i >= 0 ; --i)
    {
        mpModel->removeRow(s.at(i).row());
    }
}

void PlayList::clearItems()
{
    mpModel->removeRows(0, mpModel->rowCount());
}

void PlayList::addItems()
{
    // TODO: add url;

    QStringList files = QFileDialog::getOpenFileNames(0, i18n("Select files"));

    if (files.isEmpty())
        return;

    // TODO: check playlist file: m3u, pls... In another thread

    for (int i = 0 ; i < files.size() ; ++i)
    {
        QString file = files.at(i);

        if (!QFileInfo(file).isFile())
            continue;

        insert(file, i);
    }
}

void PlayList::onAboutToPlay(const QModelIndex &index)
{
    emit aboutToPlay(index.data(Qt::DisplayRole).value<PlayListItem>().url());
    save();
}

} // namespace QtAVPlayer

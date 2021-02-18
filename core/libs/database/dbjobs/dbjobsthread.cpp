/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-01
 * Description : DB Jobs thread for listing and scanning
 *
 * Copyright (C) 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#include "dbjobsthread.h"

// Local includes

#include "coredbaccess.h"
#include "dbjobinfo.h"
#include "dbjob.h"
#include "duplicatesprogressobserver.h"
#include "digikam_debug.h"

namespace Digikam
{

DBJobsThread::DBJobsThread(QObject* const parent)
    : ActionThreadBase(parent)
{
    setObjectName(QLatin1String("DBJobsThread"));
}

DBJobsThread::~DBJobsThread()
{
}

bool DBJobsThread::hasErrors()
{
    return !m_errorsList.isEmpty();
}

QList<QString> &DBJobsThread::errorsList()
{
    return m_errorsList;
}

void DBJobsThread::connectFinishAndErrorSignals(DBJob* const j)
{
    connect(j, SIGNAL(signalDone()),
            this, SIGNAL(finished()));

    connect(j, SIGNAL(error(QString)),
            this, SLOT(error(QString)));
}

void DBJobsThread::error(const QString& errString)
{
    m_errorsList.append(errString);
}

// -------------------------------------------------

AlbumsDBJobsThread::AlbumsDBJobsThread(QObject* const parent)
    : DBJobsThread(parent)
{
}

AlbumsDBJobsThread::~AlbumsDBJobsThread()
{
}

void AlbumsDBJobsThread::albumsListing(const AlbumsDBJobInfo& info)
{
    AlbumsJob* const j = new AlbumsJob(info);

    connectFinishAndErrorSignals(j);

    if (info.isFoldersJob())
    {
        connect(j, SIGNAL(foldersData(QMap<int,int>)),
                this, SIGNAL(foldersData(QMap<int,int>)));
    }
    else
    {
        connect(j, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));
    }

    ActionJobCollection collection;
    collection.insert(j, 0);

    appendJobs(collection);
}

// -------------------------------------------------

TagsDBJobsThread::TagsDBJobsThread(QObject* const parent)
    : DBJobsThread(parent)
{
}

TagsDBJobsThread::~TagsDBJobsThread()
{
}

void TagsDBJobsThread::tagsListing(const TagsDBJobInfo& info)
{
    TagsJob* const j = new TagsJob(info);

    connectFinishAndErrorSignals(j);

    if (info.isFoldersJob())
    {
        connect(j, SIGNAL(foldersData(QMap<int,int>)),
                this, SIGNAL(foldersData(QMap<int,int>)));
    }
    else if (info.isFaceFoldersJob())
    {
        connect(j, SIGNAL(faceFoldersData(QMap<QString,QMap<int,int> >)),       // krazy:exclude=normalize
                this, SIGNAL(faceFoldersData(QMap<QString,QMap<int,int> >)));   // krazy:exclude=normalize
    }
    else
    {
        connect(j, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));
    }

    ActionJobCollection collection;
    collection.insert(j, 0);

    appendJobs(collection);
}

// -------------------------------------------------

DatesDBJobsThread::DatesDBJobsThread(QObject* const parent)
    : DBJobsThread(parent)
{
}

DatesDBJobsThread::~DatesDBJobsThread()
{
}

void DatesDBJobsThread::datesListing(const DatesDBJobInfo& info)
{
    DatesJob* const j = new DatesJob(info);

    connectFinishAndErrorSignals(j);

    if (info.isFoldersJob())
    {
        connect(j, SIGNAL(foldersData(QHash<QDateTime,int>)),
                this, SIGNAL(foldersData(QHash<QDateTime,int>)));
    }
    else
    {
        connect(j, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));
    }

    ActionJobCollection collection;
    collection.insert(j, 0);

    appendJobs(collection);
}

// -------------------------------------------------

GPSDBJobsThread::GPSDBJobsThread(QObject* const parent)
    : DBJobsThread(parent)
{
}

GPSDBJobsThread::~GPSDBJobsThread()
{
}

void GPSDBJobsThread::GPSListing(const GPSDBJobInfo& info)
{
    GPSJob* const j = new GPSJob(info);

    connectFinishAndErrorSignals(j);

    if (info.isDirectQuery())
    {
        connect(j, SIGNAL(directQueryData(QList<QVariant>)),
                this, SIGNAL(directQueryData(QList<QVariant>)));
    }
    else
    {
        connect(j, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));
    }

    ActionJobCollection collection;
    collection.insert(j, 0);

    appendJobs(collection);
}

// -------------------------------------------------

SearchesDBJobsThread::SearchesDBJobsThread(QObject* const parent)
    : DBJobsThread(parent)
{
}

SearchesDBJobsThread::~SearchesDBJobsThread()
{
}

void SearchesDBJobsThread::searchesListing(const SearchesDBJobInfo& info)
{
    SearchesJob* const j = new SearchesJob(info);

    connectFinishAndErrorSignals(j);

    if (info.isDuplicatesJob())
    {
        connect(j, SIGNAL(totalSize(int)),
                this, SIGNAL(totalSize(int)));

        connect(j, SIGNAL(processedSize(int)),
                this, SIGNAL(processedSize(int)));
    }
    else
    {
        connect(j, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));
    }

    ActionJobCollection collection;
    collection.insert(j, 0);

    appendJobs(collection);
}

} // namespace Digikam

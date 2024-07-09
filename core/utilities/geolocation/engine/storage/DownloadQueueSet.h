/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QList>
#include <QQueue>
#include <QObject>
#include <QSet>
#include <QStack>

// Local includes

#include "DownloadPolicy.h"

class QUrl;

namespace Marble
{

class HttpJob;

/**
   Life of a HttpJob
   =================
   - Job is added to the QueueSet (by calling addJob() )
     the HttpJob is put into the m_jobQueue where it waits for "activation"
     signal jobAdded is Q_EMITted
   - Job is activated
     Job is moved from m_jobQueue to m_activeJobs and signals of the job
     are connected to slots (local or HttpDownloadManager)
     Job is executed by calling the jobs execute() method

   now there are different possibilities:
   1) Job Q_EMITs jobDone (some error occurred, or canceled (kio))
      Job is disconnected
      signal jobRemoved is Q_EMITted
      Job is either moved from m_activeJobs to m_retryQueue
        or destroyed and blacklisted

   2) Job Q_EMITs redirected
      Job is removed from m_activeJobs, disconnected and destroyed
      signal jobRemoved is Q_EMITted
      (HttpDownloadManager creates new Job with new source url)

   3) Job Q_EMITs dataReceived
      Job is removed from m_activeJobs, disconnected and destroyed
      signal jobRemoved is Q_EMITted

   so we can conclude following rules:
   - Job is only connected to signals when in "active" state


   questions:
   - update of initiatorId needed?
     "We update the initiatorId as the previous initiator
      likely doesn't exist anymore"
   - blacklist or black list?

 */

class DownloadQueueSet: public QObject
{
    Q_OBJECT

public:

    explicit DownloadQueueSet(QObject* const parent = nullptr);
    explicit DownloadQueueSet(const DownloadPolicy& policy, QObject* const parent = nullptr);
    ~DownloadQueueSet() override;

    DownloadPolicy downloadPolicy() const;
    void setDownloadPolicy(const DownloadPolicy&);

    bool canAcceptJob(const QUrl& sourceUrl,
                      const QString& destinationFileName) const;
    void addJob(HttpJob* const job);

    void activateJobs();
    void retryJobs();
    void purgeJobs();

Q_SIGNALS:

    void jobAdded();
    void jobRemoved();
    void jobRetry();
    void jobFinished(const QByteArray& data, const QString& destinationFileName,
                     const QString& id);
    void jobRedirected(const QUrl& newSourceUrl, const QString& destinationFileName,
                       const QString& id, DownloadUsage);
    void progressChanged(int active, int queued);

private Q_SLOTS:

    void finishJob(HttpJob* job, const QByteArray& data);
    void redirectJob(HttpJob* job, const QUrl& newSourceUrl);
    void retryOrBlacklistJob(HttpJob* job, const int errorCode);

private:

    void activateJob(HttpJob* const job);
    void deactivateJob(HttpJob* const job);
    bool jobIsActive(const QString& destinationFileName) const;
    bool jobIsQueued(const QString& destinationFileName) const;
    bool jobIsWaitingForRetry(const QString& destinationFileName) const;
    bool jobIsBlackListed(const QUrl& sourceUrl) const;

private:

    /** This is the first stage a job enters, from this queue it will get
     *  into the activatedJobs container.
     */
    class JobStack
    {
    public:

        bool contains(const QString& destinationFileName) const;
        int count() const;
        bool isEmpty() const;
        HttpJob* pop();
        void push(HttpJob* const);

    private:

        QStack<HttpJob*> m_jobs;
        QSet<QString>    m_jobsContent;
    };

private:

    DownloadPolicy      m_downloadPolicy;

    JobStack            m_jobs;

    /// Contains the jobs which are currently being downloaded.
    QList<HttpJob*>     m_activeJobs;

    /** Contains jobs which failed to download and which are scheduled for
     *  retry according to retry settings.
     */
    QQueue<HttpJob*>    m_retryQueue;

    /// Contains the blacklisted source urls
    QSet<QString>       m_jobBlackList;
};

} // namespace Marble

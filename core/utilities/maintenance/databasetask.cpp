/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-01-29
 * Description : Thread actions task for database cleanup.
 *
 * Copyright (C) 2017-2018 by Mario Frank <mario dot frank at uni minus potsdam dot de>
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

#include "databasetask.h"

// Qt includes

#include <QUrlQuery>

// Local includes

#include "digikam_debug.h"
#include "iteminfo.h"
#include "thumbsdb.h"
#include "thumbsdbaccess.h"
#include "coredb.h"
#include "coredbaccess.h"
#include "facialrecognition_wrapper.h"
#include "facetagseditor.h"
#include "maintenancedata.h"
#include "similaritydb.h"
#include "similaritydbaccess.h"

namespace Digikam
{

class Q_DECL_HIDDEN DatabaseTask::Private
{
public:

    explicit Private()
        : scanThumbsDb     (false),
          scanRecognitionDb(false),
          scanSimilarityDb (false),
          mode             (Mode::Unknown),
          data             (nullptr)
    {
    }

    QString          objectIdentification;

    bool             scanThumbsDb;
    bool             scanRecognitionDb;
    bool             scanSimilarityDb;

    Mode             mode;

    MaintenanceData* data;
};

// -------------------------------------------------------

DatabaseTask::DatabaseTask()
    : ActionJob(),
      d        (new Private)
{
}

DatabaseTask::~DatabaseTask()
{
    cancel();
    delete d;
}

void DatabaseTask::computeDatabaseJunk(bool thumbsDb, bool facesDb, bool similarityDb)
{
    d->scanThumbsDb      = thumbsDb;
    d->scanRecognitionDb = facesDb;
    d->scanSimilarityDb  = similarityDb;
}

void DatabaseTask::setMode(Mode mode)
{
    d->mode = mode;
}

void DatabaseTask::setMaintenanceData(MaintenanceData* const data)
{
    d->data = data;
}

void DatabaseTask::run()
{
    if (m_cancel)
    {
        return;
    }

    emit signalStarted();

    QThread::sleep(1);

    if      (d->mode == Mode::ShrinkDatabases)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Shrinking databases";

        if (CoreDbAccess().db()->integrityCheck())
        {
            CoreDbAccess().db()->vacuum();

            if (!CoreDbAccess().db()->integrityCheck())
            {
                qCWarning(DIGIKAM_DATABASE_LOG) << "Integrity check for core DB failed after vacuum. Something went wrong.";

                // Signal that the database was vacuumed but failed the integrity check afterwards.

                emit signalFinished(true, false);
            }
            else
            {
                qCDebug(DIGIKAM_DATABASE_LOG) << "Finished vacuuming of core DB. Integrity check after vacuuming was positive.";
                emit signalFinished(true, true);
            }
        }
        else
        {
            qCWarning(DIGIKAM_DATABASE_LOG) << "Integrity check for core DB failed. Will not vacuum.";

            // Signal that the integrity check failed and thus the vacuum was skipped

            emit signalFinished(false, false);
        }

        QThread::sleep(1);

        if (m_cancel)
        {
            return;
        }

        if (ThumbsDbAccess::isInitialized())
        {
            if (ThumbsDbAccess().db()->integrityCheck())
            {
                ThumbsDbAccess().db()->vacuum();

                if (!ThumbsDbAccess().db()->integrityCheck())
                {
                    qCWarning(DIGIKAM_DATABASE_LOG) << "Integrity check for thumbnails DB failed after vacuum. Something went wrong.";

                    // Signal that the database was vacuumed but failed the integrity check afterwards.

                    emit signalFinished(true, false);
                }
                else
                {
                    qCDebug(DIGIKAM_DATABASE_LOG) << "Finished vacuuming of thumbnails DB. Integrity check after vacuuming was positive.";
                    emit signalFinished(true, true);
                }
            }
            else
            {
                qCWarning(DIGIKAM_DATABASE_LOG) << "Integrity check for thumbnails DB failed. Will not vacuum.";

                // Signal that the integrity check failed and thus the vacuum was skipped

                emit signalFinished(false, false);
            }
        }
        else
        {
            qCWarning(DIGIKAM_DATABASE_LOG) << "Thumbnails DB is not initialised. Will not vacuum.";
            emit signalFinished(false, false);
        }

        QThread::sleep(1);

        if (m_cancel)
        {
            return;
        }

        if (FacialRecognitionWrapper().integrityCheck())
        {
            FacialRecognitionWrapper().vacuum();

            if (!FacialRecognitionWrapper().integrityCheck())
            {
                qCWarning(DIGIKAM_DATABASE_LOG) << "Integrity check for recognition DB failed after vacuum. Something went wrong.";

                // Signal that the database was vacuumed but failed the integrity check afterwards.

                emit signalFinished(true, false);
            }
            else
            {
                qCDebug(DIGIKAM_DATABASE_LOG) << "Finished vacuuming of recognition DB. Integrity check after vacuuming was positive.";
                emit signalFinished(true, true);
            }
        }
        else
        {
            qCWarning(DIGIKAM_DATABASE_LOG) << "Integrity check for recognition DB failed. Will not vacuum.";

            // Signal that the integrity check failed and thus the vacuum was skipped

            emit signalFinished(false, false);
        }

        QThread::sleep(1);

        if (m_cancel)
        {
            return;
        }

        if (SimilarityDbAccess::isInitialized())
        {
            if (SimilarityDbAccess().db()->integrityCheck())
            {
                SimilarityDbAccess().db()->vacuum();

                if (!SimilarityDbAccess().db()->integrityCheck())
                {
                    qCWarning(DIGIKAM_DATABASE_LOG) << "Integrity check for similarity DB failed after vacuum. Something went wrong.";

                    // Signal that the database was vacuumed but failed the integrity check afterwards.

                    emit signalFinished(true, false);
                }
                else
                {
                    qCDebug(DIGIKAM_DATABASE_LOG) << "Finished vacuuming of similarity DB. Integrity check after vacuuming was positive.";
                    emit signalFinished(true, true);
                }
            }
            else
            {
                qCWarning(DIGIKAM_DATABASE_LOG) << "Integrity check for recognition DB failed. Will not vacuum.";

                // Signal that the integrity check failed and thus the vacuum was skipped

                emit signalFinished(false, false);
            }
        }

        QThread::sleep(1);
    }
    else if (d->mode == Mode::ComputeDatabaseJunk)
    {
        QList<qlonglong> staleImageIds;
        QList<int>       staleThumbIds;
        QList<Identity>  staleIdentities;
        QList<qlonglong> staleSimilarityImageIds;
        int additionalItemsToProcess = 0;

        QList<qlonglong> coredbItems = CoreDbAccess().db()->getAllItems();

        // Get the count of image entries in DB to delete.

        staleImageIds                = CoreDbAccess().db()->getImageIds(DatabaseItem::Status::Obsolete);

        // get the count of items to process for thumbnails cleanup it enabled.

        if (d->scanThumbsDb && ThumbsDbAccess::isInitialized())
        {
            additionalItemsToProcess += coredbItems.size();
        }

        // get the count of items to process for identities cleanup it enabled.

        if (d->scanRecognitionDb)
        {
            additionalItemsToProcess += FacialRecognitionWrapper().allIdentities().size();
        }

        if (d->scanSimilarityDb)
        {
            additionalItemsToProcess += coredbItems.size();
        }

        if (additionalItemsToProcess > 0)
        {
            emit signalAddItemsToProcess(additionalItemsToProcess);
        }

        emit signalFinished();

        // Get the stale thumbnail paths.

        if (d->scanThumbsDb && ThumbsDbAccess::isInitialized())
        {
            // Thumbnails should be deleted, if the following conditions hold:
            // 1) The file path to which the thumb is assigned does not lead to an item
            // 2) The unique hash and file size are not used in core db for an item.
            // 3) The custom identifier does not exist in core db for an item.
            // OR
            // The thumbnail is stale, i.e. no thumbs db table references it.

            QSet<int> thumbIds = ThumbsDbAccess().db()->findAll().toSet();

            FaceTagsEditor editor;

            foreach (const qlonglong& item, coredbItems)
            {
                if (m_cancel)
                {
                    return;
                }

                ItemInfo info(item);

                if (!info.isNull())
                {
                    // Remove the id that is found by the file path. Finding the id -1 does no harm

                    bool removed = thumbIds.remove(ThumbsDbAccess().db()->findByFilePath(info.filePath()).id);

                    if (!removed)
                    {
                        // Remove the id that is found by the hash and file size. Finding the id -1 does no harm

                        thumbIds.remove(ThumbsDbAccess().db()->findByHash(info.uniqueHash(), info.fileSize()).id);
                    }

                    // Add the custom identifier.
                    // get all faces for the image and generate the custom identifiers

                    QUrl url;
                    url.setScheme(QLatin1String("detail"));
                    url.setPath(info.filePath());
                    QList<FaceTagsIface> faces = editor.databaseFaces(item);

                    foreach (const FaceTagsIface& face, faces)
                    {
                        QRect rect = face.region().toRect();
                        QString r  = QString::fromLatin1("%1,%2-%3x%4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
                        QUrlQuery q(url);

                        // Remove the previous query if existent.

                        q.removeQueryItem(QLatin1String("rect"));
                        q.addQueryItem(QLatin1String("rect"), r);
                        url.setQuery(q);

                        //qCDebug(DIGIKAM_GENERAL_LOG) << "URL: " << url.toString();

                        // Remove the id that is found by the custom identifier. Finding the id -1 does no harm

                        thumbIds.remove(ThumbsDbAccess().db()->findByCustomIdentifier(url.toString()).id);
                    }
                }

                // Signal that this item was processed.

                emit signalFinished();
            }

            // The remaining thumbnail ids should be used to remove them since they are stale.

            staleThumbIds = thumbIds.values();

            // Signal that the database was processed.

            emit signalFinished();
        }

        if (m_cancel)
        {
            return;
        }

        // Get the stale face identities.

        if (d->scanRecognitionDb)
        {
            QList<TagProperty> properties = CoreDbAccess().db()->getTagProperties(TagPropertyName::faceEngineUuid());
            QSet<QString> uuidSet;

            foreach (const TagProperty& prop, properties)
            {
                uuidSet << prop.value;
            }

            QList<Identity> identities = FacialRecognitionWrapper().allIdentities();

            // Get all identities to remove. Don't remove now in order to make sure no side effects occur.

            foreach (const Identity& identity, identities)
            {
                QString value = identity.attribute(QLatin1String("uuid"));

                if (!value.isEmpty() && !uuidSet.contains(value))
                {
                    staleIdentities << identity;
                }

                // Signal that this identity was processed.

                emit signalFinished();
            }

            // Signal that the database was processed.

            emit signalFinished();
        }

        if (m_cancel)
        {
            return;
        }

        if (d->scanSimilarityDb)
        {
            // Get all registered image ids from the similarity db.

            QSet<qlonglong> similarityDbItems = SimilarityDbAccess().db()->registeredImageIds();

            // Remove all image ids that are existent in the core db

            foreach (const qlonglong& imageId, coredbItems)
            {
                similarityDbItems.remove(imageId);

                // Signal that this image id was processed.

                emit signalFinished();
            }

            // The remaining image ids should be removed from the similarity db.

            staleSimilarityImageIds = similarityDbItems.values();

            // Signal that the database was processed.

            emit signalFinished();
        }

        emit signalData(staleImageIds, staleThumbIds, staleIdentities, staleSimilarityImageIds);
    }
    else if (d->mode == Mode::CleanCoreDb)
    {
        // While we have data (using this as check for non-null)

        while (d->data)
        {
            if (m_cancel)
            {
                return;
            }

            qlonglong imageId = d->data->getImageId();

            if (imageId == -1)
            {
                break;
            }

            CoreDbAccess().db()->deleteItem(imageId);

            emit signalFinished();
        }
    }
    else if (d->mode == Mode::CleanThumbsDb)
    {
        BdEngineBackend::QueryState lastQueryState = BdEngineBackend::QueryState(BdEngineBackend::ConnectionError);
        (void)lastQueryState; // prevent cppcheck warning.

        // Connect to the database

        lastQueryState                             = ThumbsDbAccess().backend()->beginTransaction();

        if (BdEngineBackend::NoErrors == lastQueryState)
        {
            // Start removing.

            // While we have data (using this as check for non-null)

            while (d->data)
            {
                if (m_cancel)
                {
                    return;
                }

                int thumbId = d->data->getThumbnailId();

                if (thumbId == -1)
                {
                    break;
                }

                lastQueryState = ThumbsDbAccess().db()->remove(thumbId);
                emit signalFinished();
            }

            // Check for errors.

            if (BdEngineBackend::NoErrors == lastQueryState)
            {
                // Commit the removal if everything was fine.

                lastQueryState = ThumbsDbAccess().backend()->commitTransaction();

                if (BdEngineBackend::NoErrors != lastQueryState)
                {
                    qCWarning(DIGIKAM_THUMBSDB_LOG) << "Could not commit the removal of "
                                                    << d->objectIdentification
                                                    << " due to error ";
                }
            }
            else
            {
                qCWarning(DIGIKAM_THUMBSDB_LOG) << "Could not start the removal of "
                                                << d->objectIdentification
                                                << " due to error ";
            }
        }
        else
        {
            qCWarning(DIGIKAM_THUMBSDB_LOG) << "Could not begin the transaction for the removal of "
                                            << d->objectIdentification
                                            << " due to error ";
        }
    }
    else if (d->mode == Mode::CleanRecognitionDb)
    {
        // While we have data (using this as check for non-null)

        while (d->data)
        {
            if (m_cancel)
            {
                return;
            }

            Identity identity = d->data->getIdentity();

            if (identity.isNull())
            {
                break;
            }

            FacialRecognitionWrapper().deleteIdentity(identity);
            emit signalFinished();
        }
    }
    else if (d->mode == Mode::CleanSimilarityDb)
    {
        // While we have data (using this as check for non-null)

        while (d->data)
        {
            if (m_cancel)
            {
                return;
            }

            qlonglong imageId = d->data->getSimilarityImageId();

            if (imageId == -1)
            {
                break;
            }

            SimilarityDbAccess().db()->removeImageFingerprint(imageId, FuzzyAlgorithm::Haar);
            SimilarityDbAccess().db()->removeImageFingerprint(imageId, FuzzyAlgorithm::TfIdf);

            emit signalFinished();
        }
    }

    emit signalDone();
}

} // namespace Digikam

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-09
 * Description : resize image job.
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "imageresizejob.h"

// Qt includes

#include <QDir>
#include <QFileInfo>
#include <QScopedPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dimg.h"
#include "previewloadthread.h"
#include "dmetadata.h"

namespace DigikamGenericSendByMailPlugin
{

ImageResizeJob::ImageResizeJob(int* count)
    : ActionJob (),
      m_settings(nullptr),
      m_count   (count)
{
}

ImageResizeJob::~ImageResizeJob()
{
}

void ImageResizeJob::run()
{
    emit signalStarted();

    QString errString;

    emit startingResize(m_orgUrl);

    m_mutex.lock();
    (*m_count)++;
    m_mutex.unlock();

    int percent = (int)(((float)(*m_count)/(float)m_settings->itemsList.count())*100.0);

    if (imageResize(m_settings, m_orgUrl, m_destName, errString))
    {
        QUrl emailUrl(QUrl::fromLocalFile(m_destName));
        emit finishedResize(m_orgUrl, emailUrl, percent);
    }
    else
    {
        emit failedResize(m_orgUrl, errString, percent);
    }

    if (m_settings->itemsList.count() == *m_count)
    {
        m_mutex.lock();
        *m_count = 0;
        m_mutex.unlock();
    }

    emit signalDone();
}

bool ImageResizeJob::imageResize(MailSettings* const settings,
                                 const QUrl& orgUrl,
                                 const QString& destName,
                                 QString& err)
{
    QFileInfo fi(orgUrl.toLocalFile());

    if (!fi.exists() || !fi.isReadable())
    {
        err = i18n("Error opening input file");
        return false;
    }

    QFileInfo tmp(destName);
    QFileInfo tmpDir(tmp.dir().absolutePath());

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "tmpDir: " << tmp.dir().absolutePath();

    if (!tmpDir.exists() || !tmpDir.isWritable())
    {
        err = i18n("Error opening temporary folder");
        return false;
    }

    DImg img = PreviewLoadThread::loadFastSynchronously(orgUrl.toLocalFile(), settings->imageSize);

    if (img.isNull())
    {
        img.load(orgUrl.toLocalFile());
    }

    if (!img.isNull())
    {
        uint sizeFactor = settings->imageSize;

        if ((img.width() > sizeFactor) || (img.height() > sizeFactor))
        {
            DImg scaledImg = img.smoothScale(sizeFactor,
                                             sizeFactor,
                                             Qt::KeepAspectRatio);

            if ((scaledImg.width() > sizeFactor) || (scaledImg.height() > sizeFactor))
            {
                err = i18n("Cannot resize image. Aborting.");
                return false;
            }

            img = scaledImg;
        }

        if      (settings->format() == QLatin1String("JPEG"))
        {
            img.setAttribute(QLatin1String("quality"), settings->imageCompression);

            if (!img.save(destName, settings->format()))
            {
                err = i18n("Cannot save resized image (JPEG). Aborting.");
                return false;
            }
        }
        else if (settings->format() == QLatin1String("PNG"))
        {
            if (!img.save(destName, settings->format()))
            {
                err = i18n("Cannot save resized image (PNG). Aborting.");
                return false;
            }
        }

        QScopedPointer<DMetadata> meta(new DMetadata);

        if (!meta->load(destName))
        {
            return false;
        }

        if (settings->removeMetadata)
        {
            meta->clearExif();
            meta->clearIptc();
            meta->clearXmp();
        }
        else
        {
            meta->setItemOrientation(MetaEngine::ORIENTATION_NORMAL);
        }

        meta->setMetadataWritingMode((int)DMetadata::WRITE_TO_FILE_ONLY);

        if (!meta->save(destName))
        {
            return false;
        }

        return true;
    }

    return false;
}

} // namespace DigikamGenericSendByMailPlugin

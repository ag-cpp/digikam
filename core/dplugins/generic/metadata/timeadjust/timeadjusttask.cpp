/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-12-31
 * Description : time adjust actions using threads.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2018      by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "timeadjusttask.h"

// Qt includes

#include <QFile>
#include <QScopedPointer>
#include <qplatformdefs.h>

// Local includes

#include "dmetadata.h"
#include "digikam_debug.h"
#include "dinfointerface.h"
#include "dfileoperations.h"
#include "timeadjustlist.h"
#include "timeadjustthread.h"
#include "metaenginesettings.h"

namespace DigikamGenericTimeAdjustPlugin
{

class Q_DECL_HIDDEN TimeAdjustTask::Private
{
public:

    explicit Private()
    {
    }

    QUrl                  url;

    // Settings from GUI.
    TimeAdjustContainer   settings;

    // Map of item urls and Updated Timestamps.
    QMap<QUrl, QDateTime> itemsMap;
};

TimeAdjustTask::TimeAdjustTask(const QUrl& url)
    : ActionJob(),
      d(new Private)
{
    d->url = url;
}

TimeAdjustTask::~TimeAdjustTask()
{
    cancel();
    delete d;
}

void TimeAdjustTask::setSettings(const TimeAdjustContainer& settings)
{
    d->settings = settings;
}

void TimeAdjustTask::setItemsMap(const QMap<QUrl, QDateTime>& itemsMap)
{
    d->itemsMap = itemsMap;
}

void TimeAdjustTask::run()
{
    if (m_cancel)
        return;

    emit signalProcessStarted(d->url);

    QDateTime dt = d->itemsMap.value(d->url);

    if (!dt.isValid())
    {
        emit signalProcessEnded(d->url, TimeAdjustList::META_TIME_ERROR);
        emit signalDone();
        return;
    }

    bool writeToSidecar  = (MetaEngineSettings::instance()->settings()
                            .metadataWritingMode != DMetadata::WRITE_TO_FILE_ONLY);

    bool metadataChanged = d->settings.updEXIFModDate || d->settings.updEXIFOriDate ||
                           d->settings.updEXIFDigDate || d->settings.updEXIFThmDate ||
                           d->settings.updIPTCDate    || d->settings.updXMPVideo    ||
                           d->settings.updXMPDate;

    int status = TimeAdjustList::NOPROCESS_ERROR;

    if (metadataChanged)
    {
        bool ret = true;

        QScopedPointer<DMetadata> meta(new DMetadata);

        ret &= meta->load(d->url.toLocalFile());

        if (ret)
        {
            QString exifDateTimeFormat = QLatin1String("yyyy:MM:dd hh:mm:ss");
            QString xmpDateTimeFormat  = QLatin1String("yyyy-MM-ddThh:mm:ss");

            if (writeToSidecar || meta->canWriteExif(d->url.toLocalFile()))
            {
                if (d->settings.updEXIFModDate)
                {
                    if (!d->settings.updIfAvailable ||
                        !meta->getExifTagString("Exif.Image.DateTime").isEmpty())
                    {
                        ret &= meta->setExifTagString("Exif.Image.DateTime",
                                                      dt.toString(exifDateTimeFormat));
                    }
                }

                if (d->settings.updEXIFOriDate)
                {
                    if (!d->settings.updIfAvailable ||
                        !meta->getExifTagString("Exif.Photo.DateTimeOriginal").isEmpty())
                    {
                        ret &= meta->setExifTagString("Exif.Photo.DateTimeOriginal",
                                                      dt.toString(exifDateTimeFormat));
                    }
                }

                if (d->settings.updEXIFDigDate)
                {
                    if (!d->settings.updIfAvailable ||
                        !meta->getExifTagString("Exif.Photo.DateTimeDigitized").isEmpty())
                    {
                        ret &= meta->setExifTagString("Exif.Photo.DateTimeDigitized",
                                                      dt.toString(exifDateTimeFormat));
                    }
                }

                if (d->settings.updEXIFThmDate)
                {
                   if (!d->settings.updIfAvailable ||
                        !meta->getExifTagString("Exif.Image.PreviewDateTime").isEmpty())
                   {
                       ret &= meta->setExifTagString("Exif.Image.PreviewDateTime",
                                                     dt.toString(exifDateTimeFormat));
                   }
                }
            }
            else if (d->settings.updEXIFModDate || d->settings.updEXIFOriDate ||
                     d->settings.updEXIFDigDate || d->settings.updEXIFThmDate)
            {
                ret = false;
            }

            if (d->settings.updIPTCDate)
            {
                if (writeToSidecar || meta->canWriteIptc(d->url.toLocalFile()))
                {
                    if (!d->settings.updIfAvailable ||
                        !meta->getIptcTagString("Iptc.Application2.DateCreated").isEmpty())
                    {
                        ret &= meta->setIptcTagString("Iptc.Application2.DateCreated",
                                                      dt.date().toString(Qt::ISODate));
                    }

                    if (!d->settings.updIfAvailable ||
                        !meta->getIptcTagString("Iptc.Application2.TimeCreated").isEmpty())
                    {
                        ret &= meta->setIptcTagString("Iptc.Application2.TimeCreated",
                                                      dt.time().toString(Qt::ISODate));
                    }
                }
                else
                {
                    ret = false;
                }
            }

            if (d->settings.updXMPDate)
            {
                if (writeToSidecar || (meta->supportXmp() && meta->canWriteXmp(d->url.toLocalFile())))
                {
                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.exif.DateTimeOriginal").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.exif.DateTimeOriginal",
                                                     dt.toString(xmpDateTimeFormat));
                    }

                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.photoshop.DateCreated").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.photoshop.DateCreated",
                                                     dt.toString(xmpDateTimeFormat));
                    }

                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.tiff.DateTime").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.tiff.DateTime",
                                                     dt.toString(xmpDateTimeFormat));
                    }

                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.xmp.CreateDate").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.xmp.CreateDate",
                                                     dt.toString(xmpDateTimeFormat));
                    }

                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.xmp.MetadataDate").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.xmp.MetadataDate",
                                                     dt.toString(xmpDateTimeFormat));
                    }

                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.xmp.ModifyDate").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.xmp.ModifyDate",
                                                     dt.toString(xmpDateTimeFormat));
                    }
                }
                else
                {
                    ret = false;
                }
            }

            if (d->settings.updXMPVideo)
            {
                if (writeToSidecar || (meta->supportXmp() && meta->canWriteXmp(d->url.toLocalFile())))
                {
                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.video.DateTimeOriginal").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.video.DateTimeOriginal",
                                                     dt.toString(xmpDateTimeFormat));
                    }

                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.video.DateTimeDigitized").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.video.DateTimeDigitized",
                                                     dt.toString(xmpDateTimeFormat));
                    }

                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.video.ModificationDate").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.video.ModificationDate",
                                                     dt.toString(xmpDateTimeFormat));
                    }

                    if (!d->settings.updIfAvailable ||
                        !meta->getXmpTagString("Xmp.video.DateUTC").isEmpty())
                    {
                        ret &= meta->setXmpTagString("Xmp.video.DateUTC",
                                                     dt.toUTC().toString(xmpDateTimeFormat));
                    }
                }
                else
                {
                    ret = false;
                }
            }

            ret &= meta->save(d->url.toLocalFile());

            if (!ret)
            {
                qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Failed to update metadata in file " << d->url.fileName();
            }
        }
        else
        {
            qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Failed to load metadata from file " << d->url.fileName();
        }

        if (!ret)
        {
            status |= TimeAdjustList::META_TIME_ERROR;
        }
    }

    if (d->settings.updFileModDate)
    {
        if (!DFileOperations::setModificationTime(d->url.toLocalFile(), dt))
        {
            status |= TimeAdjustList::FILE_TIME_ERROR;
        }
    }

    if (writeToSidecar && DMetadata::hasSidecar(d->url.toLocalFile()))
    {
        if (!DFileOperations::copyModificationTime(d->url.toLocalFile(), DMetadata::sidecarPath(d->url.toLocalFile())))
        {
            status |= TimeAdjustList::FILE_TIME_ERROR;
        }
    }

    if ((status & TimeAdjustList::META_TIME_ERROR) != TimeAdjustList::META_TIME_ERROR)
    {
        emit signalDateTimeForUrl(d->url, dt, d->settings.updFileModDate);
    }

    emit signalProcessEnded(d->url, status);
    emit signalDone();
}

} // namespace DigikamGenericTimeAdjustPlugin

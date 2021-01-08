/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-19
 * Description : Track file reader
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2014 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_TRACK_READER_H
#define DIGIKAM_TRACK_READER_H

// Qt includes

#include <QXmlDefaultHandler>

// local includes

#include "trackmanager.h"
#include "digikam_export.h"

class TestTracks;

namespace Digikam
{

class DIGIKAM_EXPORT TrackReader : public QXmlDefaultHandler
{
public:

    class TrackReadResult
    {
    public:

        explicit TrackReadResult()
          : track(),
            isValid(false),
            loadError()
        {
        }

        TrackManager::Track            track;
        bool                           isValid;
        QString                        loadError;

        typedef QList<TrackReadResult> List;
    };

    explicit TrackReader(TrackReadResult* const dataTarget);
    ~TrackReader() override;

    bool characters(const QString& ch)              override;

    bool endElement(const QString& namespaceURI,
                            const QString& localName,
                            const QString& qName)           override;

    bool startElement(const QString& namespaceURI,
                              const QString& localName,
                              const QString& qName,
                              const QXmlAttributes& atts)   override;

    static TrackReadResult loadTrackFile(const QUrl& url);
    static QDateTime ParseTime(QString timeString);

private:

    void rebuildElementPath();

    static QString myQName(const QString& namespaceURI, const QString& localName);

private:

    class Private;
    const QScopedPointer<Private> d;

    friend class ::TestTracks;
};

} // namespace Digikam

#endif // DIGIKAM_TRACK_READER_H

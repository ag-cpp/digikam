/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-14
 * Description : Face model downloader
 *
 * Copyright (C) 2020 by Maik Qualmann <metzpinguin at gmail dot com>
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

#ifndef DIGIKAM_FACE_MODEL_DOWNLOADER_H
#define DIGIKAM_FACE_MODEL_DOWNLOADER_H

// Qt includes

#include <QObject>
#include <QNetworkReply>

// Local includes

namespace Digikam
{

class Q_DECL_HIDDEN FaceModelDownloader : public QObject
{
    Q_OBJECT

public:

    explicit FaceModelDownloader(QObject* const parent);
    ~FaceModelDownloader() override;

    bool exists() const;
    void download(int model);

Q_SIGNALS:

    void downloadFinished(bool success, int model);

private:

    void nextDownload();
    bool saveArray(const QByteArray& array, const QString& name);

private Q_SLOTS:

    void downloaded(QNetworkReply* reply);

private:

    // Hidden copy constructor and assignment operator.
    FaceModelDownloader(const FaceModelDownloader&);
    FaceModelDownloader& operator=(const FaceModelDownloader&);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_MODEL_DOWNLOADER_H
